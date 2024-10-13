<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>
<script type="text/javascript">
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
	
	location.href="main.asp?lang="+langCode2+"?";

</script>
</head>
</html>

