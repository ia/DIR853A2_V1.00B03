#include <stdio.h>        
#include <unistd.h>       
#include <signal.h>        
#include <string.h>        
#include <sys/time.h>    
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/file.h> 
#include <fcntl.h>
#include "stad.h"
#include <strings.h>

void stad_save_pid()
{
	FILE *pFile = NULL;

	if ((pFile = fopen("/var/run/stad.pid", "w")) != NULL) 
	{
		fprintf(pFile, "%d\n", getpid());
		fclose(pFile);
	}
	close(pFile);
}

unsigned int stad_get_pid()
{
	FILE *pFile = NULL;
	unsigned int iPid = 0;
	char buf[16] = { 0 };
	
	if ((pFile = fopen("/var/run/stad.pid", "r")) != NULL) 
	{
		if(NULL != fgets(buf, sizeof(buf), pFile))
		{
			iPid = atoi(buf);
		}
		fclose(pFile);
	}

	return iPid;	
}

int stad_get_type_by_mac(char *mac, ST_CLIENT_INFO *pst_client_info)
{
	FILE *file = NULL;
	char buf[256] = { 0 };
	struct ifreq ifr;
    int s;
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1) {
		perror("socket");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, INTERFACE_2G);
	ioctl(s, SIOCGIFFLAGS, &ifr);
	if(ifr.ifr_flags & IFF_UP) {
		file = popen("iwpriv ra0 get_mac_table", "r");
		if(file != NULL)
		{
			while(NULL != fgets(buf, sizeof(buf), file))
			{
				if(NULL != strstr(buf, mac))
				{
					strcpy(pst_client_info->Type, "WiFi_2.4G");
					pclose(file);
					goto exit;
				}
			}
			pclose(file);
		}
	}

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, INTERFACE_5G);
	ioctl(s, SIOCGIFFLAGS, &ifr);
	if(ifr.ifr_flags & IFF_UP) {
		file = popen("iwpriv rai0 get_mac_table", "r");
		if(file != NULL)
		{
			while(NULL != fgets(buf, sizeof(buf), file))
			{
				if(NULL != strstr(buf, mac))
				{
					strcpy(pst_client_info->Type, "WiFi_5G");
					pclose(file);
					goto exit;
				}

			}
			pclose(file);
		}
	}

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, INTERFACE_2G_GUEST1);
	ioctl(s, SIOCGIFFLAGS, &ifr);
	if(ifr.ifr_flags & IFF_UP) {
		file = popen("iwpriv ra1 get_mac_table", "r");
		if(file != NULL)
		{
			while(NULL != fgets(buf, sizeof(buf), file))
			{
				if(NULL != strstr(buf, mac))
				{
					strcpy(pst_client_info->Type, "WiFi_2.4G_guest");
					pclose(file);
					goto exit;
				}

			}
			pclose(file);
		}
	}

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, INTERFACE_5G_GUEST1);
	ioctl(s, SIOCGIFFLAGS, &ifr);
	if(ifr.ifr_flags & IFF_UP) {
		file = popen("iwpriv rai1 get_mac_table", "r");
		if(file != NULL)
		{
			while(NULL != fgets(buf, sizeof(buf), file))
			{
				if(NULL != strstr(buf, mac))
				{
					strcpy(pst_client_info->Type, "WiFi_5G_guest");
					pclose(file);
					goto exit;
				}

			}
			pclose(file);
		}
	}
exit:

	close(s);
	
	return 0;

}

int stad_get_dhcplist_by_mac(char *mac, ST_CLIENT_INFO *pst_client_info)
{
	FILE *fp = NULL;
	char buf[256] = { 0 };
	char szLeaseTime[32] = { 0 };
	char szMac[32] = { 0 };
	char szIP[32] = { 0 };
	char szHostName[64] = { 0 };
	char szMac1[32] = { 0 };
	int i = 0;
	
	fp = fopen(DHCP_LEASES_FILE, "r");
	if(NULL == fp)
	{
		DEBUGP("open %s fail\n", DHCP_LEASES_FILE);
		return -1;
	}

	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		i = sscanf(buf, "%s %s %s %s %s",
			szLeaseTime, szMac, szIP, szHostName, szMac1);

		if(5 != i)
			continue;
		
		if(!strncasecmp(mac, szMac, 17))
		{
			//strncpy(pst_client_info->IPv4Address, szIP, sizeof(pst_client_info->IPv4Address));
			strncpy(pst_client_info->DeviceName, szHostName, sizeof(pst_client_info->DeviceName));
			break;
		}
	}

	fclose(fp);
	
	return 0;
}

