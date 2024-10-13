//=========================
//      Global Variable & Init Actions
//=========================
//var langCode = "en";
//var langCode2 = "en";
	if (window.navigator.language)     //在Netscape和Opera中
        langCode = window.navigator.language;    //取浏览器语言
    else if (window.navigator.userLanguage)   //在IE中
        langCode = window.navigator.userLanguage;  //取浏览器语言
    if ((langCode=="zh-tw")||(langCode=="zh-TW")||(langCode=="zh_tw")||(langCode=="zh_TW"))
        langCode2 = "tw";
    else
        langCode2 = langCode.substr(0,2);
// Vars used by function check_AuthKey
var check_AuthKey_ErrCnt = 3;
var theChkKeyTimer;
var theAuthKeyName;	// should be assigned by asp
var thAuthKeyVal;	// should be assigned by asp
// Vars used by function check_rssVarValidate
var validateErrTable = new Array();
// Vars used by function checkHtmChar
var htmAscTable = new Array();
// Vars used for RssEncryption
var isRssEncrypt;
var RssVarEncryptHeader;
var RssVarEncryptKey;

function public_js_init() {
	checkHtmChar_Init();
//	load_LangVarFile();		// Initialize all language string variables 
}

function RssDataEncrypt(str, key) {
    var len = str.length;
    var key_len = key.length;
    var i = 0, k_i = 0, kk, cc;
    out = "";
    while(i < len) {
		cc = str.charCodeAt(i++) & 0xff;
		kk = key.charCodeAt(k_i++);
		n = kk % 3;
		if (n==0) {
			if (cc!=(kk & 0xA5)) 
				cc ^= (kk & 0xA5); 
		} else if (n==1) {
			if (cc>= 32 && cc<= 128)
				cc = 160 - cc;
		}
		else if (n==2) {
			if (cc>= 32 && cc<= 79)
				cc += 48;
			else if (cc>= 80 && cc<= 127)
				cc -= 48; 
		}
		out += String.fromCharCode(cc);
        if (k_i >= key_len) k_i = 0;
	}
	return out;
}

function RandomNumber() {
	today = new Date();
	num = Math.abs(Math.sin(today.getTime()));
	return num;
}

function RssDataEncode(str) {
	if (isRssEncrypt!=1) return str;
	var rndNo = "0000"+RandomNumber();
	rndNo = rndNo.substring(rndNo.length-4);
    var the2ndKey = RssDataEncrypt(RssVarEncryptKey, rndNo);
    var dd = RssVarEncryptHeader+rndNo+RssDataEncrypt(utf16to8(str), the2ndKey);
    //alert("mask='"+rndNo+"'\n2nd-Key='"+the2ndKey+"'\ndata='"+dd+"'");
	return base64encode(RssDataEncrypt(dd, RssVarEncryptKey));
}

function RssDataDecode(str) {
	if (isRssEncrypt!=1) return str;
    var dd = RssDataEncrypt(base64decode(str), RssVarEncryptKey);
	var rndNo = dd.substring(RssVarEncryptHeader.length, RssVarEncryptHeader.length+4);
    var the2ndKey = RssDataEncrypt(RssVarEncryptKey, rndNo);
    //alert("mask='"+rndNo+"'\n2nd-Key='"+the2ndKey+"'\ndata='"+dd+"'");
    return utf8to16(RssDataEncrypt(dd.substring(RssVarEncryptHeader.length+4), the2ndKey));	
}

function chgElementEnable(elemName, isEnabled)  {
	if (isEnabled!=0)
	{
   		document.getElementById(elemName).style.color = "DodgerBlue";
   		document.getElementById(elemName).fontcolor = "DodgerBlue";
		eval("is_"+elemName+"_Enabled=1;");
	}
	else
	{
   		document.getElementById(elemName).style.color = "#707070";
   		document.getElementById(elemName).fontcolor = "#707070";
		eval("is_"+elemName+"_Enabled=0;");
	}
} 

function doClickEvent(elemName)  {  
    if(document.all)  {  
       	document.getElementById(elemName).click(); 
    }  
    else  {  
        var evt = document.createEvent("MouseEvents");  
        evt.initEvent("click", true, true);  
        document.getElementById(elemName).dispatchEvent(evt);  
    }  
} 

function updateInnerHTML(elemName, theMsg) {	
	document.getElementById(elemName).innerHTML = theMsg;
}

