#include "../unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


// can see here that 
//  1. memory is allocated only when I iterate over all items
//  2. free works from different thread, so RES is decreasing. 
void *
threadSub(void *arg) {
    int i;
    char *p = (char*)arg;
    printf(" [thread %ld has been created\n", pthread_self());

    for( i = 0; i < 1000000; i++)
        p[i] = 0;
    
    getchar();
    printf(" [thread %ld cleaning up memory\n", pthread_self());
    free( p); 
    getchar();

    pthread_exit( 0 );
}

int
main( int arg_num, char* args[]) {

    int i;
    pthread_t tid;
    const int n = 5;
    pthread_t *ts = malloc( n * sizeof(pthread_t));
    for ( i = 0; i < n; i++) {
        char *p = malloc( 1000000*sizeof(char));
        if( pthread_create( &tid, NULL, threadSub, p) != 0) 
            logFatal ("'pthread_create");
        ts[i] = tid;
    }

    for ( i = 0; i < n; i++)
        pthread_join( ts[i], NULL);

    printf( "ready to finish\n");
    getchar();

    return 0;
}
