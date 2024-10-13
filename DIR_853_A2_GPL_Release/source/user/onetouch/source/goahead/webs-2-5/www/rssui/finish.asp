<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>
<link rel="stylesheet" href="./public/dbros.css" type="text/css">
<script>
var isRssEncrypt="<% ConfigGet("/onetouch/DataEncrypt"); %>";
var RssVarEncryptKey="<% ConfigRssGet("rss_Var_RssEncryKey"); %>";
var RssVarEncryptHeader="<% ConfigRssGet("rss_Var_RssEncryHeader"); %>";
var device_model="<% ConfigGet("/sys/modelname"); %>";
var devicename="<% ConfigRssGet("rss_DeviceName"); %>";
var devicepsw="<% ConfigRssGet("rss_DevicePwd"); %>";
var HardwareType="<% ConfigRssGet("rss_Upl_HardwareType"); %>";
var extender_name="<% ConfigRssGet("rss_Upl_Ets_WirelessName"); %>";
var extender_psw="<% ConfigRssGet("rss_Upl_Ets_EncryptionKey"); %>";
var extender_type="<% ConfigRssGet("rss_Upl_Ets_WirelessType"); %>";
var wireless_enable="<% ConfigRssGet("rss_Wl2_WlEnbl"); %>";
var wireless_enable2="<% ConfigRssGet("rss_Wl5_WlEnbl"); %>";
var wireless_ssid="<% ConfigRssGet("rss_Wl2_WirelessName"); %>";
var wireless_pwd="<% ConfigRssGet("rss_Wl2_EncryptionKey"); %>";
var wireless_encryptionmode="<% ConfigRssGet("rss_Wl2_EncryptionMode"); %>";
var wireless_ssid2="<% ConfigRssGet("rss_Wl5_WirelessName"); %>";
var wireless_pwd2="<% ConfigRssGet("rss_Wl5_EncryptionKey"); %>";
var wireless_encryptionmode2="<% ConfigRssGet("rss_Wl5_EncryptionMode"); %>";
</script>
<script src="language_<% ConfigGet("/runtime/web_lang"); %>.js" type="text/javascript"></script>
<script src="public.js" type="text/javascript"></script>
<script src="base64.js" type="text/javascript"></script>

<script>
if(wireless_encryptionmode=="None")
	wireless_pwd="";
if(wireless_encryptionmode2=="None")
	wireless_pwd2="";
if(HardwareType=="WWan802.11")
{
	if (extender_type=="2.4") 
	{
		wireless_enable=3;
		wireless_enable2=2;
		wireless_ssid=extender_name;
		wireless_pwd=extender_psw;
	}
	else
	{
		wireless_enable=2;
		wireless_enable2=3;
		wireless_ssid2=extender_name;
		wireless_pwd2=extender_psw;
	}
}

function load()
{
	public_js_init();
	devicepsw = RssDataDecode(devicepsw);
	wireless_pwd = RssDataDecode(wireless_pwd);
	wireless_pwd2 = RssDataDecode(wireless_pwd2);

	document.getElementById("dev_pw").innerHTML = devicepsw;
	document.getElementById("wl2_pw").innerHTML = wireless_pwd;
	if(wireless_enable==0||wireless_enable==2)
		document.getElementById("wl2").style.display="none";
	else
	{
		if(wireless_pwd=="") 
		{
			document.getElementById("wl2_pw_t").innerHTML = m_finish_nopasswd;
			document.getElementById("wl2_pw").style.display="none";
		}
	}
	document.getElementById("wl5_pw").innerHTML = wireless_pwd2;
	if(wireless_enable2==0||wireless_enable2==2)
		document.getElementById("wl5").style.display="none";
	else
	{
		if(wireless_pwd2=="")
		{
			document.getElementById("wl5_pw_t").innerHTML = m_finish_nopasswd;
			document.getElementById("wl5_pw").style.display="none";
		}
	}
	setTimeout("logout()",2000);
}

function logout() {
	var xmlhttp = createXMLHttpRequest();
	var theUrl_logout = "/rssui/logout.xgi";
	xmlhttp.open("GET", theUrl_logout, true);
	xmlhttp.send();
	//alert("Setup complete! Please close this page, and wait 20 seconds for device ready.");
}

</script>
</head>

<body onload="load()" style="background:#FDF5E6">

<div id="layer" style="background:#0066FF;overflow-y:auto">
  <div style="margin:50px 50px;border:solid 2px white;">
	<div style="margin:5px 5px;">
		<p align="center"><img style="width:180px;" src="<% ConfigGetPath("/sys/icon:1/url"); %>" />
		<h1 align="center"><font color="white"><script>prints(m_finish_setup);</script></font></h1></p>
	</div>
	<div style="margin:5px 5px;">
		<h1 align="left"><font color="white">Model:&nbsp;<script>prints(device_model);</script></font></h1>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;<font color="white"><script>prints(m_finish_devicename);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="white"><script>prints(devicename);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;<font color="white"><script>prints(m_finish_passwd);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="white" id="dev_pw"></font></h2>
	</div>
	<div style="margin:5px 5px;" id="wl2">
		<h1 align="left"><font color="white"><script>prints(m_wireless_2);</script></font></h1>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;<font color="white"><script>prints(m_finish_wlname);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="white"><script>prints(wireless_ssid);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;<font color="white" id="wl2_pw_t"><script>prints(m_finish_wlpassword);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="white" id="wl2_pw"></font></h2>
	</div>
	<div style="margin:5px 5px;" id="wl5">
		<h1 align="left"><font color="white"><script>prints(m_wireless_5);</script></font></h1>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;<font color="white"><script>prints(m_finish_wlname);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="white"><script>prints(wireless_ssid2);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;<font color="white" id="wl5_pw_t"><script>prints(m_finish_wlpassword);</script></font></h2>
		<h2 align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="white" id="wl5_pw"></font></h2>
	</div>
	<div style="margin:75px 5px 10px 5px;" id="foot">
		<h3 align="center"><font color="#A0A0A0" size="5"><script>prints(m_finish_note);</script></font></h1>
	</div>
  </div>
</div>

</body>

</html>