function getXmlhttpTagValue(xmlHttpObj, name) 	{ 
	var theTag = xmlHttpObj.responseXML.documentElement.getElementsByTagName(name);
	if (theTag.length != 0) {
		if (theTag[0].firstChild==null)
			return "";
		else
			return theTag[0].firstChild.nodeValue;
	}
	else
		return null; 
}

function createXMLHttpRequest() 
{ 
    var newXmlHttp;
    if(window.XMLHttpRequest) {
        newXmlHttp = new XMLHttpRequest(); 	//Mozilla Browser for IE7+, Firefox, Chrome, Opera, Safari
    }
    else if(window.ActiveXObject) {
        try {  
			newXmlHttp = new ActiveXObject("Msxml2.XMLHTTP");  // IE Broweser
        } catch (e) {
            try {  
				newXmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {
            	newXmlHttp = null;
            }
        }
    }
    return newXmlHttp;
} 

function load_LangVarFile()
{
	var xmlhttp = createXMLHttpRequest();

	if (window.navigator.language)     //在Netscape和Opera中
		langCode = window.navigator.language;    //取浏览器语言
	else if (window.navigator.userLanguage)   //在IE中
		langCode = window.navigator.userLanguage;  //取浏览器语言
    if ((langCode=="zh-tw")||(langCode=="zh-TW")||(langCode=="zh_tw")||(langCode=="zh_TW")) 
		langCode2 = "tw";
	else
		langCode2 = langCode.substr(0,2);
	
    xmlhttp.onreadystatechange = function()  {
        if (xmlhttp.readyState==4) { 
			if (xmlhttp.status==200)  {
	        	//alert("language="+langCode2);
				document.write("<script type=\"text/javascript\">"+xmlhttp.responseText+"<\/script>");
	        }
	        else if (xmlhttp.status==404) {
	        	//alert("language=en")
				document.write("<script src=\"language_en.js\" type=\"text/javascript\"><\/script>");
	        }
		}
    }
    xmlhttp.open("GET", "language_"+langCode2+".js", false);
    xmlhttp.send();
}

function check_AuthKey()
{
	if (!document.getElementById("timeout"));
	{
	    var newDiv = document.createElement("div");  
	    document.body.appendChild(newDiv);  
	    newDiv.setAttribute("id", "timeout"); 
	    newDiv.setAttribute("style", "top:240px;left:0px;width:100%;z-index:99;display:none;");
		newDiv.setAttribute("align", "center");
		var cc = "<div class='press_pwd'>";
		cc += " <div style='height:75%;border-bottom:2 solid Aqua;font-size:25px;'>";
		cc += "  <table style='height:100%;width:100%;font-size:25px;'>";
		cc += "   <tr><th>"+m_timeout+"</th></tr>";
		cc += "  </table>";
		cc += " </div>";
		cc += " <div style='height:25%;'>";
		cc += "  <table style='height:100%;width:100%;font-size:25px;'>";
  		cc += "   <tr><th><input id='timeout_close' type='button' value='Close' style='width:50%;font-size:25px;' onClick='windowclose()'></th></tr>";  
		cc += "  </table>";
		cc += " </div>";
		cc += "</div>";
		updateInnerHTML("timeout", cc);
	}
	if (check_AuthKey_ErrCnt==0)
		document.getElementById("timeout").style.display="block";
	else
	{
		check_AuthKey_ErrCnt--;
		var xmlhttp = createXMLHttpRequest();
		xmlhttp.onreadystatechange = function() {
			if (xmlhttp.readyState==4 && xmlhttp.status==200) {
				var tmp = getXmlhttpTagValue(xmlhttp, "RssKey");
				if (!(tmp===null)) { 
					thAuthKeyVal = tmp;
					check_AuthKey_ErrCnt = 3;
				}
				else check_AuthKey_ErrCnt = 0;
			}
		};
		
		try {
			xmlhttp.timeout = 15000;
		} catch (e) {}	
		xmlhttp.ontimeout = function(){  
	    	check_AuthKey_ErrCnt--;
		} 

		xmlhttp.open("GET", "/tmp/rss_keyupdate.xml?"+theAuthKeyName+"="+thAuthKeyVal, true);
		xmlhttp.send();
	}
	theChkKeyTimer = setTimeout("check_AuthKey()", 30000);
}

/**** Check RssVar value validate *****/
function check_rssVarValidate(nodeID)
{
	if(nodeID == "rss_DevicePwd")
		document.getElementById("rss_DevicePwd_check").style.color="Red";
	var xmlhttp = createXMLHttpRequest();
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState==4 && xmlhttp.status==200) {
			var info = getXmlhttpTagValue(xmlhttp, "Result");
			var chkmsg = getXmlhttpTagValue(xmlhttp, "RssChkMessage");
			var errmsg;
			if (!(info===null)) {
				if (info==0)
					errmsg = "";
				else if (info==3 || info == 11)
					erm_error_content;
				else if (info==10)
					errmsg = m_error_length;
				else if (info==99)	// vendor defined message 
					errmsg = chkmsg;
				else 
					errmsg = m_error_unknown; 
				
				if (info==0)
					parse_ValidateErr_index(nodeID, 1);
				else
					parse_ValidateErr_index(nodeID, 0);
				
				document.getElementById(nodeID+"_check").value = errmsg;
			}
		}
	}
	var value = document.getElementById(nodeID).value;
	xmlhttp.open("GET", "/tmp/rssvar_checkresult.xml?rssVar="+nodeID+"&rssVal="+subHtmAsc(value)+"&msgLang="+langCode+"&t="+Rand(1000), true);
	xmlhttp.send();
	document.getElementById(nodeID+"_check").style.display="block";
}

