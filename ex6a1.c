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

#define ARR_SIZE 50
#define RANGE_RAND 10
#define MAX_PROCCESS 3
const int START =  1;

void init_array( int * );
void init( char *, int *, int * );
void client_connect( int *, int );
void terminate( char * );

int main( int argc, char ** argv )
{
    if( argc != 2 )
    {
        fprintf( stderr, "Missing values\n" );
        exit( EXIT_FAILURE );
    }

    srand( 0 );
    int arr[ ARR_SIZE ];
    int rc, main_socket, arr_client[ MAX_PROCCESS ] = { 0 };

    fd_set rfd, c_rfd;
    struct sockaddr_storage her_addr;
    socklen_t her_addr_size;


    init_array( arr );
    init( argv[ 1 ], &rc, &main_socket );

    client_connect( arr_client, main_socket );

    return( EXIT_SUCCESS );
}

void init_array( int *arr )
{
    int i;

    for( i = 0; i < ARR_SIZE; i++ )
        arr[ i ] = rand() % RANGE_RAND;
}

void init( char *my_port, int * rc, int * main_socket )
{
    struct addrinfo con_kind, *addr_info_res;

    memset( &con_kind, 0, sizeof con_kind );
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;
    con_kind.ai_flags = AI_PASSIVE;

    if( ( (* rc ) = getaddrinfo( NULL, my_port, &con_kind, &addr_info_res ) ) != 0 )
    {
        fprintf( stderr, "getaddrinfo() failed %s\n", gai_strerror( (* rc ) ) );
        exit( EXIT_FAILURE );
    }

    (* main_socket ) = socket( addr_info_res->ai_family,
                            addr_info_res->ai_socktype,
                            addr_info_res->ai_protocol );
    
    if( (* main_socket ) < 0 )
        terminate( "socket: allocation failed" );

    (* rc ) = bind( (* main_socket ), addr_info_res->ai_addr, addr_info_res->ai_addrlen );
    if( (* rc ) ) terminate( "bind failed" );

    (* rc ) = listen( (* main_socket ), MAX_PROCCESS );
    if( (* rc ) ) terminate( "listed failed" );
}

void client_connect( int * arr_clients, int main_socket )
{
    int counter_clients = MAX_PROCCESS;
    int index = 0, new_socket, i, j;
    fd_set readfds, c_readfds;

    while( counter_clients )
    {
        FD_ZERO( &readfds );
        FD_SET( main_socket, &readfds );
        if( FD_ISSET( main_socket, &readfds ) )
        {
            if( ( new_socket = accept( main_socket, NULL, NULL ) ) < 0 )
                terminate( "accept" );

            FD_SET( new_socket, &c_readfds );
            arr_clients[ index++ ] = new_socket;
            counter_clients--;
        }
    }

    for( i = main_socket + 1, j = 0; j < MAX_PROCCESS; i++ )
        write( i, &( arr_clients[ j++ ] ), sizeof( int ) );
}

void terminate( char * message_error )
{
    perror( message_error );
    exit( EXIT_FAILURE );
}