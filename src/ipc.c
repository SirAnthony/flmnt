/*
 * ipc.c
 *
 *  Created on: 11.06.2013
 *
 */

#include "ipc.h"
#include "device.h"
#include "mount.h"
#include <glib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>


const char* fifo_file = "/tmp/flmnt.fifo";
#define BUF_SIZE 1024

static GIOChannel* server_channel = NULL;
static GIOChannel* client_channel = NULL;

static gboolean fifo_read( GIOChannel* ch, GIOCondition condition, gpointer data )
{
	gsize len, tp;
	gchar *buf, *tmp, *all = NULL;
	GIOStatus rv;
	struct Device* dev = NULL;

	do{
		GError *err = NULL;
		rv = g_io_channel_read_line( ch, &buf, &len, &tp, &err );
		if( buf ){
			if( tp )
				buf[tp] = '\0';
			device_init_from_data( &dev, buf );
			g_free( buf );
		}else{
			buf = g_malloc( BUF_SIZE + 1 );
			while( TRUE ){
				buf[BUF_SIZE] = '\0';
				g_io_channel_read_chars( ch, buf, BUF_SIZE, &len, &err );
				if( len ){
					buf[len] = '\0';
					if( all ){
						tmp = all;
						all = g_strdup_printf( "%s%s", tmp, buf );
						g_free( tmp );
					}else{
						all = g_strdup( buf );
					}
				}else{
					break;
				}
			}
			if( all ){
				device_init_from_data( &dev, buf );
				g_free( all );
			}
			g_free( buf );
		}
		if( err ){
			g_error( "fifo_cb: %s", err->message );
			g_free( err );
			break;
		}
	} while( (len) && (rv == G_IO_STATUS_NORMAL) );

	if( dev )
		mount_add_device( dev );

	return TRUE;
}


static int ipc_role_client_init( int socket )
{
	client_channel = g_io_channel_unix_new( socket );
	return 0;
}


static int ipc_role_server_init( int socket )
{
	GError* error = NULL;
	int client_socket;

	server_channel = g_io_channel_unix_new( socket );
	g_io_channel_set_encoding( server_channel, NULL, &error );
	g_io_add_watch( server_channel, G_IO_IN | G_IO_PRI, fifo_read, NULL );

	client_socket = open( fifo_file, O_WRONLY | O_NONBLOCK );
	if( client_socket == -1 ){
		printf( "Error occured while open client socket: %s\n", strerror( errno) );
		return -1;
	}

	return ipc_role_client_init( client_socket );
}


static int init_fifo( int* socket )
{
	int status = IPC_SERVER;
	int flag = O_RDONLY | O_NONBLOCK;

	if( mkfifo( fifo_file, 0600 ) == -1 ){
		if( errno != EEXIST ){
			printf( "Error occured : %s\n", strerror( errno) );
			return -1;
		}
		status = IPC_CLIENT;
		flag = O_WRONLY | O_NONBLOCK;
	}

	*socket = open( fifo_file, flag );
	if( *socket == -1 )
		return -2;

	return status;
}


static int ipc_close_channel( GIOChannel* channel )
{
	int socket;
	if( !channel )
		return 1;
	socket = g_io_channel_unix_get_fd( client_channel );
	g_io_channel_close( client_channel );
	close( socket );
	return 0;
}


static int ipc_client_finalize( )
{
	return ipc_close_channel( client_channel );
}


static int ipc_server_finalize( )
{
	int ret = ipc_close_channel( server_channel );
	unlink( fifo_file );
	return ret;
}



int ipc_select_role(  )
{
	int socket;
	int role;

	role = init_fifo( &socket );

	switch( role ){
		case IPC_SERVER:
			ipc_role_server_init( socket );
			break;
		case IPC_CLIENT:
			ipc_role_client_init( socket );
			break;
		default:
			printf( "Error occured: %s\n", strerror( errno) );
			break;
	}
	return role;
}


int ipc_register_device( struct Device* device )
{
	GError* error = NULL;
	gsize device_size, written;
	if( !device || !client_channel )
		return -1;

	device_size = sizeof(struct Device);

	g_io_channel_write_chars( client_channel, (void*)device, device_size, &written, &error);
	if( error ){
		printf( "Error %s occured while data transfer.", error->message );
		return -2;
	}

	return 0;
}


int ipc_finalize( int role )
{
	int ret;
	switch( role ){
		case IPC_CLIENT:
			ret = ipc_client_finalize();
			break;
		case IPC_SERVER:
			ret = ipc_server_finalize();
			break;

	}
	return ret;
}
