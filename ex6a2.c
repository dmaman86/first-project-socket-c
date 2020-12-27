//client program:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>        // for read/write/close
#include <sys/types.h>     /* standard system types       */
#include <netinet/in.h>    /* Internet address structures */
#include <sys/socket.h>    /* socket interface functions  */
#include <netdb.h>         /* host to IP resolution       */
#include <stdbool.h>
#include <time.h>

#define RANGE_RAND 2000
const int FOUND = 1;
const int FINISH = -1;

void terminate( char * );
void init( int *, int *, char *, char * );

int main(int argc, char *argv[])
{
	 if (argc != 4)
    {
	    fprintf(stderr, "Missing server name \n");
	    exit(EXIT_FAILURE) ;
	}

    srand( atoi( argv[ 1 ] ) );
    int rc, my_socket, rand_num;
    int counter_send = 0, counter_success = 0, answer;
    bool cnt = true;


    init( &my_socket, &rc, argv[ 2 ], argv[ 3 ] );

    while( cnt )
    {
        rand_num = rand() % RANGE_RAND;

        if( ( rc = write( my_socket, &rand_num, sizeof( int ) ) ) < 0 )
            terminate( "error write" );
        counter_send++;

        if( ( rc = read( my_socket, &answer, sizeof( int ) ) ) < 0 )
            terminate( "error read" );

        if( answer == FOUND )
            counter_success++;
        if( answer == FINISH )
            cnt = false;
    }

    close( my_socket );

    printf( "%d %d\n", counter_send, counter_success );
	
    return( EXIT_SUCCESS );
}

void init( int *my_socket, int *rc, char * str, char * my_port )
{
    struct addrinfo con_kind, *addr_info_res;

    memset( &con_kind, 0, sizeof( con_kind ) );
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;

    if( ( (* rc ) = getaddrinfo( str, my_port, &con_kind, &addr_info_res ) ) != 0 )
    {
        fprintf( stderr, "getaddrinfo() failed %s\n", gai_strerror( (* rc ) ) );
        exit( EXIT_FAILURE );
    }

    (* my_socket ) = socket( addr_info_res->ai_family,
                            addr_info_res->ai_socktype,
                            addr_info_res->ai_protocol );

    if( (* my_socket ) < 0 )
        terminate( "socket: allocation failed" );
    
    (* rc ) = connect( (* my_socket ), addr_info_res->ai_addr, addr_info_res->ai_addrlen );
    if( ( *rc ) ) terminate( "connect failed" );

    freeaddrinfo( addr_info_res );
}

//=========================================================================
//this function terminate the program if there is an error in fork
void terminate( char *error_message )
{
    perror( error_message );
    exit(EXIT_FAILURE);
}
