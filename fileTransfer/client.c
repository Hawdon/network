#include "../unp.h"

// how to run
//  ./client -h 10.0.1.101 -p 1333 -f ../testLines
// how to build
// gcc client.c -o client


int
main( int args_num, char *args[]) {

    FILE *f;
    long fileSize;
    int sockfd, n;
    char buf[MAXLINE + 1];
    struct sockaddr_in sockAddress;
    struct procParams *sp;

    sp = getParams( args_num, args);
    printf("%s:%d, %s\n", sp->host, sp->port, sp->fileName);
    sockfd = Socket( AF_INET, SOCK_STREAM, 0 );
    SetClientAddress( &sockAddress, sp->host, sp->port);
    Connect( sockfd, (SA*)&sockAddress, sizeof( sockAddress));

    bzero( buf, MAXLINE);
    f = FileOpen( sp->fileName, "r"); 
    //fseek( f, 0L, SEEK_END);
    //fileSize = ftell( f);

    while( (n = read( fileno(f), buf, MAXLINE)) > 0) { 
        printf("read %d bytes\n", n);
        Writen( sockfd, buf, n);
        bzero( buf, MAXLINE);
    }


    Shutdown( sockfd, SHUT_WR);

    logInfo("waiting for FIN message\n");
    if( (n = Read( sockfd, buf, MAXLINE)) == 0) {
        logInfo("connection is closed\n");
        close( sockfd);
        Fclose( f);
    }
    free( sp);

    return 0;
}