int stad_check_client_online(char *pszIP, char *pszMac, char *pszInterface)
{
	char szCmd[128] = { 0 };
	FILE *pFile = NULL;
	char szBuf[256] = { 0 };

	if(NULL == pszIP || NULL == pszMac || NULL == pszInterface)
	{
		return -1;
	}

	if( 0 != strncmp(pszInterface, "br", 2) )
	{
		return -1;
	}
	
	snprintf(szCmd, sizeof(szCmd),"arping -I %s %s -c 1 > /var/arping_result", pszInterface, pszIP);
	twsystem(szCmd, 0);

	pFile = fopen("/var/arping_result", "r");
	if(pFile != NULL)
	{
		while(NULL != fgets(szBuf, sizeof(szBuf), pFile))
		{
			if(NULL != strstr(szBuf, pszMac))
			{
				fclose(pFile);
				unlink("/var/arping_result");
				return 0;
			}

		}
		fclose(pFile);
		unlink("/var/arping_result");
	}
	
	return -1;

}

int stad_get_ipaddr_by_mac(char *mac, ST_CLIENT_INFO *pst_client_info)
{
	FILE *fp = NULL;
	char buf[256] = { 0 };
	char szIP[17] = { 0 };
	char szHWtype[12] = { 0 };
	char szFlag[12] = { 0 };
	char szHWaddress[22] = { 0 };
	char szMask[9] = { 0 };
	char szDevice[16] = { 0 };
	int i = 0, j = 0;
	ST_ARP_INFO stArpInfo[15];

	fp = popen("cat /proc/net/arp", "r");

	if(NULL == fp)
	{
		DEBUGP("open /proc/net/arp fail\n");
		return -1;
	}
	
	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		if(NULL != strstr(buf, "IP address"))
			continue;
		
		i = sscanf(buf, "%17s %12s %12s %22s %9s %5s",
			szIP, szHWtype, szFlag, szHWaddress, szMask, szDevice);

		if(6 != i)
			continue;

		//DEBUGP("szIP=%s, hwaddr=%s, device=%s\n", szIP, szHWaddress, szDevice);

		if( !strcmp(szIP, "0.0.0.0") )
			continue;
		
		if(!strncasecmp(mac, szHWaddress, 17))
		{
			if(j > 14)
				continue;
			strncpy(stArpInfo[j].IPAddress, szIP, sizeof(stArpInfo[j].IPAddress));
			strncpy(stArpInfo[j].szHWaddress, szHWaddress, sizeof(stArpInfo[j].szHWaddress));
			strncpy(stArpInfo[j].szDevice, szDevice, sizeof(stArpInfo[j].szDevice));
			j++;
		}
	}

	pclose(fp);

	while(j--)
	{
		if( 0 == stad_check_client_online(stArpInfo[j].IPAddress, mac, stArpInfo[j].szDevice) )
		{
			DEBUGP("szIP=%s, hwaddr=%s, device=%s is online\n", stArpInfo[j].IPAddress, stArpInfo[j].szHWaddress, stArpInfo[j].szDevice);
			if(!strncmp(stArpInfo[j].szDevice, "br", 2))
			{
				strncpy(pst_client_info->IPv4Address, stArpInfo[j].IPAddress, sizeof(pst_client_info->IPv4Address));
				if(!strcmp(stArpInfo[j].szDevice, "br1"))
				{
					if(!strcmp(pst_client_info->Type, "WiFi_2.4G"))
						strcpy(pst_client_info->Type, "WiFi_2.4G_guest");

					if(!strcmp(pst_client_info->Type, "WiFi_5G"))
						strcpy(pst_client_info->Type, "WiFi_5G_guest");
				}
				break;
			}
		}
	}
	
	return 0;
}
int stad_get_ipv6addr_by_mac(char *mac, ST_CLIENT_INFO *pst_client_info)
{
	FILE *fp = NULL;
	char buf[256] = { 0 };
	char szMac[32] = { 0 };
	char szIPv6[64] = { 0 };
	int i = 0;

	fp = fopen(IPV6_LEASES_FILE, "r");
	if(NULL == fp)
	{
		DEBUGP("open %s fail\n", IPV6_LEASES_FILE);
		return -1;
	}

	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		i = sscanf(buf, "%s %s",szIPv6, szMac);

		if(2 != i)
			continue;
		
		if(!strncasecmp(mac, szMac, 17))
		{
			strncpy(pst_client_info->IPv6Address, szIPv6, sizeof(pst_client_info->IPv6Address));
			break;
		}
	}

	fclose(fp);
	
	return 0;

}

