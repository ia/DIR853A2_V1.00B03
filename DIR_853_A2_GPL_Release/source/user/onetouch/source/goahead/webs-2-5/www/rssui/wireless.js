var on_imgname="ios_kai.png";
var off_imgname="ios_guan.png";

//获得页面内容
function init_wireless(){
	//document.getElementById("2.4g_enable1").innerHTML=(wireless_enable=="1" || wireless_enable=="3")?m_open:m_close;
	document.getElementById("wireless2_onoff").src=(wireless_enable=="1" || wireless_enable=="3")?"img/ios_kai.png":"img/ios_guan.png";
	if(wireless_encryptionmode=="WEP" || wireless_encryptionmode=="None")
		document.getElementById("2.4g_option").selectedIndex=0;
	else if(wireless_encryptionmode=="WPA")
		document.getElementById("2.4g_option").selectedIndex=2;
	else if(wireless_encryptionmode=="WPA2")
		document.getElementById("2.4g_option").selectedIndex=1;
	else
		document.getElementById("2.4g_option").selectedIndex=2;

	/*if(document.getElementById("2.4g_enable1").innerHTML==m_open)
		document.getElementById("wireless_2").style.display="block";
	else
		document.getElementById("wireless_2").style.display="none";*/
	if((document.getElementById("wireless2_onoff").src).indexOf(on_imgname)>0)
		document.getElementById("wireless_2").style.display="block";
	else if((document.getElementById("wireless2_onoff").src).indexOf(off_imgname)>0)
		document.getElementById("wireless_2").style.display="none";

	//document.getElementById("5g_enable1").innerHTML=(wireless_enable2=="1" || wireless_enable2=="3")?m_open:m_close;
	document.getElementById("wireless5_onoff").src=(wireless_enable2=="1" || wireless_enable2=="3")?"img/ios_kai.png":"img/ios_guan.png";
	if(wireless_encryptionmode2=="WEP" || wireless_encryptionmode2=="None")
		document.getElementById("5g_option").selectedIndex=0;
	else if(wireless_encryptionmode2=="WPA")
		document.getElementById("5g_option").selectedIndex=2;
	else if(wireless_encryptionmode2=="WPA2")
		document.getElementById("5g_option").selectedIndex=1;
	else 
		document.getElementById("5g_option").selectedIndex=2;

	/*if(document.getElementById("5g_enable1").innerHTML==m_open)
		document.getElementById("wireless_5").style.display="block";
	else
		document.getElementById("wireless_5").style.display="none";*/
	if((document.getElementById("wireless5_onoff").src).indexOf(on_imgname)>0)
		document.getElementById("wireless_5").style.display="block";
	else if((document.getElementById("wireless5_onoff").src).indexOf(off_imgname)>0)
		document.getElementById("wireless_5").style.display="none";

	if(factory_flag==0)
	{
		document.getElementById("rss_Wl2_WirelessName").value=wireless_ssid;
		document.getElementById("rss_Wl2_EncryptionKey").value=(wireless_pwd!="")?wireless_pwd:"";
		document.getElementById("rss_Wl5_WirelessName").value=wireless_ssid2;
		document.getElementById("rss_Wl5_EncryptionKey").value=(wireless_pwd2!="")?wireless_pwd2:"";
	}
	else if(factory_flag==1)
	{
		document.getElementById("rss_Wl2_EncryptionKey").value=generateMixed(10);
		document.getElementById("rss_Wl5_EncryptionKey").value=generateMixed(10);
		mac_count=device_mac.split(":");
		data="";
		for(i=3;i<mac_count.length;i++)
		{
			data+=mac_count[i];
		}
		document.getElementById("rss_Wl2_WirelessName").value="dlink-"+data.toLowerCase();
		document.getElementById("rss_Wl5_WirelessName").value="dlink-"+data.toLowerCase()+" 5G";
		document.getElementById("2.4g_option").selectedIndex=2;
		document.getElementById("5g_option").selectedIndex=2;
	}
	changmode();
	changmode2();
	if(wireless_enable == "2")
	{
		//document.getElementById("2.4g_enable1").innerHTML=m_close;
		//document.getElementById("wireless2_onoff").src="img/ios_guan.png";
		//document.getElementById("wireless_2").style.display="none";
		document.getElementById("total_wireless24").style.display="none";
	}
	if(wireless_enable2 == "2")
	{
		//document.getElementById("5g_enable1").innerHTML=m_close;
		//document.getElementById("wireless5_onoff").src="img/ios_guan.png";
		//document.getElementById("wireless_5").style.display="none";
		document.getElementById("total_wireless5").style.display="none";
	}
	//setTimeout("check_key()",5000);
}  

