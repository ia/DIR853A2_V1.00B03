<?

/* The $OT_Path is PHP variable, sent by parameter "-V". 
   The some variables should be provided in required $tempDirecory."/rss.php",
   $isOnlyApNode, if the value is "1", only generate nodes used for AP Cloning
   $specVer, should be "1.0" or "1.1", if otherelse(or ""), use $OT_Path/specversion
   $isCipher, if the value is "1", Rss Client support Data Encryption
   $isDslWan, $isApnWan and $is80211Wan, its value is "1", when <HardwareSupport> support DSL, APN, 802.11
   and so on.
*/

/* temp directory & file */
$tempDirecory = queryxml($OT_Path."/tempdirecory");
require($tempDirecory."/rss.php");

/* echo "<!-- OT_Path=".$OT_Path.", isOnlyApNode=".$isOnlyApNode.", specVer=".$specVer." -->\n"; 
echo "<!-- isDslWan=".$isDslWan.", isApnWan=".$isApnWan.", is80211Wan=".$is80211Wan.", isCipher=".$isCipher." -->\n"; */

/* one-touch Basic Information */
anchor($OT_Path);
/* feature enable/disable status */
$apCloning   = queryxml("apcloning");
$fwUpgrade   = queryxml("fwupgrade");
$easyRoaming = queryxml("easyroaming");
$safety360   = queryxml("safety360");
if ($isCipher == "1") {
	$CipherTxt = " Cipher='rss'";
	$rssEncryptKey = queryxml("/runtime/rssencryptkey");
}
else {
	$CipherTxt = "";
	$rssEncryptKey = "";
}
/* easy roaming info */
if ($isCipher == "1") {
	$easyRoamingInfo = queryencry("easyroaminginfo");
}
else {
	$easyRoamingInfo = queryxml("easyroaminginfo");
}
$deviceConfigurable = queryxml("configurable");
if ($deviceConfigurable == "") {
    $deviceConfigurable = "1"; 
    set("configurable", $deviceConfigurable);
}
/* echo "#php deviceConfigurable=".$deviceConfigurable."\n"; */

if ($apCloning == "") {
    $apCloning = "0"; 
    set("apcloning", $apCloning);
}
if ($fwUpgrade == "") {
    $fwUpgrade = "1"; 
    set("fwupgrade",$fwUpgrade);
}
if ($easyRoaming == "") {
    $easyRoaming = "1";
    set("easyroaming",$easyRoaming);
}
if ($safety360 == "") {
    $safety360 = "0"; 
    set("safety360", $safety360);
}
/* echo "#php apCloning=".$apCloning.", fwUpgrade=".$fwUpgrade.", easyRoaming=".$easyRoaming.", safety360=".$safety360."\n"; */

$rssxml =  $tempDirecory."/rss.xml";
/* version info */
if ($specVer!="1.0" && $specVer!="1.1") {
    anchor($OT_Path."/specversion");
    $specVerMajor = queryxml("major");
    $specVerMinor = queryxml("minor");
    $specVer = $specVerMajor.".".$specVerMinor;
}
else {
    $specVerMajor = "1";
    if ($specVer=="1.0") {
        $specVerMinor = "0";
    }
    if ($specVer=="1.1") {
        $specVerMinor = "1";
    }
}

/* apcloning info */
anchor($OT_Path."/apcloninginfo/connmaster");
$apCloningConnMasterMac    = queryxml("mac");
$apCloningConnMasterSeq    = queryxml("seq");
$apCloningConnMasterStatus = queryxml("status");
/* echo "#php specversion=".$specVer."\n"; */

/* System Basic Information */
/* basic info */
anchor("/sys");
$friendlyName = queryxml("friendlyname");
$friendlyNameValidate = queryxml("friendlyname_validate");
/* $presentationUrl = queryxml("presentationurl"); */
$manufacturer = queryxml("manufacturer");
$manufacturerUrl = queryxml("manufacturerurl");
$modelDescription = queryxml("modeldescription");
$modelName = queryxml("modelname");
$modelNumber = queryxml("modelnumber");
$modelUrl = queryxml("modelurl");
$udn = queryxml("udn");
if ($isCipher == "1") {
	$sysUserPassword = queryencry("user:1/password");
}
else {
	$sysUserPassword = queryxml("user:1/password");
}
$sysUserPasswordValidate = queryxml("user:1/password_validate");
$factoryDefault = queryxml("factorydefault");
$deviceType = queryxml("devicetype");
$routerenbl = queryxml("routerenbl");
/* Icon info used by APP */
anchor("/sys/icon:1");
$iconMimeType = queryxml("mimetype");
$iconWidth = queryxml("width");
$iconHeight = queryxml("height");
$iconDepth = queryxml("depth");
$iconUrl = queryxml("url");

/* LAN Information */
/* basic info */
$lanIP = queryxml("/runtime".$OT_Path."/lanip");
$lanWlanConcurrent = queryxml("/lan/wirelessconcurrent");