int stad_get_homename_by_ip(char *ipaddr, ST_CLIENT_INFO *pst_client_info)
{
	FILE *fp = NULL;
	char buf[256] = { 0 };
	char szIPaddr[32] = { 0 };
	char szHostname[64] = { 0 };
	int i = 0;

	fp = fopen(NBNS_HOSTNAME_FILE, "r");
	if(NULL == fp)
	{
		DEBUGP("open %s fail\n", NBNS_HOSTNAME_FILE);
		return -1;
	}

	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		i = sscanf(buf, "%s %s",szIPaddr, szHostname);

		if(2 != i)
			continue;
		
		if(!strcmp(szIPaddr, ipaddr))
		{
			strncpy(pst_client_info->DeviceName, szHostname, sizeof(pst_client_info->DeviceName));
			break;
		}
	}

	fclose(fp);
	
	return 0;
}

int stad_add_clients(ST_CLIENT_INFO st_client_info)
{
	FILE *fp = NULL;
	FILE *tmp_fp = NULL;
	char buf[sizeof(ST_CLIENT_INFO)+16] = { 0 };
	char mac_addr[18] = { 0 };
	int i_find = 0;

	if(-1 == access(CLIENT_INFO_TMP1_PATH, F_OK))
	{
		fp = fopen(CLIENT_INFO_TMP1_PATH, "w+");
	}
	else
	{
		fp = fopen(CLIENT_INFO_TMP1_PATH, "r+");
	}

	if(NULL == fp)
	{
		DEBUGP("open %s fail\n", CLIENT_INFO_TMP1_PATH);
		return -1;
	}

	tmp_fp = fopen(CLIENT_INFO_TMP_PATH, "w+");

	if(NULL == tmp_fp)
	{
		DEBUGP("open %s fail\n", CLIENT_INFO_TMP_PATH);
		fclose(fp);
		return -1;
	}

	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		memset(mac_addr, 0, sizeof(mac_addr));
		if(TW_get_rule_safe(0, buf, ',', mac_addr, sizeof(mac_addr)) == -1)
		{
			continue;		
		}

		if( !strcasecmp(mac_addr, st_client_info.MacAddress) )
		{
			fprintf(tmp_fp, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", st_client_info.MacAddress, st_client_info.IPv4Address,
				st_client_info.IPv6Address, st_client_info.Type, st_client_info.DeviceName, st_client_info.NickName,
				st_client_info.ReserveIP, st_client_info.Band, st_client_info.SignalLevel, st_client_info.DeviceOS,
				st_client_info.DeviceType, st_client_info.DeviceFamily);
			i_find = 1;
			continue;
		}
		fprintf(tmp_fp, "%s", buf);
	}

	if( !i_find )
	{
		fprintf(tmp_fp, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", st_client_info.MacAddress, st_client_info.IPv4Address,
				st_client_info.IPv6Address, st_client_info.Type, st_client_info.DeviceName, st_client_info.NickName,
				st_client_info.ReserveIP, st_client_info.Band, st_client_info.SignalLevel, st_client_info.DeviceOS,
				st_client_info.DeviceType, st_client_info.DeviceFamily);
	}
	
	fclose(fp);
	fclose(tmp_fp);

	rename(CLIENT_INFO_TMP_PATH, CLIENT_INFO_TMP1_PATH);

	return 0;
}

