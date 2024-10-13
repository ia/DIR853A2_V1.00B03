<HTML>
<HEAD>
<TITLE>RSS Encryption Test</TITLE>
<script language=javascript>

var RssVarEncryptHeader="<% ConfigRssGet("rss_Var_RssEncryHeader"); %>";
var RssVarEncryptKey="<% ConfigRssGet("rss_Var_RssEncryKey"); %>";

function RssDataEncrypt(str, key) {
    var len = str.length;
    var key_len = key.length;
    var i = 0, k_i = 0, kk, cc, n;
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
		} else if (n==2) {
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

function _RssDataEncode(str, key) {
	var rndNo = "0000"+RandomNumber();
	rndNo = rndNo.substring(rndNo.length-4);
    var the2ndKey = RssDataEncrypt(key, rndNo);
    var dd = RssVarEncryptHeader+rndNo+RssDataEncrypt(utf16to8(str), the2ndKey);
    alert("mask='"+rndNo+"'\n2nd-Key='"+the2ndKey+"'\ndata='"+dd+"'");
	return base64encode(RssDataEncrypt(dd, key));
}

function _RssDataDecode(str, key) {
    var dd = RssDataEncrypt(base64decode(str), key);
	var rndNo = dd.substring(RssVarEncryptHeader.length, RssVarEncryptHeader.length+4);
    var the2ndKey = RssDataEncrypt(key, rndNo);
    alert("mask='"+rndNo+"'\n2nd-Key='"+the2ndKey+"'\ndata='"+dd+"'");
    return utf8to16(RssDataEncrypt(dd.substring(RssVarEncryptHeader.length+4), the2ndKey));	
}

function myencode() {
    var f = document.f;
    f.output.value = _RssDataEncode(f.source.value, f.key.value);
}
function mydecode() {
    var f = document.f;
    f.decode.value = _RssDataDecode(f.output.value, f.key.value);
}

function init_page() {
	document.f.key.value = RssVarEncryptKey;
}

</script>
<script src="/rssui/base64.js" type="text/javascript"></script>

</HEAD>
<BODY onload="init_page()">
<H1>RSS Encryption</H1>
<FORM NAME="f">
Key<BR>
<TEXTAREA NAME="key" ROWS=4 COLS=60 WRAP="soft">1234567890</TEXTAREA><BR><BR>
ԭ��<BR>
<TEXTAREA NAME="source" ROWS=4 COLS=60 WRAP="soft"></TEXTAREA><BR><BR>
<INPUT TYPE=BUTTON VALUE="RSS Encode" ONCLICK="myencode()"><BR>
<TEXTAREA NAME="output" ROWS=4 COLS=60 WRAP="soft"></TEXTAREA><BR><BR>
<INPUT TYPE=BUTTON VALUE="RSS Decode" ONCLICK="mydecode()"><BR>
<TEXTAREA NAME="decode" ROWS=4 COLS=60 WRAP="soft"></TEXTAREA><BR><BR>
</FORM>
</BODY>
