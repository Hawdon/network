#include "../unp.h"

// how to build:
// cc client.c -o client
// gcc client.c -o client
//

void str_cli(FILE *fp, int sockfd) {
    int maxfdp1, stdineof;
    fd_set rset;
    char buf[MAXLINE + 1] ;
    int n;

    stdineof = 0;
    FD_ZERO( &rset);
    for(; ;) {
       if( stdineof == 0)
          FD_SET( fileno( fp), &rset);
       FD_SET( sockfd, &rset);
       maxfdp1 = max( fileno(fp), sockfd) + 1;

       if( select( maxfdp1, &rset, NULL, NULL, NULL) == -1) {
           logFatal("'select");
       }

       if( FD_ISSET( sockfd, &rset)){
           if( (n = Read( sockfd, buf, MAXLINE)) == 0) {
              if( stdineof == 1)
                 return;
              else
                 logFatal("'server terminated");
           }
           Write( fileno( stdout), buf, n);
       }

       if( FD_ISSET( fileno( fp), &rset)) {             // have data in the buffer to read
           if( (n = Read( fileno(fp), buf, MAXLINE)) == 0) {
              stdineof = 1;
              Shutdown( sockfd, SHUT_WR);              // stop writing to the socket
              FD_CLR( fileno(fp), &rset);
              //printf("did shutdown\n");
              continue;
           }
           //printf("read %d:%s\n", n, buf);    
           Writen( sockfd, buf, n);
           bzero( buf, MAXLINE);
       }
    }
}
            
int 
main(int args_num, char *args[]) {
    int maxfdp1;
    fd_set rset;
    int sockh, consh;
    struct sockaddr_in sockAddr;
    char buffer[MAXLINE + 1];

    sockh = Socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
    consh = fileno( stdin);

    SetClientAddress(&sockAddr, "10.0.1.101", 1333);
    Connect( sockh, (SA*)&sockAddr, sizeof(sockAddr));

    str_cli( stdin, sockh);

    Close( sockh);

    return 0;
}
