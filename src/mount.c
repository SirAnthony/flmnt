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
#include <glib-object.h>


static GArray* attached_devices;
static int last_device_id = 0;


static void mount_remove_item(struct Device* ditem)
{
	tray_signal_emit("device-unmounted", ditem);
	device_free(ditem);
}


/**************************************************************************/
/***************************PUBLIC METHODS*********************************/
/**************************************************************************/

void mount_init()
{
	attached_devices = g_array_new(FALSE, FALSE, sizeof(struct Device*));
}


void mount_clean()
{
	int index;
	struct Device* ditem;
	for (index = 0; index < attached_devices->len; ++index) {
		ditem = g_array_index( attached_devices, struct Device*, index ) ;
		mount_remove_item( ditem );
	}
	g_array_free(attached_devices, FALSE);
}


int mount_add_device(struct Device* device)
{
	if (!device)
		return 0;

	device->id = last_device_id++;
	g_array_append_val(attached_devices, device);
	tray_signal_emit("device-mounted", device);
	return 1;
}


int mount_remove_device( int id )
{
	int index;
	struct Device* ditem;
	struct Device* fitem = NULL;
	for( index = 0; index < attached_devices->len; ++index ){
		ditem = g_array_index( attached_devices, struct Device*, index ) ;
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

