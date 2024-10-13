<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="robots" content="all">
<meta http-equiv="Access-Control-Allow-Origin" content="*">
<title><% ConfigRssGet("rss_Var_OTTitle"); %></title>
<link rel="stylesheet" href="./dbros.css" type="text/css">

</head>
<body>
[Daemons]
<br>rss-spec: <% ConfigRssGet("rss_Var_Ver_RssSpec"); %>
<br>lib: <% ConfigRssGet("rss_Var_Ver_Lib"); %>
<br>dbapilib: <% ConfigRssGet("rss_Var_Ver_DBApiLib"); %>
<br>db: <% ConfigRssGet("rss_Var_Ver_DB"); %>
<br><% ConfigRssGet("rss_Var_OTName"); %>: <% ConfigRssGet("rss_Var_Ver_Starter"); %>
<br>web: <% ConfigRssGet("rss_Var_Ver_Web"); %>
<br><br>
[Device] 
<br>Model-Number: <% ConfigRssGet("rss_DeviceModel"); %>
<br>Default-Setting: <% ConfigRssGet("rss_FactoryDefault"); %>
<br>LAN-MAC: <% ConfigRssGet("rss_Ext_Fwu_DeviceMAC"); %>
<br>Hw-Version: <% ConfigRssGet("rss_Ext_Fwu_HwVersion"); %>
<br>Region-Code: <% ConfigRssGet("rss_Ext_Fwu_RegionCode"); %>
<br>FW-Version: <% ConfigRssGet("rss_Ext_Fwu_CurrFwVersion"); %>
<br>FW-Name: <% ConfigRssGet("rss_Ext_Fwu_CurrFwFileName"); %>
<br>FW-AccessKey: <% ConfigRssGet("rss_Ext_Fwu_AccessKey"); %>
<br><br>
[Status] 
<br>Cloning: <% ConfigRssGet("rss_Var_CloneMode"); %>
</body>
</html>
