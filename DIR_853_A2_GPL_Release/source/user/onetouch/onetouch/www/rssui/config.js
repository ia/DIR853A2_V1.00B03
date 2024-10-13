var txt="";
var txt2="";
var web_content="";

//setting
function init_config_page()
{
	// Init Actions
	public_js_init();

	// decode encryption data
	device_passwd = RssDataDecode(device_passwd);
	ppp_password = RssDataDecode(ppp_password);
	wireless_pwd = RssDataDecode(wireless_pwd);
	wireless_pwd2 = RssDataDecode(wireless_pwd2);

	//alert(device_icon);

	document.getElementById("device_icon").src=device_icon;
	document.getElementById("devicename").value=friendlyname;
	document.getElementById("devicemodel").value=device_model;
	document.getElementById("devicefw").value="Ver "+firmversion;
	document.getElementById("rss_DeviceName").value=friendlyname;
	if(factory_flag==0)
		document.getElementById("rss_DevicePwd").value=device_passwd;
	else
	{
		document.getElementById("rss_DevicePwd").value=generateMixed(6);
		document.getElementById("rss_DevicePwd_check").value="Default Password";
		document.getElementById("rss_DevicePwd_check").style.color="LawnGreen ";
	}
	web_content="";
	if(document.getElementById("device_mode").options.length == 0)
	{
		for(var i=0;i<device_mode2.length;i++)
		{
			//web_content+="<option value=\""+device_mode2[i]+"\">"+device_mode2[i]+"</option>";
			var y=document.createElement('option');
			y.text=device_mode2[i];
			y.value=device_mode2[i];
			document.getElementById("device_mode").add(y, i);
		}
	}
	if(routerenbl == "0")
	{
		if(HardwareType=="ADSL" || HardwareType=="VDSL" || HardwareType=="Ethernet")
			document.getElementById("device_mode").value="AP";
		else if(HardwareType=="WWan802.11")
		{
			if(wireless_enable=="1" || wireless_enable=="3" || wireless_enable2=="1" || wireless_enable2=="3")
				document.getElementById("device_mode").value="Repeater";
			else
				document.getElementById("device_mode").value="Extender";
		}
	}
	else if(routerenbl == "1")
	{
		if(HardwareType=="ADSL" || HardwareType=="VDSL" || HardwareType=="Ethernet")
                        document.getElementById("device_mode").value="Router";
		else if(HardwareType=="WWan802.11" || HardwareType=="WWanAPN")
			document.getElementById("device_mode").value="Hotspot";
	}
	//document.getElementById("device_mode").innerHTML=web_content;
	setTimeout("check_AuthKey()", 30000);
}
		
function apply_config()
{
	/*if(document.getElementById("rss_DevicePwd").value.length<=1)
	{
		alert("device passwd length must more than 1 !");
		return false;
	}*/
	if(document.getElementById("rss_DevicePwd").value == device_defaultpasswd)
	{
		//alert("device passwd cann't be set to \"admin\"");
		document.getElementById("rss_DevicePwd_check").value=m_passwd_error;
		return false;
	}

	txt="";
	txt="rss_DeviceName="+subHtmAsc(document.getElementById("rss_DeviceName").value);
	txt+="&rss_DevicePwd="+subHtmAsc(RssDataEncode(document.getElementById("rss_DevicePwd").value));
	//txt+="&rss_Sys_devicetype="+escape(document.getElementById("device_mode").value);
	if(document.getElementById("device_mode").value == "AP")
	{
		if(routerenbl!=2)
			txt+="&rss_RouterEnbl=0";
		if((HardwareType!="ADSL") && (HardwareType!="VDSL") && (HardwareType!="Ethernet"))
		{
			var txt2="";
			for(var i=0;i<HardwareSupport2.length;i++)
			{
				if(HardwareSupport2[i]=="ADSL")
					txt2="&rss_Upl_HardwareType=ADSL";
				else if(HardwareSupport2[i]=="VDSL")
					txt2="&rss_Upl_HardwareType=VDSL";
				else if(HardwareSupport2[i]=="Ethernet")
					txt2="&rss_Upl_HardwareType=Ethernet";
			}
			txt+=txt2;
		}
		else
			txt+="&rss_Upl_HardwareType="+HardwareType;
	}
	else if(document.getElementById("device_mode").value == "Router")
	{
		if(routerenbl!=3)
			txt+="&rss_RouterEnbl=1";
		if((HardwareType!="ADSL") && (HardwareType!="VDSL") && (HardwareType!="Ethernet"))
		{
			var txt2="";
			for(var i=0;i<HardwareSupport2.length;i++)
			{
				
				if(HardwareSupport2[i]=="ADSL")
					txt2="ADSL";
				else if(HardwareSupport2[i]=="VDSL" && txt2!="ADSL")
					txt2="VDSL";
				else if(HardwareSupport2[i]=="Ethernet" && txt2!="VDSL" && txt2!="ADSL")
					txt2="Ethernet";
			}
			//txt+=txt2;
			txt+="&rss_Upl_HardwareType="+txt2;
		}
		else
			txt+="&rss_Upl_HardwareType="+HardwareType;
	}
	else if((document.getElementById("device_mode").value == "Extender") || (document.getElementById("device_mode").value == "Repeater"))
	{
		if(routerenbl!=2)
			txt+="&rss_RouterEnbl=0";
		txt+="&rss_Upl_HardwareType=WWan802.11";
	}
	else if(document.getElementById("device_mode").value == "Hotspot")
	{
		if(routerenbl!=3)
			txt+="&rss_RouterEnbl=1";
		if((HardwareType!="WWan802.11") && (HardwareType!="WWanAPN"))
		{
			var txt2="";
			for(var i=0;i<HardwareSupport2.length;i++)
			{
				if(HardwareSupport2[i]=="WWan802.11")
					txt2="WWan802.11";
				else if(HardwareSupport2[i]=="WWanAPN" && txt2!="WWan802.11")
					txt2="WWanAPN";
			}
			txt+="&rss_Upl_HardwareType="+txt2;
		}
		else
			txt+="&rss_Upl_HardwareType="+HardwareType;
	}
	
	//alert(document.getElementById("device_mode").value);
	if ((document.getElementById("device_mode").value == "AP") || (document.getElementById("device_mode").value == "Router"))
	{
	
	}
	else if((document.getElementById("device_mode").value == "Repeater") ||(document.getElementById("device_mode").value == "Extender") || (document.getElementById("device_mode").value == "HostAP"))
	{
	//	txt+="&rss_Act_SiteSurvey="+"1";
		//location.href="scan.asp?"+txt;
	}
	return true;
}