/* Uplink Information */
anchor("/uplink");
$upConfigurable = queryxml("configurable");
if ($upConfigurable == "") {
    $upConfigurable = "1"; 
    set("configurable", $upConfigurable);
}
$upHardwareSupport = queryxml("hardwaresupport");
$upHardwareType = queryxml("hardwaretype");
$upConnType = queryxml("conntype");
$upLoginUserName = queryxml("loginusername"); 
if ($isCipher == "1") {
	$upLoginPassword = queryencry("loginpassword");
}
else {
	/* $upLoginPassword = queryxml("loginpassword"); */
	$upLoginPassword = "";
}
$upLoginUserNameValidate = queryxml("loginusername_validate"); 
$upLoginPasswordValidate = queryxml("loginpassword_validate"); 
$upIp = queryxml("ip");
$upNetMask = queryxml("netmask");
$upGateway = queryxml("gateway");
$upDns = queryxml("dns");
if ($upHardwareType == "ADSL" || $upHardwareType == "VDSL" || $isDslWan == "1") {
    $upDslCountryName = queryxml("dsl/countryname");
    $upDslIspName = queryxml("dsl/ispname");
}
if ($upHardwareType == "WWanAPN" || $isApnWan == "1") {
    $upApnCountryName = queryxml("apn/countryname");
    $upApnIspName = queryxml("apn/ispname");
}
if ($upHardwareType == "WWan802.11" || $is80211Wan == "1") {
    $upWlanSurvey = queryxml("extendersite/survey");
    if ($upWlanSurvey=="") {
        $upWlanSurvey = "0";
    }
    $upWlanConnRefSiteBssid = queryxml("extendersite/connrefsitebssid");
    $upWlanFailOverBSSIDList = queryxml("extendersite/failoverbssidlist");
    $upWlanConnStatus = queryxml("extendersite/connstatus");
    $upWlanType = queryxml("extendersite/wirelesstype");
    $upWlanName = queryxml("extendersite/wirelessname");
	if ($isCipher == "1") {
		$upWlanEncryptionKey = queryencry("extendersite/encryptionkey");
	}
	else {
		$upWlanEncryptionKey = queryxml("extendersite/encryptionkey");
	}
}

/* Safety 360 Setting & Status */
if ($safety360 == "1")  {
    anchor("/safety360");
    $safety360Enbl = queryxml("safety360enbl");
    $safety360UrlEnbl = queryxml("urldetectenbl");
    $safety360DayNotifyEnbl = queryxml("safetynotifyenbl");
    $safety360ToolNotifyEnbl = queryxml("toolrecommendenbl");
    $safety360Cntr0 = queryxml("urlcounters/acceptcntr");
    $safety360Cntr1 = queryxml("urlcounters/blockcntrlist/cntr:1");
    $safety360Cntr2 = queryxml("urlcounters/blockcntrlist/cntr:2");
    $safety360Cntr3 = queryxml("urlcounters/blockcntrlist/cntr:3");
    $safety360Cntr4 = queryxml("urlcounters/blockcntrlist/cntr:4");
    $safety360Cntr5 = queryxml("urlcounters/blockcntrlist/cntr:5");
}

/* Firmware Info */
anchor("/fwupgradeinfo");
$deviceVendor = queryxml("vendor");
$deviceMac = queryxml("devicemac");
$hwVersion = queryxml("hwversion");
$fwRegionCode = queryxml("regioncode");
$fwVersion = queryxml("currfwversion");
$fwFileName = queryxml("currfwfilename");
$fwAccessKey = queryxml("accesskey");
/* $fwUpgradeUrl = queryxml("fwupgradeurl"); */

if ($specVer == "1.0") {
    anchor("/uplink");
    $upMTU = queryxml("mtu");
    if ($upHardwareType == "ADSL" || $upHardwareType == "VDSL" || $isDslWan == "1") {
        $upDslVpi = queryxml("dsl/vpi");
        $upDslVci = queryxml("dsl/vci");
        $upDslEncapMode = queryxml("dsl/encapsulationmode");
        if ($upDslVpi < 0) {
            $upDslVpi = "0";
            set("dsl/vpi", $upDslVpi);
        }
        if ($upDslVci <= 0) {
            $upDslVci = "35";
            set("dsl/vci", $upDslVci);
        }
        if ($upDslEncapMode == "") {
            $upDslEncapMode = "LLC";
            set("dsl/encapsulationmode", $upDslEncapMode);
        }
        if ($upMTU=="") {
            $upMTU = 1492;
            set("mtu", $upMTU);
        }
    }
    else {
        if ($upMTU=="") {
            $upMTU = 1500;
            set("mtu", $upMTU);
        }
    }
}

/* Start to generate rss.xml*/
/* used special string */
/* $SS0 = "echo \""; */
$SS0 = "";
if ($isOnlyApNode != "1") {
    $SS1 = $SS0."	";
    $SS2 = $SS0."		";
    $SS3 = $SS0."			";
    $SS4 = $SS0."				";
    $SS5 = $SS0."					";
    $SS6 = $SS0."						";
    $SS7 = $SS0."							";
}
else {
    $SS1 = $SS0;
    $SS2 = $SS0;
    $SS3 = $SS0;
    $SS4 = $SS0;
    $SS5 = $SS0;
    $SS6 = $SS0;
    $SS7 = $SS0;
}
/* $EE0 = "\" >> ".$rssxml."\n";
   $EE1 = "\" > ".$rssxml."\n"; */
