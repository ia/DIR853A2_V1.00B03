var index,index2,i;
var txt2="";

function optionsClear(e)
{
    e.options.length = 0;
}

//获得页面内容
function init_isp(){
	if(HardwareType =="ADSL" || HardwareType == "VDSL")
	{
		optionsClear(document.getElementById("select-country"));
		document.getElementById("select-country").add(new Option(m_isp_select),0);
		for(i=0;i<countryList.length;i++)	{
			var y=document.createElement('option');
			y.text=countryList[i][0];
			y.value=countryList[i][0];
			document.getElementById("select-country").add(y, i+1);
		}
		//document.getElementById("select-country").add(new Option("Other"),countryList.length+1);
		document.getElementById("ether_mode").style.display="none";
		document.getElementById("adslvdsl_mode").style.display="block";
		change_country();
	}
	else if(HardwareType =="Ethernet")
	{
		document.getElementById("ether_mode").style.display="block";
		document.getElementById("adslvdsl_mode").style.display="none";
		if(ConnType == "DHCP" || ConnType == "Bridge")
		{
			document.getElementById("static").style.display="none";
			document.getElementById("ppp").style.display="none";
			document.getElementById("dhcp").style.display="block";
			document.getElementById("select-ether").selectedIndex=0;
		}
		else if(ConnType=="PPPoE")
		{
			document.getElementById("static").style.display="none";
			document.getElementById("ppp").style.display="block";
			document.getElementById("dhcp").style.display="none";
			document.getElementById("select-ether").selectedIndex=2;
		}
		else if(ConnType=="Static")
		{
			document.getElementById("static").style.display="block";
			document.getElementById("ppp").style.display="none";
			document.getElementById("dhcp").style.display="none";
			document.getElementById("select-ether").selectedIndex=1;
		}
		change_ether();
	}
	else
	{
		if(isp_exist!="")
		{
			optionsClear(document.getElementById("select-country"));
	                document.getElementById("select-country").add(new Option(m_isp_select),0);
        	        for(i=0;i<countryList.length;i++)       {
	                        var y=document.createElement('option');
	                        y.text=countryList[i][0];
	                        y.value=countryList[i][0];
	                        document.getElementById("select-country").add(y, i+1);
	                }
	                //document.getElementById("select-country").add(new Option("Other"),countryList.length+1);
	                document.getElementById("ether_mode").style.display="none";
	                document.getElementById("adslvdsl_mode").style.display="block";
	                change_country();
		}
		else
		{
			document.getElementById("ether_mode").style.display="block";
	                document.getElementById("adslvdsl_mode").style.display="none";
	                if(ConnType == "DHCP" || ConnType == "Bridge")
        	        {
                	        document.getElementById("static").style.display="none";
	                        document.getElementById("ppp").style.display="none";
	                        document.getElementById("dhcp").style.display="block";
	                        document.getElementById("select-ether").selectedIndex=0;
	                }
	                else if(ConnType=="PPPoE")
	                {
	                        document.getElementById("static").style.display="none";
	                        document.getElementById("ppp").style.display="block";
	                        document.getElementById("dhcp").style.display="none";
	                        document.getElementById("select-ether").selectedIndex=2;
	                }
	                else if(ConnType=="Static")
	                {
	                        document.getElementById("static").style.display="block";
	                        document.getElementById("ppp").style.display="none";
	                        document.getElementById("dhcp").style.display="none";
	                        document.getElementById("select-ether").selectedIndex=1;
	                }
	                change_ether();
		}	
	}
}  

/*function load()
{
	getContent();
	setTimeout("check_AuthKey()",30000);
}*/

