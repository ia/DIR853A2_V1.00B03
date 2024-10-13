var flag_survey="";
var siteList="";

function loadXMLDoc2(url)
{
	var xmlhttp,x;
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
			eval(xmlhttp.responseText);
			//alert(xmlhttp.responseText);
			if(flag_survey==0)
			{
				//alert(flag_survey);
				document.getElementById("list").innerHTML="";
				init_page();
			}
			else
			{
				setTimeout("check_survey(0)",3000);
			}
				
		}
	}
	xmlhttp.open("GET",url,true);
	xmlhttp.send();
}

function check_survey(isDoSurvey)
{
	urlsite="check_survey.asp";
	if (isDoSurvey) 
    	urlsite=urlsite+"?rss_Upl_HardwareType=WWan802.11&rss_Act_SiteSurvey=1";
    loadXMLDoc2(urlsite);
}

function init_page()
{	
	var src="";
	var txt="";
	for(var i=0;i<siteList.length;i++)
	{
		if(siteList[i][4]<25)
			src="./img/wireless_icon_25.png";
		else if(siteList[i][4]>=25 && siteList[i][4]<50)
			src="./img/wireless_icon_50.png";
		else if(siteList[i][4]>=50 && siteList[i][4]<75)
			src="./img/wireless_icon_75.png";
		else if(siteList[i][4]>=75 && siteList[i][4]<=100)
			src="./img/wireless_icon_100.png";

		var theClick = "class=\"pointer\" onClick=\"show_window(siteList["+i+"]);\""

		txt+="<div style=\"margin:25px 20px;\">";
		txt+= "<table style=\"width:100%;\">";
		txt+=  "<tr>";
		txt+=   "<th style=\"width:80%;white-space:nowrap;overflow:hidden;\">";
		txt+=    "<font "+theClick+" style=\"text-align:left;float:left;font-size:45px;\">"+siteList[i][2]+"</font>";
		txt+=   "</th>";
		txt+=   "<td rowspan=\"2\" style=\"width:20%;\">";
		txt+=    "<image "+theClick+"style=\"float:right;height:80px;\" src=\""+src+"\">";
		txt+=   "</td>";
		txt+=  "</tr>";
		txt+=  "<tr>";
		txt+=   "<th>";
		txt+=   "<font "+theClick+" style=\"text-align:left;float:left;font-size:25px;\">"+siteList[i][1]+"</font>";
		txt+=   "</th>";
		txt+=  "</tr>";
		txt+= "</table>";
		txt+="</div>";

	}
	document.getElementById("list").innerHTML=txt;
}

var name;
function show_window(name)
{
	document.getElementById("panel-wrap").style.display="block";
	document.getElementById("Loading").style.display="block";
	document.getElementById("ssid_name").innerHTML="&nbsp&nbsp"+name[2];
	document.getElementById("rss_Ets_connrefsitebssid").value=name[1];
	document.getElementById("rss_Ets_wirelesstype").value=name[0];
	document.getElementById("rss_Ets_wirelessname").value=name[2];
	//document.getElementById("rss_Ets_encryptionkey").value=name[3];
	// Clear Objects
	document.getElementById("passwd_check").checked = false;
	document.getElementById("passwd").value = "";
	document.getElementById("passwd").type="password";
}

function close_window()
{
	document.getElementById("panel-wrap").style.display="none";
	document.getElementById("Loading").style.display="none";
}

function scan()
{
	/*txt="";
	txt+="&rss_Act_SiteSurvey=1";
	location.href="scan.asp?"+txt;*/
	check_survey(1);
}

function display_pwd()
{
	if(document.getElementById("passwd_check").checked == true)
		document.getElementById("passwd").type="text";
	else
		document.getElementById("passwd").type="password";
}

function connect()
{
	/*document.getElementById("rss_Ets_encryptionkey").value=document.getElementById("passwd").value;
	with ( document.forms[0] ) {
	document.forms[0].submit();
	return true;
	}*/
	txt+="&rss_Upl_Ets_ConnRefSiteBSSID="+document.getElementById("rss_Ets_connrefsitebssid").value;
	txt+="&rss_Upl_Ets_WirelessType="+document.getElementById("rss_Ets_wirelesstype").value;
	txt+="&rss_Upl_Ets_WirelessName="+subHtmAsc(document.getElementById("rss_Ets_wirelessname").value);
	txt+="&rss_Upl_Ets_EncryptionKey="+subHtmAsc(RssDataEncode(document.getElementById("passwd").value));
	txt+="&rss_Upl_ConnType=DHCP";
	location.href="finish.asp?lang="+langCode2+"?"+txt;
}
