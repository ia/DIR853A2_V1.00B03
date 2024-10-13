var upgFwState = 0, bootTime, newFwVer, newFwSize, errActMsg;
var fwXmlTimer, fwXmlErrNo = 0;
var fwNotesTimer, fwNotesErrNo = 0;
var fwXmlAct, waitSec, isExistManyFW = 0;

var theUrl_FwChkXml = "/tmp/rsscheckfw.xml";
var theUrl_FwNotesXml = "/tmp/rssfwnotes.xml";

var is_Back_Enabled = 1;
var is_Upgrade_Enabled = 0;

var	FWUPGRADE_ERR_FWXML = -1;
var	FWUPGRADE_ERR_BIN = -2;
var	FWUPGRADE_ERR_ACT = -3;
var	FWUPGRADE_ERR_TIMEOUT = -4;
var	FWUPGRADE_ERR_WAN = -5;
var	FWUPGRADE_CHKXML = 0;
var	FWUPGRADE_XMLGETTING = 1;
var	FWUPGRADE_XMLREADY = 2;
var	FWUPGRADE_FINDNEW = 10;
var	FWUPGRADE_BINGETTING = 11;
var	FWUPGRADE_BINREADY = 20;
var	FWUPGRADE_WRITINGFLASH = 30;
var	FWUPGRADE_UPToDATE = 91;

var	FWUPGRADE_ACT_INIT = 0;
var	FWUPGRADE_ACT_SEARCH = 1;
var	FWUPGRADE_ACT_DOWNLOAD = 2;
var	FWUPGRADE_ACT_CHKBIN = 3;
var	FWUPGRADE_ACT_UPGRADE = 4;
var	FWUPGRADE_ACT_NONE = 9;

function updateOldMsg(theMsg) 
{	
	document.getElementById("info4").innerHTML = theMsg;
}

function updateNewMsg(theMsg) 
{	
	document.getElementById("info2").innerHTML = theMsg;
}

function updateFwStatusMsg() {	
	if (upgFwState == FWUPGRADE_ERR_FWXML)
		updateNewMsg(theMsg_FwXmlErr);
	else if (upgFwState == FWUPGRADE_ERR_BIN)
		updateNewMsg(theMsg_FwBinErr);
	else if (upgFwState == FWUPGRADE_ERR_ACT) {
		updateNewMsg(theMsg_ActErr);
		alert(errActMsg);
	}
	else if (upgFwState == FWUPGRADE_ERR_TIMEOUT)
		updateNewMsg(theMsg_TimeOutErr);
	else if (upgFwState == FWUPGRADE_ERR_WAN)
		updateNewMsg(theMsg_WanErr);
	else if (upgFwState == FWUPGRADE_CHKXML)
		updateNewMsg(theMsg_ChkFwXml);
	else if (upgFwState == FWUPGRADE_UPToDATE)
		updateNewMsg(theMsg_FwUpToDate);
	else if (upgFwState == FWUPGRADE_FINDNEW) { 
		updateNewMsg(newFwVer);
		chgElementEnable("Upgrade", 1); 	// enable "Upgrade" button
	}	
	else if (upgFwState == FWUPGRADE_BINGETTING) 
		updateNewMsg(newFwVer+" ("+newFwSize+")");
	else if (upgFwState == FWUPGRADE_BINREADY)
		updateNewMsg(newFwVer+" ("+newFwSize+")");
	else if (upgFwState == FWUPGRADE_XMLGETTING)
		updateNewMsg(theMsg_ChkFwXml+" ...");
	else if (upgFwState == FWUPGRADE_XMLREADY)
		updateNewMsg(theMsg_ChkFwXml);
	else if (upgFwState == FWUPGRADE_WRITINGFLASH) {
		updateOldMsg(newFwVer);
		if (waitSec>0) {
			updateNewMsg(theMsg_WriteBIN+waitSec+" sec<br>"+theMsg_PowerWarning);
			waitSec--;
			setTimeout("updateFwStatusMsg()", 1000);
			clearTimeout(theChkKeyTimer);		
		}
		else {
			updateNewMsg(theMsg_WriteDone);
		}
	}
}

function doUpgrade()
{
	if (is_Upgrade_Enabled==1) { // upgFwState == FWUPGRADE_FINDNEW
		if (isExistManyFW == 1) 
			alert(m_twice_upgrade);
		updateNewMsg(newFwVer+" (Downloading)");
		fwXmlAct = FWUPGRADE_ACT_DOWNLOAD;	
	    loadFwChkXml();	// Download -->  Write to Flash
		chgElementEnable("Back", 0); 		// disable "Back" button
		chgElementEnable("Upgrade", 0); 	// disable "Upgrade" button
	}
}

