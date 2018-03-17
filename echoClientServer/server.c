
#include "../unp.h"

// how to build:
// 


int 
main(int args_num, char *args[]) {
  int sockh, clienth;
  struct sockaddr_in sockAddr, clientAddr;
  socklen_t clientAddrLen = sizeof( clientAddr);
  char buffer[MAXLINE + 1];
  const char exitSeq[] = "\\";

  sockh = Socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
  SetServerAddress( &sockAddr, 1333);
  Bind( sockh, (SA*)&sockAddr, sizeof( sockAddr));
  Listen( sockh, LISTENQ);                                 

  memset( buffer, 0, MAXLINE);

  while( 1 ) {
    uint16_t port = 0;
    char host[128];
    size_t size;
    //struct sockaddr_in *sin;
    clienth = Accept( sockh, (SA*)&clientAddr, &clientAddrLen);

    //sin = (struct sockaddr_in *)clientAddr;
    if( inet_ntop( clientAddr.sin_family, &clientAddr.sin_addr, host, sizeof(host)-1) == NULL)
        logFatal( "'inet_ntop");
    if( (port = ntohs( clientAddr.sin_port)) != 0) 
        printf(" got connection from %s:%d\n",host, port); 
    while ( (size = read( clienth, buffer, MAXLINE)) > 0) {
        printf( "[%s:%d] %s\n", host, port, buffer);
        Write( clienth, buffer, MAXLINE);
    }
    //Close( clienth);

  }


    return 0;
}

     
