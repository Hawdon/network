#include "../unp.h"

// how to build:
// 

int 
main(int args_num, char *args[]) {

    size_t n;
    int sockh;
    struct sockaddr_in sockAddr;
    char *buffer_p;
    char buffer[MAXLINE + 1];

    sockh = Socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SetClientAddress(&sockAddr, "127.0.0.1", 1333);
    Connect( sockh, (SA*)&sockAddr, sizeof(sockAddr));

    while ( 1 ) {
      buffer_p = fgets( buffer, MAXLINE, stdin); 
      printf(" sending %s", buffer_p);
      Write( sockh, buffer_p, MAXLINE);
      if ( (n = read( sockh, buffer, MAXLINE)) > 0) {
          printf( " receiving %s\n", buffer);
      }
    }

    Close( sockh);
    return 0;
}
