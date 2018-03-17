
#include "../unp.h"
#include <pthread.h>

// how to build:
// 

struct clientInfo_t {
  int clienth;
  size_t clientAddrLen;
  struct sockaddr_in clientAddr;
};
typedef struct clientInfo_t clientInfo;

void* clientThread(void *arg) {
    size_t size;
    char host[128];
    uint16_t port = 0;
    char buffer[MAXLINE + 1];
    clientInfo *ci = (clientInfo*)arg;
    pthread_t tid = pthread_self();
    int clienth = ci->clienth;
    struct sockaddr_in clientAddr = ci->clientAddr;

    memset( buffer, 0, MAXLINE);

    if( inet_ntop( clientAddr.sin_family, &clientAddr.sin_addr, host, sizeof(host)-1) == NULL)
        logFatal( "'inet_ntop");
    if( (port = ntohs( clientAddr.sin_port)) != 0) 
        printf(" [thread %lu] got connection from %s:%d\n",tid, host, port); 
    while ( (size = read( clienth, buffer, MAXLINE)) > 0) {
        printf( "[%s:%d] %s\n", host, port, buffer);
        Write( clienth, buffer, MAXLINE);
    }
    Close( clienth);
    free( ci);
    pthread_exit( 0 );
}

int 
main(int args_num, char *args[]) {
    int sockh, clienth;
    struct sockaddr_in sockAddr, clientAddr;
    socklen_t clientAddrLen = sizeof( clientAddr);
    const char exitSeq[] = "\\";

    printf( "************************************************\n");
    printf( "*               SERVER [%lu]                  *\n", getpid());
    printf( "************************************************\n");

    sockh = Socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SetServerAddress( &sockAddr, 1333);
    Bind( sockh, (SA*)&sockAddr, sizeof( sockAddr));
    Listen( sockh, LISTENQ);                                 


    while( 1 ) {
        int err;
        pthread_t tid;
        clienth = Accept( sockh, (SA*)&clientAddr, &clientAddrLen);
        clientInfo *ci = (clientInfo*)malloc( sizeof( clientInfo));
        memset( ci, 0, sizeof( clientInfo));
        ci->clienth = clienth;
        ci->clientAddr = clientAddr;
        ci->clientAddrLen = clientAddrLen;
        if( (err = pthread_create( &tid, NULL, clientThread, (void*)ci)) != 0) {
            logFatal( "'new_thread");  
        }
    }


    return 0;
}

     
