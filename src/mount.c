/*
 * mount.c
 *
 *  Created on: 10.06.2013
 *
 */

#include "mount.h"
#include "tray.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <glib-object.h>
#include <sys/mount.h>
#include <stdio.h>
#include <unistd.h>


static GArray* attached_devices;
static int last_device_id = 0;



static void mount_add_item(struct Device* ditem)
{
	char cmd[1024];
	char driver[10];
	int status;

	if (strcmp(ditem->fstype, "ntfs") == 0)
		sprintf(&driver[0], "ntfs-3g");
	else
		sprintf(&driver[0], "auto");

	status = create_dir(ditem->target);
	if (status != CREATE_PATH_SUCCESS)
		return;

	// TODO: use mount call instead of shell
	// It requires arguments parsing
	sprintf(&cmd[0], "mount -t%s %s %s -O %s", driver,
			ditem->source, ditem->target, ditem->options);
	status = system(cmd);
	if (status > 0){
		g_warning("Mount returns status %i for %s.", status, ditem->source);
		return;
	}
	ditem->mounted = 1;

	tray_signal_emit("device-mounted", ditem);
}


static void mount_remove_item(struct Device* ditem)
{
	int status;

	g_message("Unmounting device %s.", ditem->source);

	status = umount(ditem->source);
	if (status < 0){
		g_warning("Error while unmounting: %i.", errno);
		return;
	}

	status = unlink(ditem->target);
	if (status < 0)
		g_warning("Unlink failed: %i.", errno);

	ditem->mounted = 0;
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

	g_message("Mounting device %s.", device->source);

	device->id = last_device_id++;
	g_array_append_val(attached_devices, device);
	mount_add_item(device);
	return 1;
}


int mount_remove_device(struct Device* device)
{
	int index;
	struct Device* ditem;

	if( !device )
		return 0;

	ditem = NULL;

	for( index = 0; index < attached_devices->len; ++index ){
		ditem = g_array_index( attached_devices, struct Device*, index ) ;
		if( ditem == device )
			break;
	}

	if( !ditem )
		return 0;

	g_array_remove_index( attached_devices, index );
	mount_remove_item( ditem );
	return 1;
}

