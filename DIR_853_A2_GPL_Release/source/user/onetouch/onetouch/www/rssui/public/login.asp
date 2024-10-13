<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<meta http-equiv="Access-Control-Allow-Origin" content="*">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>

<link rel="stylesheet" href="./dbros.css" type="text/css">
<script src="./__md5.js" type="text/javascript"></script>

<script type="text/javascript">

var login_key="<% ConfigGet("/runtime/login_key"); %>";

function uiDoOnload()
{
	document.getElementById("login_name").value = "";
	document.getElementById("login_pass").value = "";
	document.getElementById("login_name").focus();
}

function uiSave()
{
    var login_name = document.getElementById("login_name").value;
    var login_pass = document.getElementById("login_pass").value;

	if(login_name == "")
	{
		alert("Username can't be empty!");
		return false;
	}
	/*if(login_pass == "")
	{
		alert("Password can't be empty!");
		return false;
	} */
	
    login_name = hex_md5(login_name);
	login_pass = hex_md5(login_pass);

	if(window.navigator.language){
        langCode=window.navigator.language;
    }
    else if(window.navigator.userLanguage){
        langCode=window.navigator.userLanguage;
    }
    if ((langCode=="zh-tw")||(langCode=="zh-TW")||(langCode=="zh_tw")||(langCode=="zh_TW"))
        langCode2 = "tw";
    else
        langCode2 = langCode.substr(0,2);
	
	// POST login info 
	document.getElementById("c1").value = login_name;
	document.getElementById("c2").value = login_pass;
	document.getElementById("key").value = login_key;
	document.getElementById("lang").value = langCode2;
    document.login_frm.submit();
    return;

    // GET login verification
    var url = "/rssui/public/checkuser.xgi?c1="+login_name+"&c2="+login_pass;
	url = url+"&key="+login_key;
	//alert(url);

   	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	}
	else
	{// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			var x = xmlhttp.responseXML.documentElement.getElementsByTagName("Result");
			if(x.length!=0)
			{
				//alert(x[0].firstChild.nodeValue);
                if (x[0].firstChild.nodeValue==0) {
	               location.href="/rssui/main.asp";
                }
                else {
                   alert("Invalid Username and/or Password.");
                }
			}
		}
	}
	xmlhttp.open("GET",url,true);
	xmlhttp.send();
	//xmlhttp.timeout = 15000;
}

function JumpByEnter(e)
{
	if(window.event) // IE
	{
		keynum = e.keyCode
	}
	else if(e.which) // Netscape/Firefox/Opera
	{
		keynum = e.which
	}
	if ( keynum == 13 )
	{
		document.getElementById("loginId").click();
       } 
}

</script>

</head>

<body onload="uiDoOnload()" onkeydown="JumpByEnter(event)" >
<div style="display:none;" align="center">
	<form name="login_frm" id="login_frm" action="/rssui/public/checkuser.xgi" method="POST">
    	<input type="hidden" name="c1" id="c1">
    	<input type="hidden" name="c2" id="c2">
    	<input type="hidden" name="key" id="key">
    	<input type="hidden" name="lang" id="lang">
	</form>
</div>
<div id="layer">
	<div class="header" style="position:absolute;top:0px;">
		<table style="margin-left: auto;margin-right: auto;width:100%;height:100%;background:#ccc;border-collapse: collapse; ">
			<tr>
				<th><font size="16">Login</font></th>
			</tr>
		</table>
	</div>
	<br>
	<div style="margin:150 20 5 20px;">
		<table style="margin-left: auto;margin-right: auto;width:90%;height:280px;background:#ccc; ">
			<tr>
				<th width="50%"><font size="12">Username&nbsp;:</font></th>
				<td width="50%"><input type="text" width="35%" class="input-text11" id="login_name" name="login_name"></td>
			</tr>
			<tr>
				<th width="50%"><font size="12">Password&nbsp;:</font></th>
				<td width="50%"><input type="password" width="35%" class="input-text11" id="login_pass" name="login_pass"></td>
			</tr>
		</table>
	</div>
	<br>
	<div style="height:25%;">
		<table style="height:100%;width:90%;">
			<tr>
				<th style="width:30%;"></th>
				<th style="width:70%;"><input name="loginId" id="loginId" type="button" value="Login" style="width:30%;font-size:40px;" onclick="uiSave()"></th>
			</tr>
		</table>
	</div>
</div>
</body>
</html>
