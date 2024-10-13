<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>
<link rel="stylesheet" href="./public/dbros.css" type="text/css">
<script type="text/javascript">
	var lanip="<% ConfigRssGet("rss_Var_LanIP"); %>";
	var thAuthKeyVal="<% Generate_Key(); %>";
	var theAuthKeyName="<% ConfigRssGet("rss_Var_AuthKeyName"); %>"; // "DKey";
	var curfw="<% ConfigRssGet("rss_Ext_Fwu_CurrFwVersion"); %>";

</script>
<script src="public.js" type="text/javascript"></script>
<script src="fwupgrade.js" type="text/javascript"></script>
<script src="language_<% ConfigGet("/runtime/web_lang"); %>.js" type="text/javascript"></script>
<body onload="init_page()">
<div id="layer">
	<div class="header" style="position:absolute;top:0px;">
		<table style="margin-left: auto;margin-right: auto;width:100%;height:100%;background:#ccc;border-collapse: collapse; ">
			<tr>
				<th><font size="16"><script>prints(m_firm_upgrade);</script></font></th>
			</tr>
		</table>
	</div>
	<br>
	<div class="body" id="body1" style="position:absolute;top:100px;bottom:100px;width:100%;overflow: scroll;display:block;">
		<table style="width:100%;height:100%;">
			<tr style="width:100%;" align="center">
				<th colspan="2"><font id="info4" size="18"></font></th>
			</tr>
			<tr style="width:100%;" align="center">
				<th colspan="2"><font id="info2" size="18"></font></th>
			</tr>
			<tr style="width:100%;" align="center">
				<th colspan="2"><textarea id="info" rows=12 style="width:80%;background:transparent;border-style:none;font-size:26px;"></textarea></th>
			</tr>
		</table>
	</div>
	<br>
	<div class="footer" style="position:absolute;bottom:0px;">
	<table style="margin-left: auto;margin-right: auto;width:100%;height:100%;background:#ccc;border-collapse: collapse; ">
		<tr>
			<th style="cursor:pointer;border-right:2px solid black;text-align:left;" onclick="chkBack();" width="50%"><font size="16" color="DodgerBlue" id="Back">&nbsp;&nbsp;<&nbsp;<script>prints(m_back);</script></font></th>
			<th style="cursor:pointer;text-align:right;" onclick="doUpgrade();" width="50%"><font size="16" color="DodgerBlue" id="Upgrade"><script>prints(m_upgrade);</script>&nbsp;&nbsp;</font></th>
		</tr>
		</table>
	</div>
</div>
</body>
</html>