$EE0 = "\n";
$EE1 = "\n";

/* generate rss xml contents */
if ($isOnlyApNode != "1") {
    /* echo $SS0."\<\?xml version=\\\"1.0\\\" encoding=\\\"utf-8\\\"\?\>".$EE1; */
    echo $SS0."\<\?xml version=\"1.0\" encoding=\"utf-8\"\?\>".$EE1;
    echo $SS0."<root>".$EE0;
}
else  {
    echo $SS0."<root>".$EE1;
}
echo $SS1."<specVersion>".$EE0;
echo $SS2."<major>".$specVerMajor."</major>".$EE0; 
echo $SS2."<minor>".$specVerMinor."</minor>".$EE0; 
echo $SS1."</specVersion>".$EE0;
echo $SS1."<Device>".$EE0;
if ($isOnlyApNode != "1") {
/*    echo $SS2."<Configurable>".$deviceConfigurable."</Configurable>".$EE0; */
    echo $SS2."<FactoryDefault>".$factoryDefault."</FactoryDefault>".$EE0;
    echo $SS2."<DeviceModel>".$modelNumber."</DeviceModel>".$EE0;
    echo $SS2."<DeviceName>".$friendlyName."</DeviceName>".$EE0;
    if ($specVer != "1.0") {
    	echo $SS2."<DeviceName_Validate>".$friendlyNameValidate."</DeviceName_Validate>".$EE0;
    }
    echo $SS2."<DevicePwd".$CipherTxt.">".$sysUserPassword."</DevicePwd>".$EE0;
    if ($specVer != "1.0") {
    	echo $SS2."<DevicePwd_Validate>".$sysUserPasswordValidate."</DevicePwd_Validate>".$EE0;
    }
    echo $SS2."<DeviceType>".$deviceType."</DeviceType>".$EE0;
    echo $SS2."<UpLinkConnection>".$EE0;
    echo $SS3."<Configurable>".$upConfigurable."</Configurable>".$EE0;
    if ($specVer != "1.0") {
        echo $SS3."<HardwareSupport>".$upHardwareSupport."</HardwareSupport>".$EE0;
    }
    if ($specVer == "1.0") {
        if ($upHardwareType == "ADSL" || $upHardwareType == "VDSL" || $isDslWan == "1") { 
            echo $SS3."<HardwareType>DSL</HardwareType>".$EE0;
        }
        else {
            echo $SS3."<HardwareType>Ethernet</HardwareType>".$EE0;
        }
        echo $SS3."<MTU>".$upMTU."</MTU>".$EE0;
    }
    else {
        echo $SS3."<HardwareType>".$upHardwareType."</HardwareType>".$EE0;
    }
    /* if HardwareType==ADSL or VDSL, provide <DSL> */
    if ($upHardwareType == "ADSL" || $upHardwareType == "VDSL" || $isDslWan == "1") {
        echo $SS3."<DSL>".$EE0;
        if ($specVer != "1.0") {
            echo $SS4."<CountryName>".$upDslCountryName."</CountryName>".$EE0;
            echo $SS4."<IspName>".$upDslIspName."</IspName>".$EE0;
        }
        else {
            echo $SS4."<VPI>".$upDslVpi."</VPI>".$EE0;
            echo $SS4."<VCI>".$upDslVci."</VCI>".$EE0;
            echo $SS4."<EncapsulationMode>".$upDslEncapMode."</EncapsulationMode>".$EE0;
        }
        echo $SS3."</DSL>".$EE0;
    }
    if ($specVer != "1.0") {
        /* if HardwareType== WWanAPN, provide <APN> */
        if ($upHardwareType == "WWanAPN" || $isApnWan == "1") {
            echo $SS3."<APN>".$EE0;
            echo $SS4."<CountryName>".$upApnCountryName."</CountryName>".$EE0;
            echo $SS4."<IspName>".$upApnIspName."</IspName>".$EE0;
            echo $SS3."</APN>".$EE0;
        }
        /* if HardwareType==WWan802.11, provide <ExtenderSite> */
        if ($upHardwareType == "WWan802.11" || $is80211Wan == "1") {
            echo $SS3."<ExtenderSite>".$EE0;
            echo $SS4."<Survey>".$upWlanSurvey."</Survey>".$EE0;
            echo $SS4."<ConnRefSiteBSSID>".$upWlanConnRefSiteBssid."</ConnRefSiteBSSID>".$EE0;
            echo $SS4."<FailOverBSSIDList>".$upWlanFailOverBSSIDList."</FailOverBSSIDList>".$EE0;
            echo $SS4."<ConnStatus>".$upWlanConnStatus."</ConnStatus>".$EE0;
            echo $SS4."<WirelessType>".$upWlanType."</WirelessType>".$EE0;
            echo $SS4."<WirelessName>".$upWlanName."</WirelessName>".$EE0;
            echo $SS4."<EncryptionKey".$CipherTxt.">".$upWlanEncryptionKey."</EncryptionKey>".$EE0;
            echo $SS3."</ExtenderSite>".$EE0;
        }
    }
    echo $SS3."<ConnType>".$upConnType."</ConnType>".$EE0;
    if ($specVer != "1.0") {
        echo $SS3."<LoginUserName>".$upLoginUserName."</LoginUserName>".$EE0;
        echo $SS3."<LoginUserName_Validate>".$upLoginUserNameValidate."</LoginUserName_Validate>".$EE0;
		if ($isCipher == "1") {
			echo $SS3."<LoginPassword".$CipherTxt.">".$upLoginPassword."</LoginPassword>".$EE0;
		}
		else {
			echo $SS3."<LoginPassword></LoginPassword>".$EE0;
        }
        echo $SS3."<LoginPassword_Validate>".$upLoginPasswordValidate."</LoginPassword_Validate>".$EE0;
    }
    else {
        echo $SS3."<PppUserName>".$upLoginUserName."</PppUserName>".$EE0;
        echo $SS3."<PppPassword></PppPassword>".$EE0;
    }
    echo $SS3."<IP>".$upIp."</IP>".$EE0;
    echo $SS3."<NetMask>".$upNetMask."</NetMask>".$EE0;
    echo $SS3."<Gateway>".$upGateway."</Gateway>".$EE0;
    echo $SS3."<DNS>".$upDns."</DNS>".$EE0;
    echo $SS2."</UpLinkConnection>".$EE0;
    echo $SS2."<RouterEnbl>".$routerenbl."</RouterEnbl>".$EE0;
    if ($specVer == "1.0")  {
        $upExtenderEnbl = queryxml("/uplink/extenderenbl");
        if ($upExtenderEnbl == "") {
            if ($upHardwareType == "WWan802.11") {
                $upExtenderEnbl = "3";
            }
            else {
                $upExtenderEnbl = "2";
            }
            set("/uplink/extenderenbl", $upExtenderEnbl);
        }
        echo $SS2."<ExtenderEnbl>".$upExtenderEnbl."</ExtenderEnbl>".$EE0;
    }
    echo $SS2."<WirelessConcurrent>".$lanWlanConcurrent."</WirelessConcurrent>".$EE0;
    if ($specVer == "1.0") {
    	if ($upHardwareType == "WWan802.11" || $is80211Wan == "1") {
	        echo $SS2."<ExtenderSite>".$EE0;
	        echo $SS3."<Survey>".$upWlanSurvey."</Survey>".$EE0;
	        echo $SS3."<ConnRefSiteBSSID>".$upWlanConnRefSiteBssid."</ConnRefSiteBSSID>".$EE0;
	        echo $SS3."<ConnStatus>".$upWlanConnStatus."</ConnStatus>".$EE0;
	        echo $SS3."<WirelessType>".$upWlanType."</WirelessType>".$EE0;
	        echo $SS3."<WirelessName>".$upWlanName."</WirelessName>".$EE0;
	        echo $SS3."<EncryptionKey>".$upWlanEncryptionKey."</EncryptionKey>".$EE0;
	        echo $SS2."</ExtenderSite>".$EE0;
	    }
    }
}
/* echo "#php.8\n"; */
/* if Device support 2.4G Wireless, provide <Wireless2.4> */
anchor("/lan/wireless2.4:1");	
$lanWlan24Enbl = queryxml("wlenbl");
if ($lanWlan24Enbl!="0" && $lanWlan24Enbl!="1" && $lanWlan24Enbl!="2" && $lanWlan24Enbl!="3")  {
    $lanWlan24Enbl = "";
}
if ($lanWlan24Enbl != "")  {
    $lanWlan24Configurable = queryxml("Configurable");
    if ($lanWlan24Configurable == "") {
        $lanWlan24Configurable = "1"; 
    }
    $lanWlan24Schedule = queryxml("wlschedule");
    $lanWlan24Name = queryxml("wirelessname");
    $lanWlan24NameValidate = queryxml("wirelessname_validate");
    $lanWlan24BSSID = queryxml("wirelessbssid");
	$lanWlan24APList = queryxml("/runtime/apcloninginfo/failoverbssid_w2.4");
    $lanWlan24EncryptionMode = queryxml("encryptionmode");
    $lanWlan24EncryptionType = queryxml("encryptiontype");
	if ($isCipher == "1") {
		$lanWlan24EncryptionKey = queryencry("encryptionkey");
	}
	else {
	    $lanWlan24EncryptionKey = queryxml("encryptionkey");
	}
    $lanWlan24EncryptionKeyValidate = queryxml("encryptionkey_validate");
    $lanWlan24Country = queryxml("country");
    $lanWlan24ChannelList = queryxml("channellist");
    $lanWlan24Channel = queryxml("channel");
    $lanWlan24ChannelUsing = queryxml("channelusing");
    $lanWlan24AcEnbl = queryxml("accesscontrol/wlacenbl");
    $lanWlan24AcMode = queryxml("accesscontrol/wlacmode");
    if ($lanWlan24AcEnbl!="0" && $lanWlan24AcEnbl!="1") {
        $lanWlan24AcEnbl = "2";
        set("accesscontrol/wlacenbl", $lanWlan24AcEnbl);
    }
    if ($lanWlan24AcMode=="") {
        $lanWlan24AcMode = "0";
        set("accesscontrol/wlacmode", $lanWlan24AcMode);
    }
    echo $SS2."<Wireless2.4>".$EE0;
/*        if ($isOnlyApNode != "1") {
         echo $SS3."<Configurable>".$lanWlan24Configurable."</Configurable>".$EE0;
    } */
    echo $SS3."<WlEnbl>".$lanWlan24Enbl."</WlEnbl>".$EE0;
    echo $SS3."<WlSchedule>".$lanWlan24Schedule."</WlSchedule>".$EE0;
    if ($specVer != "1.0")  {
        echo $SS3."<WirelessBSSID>".$lanWlan24BSSID."</WirelessBSSID>".$EE0;
    }
    echo $SS3."<WirelessName>".$lanWlan24Name."</WirelessName>".$EE0;
    if ($specVer != "1.0")  {
    	echo $SS3."<WirelessName_Validate>".$lanWlan24NameValidate."</WirelessName_Validate>".$EE0;
    }
    echo $SS3."<EncryptionMode>".$lanWlan24EncryptionMode."</EncryptionMode>".$EE0;
    echo $SS3."<EncryptionKey".$CipherTxt.">".$lanWlan24EncryptionKey."</EncryptionKey>".$EE0;
    if ($specVer != "1.0")  {
	    echo $SS3."<EncryptionKey_Validate>".$lanWlan24EncryptionKeyValidate."</EncryptionKey_Validate>".$EE0;
        echo $SS3."<EncryptionType>".$lanWlan24EncryptionType."</EncryptionType>".$EE0;
    	echo $SS3."<Country>".$lanWlan24Country."</Country>".$EE0;
    	echo $SS3."<ChannelList>".$lanWlan24ChannelList."</ChannelList>".$EE0;
    	if ($isOnlyApNode != "1") {
    	    echo $SS3."<Channel>".$lanWlan24Channel."</Channel>".$EE0;
    	}
    	else {
        	echo $SS3."<APSiteBSSIDList>".$lanWlan24APList."</APSiteBSSIDList>".$EE0;
    		if ($lanWlan24Channel != "0") {
    	    	echo $SS3."<Channel>".$lanWlan24Channel."</Channel>".$EE0;
    	    }
    	    else {
    	    	echo $SS3."<Channel>".$lanWlan24ChannelUsing."</Channel>".$EE0;
    	    }
    	}
    	echo $SS3."<AccessControl>".$EE0;
      	echo $SS4."<WlAcEnbl>".$lanWlan24AcEnbl."</WlAcEnbl>".$EE0;
        if ($lanWlan24AcEnbl!="2") {
        	echo $SS4."<WlAcMode>".$lanWlan24AcMode."</WlAcMode>".$EE0;
        	echo $SS4."<WlAcLists>".$EE0;
            for("/lan/wireless2.4:1/accesscontrol/acrule") {	
                $lanWlanAdvAcActive = queryxml("active");
                if ($lanWlanAdvAcActive=="") {
                    $lanWlanAdvAcActive = "0";
                    set("active", $lanWlanAdvAcActive);
                }
                $lanWlanAdvAcMac = queryxml("mac");
                if ($lanWlanAdvAcMac != "") {
                    echo $SS5."<AcRule>".$EE0;
            		echo $SS6."<Active>".$lanWlanAdvAcActive."</Active>".$EE0;
            		echo $SS6."<MAC>".$lanWlanAdvAcMac."</MAC>".$EE0;
            		echo $SS5."</AcRule>".$EE0;
        	    }
            }
        	echo $SS4."</WlAcLists>".$EE0;
        }
    	echo $SS3."</AccessControl>".$EE0;
    }
    echo $SS2."</Wireless2.4>".$EE0;
}
/* echo "#php.9\n"; */