int stad_del_clients(ST_CLIENT_INFO st_client_info)
{
	FILE *fp = NULL;
	FILE *tmp_fp = NULL;
	char buf[sizeof(ST_CLIENT_INFO)+16] = { 0 };
	char mac_addr[18] = { 0 };

	fp = fopen(CLIENT_INFO_TMP1_PATH, "r+");

	if(NULL == fp)
	{
		DEBUGP("open %s fail\n", CLIENT_INFO_TMP1_PATH);
		return -1;
	}

	tmp_fp = fopen(CLIENT_INFO_TMP_PATH, "w+");

	if(NULL == tmp_fp)
	{
		DEBUGP("open %s fail\n", CLIENT_INFO_TMP_PATH);
		fclose(fp);
		return -1;
	}

	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		if(TW_get_rule_safe(0, buf, ',', mac_addr, sizeof(mac_addr)) == -1)
		{
			continue;		
		}

		if( !strcasecmp(mac_addr, st_client_info.MacAddress) )
		{
			continue;
		}
		fprintf(tmp_fp, "%s", buf);
	}
	
	fclose(fp);
	fclose(tmp_fp);

	rename(CLIENT_INFO_TMP_PATH, CLIENT_INFO_TMP1_PATH);

	return 0;
}

stad_add_mac(char *pszMac)
{
	FILE *fp = NULL;
	FILE *tmp_fp = NULL;
	char buf[sizeof(ST_CLIENT_INFO)+16] = { 0 };
	char szMac[18] = { 0 };
	int i_find = 0;

	if(-1 == access(MACLIST_INFO_PATH, F_OK))
	{
		fp = fopen(MACLIST_INFO_PATH, "w+");
	}
	else
	{
		fp = fopen(MACLIST_INFO_PATH, "r+");
	}

	if(NULL == fp)
	{
		DEBUGP("open %s fail\n", MACLIST_INFO_PATH);
		return -1;
	}

	tmp_fp = fopen(MACLIST_INFO_TMP_PATH, "w+");

	if(NULL == tmp_fp)
	{
		DEBUGP("open %s fail\n", MACLIST_INFO_TMP_PATH);
		fclose(fp);
		return -1;
	}

	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		memset(szMac, 0, sizeof(szMac));
		if(TW_get_rule_safe(0, buf, ',', szMac, sizeof(szMac)) == -1)
		{
			continue;		
		}

		if( !strncasecmp(szMac, pszMac, sizeof(szMac)) )
		{
			fprintf(tmp_fp, "%s\n", szMac);
			i_find = 1;
			continue;
		}
		fprintf(tmp_fp, "%s", buf);
	}

	if( !i_find )
	{
		fprintf(tmp_fp, "%s\n", pszMac);
	}
	
	fclose(fp);
	fclose(tmp_fp);

	rename(MACLIST_INFO_TMP_PATH, MACLIST_INFO_PATH);

	return 0;

}

int stad_del_mac(char *pszMac)
{
	FILE *fp = NULL;
	FILE *tmp_fp = NULL;
	char buf[sizeof(ST_CLIENT_INFO)+16] = { 0 };
	char szMac[18] = { 0 };

	fp = fopen(MACLIST_INFO_PATH, "r+");

	if(NULL == fp)
	{
		DEBUGP("open %s fail\n", MACLIST_INFO_PATH);
		return -1;
	}

	tmp_fp = fopen(MACLIST_INFO_TMP_PATH, "w+");

	if(NULL == tmp_fp)
	{
		DEBUGP("open %s fail\n", MACLIST_INFO_TMP_PATH);
		fclose(fp);
		return -1;
	}

	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		if(TW_get_rule_safe(0, buf, ',', szMac, sizeof(szMac)) == -1)
		{
			continue;		
		}

		if( !strncasecmp(szMac, pszMac, sizeof(szMac)) )
		{
			continue;
		}
		fprintf(tmp_fp, "%s", buf);
	}
	
	fclose(fp);
	fclose(tmp_fp);

	rename(MACLIST_INFO_TMP_PATH, MACLIST_INFO_PATH);

	return 0;
}

int  stad_process(struct sta_msg * recv_msg)
{
	char mac_addr[32] = {0};
	int fd = 0;
	int iFlock = 0;
	//unsigned int iPid = 0;
	//char szCmd[64] = { 0 };

	iFlock = file_lock(MACLIST_FILE_LOCK);

	
	/* �жϸ�mac�Ƿ���ڣ����ھ͸���*/
	sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", recv_msg->mac_addr[0], recv_msg->mac_addr[1], recv_msg->mac_addr[2],
		recv_msg->mac_addr[3], recv_msg->mac_addr[4], recv_msg->mac_addr[5]);

	DEBUGP("mac=%s\n", mac_addr);

	if(recv_msg->type)
	{	
		DEBUGP("add stad ---------------------------------------------receive from kernel\n");
		stad_add_mac(mac_addr);
	}
	/* �����豸*/
	else if(!recv_msg->type)
	{
		DEBUGP("del stad ---------------------------------------------receive from kernel\n");
		stad_del_mac(mac_addr);
	}

	file_unlock(iFlock);