function loadFwChkXml()
{
	var xmlhttp;

	xmlhttp = createXMLHttpRequest();

	updateFwStatusMsg();
	xmlhttp.open("GET", theUrl_FwChkXml+"?Act="+fwXmlAct+"&t="+Rand(1000), true);
	xmlhttp.send();

	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState == 4) {
			//alert("upgFwState="+upgFwState+", fwXmlAct="+fwXmlAct+", loadFwChkXml.status="+xmlhttp.status);
			if (xmlhttp.status == 200) {
				fwXmlErrNo = 0;	// clear ErrNo
				upgFwState = getXmlhttpTagValue(xmlhttp, "result");

				if (upgFwState == FWUPGRADE_ERR_ACT)
					errActMsg = getXmlhttpTagValue(xmlhttp, "erractmsg");
				else if (upgFwState == FWUPGRADE_CHKXML){	
					isExistManyFW = 0;
					fwXmlAct = FWUPGRADE_ACT_SEARCH;	
					fwXmlTimer = setTimeout("loadFwChkXml()", 1000);
				}
				else if (upgFwState == FWUPGRADE_XMLGETTING){
					fwXmlAct = FWUPGRADE_ACT_SEARCH;	
					fwXmlTimer = setTimeout("loadFwChkXml()", 1000);
				}
				else if (upgFwState == FWUPGRADE_XMLREADY){
					fwXmlAct = FWUPGRADE_ACT_SEARCH;	
					fwXmlTimer = setTimeout("loadFwChkXml()", 1000);
				}
				else if (upgFwState == FWUPGRADE_FINDNEW) {	
					newFwVer = m_newfw+getXmlhttpTagValue(xmlhttp, "fwver");
					isExistManyFW = getXmlhttpTagValue(xmlhttp, "manyfw");
					// Load FW Release Notes
					fwNotesTimer = setTimeout("loadFwNotes()", 1000);
				}
				else if (upgFwState == FWUPGRADE_BINGETTING){
					newFwSize = getXmlhttpTagValue(xmlhttp, "fwsize");
					fwXmlAct = FWUPGRADE_ACT_CHKBIN;	
					fwXmlTimer = setTimeout("loadFwChkXml()", 1000);
				}
				else if (upgFwState == FWUPGRADE_BINREADY) {	
					newFwSize = getXmlhttpTagValue(xmlhttp, "fwsize");
					fwXmlAct = FWUPGRADE_ACT_UPGRADE;	
					fwXmlTimer = setTimeout("loadFwChkXml()", 3000);
				}
				else if (upgFwState == FWUPGRADE_WRITINGFLASH) 	
					bootTime = getXmlhttpTagValue(xmlhttp, "time");

				updateFwStatusMsg();
			    //alert("New>upgFwState="+upgFwState+", fwXmlAct="+fwXmlAct);
				
				if (upgFwState == FWUPGRADE_WRITINGFLASH)  {
					//alert("Please wait "+bootTime+" seconds and don't turn off the power of device !!!");
					waitSec = bootTime;
					setTimeout("updateFwStatusMsg()", 1000);
				}
			}
			else {	// isn't 200 OK
				upgFwState = FWUPGRADE_ERR_TIMEOUT;
				updateFwStatusMsg();
				alert(m_alert_error+xmlhttp.status);
			}
		}
	}

	xmlhttp.ontimeout = function(){  
		upgFwState = FWUPGRADE_ERR_TIMEOUT;	
		updateFwStatusMsg();
	} 
}

function loadFwNotes()
{
	var xmlhttp;

	xmlhttp = createXMLHttpRequest();

	xmlhttp.open("GET", theUrl_FwNotesXml+"?t="+Rand(1000), true);
	xmlhttp.timeout = 10000;
	xmlhttp.send();
	if (fwNotesErrNo < 6) {
		fwNotesTimer = setTimeout("loadFwNotes()", 5000);
	}	
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4) {
			if (xmlhttp.status==200) { 
				document.getElementById("info").innerHTML=xmlhttp.responseText;
				clearTimeout(fwNotesTimer);
			}
			else {
		    	fwNotesErrNo++;
			}
		}
	}
	
	xmlhttp.ontimeout = function() {  
    	fwNotesErrNo += 2;
	} 
}

function chkBack()
{
	if (is_Back_Enabled==1)
		window.history.go(-1);
}

function init_page()
{
	public_js_init();
	theChkKeyTimer = setTimeout("check_AuthKey()",30000);
	document.getElementById("info4").innerHTML=m_currfw+curfw;
	fwXmlAct = FWUPGRADE_ACT_SEARCH;
	loadFwChkXml();
	chgElementEnable("Upgrade", 0); 	// disable "Upgrade" button
}
