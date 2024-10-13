#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/bufferevent_ssl.h>

#include <openssl/ssl.h>

//#define SVRADDR "192.168.6.103"
//#define SVRADDR "rd-rdsg-dcdda.auto.mydlink.com"
//#define SVRADDR "13.228.7.6"

//#define SVRADDR "www.yahoo.com.tw"
#define SVRADDR "106.10.160.45"
#define PORT 443



pthread_t my_thread;
struct sockaddr_in addr;

static void buff_input_cb(struct bufferevent *bev, void*ctx) {

    char result[10240];
    printf("***in %s\n", __func__);

    //printf("len=%d\n", evbuffer_get_length(bufferevent_get_input(bev)));
    //printf("data=%s\n", bufferevent_get_input(bev));

    printf("len=%d\n", bufferevent_read(bev, result, sizeof(result) - 1));
    printf("data=%s\n", result);
    
    return;
}

static void buff_ev_cb(struct bufferevent *bev, short events, void*ctx) {

    printf("in %s\n", __func__);

    if (events & BEV_EVENT_CONNECTED) {
        printf("***BEV_EVENT_CONNECTED\n");
    } else if (events & BEV_EVENT_ERROR) {
        printf("***BEV_EVENT_ERROR\n");
    }
    printf("<<<< %x >>>\n", events);
    bufferevent_write(bev, "test-test", sizeof("test-test"));
    return;
}

void* start_async_thread(void* arg) {
    struct event_base *p_base = arg;
    struct bufferevent *p_event;
    struct timeval tv = {10, 0};
    SSL_CTX* ssl_ctx = SSL_CTX_new(TLSv1_client_method());
    SSL* ssl;
    int option;
    in_addr_t             ip_addr;
    char  dest_name[]="rd-rdsg-dcdda.auto.mydlink.com";

    printf("start_async_thread running\n");
    /*if ((p_event = bufferevent_socket_new(p_base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE)) == NULL) {
      printf("bufferevent_socket_new \n");
      return NULL;
    }*/
    ssl = SSL_new(ssl_ctx);
    /*if ((sockfd = bufferevent_socket_connect(p_event, (struct sockaddr *)&addr, sizeof(addr))) <0) {
      printf("bufferevent_socket_connect ");
      return NULL;
    }*/
    option = BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE;
    printf("zzzzzzzzzzzzzzzzz\n");
    if ((p_event = bufferevent_openssl_socket_new(p_base, -1, ssl, BUFFEREVENT_SSL_CONNECTING, option)) == NULL) {
      printf("bufferevent_openssl_socket_new\n");
    }
    printf("xxxxxxxxxxxxxxxxxx\n");

    if (bufferevent_socket_connect_hostname(p_event, NULL, AF_INET, SVRADDR, PORT) != 0) {
      printf("bufferevent_socket_connect_hostname\n");
    }

    printf("yyyyyyyyyyyyyy\n");
    bufferevent_setcb(p_event, buff_input_cb, NULL, buff_ev_cb, p_base);
    bufferevent_enable(p_event, EV_READ);

    bufferevent_set_timeouts(p_event, &tv, NULL);
    printf("waiting ....................\n");
    event_base_dispatch(p_base);
    printf("start_async_thread stoping\n");

    return NULL;
}

int main() {
    struct event_base *p_base;

    SSL_library_init();
    SSL_load_error_strings();

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SVRADDR, &addr.sin_addr) <=0) {
        printf("inet_pton");
        exit(1);
    }

    if ((p_base = event_base_new()) == NULL) {
        printf("event_base_new ");
        return 1;
    }

    printf("start to running thread to execute connection\n");

	//start_async_thread((void *)p_base);
	
#if 1
    //start_async_thread(p_base);
    if (pthread_create(&my_thread, NULL, start_async_thread, p_base) != 0) {
      printf("can not create pthread\n");
    }

    pthread_join(my_thread, NULL);
#endif

    printf("libevent stop !!!");

    return 0;
}