#if 0	
	iPid = stad_get_pid();

	if(0 != iPid && recv_msg->type)
	{
		snprintf(szCmd, sizeof(szCmd), "kill -SIGALRM %d", iPid);
		system(szCmd);
	}
#endif	
	return 0;
}


void stad_paras_recv_msg_info(unsigned char *recv_buf)
{
	struct sta_msg * recv_msg = NULL;
	
	recv_msg = (struct sta_msg *)recv_buf;

	DEBUGP("type = %d\n", recv_msg->type);
	DEBUGP("mac=%02x:%02x:%02x:%02x:%02x:%02x\n", recv_msg->mac_addr[0], recv_msg->mac_addr[1], recv_msg->mac_addr[2],
		recv_msg->mac_addr[3], recv_msg->mac_addr[4], recv_msg->mac_addr[5]);
	DEBUGP("access_time = %d\n", recv_msg->access_time/100);

	stad_process(recv_msg);
	
	return;
}


/* ap receive ue macs from netlink */
int stad_efence_receive_fdb_info_from_kernel(int socket_fd)
{
	int ret = -1;
	struct iovec iov;
	struct msghdr msg;
	struct nlmsghdr *nlh = NULL;

	nlh = g_nlh;

	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	memset(&iov,0, sizeof(iov));
	iov.iov_base = (void *)nlh;
	iov.iov_len  = NLMSG_SPACE(MAX_PAYLOAD);

	memset(&msg, 0, sizeof(msg));
	msg.msg_iov     = &iov;
	msg.msg_iovlen  = 1;
	DEBUGP("start_capture1!\n");
	ret = recvmsg(socket_fd, &msg, 0);
	if(ret < 0)
	{
	    DEBUGP("[%s]: call recvmsg fail!\n", __FUNCTION__);
	    goto exit;
	}

	DEBUGP("start_capture2!\n");
#if 0
	{
		char szcmd[64] = {0};
		sprintf(szcmd, "echo start_capture2 >> /var/sta_info");
		system(szcmd);
	}
#endif
	stad_paras_recv_msg_info(((unsigned char *)NLMSG_DATA(nlh)));

exit:

	return 0;
}


int stad_efence_open_netlink_listen()
{
    int ret = -1;
    int sock_fd;
    struct iovec iov;
    struct msghdr msg;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl src_addr, dst_addr;

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_STA_MSG);
    if(sock_fd < 0)
    {
        DEBUGP("[%s]: call socket fail!\n", __FUNCTION__);
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid    = 0;
    src_addr.nl_groups = 0;

    ret = bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
    if(ret < 0)
    {
        DEBUGP("[%s]: call bind fail!\n", __FUNCTION__);
        close(sock_fd);
        return -1;
    }

    nlh = (struct nlmsghdr *)malloc(32);
    if(!nlh)
    {
        DEBUGP("[%s]: call malloc fail!\n", __FUNCTION__);
        close(sock_fd);
        return -1;
    }

    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.nl_family = AF_NETLINK;
    dst_addr.nl_pid    = 0;
    dst_addr.nl_groups = 0;

    nlh->nlmsg_len   = NLMSG_SPACE(32);
    nlh->nlmsg_pid   = getpid();
    nlh->nlmsg_flags = 0;

     strcpy(NLMSG_DATA(nlh),"Hello you!");

    iov.iov_base = (void *)nlh;
    iov.iov_len  = NLMSG_SPACE(32);

    memset(&msg, 0, sizeof(msg));
    msg.msg_name    = (void *)&dst_addr;
    msg.msg_namelen = sizeof(dst_addr);
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    DEBUGP("g_pid = %d\n", nlh->nlmsg_pid);

    ret = sendmsg(sock_fd, &msg, 0);
    if(ret <= 0)
    {
        DEBUGP("[%s]: call sendmsg fail!\n", __FUNCTION__);
        close(sock_fd);
        return -1;
    }

    g_netlink_2g_fd = sock_fd;
	
    return 0;
}

