#!/bin/sh

UNIT_CHECK_T=5
MYDLINK_BASE="/mydlink"
PID_BASE="/tmp/run"
restart_cnt=0
WATCHDOG_PID="$PID_BASE/mydlink-watch-dog.pid"
TMP_MYDLINK="/tmp/mydlink"
mkdir -p $TMP_MYDLINK
mkdir -p $PID_BASE

#
log() {
  #echo "[`date +"%Y-%m-%d %H:%M:%S"`] $1" >> $LOG_FILE
  echo "[`date +"%Y-%m-%d %H:%M:%S"`] $1" > /dev/null 2>&1
}


# Manage the watchdog PID
wd_pid="-1"
if [ -f ${WATCHDOG_PID} ]; then
    wd_pid=`cat ${WATCHDOG_PID}`
fi
if [ -d "/proc/$wd_pid" ] && [ "0$wd_pid" -ne "0$$" ]; then
  log "Watchdog is running, exit."
  exit 255
else
  echo "$$" > "${WATCHDOG_PID}"
fi


if [ ! -f /mydlink/config/device.cfg ]; then
  `cp /mydlink/device.cfg /mydlink/config/device.cfg`
fi

# modify the cache of kernel by checking the the dev_list
# the model which support SDcard = return 32(0x20) in ctrl value should be modified
DEV_LIST_DECODED=$(echo -e `mdb get dev_list | sed 's/+/ /g;s/%/\\\\x/g;'` | cut -d '&' -f 3 | grep 32 -c)

check_memory() {
  # Don't modify the line, because of it will be replaced by watchdog.
  # If need, please modify env variable (MEM_THRESHOLD).
  Memory_Threshold=10000

  Free_Memory=`cat /proc/meminfo | grep 'MemFree:' | sed 's/^.*MemFree://g' | sed 's/kB*$//g'`
    
  if [ $Free_Memory -le $Memory_Threshold ]; then
     sync; echo 3 > /proc/sys/vm/drop_caches
     echo "!!! Notice! Clear memory cache !!!"
  fi
}

check_alive() {
  # check if the program exists or not
  if [ ! -f "$MYDLINK_BASE/$1" ]; then
    return
  fi

  # check if process exists by pid
  pid="-1"
  if [ -f "${PID_BASE}/${1}.pid" ]; then
    pid=`cat ${PID_BASE}/${1}.pid`
  fi
  if [ -d "/proc/${pid}" ]; then
    restart_cnt=0
    return
  fi

  restart_cnt=`expr $restart_cnt + 1`
  if [ "$restart_cnt" -gt 6 ]; then
    log "reboot cause device agent can't startup"
    reboot
  fi

  log "$1 is not running! ($pid)"
  # kill all remaining processes and wait a moment
  killall -q $1 2>/dev/null
  sleep 1

  # launch the process
  # $MYDLINK_BASE/$1 $2 >> "${LOG_BASE}/${1}.log" 2>&1 &
   LD_LIBRARY_PATH="/mydlink/lib" /mydlink/$1 "$2" > /dev/null 2>&1 &
  pid="$!"
  res="$?"
  # keep the pid
  echo $pid > "${PID_BASE}/${1}.pid"

  log " - launch $1 ($pid, $res)"
}

if [ ""$1 == "restart" ]; then
killall -9 da_adaptor cda sa da_fctl strmsvr
fi

( 
while [ 1 ]
do
  curpid=`cat ${WATCHDOG_PID}`
  if [ "0$$" -ne "0$curpid" ]; then
    log "Unexpected pid (self: $$ cur: $curpid), exit!"
    exit 255
  fi

  check_alive da_adaptor "-s LD_LIBRARY_PATH=/mydlink/lib /mydlink/mdb"
  check_alive cda
  check_alive sa
  check_alive da_fctl
  check_alive strmsvr
  
  if [ "1" -eq "$DEV_LIST_DECODED" ]; then
    check_memory
  fi

  sleep $UNIT_CHECK_T
done
) &
