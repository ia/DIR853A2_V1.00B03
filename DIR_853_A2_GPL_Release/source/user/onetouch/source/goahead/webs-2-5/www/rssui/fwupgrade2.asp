<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>

<link rel="stylesheet" href="./public/dbros.css" type="text/css">

<script src="public.js" type="text/javascript"></script>
<script src="language_<% ConfigGet("/runtime/web_lang"); %>.js" type="text/javascript"></script>

<body onload="init_page()">
<div style="display:none;" align="center">
	<iframe name="UploadResult" id="UploadResult" src="about:blank">
	</iframe><br>
</div>
<div id="layer">
	<div class="header" style="position:absolute;top:0px;">
		<table style="margin-left: auto;margin-right: auto;width:100%;height:100%;background:#ccc;border-collapse: collapse; ">
			<tr>
				<th><font size="16"><script>prints(m_firm_upgrade_local);</script></font></th>
			</tr>
		</table>
	</div>
	<br>
	<div class="body" id="body1" style="position:absolute;top:100px;bottom:100px;width:100%;display:block;">
		<table style="width:100%;height:100%;">
			<tr style="width:100%;" align="center">
				<th colspan="2"><font id="currFwVer" size="18"></font></th>
			</tr>
			<tr style="width:100%;" align="center">
				<th colspan="2"><font id="upgdStatus" size="18"></font></th>
			</tr>
			<tr id="fwbox" style="width:100%;" align="center">
				<th colspan="2">
					<form id="fwupdate" name="fwupdate" action=/goform/rssfwupgd.xgi method=POST enctype="multipart/form-data" target="UploadResult" >
					    <font size="18">New FW : <input id="uploadfile" type="file" name="uploadfile" value="" style="font-size:35px;"/>  
					    <input type="submit" value="Upload" style="display:none" /></font> 
					</form>
				</th>
			</tr>
		</table>
	</div>
	<br>
	<div class="footer" style="position:absolute;bottom:0px;">
		<table style="margin-left: auto;margin-right: auto;width:100%;height:100%;background:#ccc;border-collapse: collapse; ">
			<tr>
				<th style="cursor:pointer;border-right:2px solid black;text-align:left;" onclick="doBack();" width="50%"><font size="16" color="DodgerBlue" id="Back">&nbsp;&nbsp;<&nbsp;<script>prints(m_back);</script></font></th>
				<th style="cursor:pointer;text-align:right;" onclick="doUpgrade();" width="50%"><font size="16" color="DodgerBlue" id="Upgrade"><script>prints(m_upgrade);</script>&nbsp;&nbsp;</font></th>
			</tr>
		</table>
	</div>
<div>