function next()
{
	var flag=true;;
	for(var i=0;i<validateErrTable.length;i++)
	{
		if(validateErrTable[i]!="0")
			return false;
	}
	if(document.getElementById("body1").style.display=="block")
	{
		flag=apply_config();
		if(flag==true)
		{
			if((document.getElementById("device_mode").value == "AP") || (document.getElementById("device_mode").value == "Router"))
			{
					document.getElementById("body1").style.display="none";
					document.getElementById("body2").style.display="block";
					document.getElementById("body3").style.display="none";
					document.getElementById("body4").style.display="none";
					init_isp();
			}
			else if((document.getElementById("device_mode").value == "Repeater") ||(document.getElementById("device_mode").value == "Extender") || (document.getElementById("device_mode").value == "HostAP"))
			{
				document.getElementById("body1").style.display="none";
				document.getElementById("body2").style.display="none";
				document.getElementById("body3").style.display="none";
				document.getElementById("body4").style.display="block";
				document.getElementById("th_next").style.display="none";
				document.getElementById("th_scan").style.display="table-cell";
				check_survey(1);
			}
		}
		else
			return false;
	}
	else if(document.getElementById("body2").style.display=="block")
	{
		flag=apply_isp();
		if(flag==true)
		{
			document.getElementById("body1").style.display="none";
			document.getElementById("body2").style.display="none";
			document.getElementById("body3").style.display="block";
			document.getElementById("body4").style.display="none";
			init_wireless();
		}
		else
			return false
	}
	else if(document.getElementById("body3").style.display=="block")
	{
		apply_wireless();
	}
}

function back()
{
	if(document.getElementById("body1").style.display=="block")
	{
		window.history.go(-1);
	}
	else if(document.getElementById("body2").style.display=="block")
	{
		document.getElementById("body1").style.display="block";
		document.getElementById("body2").style.display="none";
		document.getElementById("body3").style.display="none";
		document.getElementById("body4").style.display="none";
		init_config_page();
	}
	else if(document.getElementById("body3").style.display=="block")
	{
		document.getElementById("body1").style.display="none";
		document.getElementById("body2").style.display="block";
		document.getElementById("body3").style.display="none";
		document.getElementById("body4").style.display="none";
		init_isp();
	}
	else if(document.getElementById("body4").style.display=="block")
	{
		document.getElementById("body1").style.display="block";
		document.getElementById("body2").style.display="none";
		document.getElementById("body3").style.display="none";
		document.getElementById("body4").style.display="none";
		document.getElementById("th_next").style.display="table-cell";
		document.getElementById("th_scan").style.display="none";
	}
	validateErrTable.splice(0,validateErrTable.length);
}
