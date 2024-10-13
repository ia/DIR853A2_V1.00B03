#!/bin/sh
echo [$0] [$1] [$2] [$3] [$4]... > /dev/console
XMLCMDC="dxmlc"
if [ "`xmldbc -H`" = "2" ]; then XMLCMDC="xmldbc"; fi
OTDIR="$2"
SCRPDIR=`$XMLCMDC -g "$OTDIR/scriptdirecory"`
TMPDIR=`$XMLCMDC -g "$OTDIR/tempdirecory"`
RSS_PHP=$SCRPDIR/rss_xml.php
RSSTMP_PHP=$TMPDIR/rss.php

# Prepare Special Node Values
lanip=`$XMLCMDC -i -g "/runtime$OTDIR/lanip"`
rssport=`$XMLCMDC -g "$OTDIR/rsshttpport"`
hwSupport=`$XMLCMDC -g /uplink/hardwaresupport`
if [ "${hwSupport#*DSL}" = "$hwSupport" ]; then isDslWan=0; else isDslWan=1; fi
if [ "${hwSupport#*WWanAPN}" = "$hwSupport" ]; then isApnWan=0; else isApnWan=1; fi
if [ "${hwSupport#*WWan802}" = "$hwSupport" ]; then is80211Wan=0; else is80211Wan=1; fi

# Prepare Encryption Data
isCipher="$3"

echo "<?" > $RSSTMP_PHP
echo "\$isOnlyApNode = \"$1\";" >>$RSSTMP_PHP
#echo "\$OT_Path = \"$OTDIR\";" >>$RSSTMP_PHP
echo "\$isDslWan = \"$isDslWan\";" >>$RSSTMP_PHP
echo "\$isApnWan = \"$isApnWan\";" >>$RSSTMP_PHP
echo "\$is80211Wan = \"$is80211Wan\";" >>$RSSTMP_PHP
echo "\$fwUpgradeUrl = \"`$XMLCMDC -G /fwupgradeinfo/fwupgradeurl |sed 's/$lanip/'$lanip'/g' |sed 's/$rssport/'$rssport'/g'`\";" >>$RSSTMP_PHP
echo "\$isCipher = \"$isCipher\";" >>$RSSTMP_PHP
echo "\$specVer = \"$4\";" >>$RSSTMP_PHP
echo "?>" >>$RSSTMP_PHP

#$XMLCMDC -A $RSS_PHP -V isOnlyApNode=$1 -V isDslWan=$isDslWan -V isApnWan=$isApnWan -V is80211Wan=$is80211Wan > $TMPDIR/rss.sh
#sh $TMPDIR/rss.sh > /dev/console
$XMLCMDC -A $RSS_PHP -V OT_Path=$OTDIR > $TMPDIR/rss.xml
