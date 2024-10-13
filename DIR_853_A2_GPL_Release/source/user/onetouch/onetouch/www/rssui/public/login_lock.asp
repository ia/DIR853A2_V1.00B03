<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<meta http-equiv="Access-Control-Allow-Origin" content="*">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>
<link rel="stylesheet" href="./dbros.css" type="text/css">

<script type="text/javascript">

function windowclose() {
    var browserName = navigator.appName;
    if (browserName=="Netscape") {
        window.open('', '_self', '');
        window.close();
    }
    else {
        //if (browserName == "Microsoft Internet Explorer"){
            window.opener = "whocares";
            window.opener = null;
            window.open('', '_top');
            window.close();
        //}
    }
}

function uiDoOnload()
{
	var locksec = "<% ConfigGet("/runtime/login_locksec"); %>";
	alert("Invalid Login Action, please try again after "+ locksec + " seconds!");
	windowclose();
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