function check_wireless()
{
	var flag=true;
	var count=2;
	if(wireless_enable != "2")
	{
		if(document.getElementById("2.4g_option").selectedIndex != 0)
		{
			if(document.getElementById("rss_Wl2_EncryptionKey").value.length<8||document.getElementById("rss_Wl2_EncryptionKey").value.length>64)
			{
				document.getElementById("rss_Wl2_EncryptionKey_check").value=m_error_length;
				count--;
			}
			else
			{
				document.getElementById("rss_Wl2_EncryptionKey_check").value="";
			}
		}
	}
	if(wireless_enable2 != "2")
	{
		if(document.getElementById("5g_option").selectedIndex != 0)
		{
			if(document.getElementById("rss_Wl5_EncryptionKey").value.length<8||document.getElementById("rss_Wl5_EncryptionKey").value.length>64)
			{
				document.getElementById("rss_Wl5_EncryptionKey_check").value=m_error_length;
				count--;
			}
			else
			{
				document.getElementById("rss_Wl5_EncryptionKey_check").value="";
			}
		}
	}
	if(count==2)
		return true;
	else
		return false;
}

function apply_wireless()
{
	//document.getElementById("2.4g_enable").value=(document.getElementById("2.4g_enable1").innerHTML==m_open)?"1":"0";
	//document.getElementById("5g_enable").value=(document.getElementById("5g_enable1").innerHTML==m_open)?"1":"0";
	document.getElementById("2.4g_enable").value=((document.getElementById("wireless2_onoff").src).indexOf(on_imgname)>0)?"1":"0";
	document.getElementById("5g_enable").value=((document.getElementById("wireless5_onoff").src).indexOf(on_imgname)>0)?"1":"0";

	var flag=true;
	flag=check_wireless();
	if(flag==false)
		return;
	
	if(wireless_enable != "2")
	{
		if(wireless_enable == "3")
			txt+="&rss_Wl2_WlEnbl=3";
		else
			txt+="&rss_Wl2_WlEnbl="+(document.getElementById("2.4g_enable").value);
		txt+="&rss_Wl2_WirelessName="+subHtmAsc(document.getElementById("rss_Wl2_WirelessName").value);
		txt+="&rss_Wl2_EncryptionMode="+(document.getElementById("2.4g_option").value);
		txt+="&rss_Wl2_EncryptionKey="+subHtmAsc(RssDataEncode(document.getElementById("rss_Wl2_EncryptionKey").value));
		if(document.getElementById("2.4g_option").value == m_wireless_mode_none)
		{
			if(wireless_encryptionype!="" || wireless_encryptionype!="AUTO")
				txt+="&rss_Wl2_EncryptionType=AUTO";
			else
				txt+=wireless_encryptionype;
		}
		else if(document.getElementById("2.4g_option").value == m_wireless_mode_wep)
		{
			if(wireless_encryptionype!="AUTO" || wireless_encryptionype!="OPEN" || wireless_encryptionype!="SHARED")
				txt+="&rss_Wl2_EncryptionType=AUTO";
			else
				txt+=wireless_encryptionype;
		}
		else if(document.getElementById("2.4g_option").value == m_wireless_mode_wpa || document.getElementById("2.4g_option").value == m_wireless_mode_wpa2 || document.getElementById("2.4g_option").value == m_wireless_mode_wpawpa2)
		{
			if(wireless_encryptionype!="AUTO" || wireless_encryptionype!="TKIP" || wireless_encryptionype!="AES")
				txt+="&rss_Wl2_EncryptionType=AUTO";
			else
				txt+=wireless_encryptionype;
		}
	}

	if(wireless_enable2 != "2")
	{
		if(wireless_enable2 == "3")
			txt+="&rss_Wl5_WlEnbl=3";
		else
			txt+="&rss_Wl5_WlEnbl="+(document.getElementById("5g_enable").value);
		txt+="&rss_Wl5_WirelessName="+subHtmAsc(document.getElementById("rss_Wl5_WirelessName").value);
		txt+="&rss_Wl5_EncryptionMode="+(document.getElementById("5g_option").value);
		txt+="&rss_Wl5_EncryptionKey="+subHtmAsc(RssDataEncode(document.getElementById("rss_Wl5_EncryptionKey").value));
		if(document.getElementById("5g_option").value == m_wireless_mode_none)
		{
			if(wireless_encryptionype2!="" || wireless_encryptionype2!="AUTO")
				txt+="&rss_Wl5_EncryptionType=AUTO";
			else
				txt+=wireless_encryptionype2;
		}
		else if(document.getElementById("5g_option").value == m_wireless_mode_wep)
		{
			if(wireless_encryptionype2!="AUTO" || wireless_encryptionype2!="OPEN" || wireless_encryptionype2!="SHARED")
				txt+="&rss_Wl5_EncryptionType=AUTO";
			else
				txt+=wireless_encryptionype2;
		}
		else if(document.getElementById("5g_option").value == m_wireless_mode_wpa || document.getElementById("5g_option").value == m_wireless_mode_wpa2 || document.getElementById("5g_option").value == m_wireless_mode_wpawpa2)
		{
			if(wireless_encryptionype2!="AUTO" || wireless_encryptionype2!="TKIP" || wireless_encryptionype2!="AES")
				txt+="&rss_Wl5_EncryptionType=AUTO";
			else
				txt+=wireless_encryptionype2;
		}
	}
	//txt+="&rss_FactoryDefault=0";
	//location.href="finish.asp?"+txt;
	location.href="finish.asp?lang="+langCode2+"?"+txt;
}

