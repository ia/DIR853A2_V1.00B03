<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<meta http-equiv="Access-Control-Allow-Origin" content="*">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>
<link rel="stylesheet" href="./public/dbros.css" type="text/css">
<script type="text/javascript">
	var isRssEncrypt="<% ConfigGet("/onetouch/DataEncrypt"); %>";
	var RssVarEncryptKey="<% ConfigRssGet("rss_Var_RssEncryKey"); %>";
	var RssVarEncryptHeader="<% ConfigRssGet("rss_Var_RssEncryHeader"); %>";
	var friendlyname="<% ConfigRssGet("rss_DeviceName"); %>";
	var device_model="<% ConfigGet("/sys/modelname"); %>";
	var device_passwd="<% ConfigRssGet("rss_DevicePwd"); %>";
	var device_defaultpasswd="<% ConfigRssGet("rss_Var_UIDefaultPW"); %>";
	var device_mode="<% ConfigRssGet("rss_DeviceType"); %>";
	var device_mode2=device_mode.split(",");
	var firmversion="<% ConfigGet("/fwupgradeinfo/currfwversion"); %>";
	var device_icon="<% ConfigGetPath("/sys/icon:1/url"); %>";
	var HardwareSupport="<% ConfigGet("/uplink/hardwaresupport"); %>";
	var HardwareSupport2=HardwareSupport.split(",");
	var HardwareType="<% ConfigRssGet("rss_Upl_HardwareType"); %>";
	var routerenbl="<% ConfigRssGet("rss_RouterEnbl"); %>";
	var thAuthKeyVal="<% Generate_Key(); %>";
	var theAuthKeyName="<% ConfigRssGet("rss_Var_AuthKeyName"); %>"; // "DKey";
	var factory_flag="<% ConfigGet("/sys/factorydefault"); %>";
	if(device_model == "DSL-2875AL")
		factory_flag=1-factory_flag;
	var lanip="<% ConfigRssGet("rss_Var_LanIP"); %>";
	var lanip_port="<% ConfigGet("/onetouch/rsshttpport"); %>";

	var countryList = [<% ConfigGetArray("/dslisplist/country#", "countryname"); %>];
	var ispDataList = [<% ConfigGetArray("/dslisplist/country#","isp#/ispname","isp#/conntype"); %>];
	var ppp_username="<% ConfigRssGet("rss_Upl_LoginUserName"); %>";
	var ppp_password="<% ConfigRssGet("rss_Upl_LoginPassword"); %>";
	var ppp_ip="<% ConfigRssGet("rss_Upl_IP"); %>";
	var ppp_netmask="<% ConfigRssGet("rss_Upl_NetMask"); %>";
	var ppp_gateway="<% ConfigRssGet("rss_Upl_Gateway"); %>";
	var ppp_dns="<% ConfigRssGet("rss_Upl_DNS"); %>";
	var HardwareType="<% ConfigRssGet("rss_Upl_HardwareType"); %>";
	var ConnType="<% ConfigRssGet("rss_Upl_ConnType"); %>";

	var wireless_enable="<% ConfigRssGet("rss_Wl2_WlEnbl"); %>";
	var wireless_ssid="<% ConfigRssGet("rss_Wl2_WirelessName"); %>";
	var wireless_pwd="<% ConfigRssGet("rss_Wl2_EncryptionKey"); %>";
	var wireless_encryptionmode="<% ConfigRssGet("rss_Wl2_EncryptionMode"); %>";
	var wireless_encryptionype="<% ConfigRssGet("rss_Wl2_EncryptionType"); %>";

	var wireless_enable2="<% ConfigRssGet("rss_Wl5_WlEnbl"); %>";
	var wireless_ssid2="<% ConfigRssGet("rss_Wl5_WirelessName"); %>";
	var wireless_pwd2="<% ConfigRssGet("rss_Wl5_EncryptionKey"); %>";
	var wireless_encryptionmode2="<% ConfigRssGet("rss_Wl5_EncryptionMode"); %>";
	var wireless_encryptionype2="<% ConfigRssGet("rss_Wl5_EncryptionType"); %>";
	var factory_flag="<% ConfigGet("/sys/factorydefault"); %>";
	if(device_model == "DSL-2875AL")
		factory_flag=1-factory_flag;
	var device_mac="<% ConfigGet("/fwupgradeinfo/devicemac"); %>";
	var isp_exist="<% ConfigGet("/dslisplist/country:1/countryname"); %>";

	function window_close() {
		self.location="about:blank";
	}
