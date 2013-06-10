/*
 * mount.c
 *
 *  Created on: 10.06.2013
 *
 */

#include "mount.h"
#include "tray.h"
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <libgen.h>


static GArray* attached_devices;
static int last_device_id = 0;


static int mount_do_mount(struct device_item* item){
	if( !item )
		return 0;

	item->name = basename( item->text );
	item->path = malloc( 14 + strlen(item->name) );
	sprintf( item->path, "/media/flash/%s", item->name );
	return 0;
}


static struct device_item* mount_create_item( const char* target )
{
	struct device_item* ditem;
	ditem = (struct device_item*)malloc( sizeof(struct device_item*) );
	ditem->text = strdup( target );
	ditem->id = last_device_id++;
	mount_do_mount( ditem );
	tray_add_item( ditem );
	return ditem;
}

static void mount_remove_item( struct device_item* ditem )
{
	tray_remove_item( ditem );
	free( ditem->text );
	free( ditem->name );
	free( ditem->path );
	free( ditem );
}



/**************************************************************************/
/***************************PUBLIC METHODS*********************************/
/**************************************************************************/


void mount_init( )
{
	attached_devices = g_array_new( FALSE, FALSE, sizeof(struct device_item*) );
}


void mount_clean()
{
	int index;
	struct device_item* ditem;
	for( index = 0; index < attached_devices->len; ++index ){
		ditem = g_array_index( attached_devices, struct device_item*, index ) ;
		mount_remove_item( ditem );
	}
	g_array_free( attached_devices, FALSE);
}


int mount_add_device( int count, char** devices )
{
	int dn;
	struct device_item* ditem;
	for( dn = 0; dn < count; ++dn ){
		ditem = mount_create_item( devices[dn] );
		g_array_append_val( attached_devices, ditem );
	}
	return count;
}


int mount_remove_device( int id )
{
	int index;
	struct device_item* ditem;
	struct device_item* fitem = NULL;
	for( index = 0; index < attached_devices->len; ++index ){
		ditem = g_array_index( attached_devices, struct device_item*, index );
		if( ditem->id == id ){
			fitem = ditem;
			break;
		}
	}

	if( !fitem )
		return 0;

	g_array_remove_index( attached_devices, index );
	mount_remove_item( fitem );
	return 1;
}




