#include "../unp.h"

int
main( int args_num, char *args[]) {

    int n;
    int sockh, client;
    struct sockaddr_in sockAddress;
    char buf[MAXLINE];

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof( clientAddress);

    sockh = Socket( AF_INET, SOCK_STREAM, 0 );

    SetServerAddress( &sockAddress, 1333);

    Bind( sockh, (SA*)&sockAddress, sizeof( sockAddress));
    Listen( sockh, LISTENQ);

    client = Accept( sockh, (SA*)&clientAddress, &clientAddressLength);
    //setbuf(stdout, NULL);
    bzero( buf, MAXLINE);
    while((n = Read( client, buf, MAXLINE)) > 0) {
        printf("%s", buf);
        bzero( buf, MAXLINE);
    }

    //if((n = Read( client, buf, MAXLINE)) == 0) {
    //    printf("confirmed FIN message\n");
    //}

    Close( client);

    Close( sockh);
    //Fclose( fileHandle);
    return 0;

}


