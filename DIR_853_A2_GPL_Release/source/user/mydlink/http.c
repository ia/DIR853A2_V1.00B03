
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <linux/un.h>


int connect_tcp_server(struct sockaddr_in server, int port)
{
	int fd = 0;
	int ret;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		return -1;
	ret = connect(fd, (struct sockaddr *)&server, sizeof(server));
	if (ret < 0)
	{
		printf("conn===============\n");
		close(fd);
		return -1;
	}
	return fd;
}

int http_packet(char *buff, int msize, const char *host, const char *body, const char *soapmethod)
{
	int len;
	len = 0;

	len += snprintf(&buff[len], msize - len, "POST /HNAP1/ HTTP/1.1\r\n");
	len += snprintf(&buff[len], msize - len, "Host: %s\r\n", host);
	len += snprintf(&buff[len], msize - len, "Content-Type: text/xml; charset=UTF-8\r\n");
	len += snprintf(&buff[len], msize - len, "Content-Length: %d\r\n", strlen(body));
	len += snprintf(&buff[len], msize - len, "X-Requested-With: XMLHttpRequest\r\n");
	//len += snprintf(&buff[len], msize - len, "HNAP_AUTH: 8B6AA4A940E58AB9DDE897BD617C41AE 1501491974605\r\n");
	len += snprintf(&buff[len], msize - len, "SOAPAction: \"http://purenetworks.com/HNAP1/%s\"\r\n", soapmethod);
	//len += snprintf(&buff[len], msize - len, "Cookie: uid=xKjEUQJq\r\n");
	len += snprintf(&buff[len], msize - len, "\r\n");

	len += snprintf(&buff[len], msize - len, "%s", body);
	return len;
	
}
int make_hnap_packet(char *buff, int msize, const char *soapmethod)
{
	int len = 0;

	len += snprintf(&buff[len], msize - len, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n");
	len += snprintf(&buff[len], msize - len, "<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ");
	len += snprintf(&buff[len], msize - len, "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" ");
	len += snprintf(&buff[len], msize - len, "xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n");
	len += snprintf(&buff[len], msize - len, "<soap:Body>\n");
	len += snprintf(&buff[len], msize - len, "<%s xmlns=\"http://purenetworks.com/HNAP1/\" />\n", soapmethod);
	len += snprintf(&buff[len], msize - len, "</soap:Body>\n");
	len += snprintf(&buff[len], msize - len, "</soap:Envelope>\n");
	return len;
}

int make_hnap_set_packet(char *buff, int msize, const char *soapmethod, char *setvalue)
{
	int len = 0;

	len += snprintf(&buff[len], msize - len, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n");
	len += snprintf(&buff[len], msize - len, "<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ");
	len += snprintf(&buff[len], msize - len, "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" ");
	len += snprintf(&buff[len], msize - len, "xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n");
	len += snprintf(&buff[len], msize - len, "<soap:Body>\n");
	len += snprintf(&buff[len], msize - len, "<%s xmlns=\"http://purenetworks.com/HNAP1/\">\n", soapmethod);
	len += snprintf(&buff[len], msize - len, "%s\n", setvalue);
	len += snprintf(&buff[len], msize - len, "</%s>\n", soapmethod);
	len += snprintf(&buff[len], msize - len, "</soap:Body>\n");
	len += snprintf(&buff[len], msize - len, "</soap:Envelope>\n");
	return len;
}


int make_wlanradiosecurity_packet(char *buff, int msize, const char *soapmethod)
{
	int len = 0;

	len += snprintf(&buff[len], msize - len, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
	len += snprintf(&buff[len], msize - len, "<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ");
	len += snprintf(&buff[len], msize - len, "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" ");
	len += snprintf(&buff[len], msize - len, "xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
	len += snprintf(&buff[len], msize - len, "<soap:Body>");
	len += snprintf(&buff[len], msize - len, "<%s xmlns=\"http://purenetworks.com/HNAP1/\">", soapmethod);
	len += snprintf(&buff[len], msize - len, "<RadioID>RADIO_2.4GHz</RadioID>");
	len += snprintf(&buff[len], msize - len, "</%s>", soapmethod);
	len += snprintf(&buff[len], msize - len, "</soap:Body>");
	len += snprintf(&buff[len], msize - len, "</soap:Envelope>\n");
	return len;
}

int  make_SetWLanRadioSecurity_body(char *body, int size, const char *wpakey)
{
	int len = 0;

	len += snprintf(body + len, size - len, "<RadioID>RADIO_2.4GHz</RadioID>");
	len += snprintf(body + len, size - len, "<Enabled>true</Enabled>");
	len += snprintf(body + len, size - len, "<Type>WPAORWPA2-PSK</Type>");
	len += snprintf(body + len, size - len, "<Encryption>TKIPORAES</Encryption>");
	len += snprintf(body + len, size - len, "<KeyRenewal>3600</KeyRenewal>");
	len += snprintf(body + len, size - len, "<RadiusIP1></RadiusIP1>");
	len += snprintf(body + len, size - len, "<RadiusPort1></RadiusPort1>");
	len += snprintf(body + len, size - len, "<RadiusSecret1></RadiusSecret1>");
	len += snprintf(body + len, size - len, "<RadiusIP2></RadiusIP2>");
	len += snprintf(body + len, size - len, "<RadiusPort2></RadiusPort2>");
	len += snprintf(body + len, size - len, "<RadiusSecret2></RadiusSecret2>");
	len += snprintf(body + len, size - len, "<Key>%s</Key>", wpakey);
	return len;
}

int main(int argc, char **argv)
{
	int fd;
	char buff[2048];
	char body[2048];
	int len;
	struct sockaddr_in server;

	
	memset((void *)buff, 0, sizeof(buff));
	memset((void *)body, 0, sizeof(body));
	
	memset((void *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(80);
	server.sin_addr.s_addr = inet_addr(argv[1]);

	if (argc < 3)
	{
		printf("usage: http ipaddr hnapmethod wpapsk\n");
		return -1;
	}
	fd = connect_tcp_server(server, 80);
	if (fd < 0)
	{
		printf("connect failed\n");
		return -1;
	}
	if (strcmp(argv[2], "GetWLanRadioSecurity") == 0)
		make_wlanradiosecurity_packet(body, sizeof(body), argv[2]);
	else if (strcmp(argv[2], "SetMyDLinkRegistration") == 0)
	{
		make_hnap_set_packet(body, sizeof(body), argv[2], "<Registration>true</Registration>");
	}
	else if (strcmp(argv[2], "SetMyDLinkUnregistration") == 0)
	{
		make_hnap_set_packet(body, sizeof(body), argv[2], "<Unregistration>true</Unregistration>");
	}
	else if (strcmp(argv[2], "SetWLanRadioSecurity") == 0)
	{
		char data[1500];
		memset((void *)data, 0, sizeof(data));
		make_SetWLanRadioSecurity_body(data, sizeof(data), argv[3]);
		make_hnap_set_packet(body, sizeof(body), argv[2], data);
	}
	else
		len = make_hnap_packet(body, sizeof(body), argv[2]); //"GetWLanRadios");
	len = http_packet(buff, sizeof(buff), argv[1], body, argv[2]); //"GetWLanRadios");
	printf("%s\n\n\n", buff);
	send(fd, buff, len, 0);
	memset((void *)body, 0, sizeof(body));
	recv(fd, body, sizeof(body), 0);
	printf("%s\n", body);
	close(fd);
	return 0;
}



