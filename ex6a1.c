/* Reverse Bingo
 *=======================================
 * NAME: David maman & Daniel Cohen
 * class: a2
 * login: davidmam Danielcoh
 * 
 * This program get by vector argument seed
 * and create array by 1000 cells.
 * 
 * Then other process try to delete cells
 * from array.
 * 
 * When a array is empty a program get a signal
 * and print how long did it take to complete
 * the task.
*/

//--------- include section --------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdbool.h>

//--------- define & const section --------------
#define ARR_SIZE 1000
#define MAX_CLIENTS 3
#define RANGE_RAND 200000
const int NOT_FOUND = 0;
const int FOUND = 1;
const int FINISH = -1;

//--------- prototypes --------------
void build_array( int * );
void connect_clients( int );
int find( int *, int );
void init( int *, int *, char *  );
void terminate( char * );
void sig_term( int );

//------------ main -------------
int main( int argc, char ** argv )
{
    if( argc != 2 )
    {
        fprintf( stderr, "Mising values\n" );
        exit( EXIT_FAILURE );
    }
    signal( SIGTERM, sig_term );
    srand( 0 );

    int rc, main_socket, serving_socket, fd, total_time;
    int get_num, counter_receive, counter_delete, index, clients_connect;
    int arr_bingo[ ARR_SIZE ];
    fd_set rfd;
    fd_set c_rfd;
    time_t start, end;

    counter_receive = 0;
    counter_delete = ARR_SIZE;
    clients_connect = MAX_CLIENTS;

    build_array( arr_bingo );

    init( &main_socket, &rc, argv[ 1 ] );

    FD_ZERO( &rfd );
    FD_SET( main_socket, &rfd );

    while( counter_delete )
    {
        c_rfd = rfd;

        rc = select( getdtablesize(), &c_rfd, NULL, NULL, NULL );

        if( rc < 0 )
            terminate( "error select" );
        
        // start a time when 3 proccess connect
        if( FD_ISSET( main_socket, &c_rfd ) )
        {
            serving_socket = accept( main_socket, NULL, NULL );
            if( serving_socket >= 0 )
            {
                FD_SET( serving_socket, &rfd );
                clients_connect--;
                // !clients_connect -> clients_connect == 0
                if( !clients_connect )
                    start = time( NULL );
            }
        }

        if( !clients_connect )
        {
            for( fd = main_socket + 1; fd < getdtablesize(); fd++ )
            {
                if( FD_ISSET( fd, &c_rfd ) )
                {
                    rc = read( fd, &get_num, sizeof( int ) );
                    counter_receive++;

                    if( rc == 0 )
                    {
                        close( fd );
                        FD_CLR( fd, &rfd );
                    }
                    else if( rc > 0 )
                    {
                        index = find( arr_bingo, get_num );

                        if( index < 0 )
                            write( fd, &NOT_FOUND, sizeof( int ) );
                        else
                        {
                            write( fd, &FOUND, sizeof( int ) );
                            counter_delete--;
                        }
                    }
                    else
                        terminate( "error read()" );
                }
            }
        }
    }

    // send to all proccess finish, array is empty
    for( fd = main_socket + 1; fd < getdtablesize(); fd++ )
    {
        read( fd, &get_num, sizeof( int ) );
        write( fd, &FINISH, sizeof( int ) );
    }

    end = time( NULL );
    total_time = end - start;

    printf( "%d %d %d\n", total_time, counter_receive, ARR_SIZE - counter_delete );

    exit( EXIT_SUCCESS );
}
//----------------------------------------
/* terminate get array char
 * This function finish a program if 
 * some function failed
 * */
void terminate( char * error_message )
{
    perror( error_message );
    exit( EXIT_FAILURE );
}
//----------------------------------------
/* sig_term get integer
 * signal handler
 * */
void sig_term( int sig )
{
    exit( EXIT_SUCCESS );
}
//----------------------------------------
/* init get 2 pointers integer and array char
 * fthis function connect to socket
 */
void init( int * main_socket, int * rc, char * my_port )
{
    struct addrinfo con_kind, *addr_info_res;

    memset( &con_kind, 0, sizeof( con_kind ) );
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

    (* rc ) = listen( ( *main_socket ), 1 );
    if( (* rc ) ) terminate( "listen failed" );
}
//----------------------------------------
/* build_array get array of integer
 * fthis function fill array by random numbers
 */
void build_array( int *arr )
{
    int i;

    for( i = 0; i < ARR_SIZE; i++ )
        arr[ i ] = rand() % RANGE_RAND;
}
//----------------------------------------
/* find get array of integer and integer
 * fthis function search some number in array
 * if find return a index, else return -1
 */
int find( int * arr, int num )
{
    int i;

    for( i = 0; i < 10; i++ )
    {
        if( arr[ i ] == num )
            return i;
    }
    return -1;
}