function change_ether()
{
	validateErrTable.splice(0,validateErrTable.length);
	index=document.getElementById("select-ether").selectedIndex;
	if(index==1){
		document.getElementById("static").style.display="block";
		document.getElementById("ppp").style.display="none";
		document.getElementById("dhcp").style.display="none";
		if(ppp_ip != "")
			document.getElementById("rss_Upl_IP").value=ppp_ip;
		if(ppp_netmask != "")
			document.getElementById("rss_Upl_NetMask").value=ppp_netmask;
		if(ppp_gateway != "")
			document.getElementById("rss_Upl_Gateway").value=ppp_gateway;
		if(ppp_dns != "")
			document.getElementById("rss_Upl_DNS").value=ppp_dns;
		check_ip("rss_Upl_IP");
		check_mask("rss_Upl_NetMask");
		check_gateway("rss_Upl_Gateway");
		txt2="&rss_Upl_ConnType=Static";
	}
	else if(index==0){
		document.getElementById("static").style.display="none";
		document.getElementById("ppp").style.display="none";
		document.getElementById("dhcp").style.display="block";
		if(ppp_ip != "")
			document.getElementById("rss_Upl_IP2").value=ppp_ip;
		else
			document.getElementById("rss_Upl_IP2").value="0.0.0.0";
		if(ppp_netmask != "")
			document.getElementById("rss_Upl_NetMask2").value=ppp_netmask;
		else
			document.getElementById("rss_Upl_NetMask2").value="0.0.0.0";
		if(ppp_gateway != "")
			document.getElementById("rss_Upl_Gateway2").value=ppp_gateway;
		else
			document.getElementById("rss_Upl_Gateway2").value="0.0.0.0";
		if(ppp_dns != "")
			document.getElementById("rss_Upl_DNS2").value=ppp_dns;
		else
			document.getElementById("rss_Upl_DNS2").value="0.0.0.0";
		txt2="&rss_Upl_ConnType=DHCP";
	}
	else if(index==2){
		document.getElementById("static").style.display="none";
		document.getElementById("ppp").style.display="block";
		document.getElementById("dhcp").style.display="none";
		if(ppp_ip != "")
			document.getElementById("rss_Upl_IP3").value=ppp_ip;
		else
			document.getElementById("rss_Upl_IP3").value="0.0.0.0";
		if(ppp_netmask != "")
			document.getElementById("rss_Upl_NetMask3").value=ppp_netmask;
		else
			document.getElementById("rss_Upl_NetMask3").value="0.0.0.0";
		if(ppp_gateway != "")
			document.getElementById("rss_Upl_Gateway3").value=ppp_gateway;
		else
			document.getElementById("rss_Upl_Gateway3").value="0.0.0.0";
		if(ppp_dns != "")
			document.getElementById("rss_Upl_DNS3").value=ppp_dns;
		else
			document.getElementById("rss_Upl_DNS3").value="0.0.0.0";
		if(ppp_username!= "")
			document.getElementById("rss_Upl_LoginUserName").value=ppp_username;
		if(ppp_password != "")
			document.getElementById("rss_Upl_LoginPassword").value=ppp_password;
		check_rssVarValidate("rss_Upl_LoginUserName");
		check_rssVarValidate("rss_Upl_LoginPassword");
		txt2="&rss_Upl_ConnType=PPPoE";
	}
}

function change_country()
{
	optionsClear(document.getElementById("select-isp"));
	document.getElementById("select-isp").add(new Option(m_isp_select),0);
	index=document.getElementById("select-country").selectedIndex;
	if(index>0 && index<=countryList.length){
		for(i=0;i<ispDataList[index-1][0].length;i++) {
			var y=document.createElement("option");
			y.text=ispDataList[index-1][0][i];
			y.value=ispDataList[index-1][0][i];
			document.getElementById("select-isp").add(y, i+1);
		}
	}
	change_isp();
}

