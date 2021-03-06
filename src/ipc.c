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
	gsize len, rv, oldlen;
	gchar *buf, *resvd = NULL;
	GError *err = NULL;
	struct Device* dev = NULL;

	buf = g_malloc(BUF_SIZE + 1);
	oldlen = 1;

	do {
		err = NULL;
		buf[BUF_SIZE] = '\0';
		rv = g_io_channel_read_chars(ch, buf, BUF_SIZE, &len, &err);
		if (!len)
			break;
		resvd = (gchar*) g_realloc(resvd, oldlen + len);
		memcpy(&resvd[oldlen - 1], buf, len);
		oldlen += len;
		resvd[oldlen] = '\0';
		if (err) {
			g_warning("fifo_cb: %s", err->message);
			g_free(err);
			break;
		}
	} while ((len) && (rv == G_IO_STATUS_NORMAL));

	if (resvd) {
		if (device_init_from_data(&dev, resvd) >= 0)
			mount_add_device(dev);
		g_free(resvd);
	}
	g_free(buf);

	return TRUE;
}


static int ipc_close_channel( GIOChannel* channel )
{
	GError* error = NULL;
	int socket;

	if (!channel)
		return 1;

	socket = g_io_channel_unix_get_fd(client_channel);
	g_io_channel_shutdown(client_channel, TRUE, &error);
	if( error ){
		g_warning( "Error %s occured while channel closing.", error->message );
		return -2;
	}
	close(socket);
	return 0;
}


static int ipc_role_client_init( int socket )
{
	GError* error = NULL;
	client_channel = g_io_channel_unix_new( socket );
	g_io_channel_set_encoding( client_channel, NULL, &error );
	if( error ){
		g_warning( "Error %s occured while encoding setup.", error->message );
		ipc_close_channel(server_channel);
		return -2;
	}

	return 0;
}


static int ipc_role_server_init( int socket )
{
	GError* error = NULL;
	int client_socket;

	g_message("Starting mount server.");

	server_channel = g_io_channel_unix_new( socket );
	g_io_channel_set_encoding( server_channel, NULL, &error );
	if( error ){
		g_warning( "Error %s occured while encoding setup.", error->message );
		ipc_close_channel(server_channel);
		return -2;
	}

	g_io_add_watch( server_channel, G_IO_IN | G_IO_PRI, fifo_read, NULL );

	client_socket = open( fifo_file, O_WRONLY | O_NONBLOCK );
	if( client_socket == -1 ){
		g_warning( "Error occured while open client socket: %s", strerror( errno) );
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
			g_warning( "Error %i occured in fifo init: %s", errno, strerror(errno) );
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


static int ipc_client_finalize( )
{
	return ipc_close_channel( client_channel );
}


static int ipc_server_finalize( )
{
	int ret = ipc_close_channel( server_channel );
	g_message("Closing mount server.");
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
			if (ipc_role_server_init(socket) < 0){
				ipc_server_finalize();
				return -1;
			}
			break;
		case IPC_CLIENT:
			if (ipc_role_client_init(socket) < 0){
				ipc_client_finalize();
				return -1;
			}
			break;
		default:
			g_warning("Error occured in role selection: %s", strerror(errno));
			break;
	}

	return role;
}


int ipc_register_device( struct Device* device )
{
	GError* error = NULL;
	gsize device_size, written;
	const gchar* dev_ptr;
	if( !device || !client_channel )
		return -1;

	dev_ptr = (const gchar*)device;
	device_size = sizeof(struct Device);

	g_message("Registering device %s.", device->source);
	g_io_channel_write_chars( client_channel, dev_ptr, device_size, &written, &error);
	if( error ){
		g_warning( "Error %s occured while data transfer.", error->message );
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
