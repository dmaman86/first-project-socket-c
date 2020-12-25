// socket server multi clients

#include <stdio.h>
#include <stdlib.h>
#include <string.h>        // for memset
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

void init( int *, int *, char *, char * );
void terminate( char * );

int main( int argc, char ** argv )
{
    if( argc < 4 )
    {
        fprintf( stderr, "Missing values\n" );
        exit( EXIT_FAILURE );
    }

    srand( atoi( argv[ 1 ] ) );
    int rc, my_socket;
    int answer;

    init( &rc, &my_socket, argv[ 2 ], argv[ 3 ] );

    write( my_socket, argv[ 1 ], strlen( argv[ 1 ] ) );

    read( my_socket, &answer, sizeof( int ) );

    printf( "%d\n", answer );


    return( EXIT_SUCCESS );
}

void init( int *rc, int *my_socket, char *str, char *my_port )
{
    struct addrinfo con_kind, *addr_info_res;

    memset( &con_kind, 0, sizeof( con_kind ) );
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;

    if( ( ( *rc) = getaddrinfo( str, my_port, &con_kind, &addr_info_res ) ) != 0 )
    {
        fprintf( stderr, "getaddrinfo() failed %s\n", gai_strerror( (* rc ) ) );
        exit( EXIT_FAILURE );
    }

    (* my_socket ) = socket( addr_info_res->ai_family,
                            addr_info_res->ai_socktype,
                            addr_info_res->ai_protocol );

    if( ( *my_socket ) < 0 )
        terminate( "socket: allocation failed" );

    (* rc ) = connect( (* my_socket ),
                        addr_info_res->ai_addr,
                        addr_info_res->ai_addrlen ); 
    if( ( *rc ) ) terminate( "connect failed" );

}

void terminate( char * message_error )
{
    perror( message_error );
    exit( EXIT_FAILURE );
}