function change_isp()
{
	index=document.getElementById("select-country").selectedIndex;
	index2=document.getElementById("select-isp").selectedIndex;
	if(index==0||index2==0)
	{
		parse_ValidateErr_index("select-country_check",0);
		document.getElementById("select-country_check").value=m_isp_choose;
	}
	else
	{
		parse_ValidateErr_index("select-country_check",1);
		document.getElementById("select-country_check").value="";
	}
		
	if(index2>0)	{
		if(ispDataList[index-1][1][index2-1]=="PPPoE" || ispDataList[index-1][1][index2-1]=="PPPoA")	{
			document.getElementById("static").style.display="none";
			document.getElementById("ppp").style.display="block";
			document.getElementById("dhcp").style.display="none";
		}
		else if(ispDataList[index-1][1][index2-1]=="Static") {
			document.getElementById("static").style.display="block";
			document.getElementById("ppp").style.display="none";
			document.getElementById("dhcp").style.display="none";
		}
		else if(ispDataList[index-1][1][index2-1]=="DHCP" || ispDataList[index-1][1][index2-1]=="Bridge"){
			document.getElementById("static").style.display="none";
			document.getElementById("ppp").style.display="none";
			document.getElementById("dhcp").style.display="block";
			if(ppp_ip != "")
			document.getElementById("rss_Upl_IP2").value=ppp_ip;
			else
				document.getElementById("rss_Upl_IP2").value="0.0.0.0";
			if(ppp_netmask != "")
				document.getElementById("rss_Upl_NetMask2").value=ppp_netmask;
			else
				document.getElementById("rss_Upl_NetMask2").value="0.0.0.0";
			if(ppp_gateway != "")
				document.getElementById("rss_Upl_Gateway2").value=ppp_gateway;
			else
				document.getElementById("rss_Upl_Gateway2").value="0.0.0.0";
			if(ppp_dns != "")
				document.getElementById("rss_Upl_DNS2").value=ppp_dns;
			else
				document.getElementById("rss_Upl_DNS2").value="0.0.0.0";
		}
		if(ispDataList[index-1][1][index2-1]=="Bridge")
			txt2="&rss_Upl_ConnType=DHCP";
		else
			txt2="&rss_Upl_ConnType="+ispDataList[index-1][1][index2-1];
	}
	else if(index==0 || index-1==countryList.length) {
		document.getElementById("static").style.display="none";
		document.getElementById("ppp").style.display="block";
		document.getElementById("dhcp").style.display="none";
		txt2="&rss_Upl_ConnType=PPPoE";
	}
	if(ppp_username!= "")
		document.getElementById("rss_Upl_LoginUserName").value=ppp_username;
	if(ppp_password != "")
		document.getElementById("rss_Upl_LoginPassword").value=ppp_password;
	if(ppp_ip != "")
		document.getElementById("rss_Upl_IP3").value=ppp_ip;
	else
		document.getElementById("rss_Upl_IP3").value="0.0.0.0";
	if(ppp_netmask != "")
		document.getElementById("rss_Upl_NetMask3").value=ppp_netmask;
	else
		document.getElementById("rss_Upl_NetMask3").value="0.0.0.0";
	if(ppp_gateway != "")
		document.getElementById("rss_Upl_Gateway3").value=ppp_gateway;
	else
		document.getElementById("rss_Upl_Gateway3").value="0.0.0.0";
	if(ppp_dns != "")
		document.getElementById("rss_Upl_DNS3").value=ppp_dns;
	else
		document.getElementById("rss_Upl_DNS3").value="0.0.0.0";
	//alert(txt2);
	check_isp_validate();
}


function check_isp_validate()
{
	if(document.getElementById("static").style.display=="block")
	{
		if(document.getElementById("rss_Upl_IP").value.length<=0)
		{
			document.getElementById("rss_Upl_IP_check").value=m_error_length;
			return false;
		}
		else if(document.getElementById("rss_Upl_NetMask").value.length<=0)
		{
			document.getElementById("rss_Upl_NetMask_check").value=m_error_length;
			return false;
		}
		else if(document.getElementById("rss_Upl_Gateway").value.length<=0)
		{
			document.getElementById("rss_Upl_Gateway_check").value=m_error_length;
			return false;
		}
		else if(document.getElementById("rss_Upl_DNS").value.length<=0)
		{
			document.getElementById("rss_Upl_DNS_check").value=m_error_length;
			return false;
		}
	}
	else if(document.getElementById("ppp").style.display=="block")
	{
		if(document.getElementById("rss_Upl_LoginUserName").value.length<=0)
		{
			document.getElementById("rss_Upl_LoginUserName_check").value=m_error_length;
			return false;
		}
		else if(document.getElementById("rss_Upl_LoginPassword").value.length<=0)
		{
			document.getElementById("rss_Upl_LoginPassword_check").value=m_error_length;
			return false;
		}
	}
	return true;
}