</script>
<script src="public.js" type="text/javascript"></script>
<script src="base64.js" type="text/javascript"></script>
<script src="config.js" type="text/javascript"></script>
<script src="isp.js" type="text/javascript"></script>
<script src="wireless.js" type="text/javascript"></script>
<script src="scan.js" type="text/javascript"></script>
<script src="language_<% ConfigGet("/runtime/web_lang"); %>.js" type="text/javascript"></script>
</head>

<body onload="init_config_page()">
<input type="hidden" id="2.4g_enable" name="rss_Wl2_WlEnbl">
<input type="hidden" id="5g_enable" name="rss_Wl5_WlEnbl">
<input type="hidden" id="rss_Ets_connrefsitebssid" name="rss_Ets_ConnRefSiteBSSID">
<input type="hidden" id="rss_Ets_wirelesstype" name="rss_Ets_WirelessType">
<input type="hidden" id="rss_Ets_wirelessname" name="rss_Ets_WirelessName">
<input type="hidden" id="rss_Ets_encryptionkey" name="rss_Ets_EncryptionKey">

<div id="panel-wrap">
</div>

<!--script src="config.js" type="text/javascript"></script-->
<div id="layer">
	<div class="pic_abs" id="Loading" style="top:240px;left:0px;width:100%;z-index:99;display:none;" align="center">
		<div class="press_pwd" style="width:500px;height:350px;margin:-200px -250px;">
			<div style="height:25%;border-bottom:2 solid Aqua;">
				<table style="height:100%;width:100%;">
					<tr style="text-align:left;">
						<td style="white-space:nowrap;overflow:hidden;text-align:right;font-size:35px;"><script>prints(m_ssid);</script></td>
						<th style="white-space:nowrap;overflow:hidden;width:75%;"><font id="ssid_name" style="text-align:left;float:left;width:100px;font-size:35px;"></font></th>
					<tr>
				</table>
			</div>
			<div style="height:25%;">
				<table style="height:100%;width:100%;">
				<tr>
					<td width=80%><p style="float:right;font-size:35px;"><script>prints(m_display_passwd);</script></p></td>
				
					<td width=20%><input type="checkbox" id="passwd_check" style="margin:5px 30px;" onClick="display_pwd();"></td>
				</tr>
				</table>
			</div>
			<div style="height:25%;border-bottom:2 solid Aqua;">
				<table style="height:100%;width:100%;">
				<tr>
					<th>
						<input type="password" class="input-text11" id="passwd" autofocus="autofocus" style="width:430px;margin:5px 15px;font-size:45px;">
					</th>
				</tr>
				</table>
			</div>
			<div style="height:25%;">
				<table style="height:100%;width:100%;">
					<tr>
					<script>
						document.write("<th style=\"width:50%;\"><input type=\"button\" value=\""+m_connect+"\" style=\"width:75%;font-size:35px;\" onClick=\"connect();\"></th>");
						document.write("<th style=\"width:50%;\"><input type=\"button\" value=\""+m_cancel+"\" style=\"width:75%;font-size:35px;\" onClick=\"close_window()\"></th>");
					</script>
					</tr>
				</table>
			</div>
		</div>
	</div>

		<!--div class="header" style="position:absolute;top:0px;">
			<table style="margin-left: auto;margin-right: auto;width:100%;height:100%;background:#ccc;border-collapse: collapse; ">
				<tr>
					<th><font size="16"><script>prints(m_setup);</script></font></th>
				</tr>
			</table>
		</div-->
		<br>
		<!--Device Setting-->
		<div class="body" id="body1" style="position:absolute;top:100px;bottom:100px;width:100%;overflow: scroll;display:block;">
			<div style="position:static;margin:20px;">
				<table style="margin-left: auto;margin-right: auto;width:90%;height:280px;background-color:transparent">
					<tr height="70px">
						<th  rowspan="3" width="35%"><img id="device_icon" style="width:180px;"></img></th>
						<td width="65%"><input class="input-text3" id="devicename" readonly></td>
					</tr>
					<tr height="70px">
						<td width="65%"><input class="input-text3" id="devicemodel" readonly></td>
					</tr>
					<tr height="70px">
						<td width="65%"><input class="input-text3" id="devicefw" readonly></td>
					</tr>
				</table>
			</div>
			<br>
			<div style="position:static;margin:20px;">
				<table style="margin-left: auto;margin-right: auto;width:90%;height:280px;">
					<tr>
						<th style="text-align:left;"><font size="14"><script>prints(m_device_name);</script></font></th>
						<!--td width="60%"><input type="text" width="35%"class="input-text" id="rss_DeviceName" name="rss_DeviceName" onkeyup="check_rssVarValidate(this.id)"-->
							<!--input type="text" class="input-text4" id="rss_DeviceName_check" name="rss_DeviceName_check" readonly--></td>
					</tr>
					<tr>
						<td><input type="text" class="input-text11" id="rss_DeviceName" name="rss_DeviceName" onkeyup="check_rssVarValidate(this.id)">
					</tr>
					<tr>
						<td ><input type="text" class="input-text4" id="rss_DeviceName_check" name="rss_DeviceName_check" readonly></td>
					</tr>
					</tr>
					<tr>
						<th style="text-align:left;"><font size="14"><script>prints(m_passwd);</script></font></th>
						
							<!--input type="text" class="input-text4" id="rss_DevicePwd_check" name="rss_DevicePwd_check" readonly--></td>
					</tr>
					<tr>
						<td><input type="text" class="input-text11" id="rss_DevicePwd" name="rss_DevicePwd" onkeyup="check_rssVarValidate(this.id)">
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_DevicePwd_check" name="rss_DevicePwd_check" readonly></td>
					</tr>
				</table>
			</div>
			<br>
			<div style="position:static;margin:20px;">
				<table style="margin-left: auto;margin-right: auto;width:90%;height:280px;">
					<tr>
						<th style="text-align:left;"><font size="14"><script>prints(m_device_type);</script></font></th>
						<!--td>
							<select id="device_mode">
								<!--option value="router">Router<option-->
							<!--/select>
						</td-->
	
					</tr>
					<tr>
						<th>
							<select id="device_mode">
								<!--option value="router">Router<option-->
							</select>
						</th>
					</tr>
				</table>
			</div>
	</div>
	<!--End Device Setting-->
	<!--ISP .asp -->
	<div class="body" id="body2" style="position:absolute;top:100px;bottom:100px;width:100%;overflow: scroll;display:none;">
		<div style="position:static;margin:20px;">
			
				<table id="adslvdsl_mode" style="margin-left: auto;margin-right: auto;width:90%;height:280px;">
					<tr>
						<th class="r_tb"><font size="14"><script>prints(m_country);</script></font></th>
						<td class="l_tb">
							<select name="select-country" id="select-country" onChange="change_country();">
							</select>
						</td>
					</tr>
					<tr>
						<td width="40%"></td>
						<td width="60%"><input type="text" class="input-text4" id="select-country_check" name="select-country_check" readonly></td>
					</tr>
					<tr>
						<th class="r_tb"><font size="14"><script>prints(m_isp);</script></font></th>
						<td class="l_tb">
							<select name="select-isp" id="select-isp" onChange="change_isp();">
								<!--option value="-1">(Click to Select)</option-->
							</select>
						</td>
					</tr>
				</table>
				<table id="ether_mode" style="width:100%;margin-left: auto;margin-right: auto;display:none; ">
					<tr>
						<th class="l_tb"><font size="14"><script>prints(m_ethernet);</script></font></th>
						<td class="r_tb">
							<select name="select-ether" id="select-ether" onChange="change_ether();" style="width:50%";>
								<option><script>prints(m_ether_dhcp);</script></option>
								<option><script>prints(m_ether_static);</script></option>
								<option><script>prints(m_ether_pppoe);</script></option>
							</select>
						</td>
					</tr>
				</table>
		</div>
		<div style="position:static;margin:20px;">
			<table id='static' style="margin-left: auto;margin-right: auto;width:90%;height:280px;display:none;text-align:left;">
					<tr>
						<th><font size="14"><script>prints(m_ip_addr);</script></font></th>
							<!--input type="text" class="input-text4" id="rss_Upl_IP_check" name="rss_Upl_IP_check" readonly--></td>
					</tr>
					<tr>
							<td><input type='text' class="input-text11" id="rss_Upl_IP" name="rss_Upl_IP" onkeyup="check_ip(this.id)">
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_Upl_IP_check" name="rss_Upl_IP_check" readonly></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_mask);</script></font></th>
						
					</tr>
					<tr>
						<td>
							<input type='text' class="input-text11" id="rss_Upl_NetMask" name="rss_Upl_NetMask" onkeyup="check_mask(this.id)">
							<!--input type="text" class="input-text4" id="rss_Upl_NetMask_check" name="rss_Upl_NetMask_check" readonly-->
						</td>
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_Upl_NetMask_check" name="rss_Upl_NetMask_check" readonly></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_gateway);</script></font></th>
					</tr>
					<tr>
						<td>
							<input type='text' class="input-text11" id="rss_Upl_Gateway" name="rss_Upl_Gateway" onkeyup="check_gateway(this.id)">
							<!--input type="text" class="input-text4" id="rss_Upl_Gateway_check" name="rss_Upl_Gateway_check" readonly-->
						</td>
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_Upl_Gateway_check" name="rss_Upl_Gateway_check" readonly></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_dns);</script></font></th>
					</tr>
					<tr>
						<td>
							<input type='text' class="input-text11" id="rss_Upl_DNS" name="rss_Upl_DNS" onkeyup="check_ip(this.id)">
							<!--input type="text" class="input-text4" id="rss_Upl_DNS_check" name="rss_Upl_DNS_check" readonly-->
						</td>
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_Upl_DNS_check" name="rss_Upl_DNS_check" readonly></td>
					</tr>
				</table>
				<table id='ppp' style="margin-left: auto;margin-right: auto;width:90%;height:280px;display:none;text-align:left;">
					<tr>
						<th><font size="14"><script>prints(m_ip_addr);</script></font></th>
							<!--input type="text" class="input-text4" id="rss_Upl_IP_check" name="rss_Upl_IP_check" readonly--></td>
					</tr>
					<tr>
						<td><input type='text'  class="input-text11" id="rss_Upl_IP3" name="rss_Upl_IP3" readonly style="background-color:#ccc">
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_mask);</script></font></th>
					</tr>
					<tr>
						<td><input type='text' class="input-text11" id="rss_Upl_NetMask3" name="rss_Upl_NetMask3" readonly style="background-color:#ccc"></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_gateway);</script></font></th>
					</tr>
					<tr>
						<td><input type='text' class="input-text11" id="rss_Upl_Gateway3" name="rss_Upl_Gateway3" readonly style="background-color:#ccc"></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_dns);</script></font></th>
					</tr>
					<tr>
						<td>
							<input type='text' class="input-text11" id="rss_Upl_DNS3" name="rss_Upl_DNS3" readonly style="background-color:#ccc">
							<!--input type="text" class="input-text4" id="rss_Upl_DNS_check" name="rss_Upl_DNS_check" readonly-->
						</td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_account);</script></font></th>
						
					</tr>
					<tr>
						<td>
							<input type='text' class="input-text11" id="rss_Upl_LoginUserName" name="rss_Upl_LoginUserName" onkeyup="check_rssVarValidate(this.id)">
							<!--input type="text" class="input-text4" id="rss_Upl_LoginUserName_check" name="rss_Upl_LoginUserName_check" readonly-->
						</td>
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_Upl_LoginUserName_check" name="rss_Upl_LoginUserName_check" readonly></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_password);</script></font></th>
					</tr>
					<tr>
						<td>
							<input type='text' class="input-text11" id="rss_Upl_LoginPassword" name="rss_Upl_LoginPassword" onkeyup="check_rssVarValidate(this.id)">
							<!--input type="text" class="input-text4" id="rss_Upl_LoginPassword_check" name="rss_Upl_LoginPassword_check" readonly-->
						</td>
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_Upl_LoginPassword_check" name="rss_Upl_LoginPassword_check" readonly></td>
					</tr>
				</table>
				<table id='dhcp' style="margin-left: auto;margin-right: auto;width:90%;height:280px;display:none;text-align:left;">
					<!--tr>
						<th><font size="14"><script>prints(m_dhcp);</script></font></th>
					</tr-->
					<tr>
						<th><font size="14"><script>prints(m_ip_addr);</script></font></th>
							<!--input type="text" class="input-text4" id="rss_Upl_IP_check" name="rss_Upl_IP_check" readonly--></td>
					</tr>
					<tr>
						<td><input type='text'  class="input-text11" id="rss_Upl_IP2" name="rss_Upl_IP2" readonly style="background-color:#ccc">
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_mask);</script></font></th>
					</tr>
					<tr>
						<td><input type='text' class="input-text11" id="rss_Upl_NetMask2" name="rss_Upl_NetMask2" readonly style="background-color:#ccc"></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_gateway);</script></font></th>
					</tr>
					<tr>
						<td><input type='text' class="input-text11" id="rss_Upl_Gateway2" name="rss_Upl_Gateway2" readonly style="background-color:#ccc"></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_ip_dns);</script></font></th>
					</tr>
					<tr>
						<td>
							<input type='text' class="input-text11" id="rss_Upl_DNS2" name="rss_Upl_DNS2" readonly style="background-color:#ccc">
							<!--input type="text" class="input-text4" id="rss_Upl_DNS_check" name="rss_Upl_DNS_check" readonly-->
						</td>
					</tr>
				</table>
		</div>
	</div>
	<!--End ISP .asp -->
	<!--Wireless .asp -->
	<div class="body" id="body3" style="position:absolute;top:100px;bottom:100px;width:100%;overflow: scroll;display:none;">
		<div style="position:static;margin:20px;" id="total_wireless24">
			<table style="margin-left: auto;margin-right: auto;width:90%;height:280px;background-color:transparent">
				<th width="50%" style="text-align:left;"><font size="14"><script>prints(m_wireless_2);</script></font></th>
				<td width="50%" style="text-align:right;">
					<!--table style="margin-left: auto;margin-right: auto;width:20%;height:50px;background:#ccc; ">
						<tr><th><span style="border-bottom:1px solid #00FFFF;cursor:pointer;font-size:30px;" id="2.4g_enable1" onClick="enable_change(this.id);"></span></th></tr>
					</table-->
					<img id="wireless2_onoff" onClick="enable_change(this.id);" style="cursor:pointer;">
				</td>
			</table>
			<table id="wireless_2" style="margin-left: auto;margin-right: auto;width:90%;height:280px;text-align:left;">
					<tr>
						<th><font size="14"><script>prints(m_wireless_ssid);</script></font></th>
										<!--input type='text' class="input-text4" id="rss_Wl2_WirelessName_check" name="rss_Wl2_WirelessName_check" readonly--></td>
					</tr>
					<tr>
						<td><input type='text' class="input-text11" id="rss_Wl2_WirelessName" name="rss_Wl2_WirelessName" onkeyup="check_rssVarValidate(this.id)">
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_Wl2_WirelessName_check" name="rss_Wl2_WirelessName_check" readonly></td>
					</tr>
					<tr>
						<th><font size="14"><script>prints(m_wireless_mode);</script></font></th>
					</tr>
					<tr>
						<th>
							<select id="2.4g_option" name="rss_Wl2_EncryptionMode" onClick="changmode();">
								<!--option><script>prints(m_wireless_mode_none);</script></option-->
								<!--option>WEP</option-->
								<!--option>WPA</option-->
								<!--option><script>prints(m_wireless_mode_wpa2);</script></option-->
								<!--option><script>prints(m_wireless_mode_wpawpa2);</script></option-->

								<option value="None">None</option>
								<!--option>WEP</option-->
								<!--option>WPA</option-->
								<option value="WPA2">WPA2</option>
								<option value="WPAWPA2">WPAWPA2</option>
							</select>
						</th>
					</tr>
					<br>
					<tr>
						<td ><input type='text' class="input-text11" id="rss_Wl2_EncryptionKey" name="rss_Wl2_EncryptionKey" onkeyup="check_wireless()">
										<!--input type='text' class="input-text4" id="rss_Wl2_EncryptionKey_check" name="rss_Wl2_EncryptionKey_check" readonly--></td>
					</tr>
					<tr>
						<td><input type="text" class="input-text4" id="rss_Wl2_EncryptionKey_check" name="rss_Wl2_EncryptionKey_check" readonly></td>
					</tr>
				</table>
		</div>
		<div style="position:static;margin:20px;" id="total_wireless5">
			<table style="margin-left: auto;margin-right: auto;width:90%;height:280px;background-color:transparent">
				<th width="50%" style="text-align:left;"><font size="14"><script>prints(m_wireless_5);</script></font></th>
				<td width="50%" style="text-align:right;">
					<!--table style="margin-left: auto;margin-right: auto;width:20%;height:50px;background:#ccc; ">
						<tr><th><span style="border-bottom:1px solid #00FFFF;cursor:pointer;font-size:30px;" id="5g_enable1" onClick="enable_change(this.id);"></span></th></tr>
					</table-->
					<img id="wireless5_onoff" onClick="enable_change(this.id);" style="cursor:pointer;">
			</td>
			</table>
			<table id="wireless_5" style="margin-left: auto;margin-right: auto;width:90%;height:280px;text-align:left;">
				<tr>
					<th><font size="14"><script>prints(m_wireless_ssid);</script></font></th>
				</tr>
				<tr>
					<td><input type='text' class="input-text11" id="rss_Wl5_WirelessName" name="rss_Wl5_WirelessName" onkeyup="check_rssVarValidate(this.id)">
									<!--input type='text' class="input-text4" id="rss_Wl5_WirelessName_check" name="rss_Wl5_WirelessName_check" readonly--></td>
				</tr>
				<tr>
					<td><input type="text" class="input-text4" id="rss_Wl5_WirelessName_check" name="rss_Wl5_WirelessName_check" readonly></td>
				</tr>
				<tr>
					<th><font size="14"><script>prints(m_wireless_mode);</script></font></th>
				</tr>
				<tr>
					<th>
						<select id="5g_option" name="rss_Wl5_EncryptionMode"  onClick="changmode2();">
							<!--option><script>prints(m_wireless_mode_none);</script></option-->
							<!--option>WEP</option-->
							<!--option>WPA</option-->
							<!--option><script>prints(m_wireless_mode_wpa2);</script></option-->
							<!--option><script>prints(m_wireless_mode_wpawpa2);</script></option-->
			
							<option value="None">None</option>
                            <!--option>WEP</option-->
                            <!--option>WPA</option-->
                            <option value="WPA2">WPA2</option>
                            <option value="WPAWPA2">WPAWPA2</option>
						</select>
					</th>
				</tr>
				<br>
				<tr>
					<td><input type='text' class="input-text11" id="rss_Wl5_EncryptionKey" name="rss_Wl5_EncryptionKey" onkeyup="check_wireless()">
									<!--input type='text' class="input-text4" id="rss_Wl5_EncryptionKey_check" name="rss_Wl5_EncryptionKey_check" readonly--></td>
				</tr>
				<tr>
					<td><input type="text" class="input-text4" id="rss_Wl5_EncryptionKey_check" name="rss_Wl5_EncryptionKey_check" readonly></td>
				</tr>
			</table>
		</div>
	</div>
	<!--End Wireless .asp -->
	<!--Scan.asp-->
	<div class="body" id="body4" style="position:absolute;top:100px;bottom:100px;width:100%;overflow: scroll;display:none;">
		<div style="position:static;margin:20px;">
			<table style="width:100%;height:100%;">
				<tr height="10%" style="text-align:left;">
					<th><font size="10"><script>prints(m_wireless_choosewifi);</script></font></th>
				</tr>
				<tr>
					<th>
						<div style="margin-left: auto;margin-right: auto;width:80%;height:90%;overflow:scroll; " id="list">
							<table style="width:100%;">
							<tr><th><font size="20"><script>prints(m_wirelss_scan);</script></font></th></tr>
							</table>
						</div>
					</th>
				</tr>
			</table>
		</div>
	</div>
	<!--End Scan .asp -->
		<br>
		<div  class="footer" style="position:absolute;bottom:0px;">
			<table style="margin-left: auto;margin-right: auto;width:100%;height:100%;background:#ccc;border-collapse: collapse; ">
				<tr>
					<th style="cursor:pointer;border-right:2px solid black;text-align:left;" onclick="back();" width="50%"><font size="16" color="DodgerBlue">&nbsp;&nbsp;<&nbsp;<script>prints(m_back);</script></font></th>
					<th id="th_next" style="cursor:pointer;text-align:right;" onclick="next();" width="50%"><font size="16" color="DodgerBlue"> <script>prints(m_next);</script>&nbsp;&nbsp;</font></th>
					<th id="th_scan" style="cursor:pointer;display:none;text-align:right;" onclick="scan();" width="50%" color="DodgerBlue"><font size="16" color="DodgerBlue"><script>prints(m_scan);</script>&nbsp;&nbsp;</font></th>
				</tr>
			</table>
		</div>
	</div>
</body>

</html>