function enable_change(id)
{	
	if(wireless_enable != "2" && wireless_enable != "3")
	{
		/*if(id=="2.4g_enable1")
		{
			document.getElementById(id).innerHTML=(document.getElementById(id).innerHTML==m_open)?m_close:m_open;
			if(document.getElementById(id).innerHTML==m_open)
				document.getElementById("wireless_2").style.display="block";
			else
				document.getElementById("wireless_2").style.display="none";
		}*/
		if(id=="wireless2_onoff")
		{
			document.getElementById(id).src=((document.getElementById(id).src).indexOf(on_imgname)>0)?"img/"+off_imgname:"img/"+on_imgname;
			if((document.getElementById(id).src).indexOf(on_imgname)>0)
				document.getElementById("wireless_2").style.display="block";
			else
				document.getElementById("wireless_2").style.display="none";
		}
	}
	if(wireless_enable2 != "2" && wireless_enable2 != "3")
	{
		/*if(id=="5g_enable1")
		{
			document.getElementById(id).innerHTML=(document.getElementById(id).innerHTML==m_open)?m_close:m_open;
			if(document.getElementById(id).innerHTML==m_open)
				document.getElementById("wireless_5").style.display="block";
			else
				document.getElementById("wireless_5").style.display="none";
		}*/
		if(id=="wireless5_onoff")
		{
			document.getElementById(id).src=((document.getElementById(id).src).indexOf(on_imgname)>0)?"img/"+off_imgname:"img/"+on_imgname;
			if((document.getElementById(id).src).indexOf(on_imgname)>0)
				document.getElementById("wireless_5").style.display="block";
			else
				document.getElementById("wireless_5").style.display="none";
		}
	}
}

function changmode()
{
	if(document.getElementById("2.4g_option").value == m_wireless_mode_none)
	{
		document.getElementById("rss_Wl2_EncryptionKey").disabled=true;
		document.getElementById("rss_Wl2_WirelessName_check").style.display="none";
		document.getElementById("rss_Wl2_EncryptionKey_check").style.display="none";
	}
	else
	{
		document.getElementById("rss_Wl2_EncryptionKey").disabled=false;
		document.getElementById("rss_Wl2_WirelessName_check").style.display="block";
		document.getElementById("rss_Wl2_EncryptionKey_check").style.display="block";
	}
	check_wireless();
}

function changmode2()
{
	if(document.getElementById("5g_option").value == m_wireless_mode_none)
	{
		document.getElementById("rss_Wl5_EncryptionKey").disabled=true;
		document.getElementById("rss_Wl5_WirelessName_check").style.display="none";
		document.getElementById("rss_Wl5_EncryptionKey_check").style.display="none";
	}
	else
	{
		document.getElementById("rss_Wl5_EncryptionKey").disabled=false;
		document.getElementById("rss_Wl5_WirelessName_check").style.display="block";
		document.getElementById("rss_Wl5_EncryptionKey_check").style.display="block";
	}
	check_wireless();
}

function checkIP(ip) 
{ 
	obj=ip;
	var exp=/^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
	var reg = obj.match(exp);

	if(reg==null)
		return false;//不合法
	else
		return true; //合法
} 

function isSpeciStaticIpMask(ip,mask)
{
    var classBIp = new Array("128.0.0.1","128.255.255.254","136.16.0.1","136.32.255.254","142.64.0.1","142.64.255.255");
    var classCIp = new Array("192.0.0.1","192.255.255.254","200.16.32.1");
    var subnetMask1 = "255.255.0.0";
    var subnetMask2 = "255.255.255.0";

    if(isIP(ip) == false)
        return false;

    var ipParts = ip.split(".");
    var ipStic = parseInt(ipParts[0])+"."+parseInt(ipParts[1])+"."+parseInt(ipParts[2])+"."+parseInt(ipParts[3]);
    var maskParts = mask.split(".");
    var maskStic = parseInt(maskParts[0])+"."+parseInt(maskParts[1])+"."+parseInt(maskParts[2])+"."+parseInt(maskParts[3]);

    for (i=0; i<classBIp.length; i++)
    {
        if(ipStic == classBIp[i])
            if(maskStic != subnetMask1)
                return false;
    }
    for (j=0; j<classCIp.length; j++)
    {
        if(ipStic == classCIp[j])
            if(maskStic != subnetMask2)
                return false;
    }

    return true;
}