<script type="text/javascript">
 
	var lanip="<% ConfigRssGet("rss_Var_LanIP"); %>";
	var thAuthKeyVal="<% Generate_Key(); %>";
	var theAuthKeyName="<% ConfigRssGet("rss_Var_AuthKeyName"); %>"; // "DKey";
	var curfw="<% ConfigRssGet("rss_Ext_Fwu_CurrFwVersion"); %>";

	var upgFwState, waitSec, uploadPercent;
	var fwXmlTimer;
    var is_Back_Enabled = 1;
    var is_Upgrade_Enabled = 1;
	var isIE = (navigator.appName=="Microsoft Internet Explorer");
	//alert("isIE="+isIE);

    function UpladFile() {
        var fileObj = document.getElementById("uploadfile").files[0]; 
        var FileController = "/goform/rssfwupgd.xgi"; 

        // FormData Object
        var form = new FormData();
        form.append("uploadfile", fileObj);         

        // XMLHttpRequest Object
        var xhr = createXMLHttpRequest();

        xhr.onload = function () {
            //alert("Upload Finished!");
        };

    	xhr.onreadystatechange = function()	{
            if (xhr.readyState==4 && xhr.status==200)
    		{
    			//alert(xhr.responseText);
    			upgFwState = getXmlhttpTagValue(xhr, "FwUpgdResult");
    			waitSec = getXmlhttpTagValue(xhr, "RssGetWaitSec");
				//alert("upgFwState="+upgFwState+", RssGetWaitSec="+waitSec);
				if (upgFwState===null) {
					upgFwState = -998;
				} 
				updateFwStatusMsg();
				if (upgFwState != 0) {
					chgElementEnable("Back", 1); 		// enable "Back" button
					chgElementEnable("Upgrade", 1); 	// enable "Upgrade" button
				}
    		}
    	};

        xhr.open("post", FileController, true);
        xhr.upload.addEventListener("progress", progressFunction, false);
        xhr.send(form);
    }

    function progressFunction(evt) {
    	uploadPercent = Math.round(evt.loaded / evt.total * 100);
    	updateNewMsg(theMsg_UploadBIN+uploadPercent+"%");
    }  

	function updateNewMsg(theMsg) {	
		updateInnerHTML("upgdStatus", theMsg);
	}

	function updateFwStatusMsg() {	
		if (upgFwState == -1) {			//	doing upload FW to device
			updateNewMsg(theMsg_UploadBIN+uploadPercent+"%");		
		}
		else if (upgFwState == -999){	// 	loadFwChkXml Timeout
			updateNewMsg("Connection Timeout!");
			clearTimeout(theChkKeyTimer);		
			chgElementEnable("Back", 1); 		// enable "Back" button
			chgElementEnable("Upgrade", 1); 	// enable "Upgrade" button
		}
		else if (upgFwState == -998){	// 	loadFwChkXml Timeout
			updateNewMsg("Firmware Upgrade Fail!");
			clearTimeout(theChkKeyTimer);		
			chgElementEnable("Back", 1); 		// enable "Back" button
			chgElementEnable("Upgrade", 1); 	// enable "Upgrade" button
		}
		else if (upgFwState == 1){		// 	Invalid FW
			updateNewMsg(theMsg_FwBinErr);
			chgElementEnable("Back", 1); 		// enable "Back" button
			chgElementEnable("Upgrade", 1); 	// enable "Upgrade" button
		}
		else if (upgFwState == 2){		// 	UnAuth Connection
			updateNewMsg("Invalid User!");
			clearTimeout(theChkKeyTimer);		
			chgElementEnable("Back", 1); 		// enable "Back" button
			chgElementEnable("Upgrade", 1); 	// enable "Upgrade" button
		}
		else if (upgFwState == 0){		// 	Upload Finish & WRITING FW to FLASH
			if (waitSec>=0) {
				updateNewMsg(theMsg_WriteBIN+waitSec+" sec<br>"+theMsg_PowerWarning);
				waitSec--;
				setTimeout("updateFwStatusMsg()", 1000);
				clearTimeout(theChkKeyTimer);		
			}
			else {
				updateNewMsg(theMsg_WriteDone);
				chgElementEnable("Back", 1); 		// enable "Back" button
				chgElementEnable("Upgrade", 0); 	// disable "Upgrade" button
			}
		}
	}

	function init_fwupgd()	{
		var xmlhttp = createXMLHttpRequest();
		var theUrl_initFwUpgd = "/rssui/init_fwpgd.xgi";

		updateFwStatusMsg();
		xmlhttp.open("GET", theUrl_initFwUpgd+"?t="+Rand(1000), true);
		xmlhttp.send();
	}

	function loadFwChkXml()	{
		var xmlhttp = createXMLHttpRequest();
		var theUrl_FwChkXml = "/tmp/fwupgd_result.xml";

		updateFwStatusMsg();
		xmlhttp.open("GET", theUrl_FwChkXml+"?t="+Rand(1000), true);
		xmlhttp.send();

		xmlhttp.onreadystatechange = function() {
			if (xmlhttp.readyState == 4) {
				//alert("upgFwState="+upgFwState+", loadFwChkXml.status="+xmlhttp.status);
				if (xmlhttp.status == 200) {

					upgFwState = getXmlhttpTagValue(xmlhttp, "FwUpgdResult");

					if (upgFwState == 0) {	// 	FWUPGRADE_WRITINGFLASH = 5,
						waitSec = getXmlhttpTagValue(xmlhttp, "RssGetWaitSec");
						//alert("Please wait "+waitSec+" seconds and don't turn off the power of device !!!");
					}
					else if (upgFwState == -1) {	//	Uploading FW
						uploadPercent = getXmlhttpTagValue(xmlhttp, "Percent");
					}
					else if (upgFwState == 1){		// 	Invalid FW
						chgElementEnable("Back", 1); 		// enable "Back" button
						chgElementEnable("Upgrade", 1); 	// enable "Upgrade" button
					}
					else if (upgFwState == 2){		// 	UnAuth Connection
						chgElementEnable("Back", 1); 		// enable "Back" button
						chgElementEnable("Upgrade", 0); 	// disable "Upgrade" button
					}
					else {		// 	unknow Error
						alert("unknown upgFwState="+upgFwState);
					}
					updateFwStatusMsg();
				}

				else {	// isn't 200 OK
					//alert(m_alert_error+xmlhttp.status);
			    	setTimeout("loadFwChkXml()", 1000);	
					//upgFwState = -999;	
					//updateFwStatusMsg();
					//chgElementEnable("Back", 1); 		// enable "Back" button
					//chgElementEnable("Upgrade", 0); 	// disable "Upgrade" button
				}
			}
		};
		xmlhttp.ontimeout = function() {  
	    	setTimeout("loadFwChkXml()", 1000);	
			//upgFwState = -999;	
			//updateFwStatusMsg();
			//chgElementEnable("Back", 1); 		// enable "Back" button
			//chgElementEnable("Upgrade", 0); 	// disable "Upgrade" button
		}; 
	}
	
	function doBack()	{
		if (is_Back_Enabled==1) window.history.go(-1);
	}
	
	function init_page() {
		// Init Actions
		public_js_init();
		
		theChkKeyTimer = setTimeout("check_AuthKey()", 30000);
		updateInnerHTML("currFwVer", m_currfw+curfw);
		if (!isIE) {
			document.getElementById("fwbox").style.display="none";
		}
		chgElementEnable("Back", 1); 		// enable "Back" button
		chgElementEnable("Upgrade", 1); 	// enable "Upgrade" button
	}

	function chk_uploadfile(){
		if (document.fwupdate.uploadfile.value=="") {
		    setTimeout("chk_uploadfile()", 1000);
		}
		else {
			chgElementEnable("Back", 0); 		// disable "Back" button
			chgElementEnable("Upgrade", 0); 	// disable "Upgrade" button
			updateNewMsg(theMsg_UploadBIN);
			UpladFile();
		}
	}

	function doUpgrade() {
		if (is_Upgrade_Enabled==0) return; 
		if (!isIE) {
			updateNewMsg("Select new Firmware...");
			document.getElementById("uploadfile").value = "";
		    setTimeout("chk_uploadfile()", 1000);
			document.fwupdate.uploadfile.click();
		}
		else {
			if (document.fwupdate.uploadfile.value=="") {
				alert("Select New Firmware File!");
			}
			else {
			    init_fwupgd();
				chgElementEnable("Back", 0); 		// disable "Back" button
				chgElementEnable("Upgrade", 0); 	// disable "Upgrade" button
				updateNewMsg(theMsg_UploadBIN);
		    	setTimeout("document.fwupdate.submit()", 1000);	
		    	setTimeout("loadFwChkXml()", 4000);	
			}
		}
	}

</script>

</body>
</html>