void stad_init()
{
	return;
}

void stad_check_stad_timer(int signo)
{
	char mac_addr[18] = { 0 };
	ST_CLIENT_INFO st_client_info;
	int i = 0, j = 0;
	int iFlock = 0;
	FILE *fp = NULL;
	char buf[64] = { 0 };
	char szMacList[200][18] = { 0 };

	iFlock = file_lock(MACLIST_FILE_LOCK);
	
	fp = fopen(MACLIST_INFO_PATH, "r+");

	if(NULL == fp)
	{
		DEBUGP("open %s fail\n", MACLIST_INFO_PATH);
		file_unlock(iFlock);
		return;
	}
	
	while(NULL != fgets(buf, sizeof(buf), fp))
	{
		if(i == 200)
			break;
		
		buf[strlen(buf)-1] = '\0';
		sprintf(szMacList[i], "%s", buf);		
		i++;
	}
	
	fclose(fp);
	
	file_unlock(iFlock);
	
	
	for(j = 0; j < i; j++)
	{		
		if(0 == strlen(szMacList[j]))
			continue;

		DEBUGP("szMacList[%d]=%s\n", j, szMacList[j]);

		memset(&st_client_info, 0, sizeof(ST_CLIENT_INFO));
		
		strncpy(st_client_info.MacAddress, szMacList[j], sizeof(st_client_info.MacAddress));
		strcpy(st_client_info.Type, "LAN");
		stad_get_type_by_mac(szMacList[j], &st_client_info);
		stad_get_dhcplist_by_mac(szMacList[j], &st_client_info);
		stad_get_ipv6addr_by_mac(szMacList[j], &st_client_info);
		stad_get_ipaddr_by_mac(szMacList[j], &st_client_info);
		if(strlen(st_client_info.IPv4Address) > 6)
			stad_get_homename_by_ip(st_client_info.IPv4Address, &st_client_info);
		DEBUGP("type=%s\n", st_client_info.Type);
		DEBUGP("ipaddr=%s\n", st_client_info.IPv4Address);
		DEBUGP("Hostname=%s\n", st_client_info.DeviceName);
		if(strlen(st_client_info.IPv4Address) > 6)
		{
			stad_add_clients(st_client_info);
		}
	}
	
	if(0 == access(CLIENT_INFO_TMP1_PATH, F_OK))
	{
		snprintf(buf, sizeof(buf), "mv %s %s", CLIENT_INFO_TMP1_PATH, CLIENT_INFO_PATH);
		iFlock = file_lock(CLIENT_FILE_LOCK);
		twsystem(buf, 0);
		file_unlock(iFlock);
	}

}

int stad_check_stad_timer_init()
{
	struct itimerval tick;
   
	signal(SIGALRM, stad_check_stad_timer);
	memset(&tick, 0, sizeof(tick));

	//Timeout to run first time
	tick.it_value.tv_sec = 60;
	tick.it_value.tv_usec = 0;

	tick.it_interval.tv_sec = 60;  
    tick.it_interval.tv_usec = 0; 

	if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
		printf("Set timer failed!\n");

    while(1)
    {
        sleep(5);
    }
    return 0;
}

int main(int argc, char* argv[])
{
	int ret = 0;

	if( 2 == argc && 0 == strcmp(argv[1],"1"))
	{
		stad_init();
		DEBUGP("[%s]: ==>start!\n", __FUNCTION__);
		/* open socket for netlink to collect efence data */
		ret = stad_efence_open_netlink_listen();
		if (ret != 0)
		{
			DEBUGP("[%s]: call efence_open_netlink_listen fail!\n", __FUNCTION__);
			return -1;
		}
		
		g_nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
		if(!g_nlh)
		{
			DEBUGP("[%s]: call malloc fail!\n", __FUNCTION__);
			return -1;
		}
		printf("waiting received!\n");
		// Read message from kernel

		while(1)
		{
			stad_efence_receive_fdb_info_from_kernel(g_netlink_2g_fd);
		}

		close(g_netlink_2g_fd);		
	}
	else if( 2 == argc && 0 == strcmp(argv[1],"2"))
	{
		stad_save_pid();
		stad_check_stad_timer_init();
	}
	
	if(g_nlh)
		free(g_nlh);
	
	return 0;
}