//flag=0,Add; flag=1,Delete;
function parse_ValidateErr_index(id, flag)
{
	var count=0;
	if(flag == 0)
	{
		if(validateErrTable.length==0)
			validateErrTable.push(id);
		else
		{
			for(var index=0;index<validateErrTable.length;index++)
			{
				if(validateErrTable[index] == id)
					count++;
			}
			if(count == 0)
				validateErrTable.push(id);
		}
	}
	else if(flag==1)
	{
		for(var index=0;index<validateErrTable.length;index++)
		{
			if(validateErrTable[index] == id)
				validateErrTable[index]=0;
		}
	}
}
/**** End Check value validate *****/

function generateMixed(n) {
    var res = "";
  	var tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for(var i = 0; i < n ; i ++) {
        res += tab.charAt(Math.ceil(Math.random()*61));
    }
    return res;
}

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

function prints(context)
{
    document.write(context);
}

function isHttpChar(a, h)
{
    this.a = a;
    this.h = h;
}

function checkHtmChar_Init()
{
	htmAscTable[0] = new isHttpChar("#", "%23");
	htmAscTable[1] = new isHttpChar("%", "%25");
	htmAscTable[2] = new isHttpChar("&", "%26");
	htmAscTable[3] = new isHttpChar("+", "%2b");
	htmAscTable[4] = new isHttpChar("/", "%2f");
	htmAscTable[5] = new isHttpChar(":", "%3a");
	htmAscTable[6] = new isHttpChar(";", "%3b");
	htmAscTable[7] = new isHttpChar("<", "%3c");
	htmAscTable[8] = new isHttpChar("=", "%3d");
	htmAscTable[9] = new isHttpChar(">", "%3e");
	htmAscTable[10] = new isHttpChar("?", "%3f");
	htmAscTable[11] = new isHttpChar("@", "%40");
	htmAscTable[12] = new isHttpChar(" ", "%20");
	htmAscTable[13] = new isHttpChar("$", "%24");
	htmAscTable[14] = new isHttpChar("'", "%27");
	htmAscTable[15] = new isHttpChar('"', "%22");
	htmAscTable[16] = new isHttpChar("^", "%5e");
	htmAscTable[17] = new isHttpChar("*", "%2a");
	htmAscTable[18] = new isHttpChar("(", "%28");
	htmAscTable[19] = new isHttpChar(")", "%29");
}

function checkHtmChar(c)
{
	for(var i=0; i< htmAscTable.length; i++)
	{
		if (c == htmAscTable[i].a)
		  return htmAscTable[i].h;
	}
	return c;
}
function subHtmAsc(str)
{
	var regStr=/(#|%|&|\+|\/|:|;|<|=|>|\?|@|\ |$|\'|\"|^|\*|\(|\))+/;
	var rntStr="";

	if (str.search(regStr) == -1)
		return str;

	for(var i=0; i<str.length; i++)
	{
		rntStr+=checkHtmChar(str.charAt(i))
	}
	return rntStr;
}

function Rand(maxInt)
{
	return Math.round(Math.random()*maxInt);
}
