<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<meta http-equiv="Access-Control-Allow-Origin" content="*">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>
<link rel="stylesheet" href="./dbros.css" type="text/css">

<script type="text/javascript">
function windowreturn() {
     self.location.href="/rssui/main.asp";
}
function uiDoOnload()
{
	alert("Invalid login page.");
	windowreturn();
}
</script>

</head>
<body onload="uiDoOnload()">
<div id="layer">
	<div class="header" style="position:absolute;top:0px;">
		<table style="margin-left: auto;margin-right: auto;width:100%;height:100%;background:#ccc;border-collapse: collapse; ">
			<tr>
				<th><font size="16">Login</font></th>
			</tr>
		</table>
	</div>
</div>
</body>
</html>

