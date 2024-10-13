#!/bin/sh
#echo [$0] [$1] [$2] [$3] [$4] [$5] [$6]... > /dev/console
XMLCMDC="dxmlc"
if [ "`xmldbc -H`" = "2" ]; then XMLCMDC="xmldbc"; fi

# initial variables
MAX_CNT=$1
TIMER=$2
OTNAME=$3
DAEMON_NAME=$4
DAEMON_ARG=$5
DAEMON_PATH=$6
if [ "$MAX_CNT" = "" ]; then MAX_CNT=12; fi  
if [ "$TIMER" = "" ]; then TIMER=10; fi  
if [ "$OTNAME" = "" ]; then OTNAME=onetouch; fi  
if [ "$DAEMON_NAME" = "" ]; then DAEMON_NAME="$OTNAME"; fi 
if [ "$DAEMON_PATH" != "" ]; then DAEMON_PATH="${DAEMON_PATH}/"; fi 
DCNT_PATH="/runtime/$OTNAME/${DAEMON_NAME}_lost"
SCRPDIR=`$XMLCMDC -g "/$OTNAME/scriptdirecory"`
COUNTER=`$XMLCMDC -i -g "$DCNT_PATH"`
if [ "$COUNTER" = "" ]; then COUNTER=0; fi  

# remove old timer
$XMLCMDC -k ${DAEMON_NAME}_checker 2> /dev/null

# check daemon's life
FIND_DM=`ps |grep $DAEMON_NAME |sed '/grep/'d |sed '/rss_checker/'d`
#echo FIND_DM=\"$FIND_DM\"

if [ "$FIND_DM" != "" ]; then 
  if [ "$COUNTER" != "0" ]; then $XMLCMDC -i -s "$DCNT_PATH" "0"; fi  
else
  echo "[$0] LOST $DAEMON_NAME ...$COUNTER/$MAX_CNT"
  if [ "$COUNTER" = "$MAX_CNT" ]; then
    # restart Daemon
    echo "[$0] Restart $DAEMON_NAME ..."
    $DAEMON_PATH$DAEMON_NAME $DAEMON_ARG &
    COUNTER=0
  else
    COUNTER=`expr $COUNTER + 1`
  fi  
  $XMLCMDC -i -s "$DCNT_PATH" "$COUNTER"
fi
$XMLCMDC -t "${DAEMON_NAME}_checker:$TIMER:sh $SCRPDIR/rss_checker.sh $MAX_CNT $TIMER $OTNAME $DAEMON_NAME $DAEMON_ARG $DAEMON_PATH" 

