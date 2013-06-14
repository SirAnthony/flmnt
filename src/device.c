/*
 * device.c
 *
 *  Created on: 13.06.2013
 *
 */

#include "device.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>


const unsigned int DEVICE_SIZE = sizeof(struct Device);
const char* DEFAULT_OPTS = "rw,noatime,flush,users";
		//"rw,noatime,flush,uid=1000,iocharset=utf8,codepage=866,users"
const char* DEFAULT_TARGET = "/media/flash/%s";


int device_init_from_data( struct Device** tgt, char* data )
{
	if( !data )
		return -1;

	if( !*tgt )
		*tgt = malloc( DEVICE_SIZE );

	memcpy( *tgt, data, DEVICE_SIZE );
	return 0;
}


int device_init_from_env( struct Device* ditem )
{
	char* base;


	strncpy( &ditem->source[0], getenv("mount_device"), DEV_MAX_PATH);
	if( ditem->source[0] == '\0' ){
		printf("Device not specified. Exit.");
		return -1;
	}

	strncpy( ditem->fstype, getenv("mount_fstype"), DEV_MAX_INFO);
	if( ditem->fstype[0] == '\0' ){
		printf("Device filesystem type not specified. Exit.");
		return -2;
	}

	base = basename( ditem->source );

	strncpy(ditem->name, getenv("mount_name"), DEV_MAX_INFO);
	if( ditem->name[0] == '\0' )
		strncpy( ditem->name, base, DEV_MAX_INFO );

	strncpy(ditem->target, getenv("mount_target"), DEV_MAX_PATH);
	if( ditem->target[0] == '\0' )
		snprintf( ditem->target, DEV_MAX_PATH, DEFAULT_TARGET, base );

	strncpy(ditem->options, getenv("mount_options"), DEV_MAX_PATH);
	if( ditem->options[0] == '\0' )
		strncpy(ditem->options, DEFAULT_OPTS, DEV_MAX_PATH);

	ditem->mounted = 0;
	ditem->id = -1;

	free(base);

	return 0;
}




void device_free( struct Device* tgt )
{
	if( !tgt )
		return;
	free( tgt );
}