function isNumber(n)
{
    if (n.length==0) return false;
    for (var i=0;i < n.length;i++)
    {
        if (n.charAt(i) < '0' || n.charAt(i) > '9') return false;
    }
    return true;
}
function isIP(ip)
{
   var sIP=ip.split(".");
   if (sIP.length!=4) return false;
   for(var i=0; i< sIP.length; i++)
   {
      if (!isNumber(sIP[i])) return false;
      if(i == 3)
        if (parseInt(sIP[i]) < 1 || parseInt(sIP[i]) > 254) return false;
    else
      if (parseInt(sIP[i]) < 0 || parseInt(sIP[i]) > 255) return false;
   }
   if(parseInt(sIP[0])==0) return false;

   return true;
}

function check_ip(id)
{
	var flag=false;
	flag=checkIP(document.getElementById(id).value);
	if(flag==true)
	{
		parse_ValidateErr_index(id,1);
		document.getElementById(id+"_check").value="";
	}
	else
	{
		parse_ValidateErr_index(id,0);
		document.getElementById(id+"_check").value = m_error_ip;
	}
}

function is_valid_mask(mask) 
{ 
	obj=mask;
	var exp=/^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/;
	var reg = obj.match(exp);

	if(reg==null)
		return false; //"非法"
    else
		return true; //"合法"
}

function check_mask(id)
{
	var flag=false;
	var mask=document.getElementById(id).value;
	flag=is_valid_mask(mask);
	if(flag==true)
	{
		parse_ValidateErr_index("rss_Upl_NetMask",1);
		document.getElementById("rss_Upl_NetMask_check").value="";
	}
	else
	{
		parse_ValidateErr_index("rss_Upl_NetMask",0);
		document.getElementById("rss_Upl_NetMask_check").value=m_error_mask;
	}
	if(flag==true)
	{
		if(document.getElementById("rss_Upl_Gateway").value!="")
			check_gateway("rss_Upl_Gateway");
	}
}

function check_gateway(id)
{
	var flag=false;
	flag=checkIP(document.getElementById(id).value);
	if(flag==true)
	{
		parse_ValidateErr_index(id,1);
		document.getElementById(id+"_check").value="";
	}
	else
	{
		parse_ValidateErr_index(id,0);
		document.getElementById(id+"_check").value=m_error_gateway;
	}
	var static_ip=document.getElementById("rss_Upl_IP").value;
	var static_mask=document.getElementById("rss_Upl_NetMask").value;
	var static_gw=document.getElementById("rss_Upl_Gateway").value;
	if(static_ip == static_mask || static_mask == static_gw )
	{
		document.getElementById("rss_Upl_NetMask_check").value=m_error_mask;
		return false; //3个地址不能相同
	}
	else if( static_mask == static_ip)
	{
		document.getElementById("rss_Upl_Gateway_check").value=m_error_gateway;
		return false; //3个地址不能相同
	}

	var static_ip_arr = new Array;
    var static_mask_arr = new Array;
    var static_gw_arr = new Array;
     
    static_ip_arr = static_ip.split(".");
    static_mask_arr = static_mask.split(".");
    static_gw_arr = static_gw.split(".");

    var res0 = parseInt(static_ip_arr[0]) & parseInt(static_mask_arr[0]);
    var res1 = parseInt(static_ip_arr[1]) & parseInt(static_mask_arr[1]);
    var res2 = parseInt(static_ip_arr[2]) & parseInt(static_mask_arr[2]);
    var res3 = parseInt(static_ip_arr[3]) & parseInt(static_mask_arr[3]);
    
    var res0_gw = parseInt(static_gw_arr[0]) & parseInt(static_mask_arr[0]);
    var res1_gw = parseInt(static_gw_arr[1]) & parseInt(static_mask_arr[1]);
    var res2_gw = parseInt(static_gw_arr[2]) & parseInt(static_mask_arr[2]);
    var res3_gw = parseInt(static_gw_arr[3]) & parseInt(static_mask_arr[3]);

	if(res0==res0_gw && res1==res1_gw && res2==res2_gw  && res3==res3_gw)
    {
        
    }else{
        document.getElementById("rss_Upl_Gateway_check").value=m_error_gateway;
        return false;
    }
}