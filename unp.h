#ifndef __UNP__
#define __UNP__
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>


#define MAXLINE 1000
#define LISTENQ 1024

#define STATICMAX 65

#define CLIENT_LEN 1024

#define max(a, b) ((a>b)?a:b)

#define SA struct sockaddr

typedef struct {
    int size, n;
    char * buf;
} Buffer;

typedef struct {
    int fd;
    int sd;
    time_t stime;
    Buffer buf;
    char user[MAXLINE];
    char hostname[MAXLINE];
} Client;

struct procParams {
    int port;
    char host[MAXLINE];
    char fileName[MAXLINE];
};


char * GetTime() {
    struct tm *time_str_tm;
    struct timeval current_time;
    static char buffer[ STATICMAX ];        // TODO: not thread safe

    gettimeofday(&current_time, NULL);
    time_str_tm = gmtime(&current_time.tv_sec);

    snprintf( buffer, STATICMAX, "%04d.%02d.%02d %02d:%02d:%02d:%06i ",
              1900 + time_str_tm->tm_year,
              1 + time_str_tm->tm_mon,
              time_str_tm->tm_mday,
              time_str_tm->tm_hour,
              time_str_tm->tm_min,
              time_str_tm->tm_sec,
              (int)current_time.tv_usec
            );

    return buffer;
}

void logError( const char *str) {
    perror( str);
}

void logFatal( const char *str) {
    perror( str);
    exit( 1);
}

void logInfo( const char* msg) {
  printf( "%s %s\n", GetTime(), msg);
}

struct procParams* 
getParams( int args_num, char*args[]) {
    int opt;
    struct procParams *sp;
    if( args_num < 2) {
        fprintf( stderr, "Usage: %s -h hostname -p port -f filename\n", args[0]);
        exit(1);
    }
    sp = malloc( sizeof( struct procParams));
    memset( sp, 0, sizeof(struct procParams));
    while ( (opt = getopt( args_num, args, "h:p:f:")) != -1) { 
        switch (opt) {
            case 'h': strncpy( sp->host, optarg, MAXLINE); break;
            case 'p': sp->port = atoi( optarg); break;
            case 'f': strncpy( sp->fileName, optarg, MAXLINE); break;
            default: fprintf( stderr, "Usage: %s -h hostname -p port -f filename\n", args[0]);
                     logFatal( "'arg parse");
        }
    }
    return sp;
}
/*
 * @arg d (int)- domain
 * @arg t (int) - type
 * @arg p (int) - protocol
 */
int 
Socket( int d, int t, int p) {
    int h; 
    if ( ( h = socket( d, t, p) ) < 0 ) 
        logFatal( "'socket error");
    return h;
}

int 
SetSockOpt( int sockfd, int level, int optname, void *optval, socklen_t optlen) {
    int r;
    if ( (r = setsockopt( sockfd, level, optname, optval, optlen)) < 0) 
        logFatal( "'setsockopt");
    return r;
}

int
Connect( const int s,const struct  sockaddr *sa, socklen_t size){
    int r;
    if ( ( r = connect( s, sa, size)) < 0) 
        logFatal ("'connect");
    return r;
} 

int
Fcntl( int fd, int cmd, ...) {
    int r;
    va_list argptr;    
    if ( (r = fcntl( fd, cmd, argptr)) < 0) 
        logFatal ("'fcntl");

    return  r;
}

FILE *
FileOpen( const char* fileName, const char* mode) {
   FILE *f;
   if ( !(f = fopen( fileName, mode)))
       logFatal( "'file");
   return f;
}

void
Fclose( FILE* fh ) {
   int n;
   if ( (n = fclose( fh)) < 0)
       logFatal( "'fclose");
   
}

/*
 * Does init and sets params for socket structure
 */
void 
SetClientAddress( struct sockaddr_in *sp, const char* host, int port) {
  memset( (void*)sp, 0, sizeof(*sp));
  sp->sin_family = AF_INET;
  sp->sin_port = htons( port);   // forgot to do this and got
                                 // connection refuced
  if ( inet_pton( AF_INET, host, &sp->sin_addr) <= 0)
      logFatal( "'address");
}

void
SetServerAddress( struct sockaddr_in *sockaddr_p, int port) {
  memset( (void*)sockaddr_p, 0, sizeof(*sockaddr_p));
  sockaddr_p->sin_family = AF_INET;
  sockaddr_p->sin_addr.s_addr = htonl( INADDR_ANY);
  sockaddr_p->sin_port = htons( port);
}

void
Bind( int fh, struct sockaddr *sa, size_t size) {
  int r;
  if ( (r = bind( fh, sa, size) < 0))
      logFatal( "'bind");
}

void 
Listen( int sh, int backlogSize) {
    int n;
    if ( (n=listen( sh, backlogSize)) < 0)
        logFatal( "'listen");
}

int
Accept( int sh, struct sockaddr *sa, socklen_t *len) {
    int n;
    if ( (n = accept( sh, sa, len)) < 0 )
        logFatal( "'accept");
    return n;
}

ssize_t 
Write( int ch, const char* str, size_t size) {
    ssize_t n;
    if ( (n = write( ch, str, size)) < 0 )
        logFatal( "'write");
    return n;
}

ssize_t
Writen( int fd, const void *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while( nleft > 0) {
        if( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if( nwritten < 0 && errno == EINTR)
                nwritten = 0;   // call write again
            else
                return -1;
        }
        //printf(" write %ld\n", nwritten);
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}


ssize_t 
Read( int ch, char* buf, size_t size) {
    ssize_t n;
    if ( (n = read( ch, buf, size)) < 0 )
        logFatal( "'read");
    return n;
}

int Shutdown( int sockfd, int how) {
    int r;
    if( (r = shutdown( sockfd, how) < 0)) {
        logFatal( "'shutdown");
    }
    return r;
}

void Close ( int ch) {
    int n ;
    if ( (n = close(ch)) < 0 ) 
        logFatal( "'close");
}



#endif  // __UNP__