function apply_isp()
{
	var flag=true;
	flag=check_isp_validate();
	if(flag==false)
		return false;
	if(HardwareType =="ADSL" || HardwareType == "VDSL")
	{
		if(document.getElementById("select-country").selectedIndex!=0)
		{
			if(HardwareType=="ADSL" || HardwareType=="VDSL" || HardwareType=="Ethernet")
				txt+="&rss_Upl_Dsl_CountryName="+unescape(document.getElementById("select-country").value);
			else if(HardwareType=="WWanAPN")
				txt+="&rss_Upl_Apn_CountryName="+unescape(document.getElementById("select-country").value);
		}
		if(document.getElementById("select-isp").selectedIndex!=0)
		{
			if(HardwareType=="ADSL" || HardwareType=="VDSL" || HardwareType=="Ethernet")
				txt+="&rss_Upl_Dsl_IspName="+escape(document.getElementById("select-isp").value);
			else if(HardwareType=="WWanAPN")
				txt+="&rss_Upl_Apn_IspName="+escape(document.getElementById("select-isp").value);
		}
		if(document.getElementById("static").style.display=="block")
		{
			txt+="&rss_Upl_IP="+(document.getElementById("rss_Upl_IP").value);
			txt+="&rss_Upl_NetMask="+(document.getElementById("rss_Upl_NetMask").value);
			txt+="&rss_Upl_Gateway="+(document.getElementById("rss_Upl_Gateway").value);
			txt+="&rss_Upl_DNS="+(document.getElementById("rss_Upl_DNS").value);
		}
		else if(document.getElementById("ppp").style.display=="block")
		{
			txt+="&rss_Upl_LoginUserName="+(document.getElementById("rss_Upl_LoginUserName").value);
			txt+="&rss_Upl_LoginPassword="+(document.getElementById("rss_Upl_LoginPassword").value);
		}
	}
	else if(HardwareType =="Ethernet")
	{
		if(document.getElementById("static").style.display=="block")
		{
			txt+="&rss_Upl_IP="+(document.getElementById("rss_Upl_IP").value);
			txt+="&rss_Upl_NetMask="+(document.getElementById("rss_Upl_NetMask").value);
			txt+="&rss_Upl_Gateway="+(document.getElementById("rss_Upl_Gateway").value);
			txt+="&rss_Upl_DNS="+(document.getElementById("rss_Upl_DNS").value);
		}
		else if(document.getElementById("ppp").style.display=="block")
		{
			txt+="&rss_Upl_LoginUserName="+(document.getElementById("rss_Upl_LoginUserName").value);
			txt+="&rss_Upl_LoginPassword="+(document.getElementById("rss_Upl_LoginPassword").value);
		}
	}
	else
	{
		if(isp_exist!="")
		{
			if(document.getElementById("select-country").selectedIndex!=0)
			{
				if(HardwareType=="ADSL" || HardwareType=="VDSL" || HardwareType=="Ethernet")
					txt+="&rss_Upl_Dsl_CountryName="+unescape(document.getElementById("select-country").value);
				else if(HardwareType=="WWanAPN")
					txt+="&rss_Upl_Apn_CountryName="+unescape(document.getElementById("select-country").value);
			}
			if(document.getElementById("select-isp").selectedIndex!=0)
			{
				if(HardwareType=="ADSL" || HardwareType=="VDSL" || HardwareType=="Ethernet")
					txt+="&rss_Upl_Dsl_IspName="+escape(document.getElementById("select-isp").value);
				else if(HardwareType=="WWanAPN")
					txt+="&rss_Upl_Apn_IspName="+escape(document.getElementById("select-isp").value);
			}
			if(document.getElementById("static").style.display=="block")
			{
				txt+="&rss_Upl_IP="+(document.getElementById("rss_Upl_IP").value);
				txt+="&rss_Upl_NetMask="+(document.getElementById("rss_Upl_NetMask").value);
				txt+="&rss_Upl_Gateway="+(document.getElementById("rss_Upl_Gateway").value);
				txt+="&rss_Upl_DNS="+(document.getElementById("rss_Upl_DNS").value);
			}
			else if(document.getElementById("ppp").style.display=="block")
			{
				txt+="&rss_Upl_LoginUserName="+(document.getElementById("rss_Upl_LoginUserName").value);
				txt+="&rss_Upl_LoginPassword="+(document.getElementById("rss_Upl_LoginPassword").value);
			}
		}
		else
		{
			if(document.getElementById("static").style.display=="block")
			{
				txt+="&rss_Upl_IP="+(document.getElementById("rss_Upl_IP").value);
				txt+="&rss_Upl_NetMask="+(document.getElementById("rss_Upl_NetMask").value);
				txt+="&rss_Upl_Gateway="+(document.getElementById("rss_Upl_Gateway").value);
				txt+="&rss_Upl_DNS="+(document.getElementById("rss_Upl_DNS").value);
			}
			else if(document.getElementById("ppp").style.display=="block")
			{
				txt+="&rss_Upl_LoginUserName="+(document.getElementById("rss_Upl_LoginUserName").value);
				txt+="&rss_Upl_LoginPassword="+(document.getElementById("rss_Upl_LoginPassword").value);
			}
		}
	}
	txt+=txt2;
	return true;
	//location.href="wireless.asp?"+txt+txt2;
}
