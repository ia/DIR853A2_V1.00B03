<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>
<link rel="stylesheet" href="./public/dbros.css" type="text/css">

<script src="public.js" type="text/javascript"></script>
<script type="text/javascript">
function go_next()
{
	location.href="config.asp?lang="+langCode2+"?";
}

function go_next2()
{
	location.href="fwupgrade.asp?lang="+langCode2+"?";
}
function go_next3()
{
	location.href="fwupgrade2.asp?lang="+langCode2+"?";
}

var lanip="<% ConfigRssGet("rss_Var_LanIP"); %>";
var thAuthKeyVal="<% Generate_Key(); %>";
var theAuthKeyName="<% ConfigRssGet("rss_Var_AuthKeyName"); %>"; // "DKey";
var factory_flag="<% ConfigGet("/sys/factorydefault"); %>";
var isChkKey_flag="<% ConfigGet("/runtime/checkstarterkey"); %>";
// Check Model DSL-2875AL, due to inverse of factory_flag
var device_model="<% ConfigGet("/sys/modelname"); %>";
if(device_model == "DSL-2875AL")
	factory_flag=1-factory_flag;
</script>
<script src="language_<% ConfigGet("/runtime/web_lang"); %>.js" type="text/javascript"></script>
</head>
<!--body  style="background:#FDF5E6;" onload="init_page()"-->
<body onload="init_page()">
<div id="layer">
	<table style="width:100%;height:100%;">
		<!--tr class="header">
			<th colspan="2"><font size="16"><% ConfigRssGet("rss_Var_OTTitle"); %></font></th>
		</tr-->
		<tr>
				<th><div id="step_1"><p onclick="go_next();" style="cursor:pointer;"><img src="img/ic_network_starter.png" style="width:200;height:200;"></p></div></th-->
				<td><div><font size="24" style="cursor:pointer;" onclick="go_next();"><b><script>prints(m_setup);</script></b></font><br><font size="8" onclick="go_next();"><script>prints(m_setup_detail);</script></font></div></td>
		</tr>
		<tr>
				<th><div id="fwupgd_1" onclick="go_next2();" style="cursor:pointer;"><img src="img/ic_fw_controller.png" style="width:180;height:180;"></div></th>
				<td><div id="fwupgd_2" onclick="go_next2();" style="cursor:pointer;"><font size="24"><b><script>prints(m_firm_upgrade);</script></b></font><br><font size="8"><script>prints(m_firm_upgrade_detail);</script></font></div></td>
		</tr>
		<tr>
				<th><div id="fwupgd2_1" onclick="go_next3();" style="cursor:pointer;display:none;"><img src="img/ic_fw_controller.png" style="width:200;height:200;"></div></th>
				<td><div id="fwupgd2_2" onclick="go_next3();" style="cursor:pointer;display:none;"><font size="24"><b><script>prints(m_firm_upgrade_local);</script></b></font><br><font size="8"><script>prints(m_firm_upgrade_detail);</script></font></div></td>
		</tr>
	</table>
</div>
<script type="text/javascript">

function init_page()
{
	public_js_init();

	if(factory_flag==1)	{
		if(isChkKey_flag=="0") {
			document.getElementById("fwupgd2_1").style.display="block";
			document.getElementById("fwupgd2_2").style.display="block";
		}
	}
	setTimeout("check_AuthKey()",30000);
}
</script>
</body>

</html>

