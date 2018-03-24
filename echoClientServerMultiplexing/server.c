
#include "../unp.h"
#include <pthread.h>

// how to build:
// 

int 
main(int args_num, char *args[]) {
    int nready,     /* select output*/
        sockfd,
        connfd,
        listenfd,
        i, maxi, maxfd;
    unsigned int recvBytes[FD_SETSIZE];
    ssize_t n;
    socklen_t clilen;
    char buf[MAXLINE];
    fd_set rset, allset;
    int clients[FD_SETSIZE];

    struct sockaddr_in servaddr, cliaddr;

    long maxOpenHandles = sysconf(_SC_OPEN_MAX);

    printf( "************************************************\n");
    printf( "*               SERVER [%lu]                   *\n", getpid());
    printf( "*               (max open handles %ld)         *\n", maxOpenHandles);
    printf( "************************************************\n");

    listenfd = Socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

    bzero( &servaddr, sizeof(servaddr));
    SetServerAddress( &servaddr, 1333);

    Bind( listenfd, (SA*)&servaddr, sizeof( servaddr));

    Listen( listenfd, LISTENQ);                                 

    /* initialization */
    maxi = -1;
    maxfd = listenfd;
    for( i = 0; i < FD_SETSIZE; i++) {
        clients[i] = -1;
        recvBytes[i] = 0;
    }
    FD_ZERO( &allset);
    FD_SET( listenfd, &allset);
    while ( 1 ) { 
        rset = allset;
        nready = Select( maxfd + 1, &rset, NULL, NULL, NULL);

        if( FD_ISSET( listenfd, &rset)) {
            clilen = sizeof( cliaddr);
            connfd = Accept( listenfd, (SA*)&cliaddr, &clilen);

            for( i = 0; i < FD_SETSIZE; i++) {
                if( clients[i] < 0) {
                    clients[i] = connfd;
                    break;
                }
            }
            if( i == FD_SETSIZE)
                logFatal( "'too many clients");

            FD_SET( connfd, &allset);
            if( connfd > maxfd)
                maxfd = connfd; // max file descriptor for select
            if( i > maxi)
                maxi = i;       // max index in client array

            if( --nready <= 0)
                continue;       // don't process further if no other fds are ready
        }

        for( i = 0; i <= maxi; i++) {
            if( (sockfd = clients[i]) < 0)
                continue;
            if( FD_ISSET( sockfd, &rset)) {
                if( (n = Read( sockfd, buf, MAXLINE)) == 0) { // FIN message
                    Close( sockfd);
                    FD_CLR( sockfd, &allset);
                    clients[i] = -1;
                    printf(" client %d (%d) received %d disconnected\n",i, sockfd, recvBytes[sockfd]);
                    recvBytes[sockfd] = 0;
                } else  {
                    //printf( "Read from client %d %d bytes\n", sockfd, n);
                    recvBytes[sockfd]+= n;
                    Writen( sockfd, buf, n);
                    // Writen for echo function
                }
                if( --nready <= 0)
                    break;
            }
        }
    }
    return 0;
}

     