/* if Device support 5G Wireless, provide <Wireless5> */
anchor("/lan/wireless5:1");	
$lanWlan5Enbl = queryxml("wlenbl");
if ($lanWlan5Enbl!="0" && $lanWlan5Enbl!="1" && $lanWlan5Enbl!="2" && $lanWlan5Enbl!="3")  {
    $lanWlan5Enbl = "";
}
if ($lanWlan5Enbl !=  "")  {
    $lanWlan5Configurable = queryxml("Configurable");
    if ($lanWlan5Configurable == "") {
        $lanWlan5Configurable = "1"; 
    }
    $lanWlan5Schedule = queryxml("wlschedule");
    $lanWlan5BSSID = queryxml("wirelessbssid");
	$lanWlan5APList = queryxml("/runtime/apcloninginfo/failoverbssid_w5");
    $lanWlan5Name = queryxml("wirelessname");
    $lanWlan5NameValidate = queryxml("wirelessname_validate");
    $lanWlan5EncryptionMode = queryxml("encryptionmode");
    $lanWlan5EncryptionType = queryxml("encryptiontype");
	if ($isCipher == "1") {
		$lanWlan5EncryptionKey = queryencry("encryptionkey");
	}
	else {
	    $lanWlan5EncryptionKey = queryxml("encryptionkey");
	}
	$lanWlan5EncryptionKeyValidate = queryxml("encryptionkey_validate");
    $lanWlan5Country = queryxml("country");
    $lanWlan5ChannelList = queryxml("channellist");
    $lanWlan5Channel = queryxml("channel");
    $lanWlan5ChannelUsing = queryxml("channelusing");
    $lanWlan5AcEnbl = queryxml("accesscontrol/wlacenbl");
    $lanWlan5AcMode = queryxml("accesscontrol/wlacmode");
    if ($lanWlan5AcEnbl!="0" && $lanWlan5AcEnbl!="1") {
        $lanWlan5AcEnbl = "2";
        set("accesscontrol/wlacenbl", $lanWlan5AcEnbl);
    }
    if ($lanWlan5AcMode=="") {
        $lanWlan5AcMode = "0";
        set("accesscontrol/wlacmode", $lanWlan5AcMode);
    }
    echo $SS2."<Wireless5>".$EE0;
/*         if ($isOnlyApNode != "1") {
         echo $SS3."<Configurable>".$lanWlan5Configurable."</Configurable>".$EE0;
     } */
    echo $SS3."<WlEnbl>".$lanWlan5Enbl."</WlEnbl>".$EE0;
    echo $SS3."<WlSchedule>".$lanWlan5Schedule."</WlSchedule>".$EE0;
    echo $SS3."<WirelessBSSID>".$lanWlan5BSSID."</WirelessBSSID>".$EE0;
    echo $SS3."<WirelessName>".$lanWlan5Name."</WirelessName>".$EE0;
    if ($specVer != "1.0")  {
    	echo $SS3."<WirelessName_Validate>".$lanWlan5NameValidate."</WirelessName_Validate>".$EE0;
    }
    echo $SS3."<EncryptionMode>".$lanWlan5EncryptionMode."</EncryptionMode>".$EE0;
    echo $SS3."<EncryptionKey".$CipherTxt.">".$lanWlan5EncryptionKey."</EncryptionKey>".$EE0;
    if ($specVer != "1.0")  {
	    echo $SS3."<EncryptionKey_Validate>".$lanWlan5EncryptionKeyValidate."</EncryptionKey_Validate>".$EE0;
        echo $SS3."<EncryptionType>".$lanWlan5EncryptionType."</EncryptionType>".$EE0;
    	echo $SS3."<Country>".$lanWlan5Country."</Country>".$EE0;
    	echo $SS3."<ChannelList>".$lanWlan5ChannelList."</ChannelList>".$EE0;
    	if ($isOnlyApNode != "1") {
    	    echo $SS3."<Channel>".$lanWlan5Channel."</Channel>".$EE0;
    	}
    	else {
        	echo $SS3."<APSiteBSSIDList>".$lanWlan5APList."</APSiteBSSIDList>".$EE0;
    		if ($lanWlan5Channel != "0") {
    	    	echo $SS3."<Channel>".$lanWlan5Channel."</Channel>".$EE0;
    	    }
    	    else {
    	    	echo $SS3."<Channel>".$lanWlan5ChannelUsing."</Channel>".$EE0;
    	    }
    	}
    	echo $SS3."<AccessControl>".$EE0;
      	echo $SS4."<WlAcEnbl>".$lanWlan5AcEnbl."</WlAcEnbl>".$EE0;
        if ($lanWlan5AcEnbl!="2") {
        	echo $SS4."<WlAcMode>".$lanWlan5AcMode."</WlAcMode>".$EE0;
        	echo $SS4."<WlAcLists>".$EE0;
            for("/lan/wireless5:1/accesscontrol/acrule") {	
                $lanWlanAdvAcActive = queryxml("active");
                if ($lanWlanAdvAcActive=="") {
                    $lanWlanAdvAcActive = "0";
                    set("active", $lanWlanAdvAcActive);
                }
                $lanWlanAdvAcMac = queryxml("mac");
                if ($lanWlanAdvAcMac != "") {
                    echo $SS5."<AcRule>".$EE0;
            		echo $SS6."<Active>".$lanWlanAdvAcActive."</Active>".$EE0;
            		echo $SS6."<MAC>".$lanWlanAdvAcMac."</MAC>".$EE0;
            		echo $SS5."</AcRule>".$EE0;
                }
            }
        	echo $SS4."</WlAcLists>".$EE0;
        }
    	echo $SS3."</AccessControl>".$EE0;
    }
    echo $SS2."</Wireless5>".$EE0;
}
/* echo "#php.10\n"; */

