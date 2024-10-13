#!/bin/sh

# $1: process name by tsrite

restart_cnt=0

while [ 1 ]
do
  sleep 3

  # check duplicate agents
  num=`ps | grep /mydlink/$1 | grep -v grep -c`
  if [ "$num" -gt "1" ]; then
    echo "[`date`] Duplicate signalc ..."
    /mydlink/opt.local start
  fi

  pid=`ps | grep /mydlink/$1 | grep -v grep | sed 's/^[ \t]*//'  | sed 's/ .*//' `
  if [ -z "$pid" ]; then
    echo "[`date`] $1 is not running!"

    killall -9 $1
    if [ -f /mydlink/$1 ]; then
      /mydlink/$1 > /dev/null 2>&1 & 
    elif [ -f /opt/$1 ]; then
      /opt/$1 > /dev/null 2>&1 & 
    fi
    restart_cnt=`expr $restart_cnt + 1`
    if [ "$restart_cnt" -gt 6 ]; then
      echo "reboot cause device agent can't startup"
      reboot
    fi
  else
    restart_cnt=0    
  fi

done
