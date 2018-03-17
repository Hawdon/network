#include "../unp.h"

// how to run
//  ./client -h 10.0.1.101 -p 1333 -f ../testLines
// how to build
// gcc client.c -o client

int
main( int args_num, char *args[]) {

    FILE *f;
    long fileSize;
    int sockfd, n, port, opt;
    char buf[MAXLINE + 1];
    char host[MAXLINE + 1];
    char fileName[MAXLINE + 1];
    struct sockaddr_in sockAddress;


    while ( (opt = getopt( args_num, args, "h:p:f:")) != -1) { 
        switch (opt) {
            case 'h': strncpy( host, optarg, MAXLINE); break;
            case 'p': port = atoi( optarg); break;
            case 'f': strncpy( fileName, optarg, MAXLINE); break;
            default: fprintf( stderr, "Usage: %s -h hostname -p port -f filename\n", args[0]);
                     logFatal( "'arg parse");
        }
    }
    
    printf("%s:%d, %s\n", host, port, fileName);
    sockfd = Socket( AF_INET, SOCK_STREAM, 0 );
    SetClientAddress( &sockAddress, host, port);
    Connect( sockfd, (SA*)&sockAddress, sizeof( sockAddress));

    bzero( buf, MAXLINE);
    f = FileOpen( fileName, "r"); 
    //fseek( f, 0L, SEEK_END);
    //fileSize = ftell( f);

    while( (n = read( fileno(f), buf, MAXLINE)) > 0) { 
        printf("read %d bytes\n", n);
        Writen( sockfd, buf, n);
    }

    Fclose( f);

    Shutdown( sockfd, SHUT_WR);

    logInfo("waiting for FIN message\n");
    if( (n = Read( sockfd, buf, MAXLINE)) == 0) {
        logInfo("connection is closed\n");
        close( sockfd);
    }

    return 0;
}