/* If device support EasyRoaming, provide <EasyRoaming>  */ 
if ($easyRoaming != "0" && $easyRoaming != "2" && $specVer != "1.0") {
   echo $SS2."<EasyRoaming".$CipherTxt.">".$easyRoamingInfo."</EasyRoaming>".$EE0;
}
if ($isOnlyApNode != "1") {
    /* if device support 360 Safety feature, provide <Safety360> block*/
    if ($safety360 == "1")  {
        echo $SS2."<Safety360>".$EE0;
        echo $SS3."<Safety360Enbl>".$safety360Enbl."</Safety360Enbl>".$EE0;
        echo $SS3."<UrlDetectEnbl>".$safety360UrlEnbl."</UrlDetectEnbl>".$EE0;
        echo $SS3."<SafetyNotifyEnbl>".$safety360DayNotifyEnbl."</SafetyNotifyEnbl>".$EE0;
        echo $SS3."<ToolRecommendEnbl>".$safety360ToolNotifyEnbl."</ToolRecommendEnbl>".$EE0;
        echo $SS2."</Safety360>".$EE0;
    }
}
echo $SS1."</Device>".$EE0;
if ($isCipher == "1") {
    echo $SS1."<EncryptInfo>".$EE0;
	echo $SS2."<RssEncryptKey>".$rssEncryptKey."</RssEncryptKey>".$EE0;
    echo $SS1."</EncryptInfo>".$EE0;
}
if ($isOnlyApNode != "1") {
    /* if device support FW controller function, provide <FwUpgradeInfo> */
    echo $SS1."<FwUpgradeInfo>".$EE0;
    echo $SS2."<Vendor>".$deviceVendor."</Vendor>".$EE0;
    echo $SS2."<ModelName>".$modelNumber."</ModelName>".$EE0;
    echo $SS2."<DeviceMAC>".$deviceMac."</DeviceMAC>".$EE0;
    echo $SS2."<HwVersion>".$hwVersion."</HwVersion>".$EE0;
    echo $SS2."<RegionCode>".$fwRegionCode."</RegionCode>".$EE0;
    echo $SS2."<CurrFwVersion>".$fwVersion."</CurrFwVersion>".$EE0;
    echo $SS2."<CurrFwFileName>".$fwFileName."</CurrFwFileName>".$EE0;
    echo $SS2."<AccessKey>".$fwAccessKey."</AccessKey>".$EE0;
    if ($fwUpgrade == "1") {
        echo $SS2."<FwUpgradeURL>".$fwUpgradeUrl."</FwUpgradeURL>".$EE0;
    }
    echo $SS1."</FwUpgradeInfo>".$EE0;
/* echo "#php.11\n";  */  
    /* if DeviceType support Extender or Repeater, provide <WirelessSiteSurvey> */
    if ($upHardwareType == "WWan802.11" || $is80211Wan == "1") {
        echo $SS1."<WirelessSiteSurvey>".$EE0;
        /* Survey==1, waiting survey result, so empty the  <WirelessSiteSurvey> block
           Survey==0, survey action was done, response survey results */ 
        if ($upWlanSurvey == "0")  {
            $i = 1;
            for("/wirelesssitesurvey/site") {	
                $siteSurveyWlanType = queryxml("wirelesstype");
                $siteSurveyWlanBssid = queryxml("bssid");
                $siteSurveyWlanName = queryxml("wirelessname");
                $siteSurveyWlanEncryptionMode = queryxml("encryptionmode");
                $siteSurveyWlanStrength = queryxml("wirelessstrength");
                if ($siteSurveyWlanBssid != "") {
                    echo $SS2."<Site>".$EE0;
                    echo $SS3."<WirelessType>".$siteSurveyWlanType."</WirelessType>".$EE0;
                    echo $SS3."<BSSID>".$siteSurveyWlanBssid."</BSSID>".$EE0;
                    echo $SS3."<WirelessName>".$siteSurveyWlanName."</WirelessName>".$EE0;
                    echo $SS3."<EncryptionMode>".$siteSurveyWlanEncryptionMode."</EncryptionMode>".$EE0;
                    echo $SS3."<WirelessStrength>".$siteSurveyWlanStrength."</WirelessStrength>".$EE0;
                    echo $SS2."</Site>".$EE0;
                }
                $i++;
            }
        }
        echo $SS1."</WirelessSiteSurvey>".$EE0;
    }
    if ($specVer != "1.0") {
        /* If device support AP Cloning feature, provide <APCloningInfo>  */
        if ($apCloning == "1" || $apCloning == "2") {
            echo $SS1."<APCloningInfo>".$EE0;
            /* if CloningMode==Slave, provide <ConnMasterInfo> */
            if ($apCloning == "2")  {
                echo $SS2."<CloningMode>Slave</CloningMode>".$EE0;
                echo $SS2."<ConnMasterInfo>".$EE0;
                echo $SS3."<MAC>".$apCloningConnMasterMac."</MAC>".$EE0;
                echo $SS3."<ConnStatus>".$apCloningConnMasterStatus."</ConnStatus>".$EE0;
                echo $SS2."</ConnMasterInfo>".$EE0;
            }
        	/* If CloningMode==Master, provide <LivingSlaveList>  */
            if ($apCloning == "1")  {
                echo $SS2."<CloningMode>Master</CloningMode>".$EE0;
                echo $SS2."<LivingSlaveList>".$EE0;
                for("/runtime/apcloninginfo/livingslave") {	
                    $livingSlaveIp = queryxml("ip");
                    $livingSlaveSid = queryxml("sid");
                    if ($livingSlaveIp != "") {
                        echo $SS3."<Slave>".$EE0;
                        echo $SS4."<IP>".$livingSlaveIp."</IP>".$EE0;
                        echo $SS4."<SID>".$livingSlaveSid."</SID>".$EE0;
                        echo $SS3."</Slave>".$EE0;
                    }
                }    
                echo $SS2."</LivingSlaveList>".$EE0;
            }    
            echo $SS1."</APCloningInfo>".$EE0;
        }
    }
/* echo "#php.12\n"; */
    /* if device support 360 safety feature, provide <Safety360Info> block */
    if ($safety360 == "1")  {
        echo $SS1."<Safety360Info>".$EE0;
        echo $SS2."<UrlCounters>".$EE0;
        echo $SS3."<AcceptCntr>".$safety360Cntr0."</AcceptCntr>".$EE0;
        echo $SS3."<BlockCntrList>".$EE0;
        echo $SS4."<Cntr-1>".$safety360Cntr1."</Cntr-1>".$EE0;
        echo $SS4."<Cntr-2>".$safety360Cntr2."</Cntr-2>".$EE0;
        echo $SS4."<Cntr-3>".$safety360Cntr3."</Cntr-3>".$EE0;
        echo $SS4."<Cntr-4>".$safety360Cntr4."</Cntr-4>".$EE0;
        echo $SS4."<Cntr-5>".$safety360Cntr5."</Cntr-5>".$EE0;
        echo $SS3."</BlockCntrList>".$EE0;
        echo $SS2."</UrlCounters>".$EE0;
        echo $SS1."</Safety360Info>".$EE0;
    }
    /* if HardwareSupport support ADSL or VDSL, MUST provide <DslIspList> */
    if ($upHardwareType == "ADSL" || $upHardwareType == "VDSL" || $isDslWan == "1") {
        echo $SS1."<DslIspList>".$EE0;
        $i = 1;
        for("/dslisplist/country")  {	
            $ispCountryName = queryxml("countryname");
            if ($specVer == "1.0") {
                $ispManual = queryxml("manual");
                if ($ispManual=="") {
                    $ispManual = "0";
                    set("manual", $ispManual);
                }
                echo $SS2."<Manual>".$ispManual."</Manual>".$EE0;
            }
            if ($ispCountryName != "") {
                echo $SS2."<Country>".$EE0;
                echo $SS3."<CountryName>".$ispCountryName."</CountryName>".$EE0;
                for("/dslisplist/country:".$i."/isp")  {
                    $ispListIspName = queryxml("ispname");
                    $ispListIspConnType = queryxml("conntype");
                    $ispListIspDescription = queryxml("description");
                    if ($ispListIspName != "") {
                        echo $SS3."<ISP>".$EE0;
                        echo $SS4."<IspName>".$ispListIspName."</IspName>".$EE0;
						/* Bridge as the dhcp mode, becasue of no need to config */
						if ($ispListIspConnType == "Bridge") {
                        	echo $SS4."<ConnType>DHCP</ConnType>".$EE0;
						} else {
                        	echo $SS4."<ConnType>".$ispListIspConnType."</ConnType>".$EE0;
						}
                        echo $SS4."<Description>".$ispListIspDescription."</Description>".$EE0;
                        if ($specVer == "1.0") {
                            $ispListIspMTU = queryxml("mtu");
                            $ispListDslVpi = queryxml("dsl/vpi");
                            $ispListDslVci = queryxml("dsl/vci");
                            $ispListDslEncapMode = queryxml("dsl/encapsulationmode");
                            echo $SS4."<MTU>".$ispListIspMTU."</MTU>".$EE0;
                            echo $SS4."<DSL>".$EE0;
                            echo $SS5."<VPI>".$ispListDslVpi."</VPI>".$EE0;
                            echo $SS5."<VCI>".$ispListDslVci."</VCI>".$EE0;
                            echo $SS5."<EncapsulationMode>".$ispListDslEncapMode."</EncapsulationMode>".$EE0;
                            echo $SS4."</DSL>".$EE0;
                        }
                        echo $SS3."</ISP>".$EE0;
                    }
                }
                echo $SS2."</Country>".$EE0;
            }
            $i++;
        }    
        echo $SS1."</DslIspList>".$EE0;
    }
/* echo "#php.13\n"; */
    /* if HardwareSupport support WWanAPN, MUST provide <ApnIspList> */
    if ($specVer != "1.0") {
	    if ($upHardwareType == "WWanAPN" || $isApnWan == "1") {
	        echo $SS1."<ApnIspList>".$EE0;
	        $i = 1;
	        for("/apnisplist/country")  {
	            $ispCountryName = queryxml("countryname");
	            if ($ispCountryName != "") {
	                echo $SS2."<Country>".$EE0;
	                echo $SS3."<CountryName>".$ispCountryName."</CountryName>".$EE0;
	                for("/apnisplist/country:".$i."/isp") {	
	                    $ispListIspName = queryxml("ispname");
	                    $ispListIspConnType = queryxml("conntype");
	                    $ispListIspDescription = queryxml("description");
	                    if ($ispListIspName != "") {
	                        echo $SS3."<ISP>".$EE0;
	                        echo $SS4."<IspName>".$ispListIspName."</IspName>".$EE0;
	                        echo $SS4."<ConnType>".$ispListIspConnType."</ConnType>".$EE0;
	                        echo $SS4."<Description>".$ispListIspDescription."</Description>".$EE0;
	                        echo $SS3."</ISP>".$EE0;
	                    }
	                }
	                echo $SS2."</Country>".$EE0;
	            }
	            $i++;
	        }    
	        echo $SS1."</ApnIspList>".$EE0;
	    }
	}
}

echo $SS0."</root>".$EE0;

?>
