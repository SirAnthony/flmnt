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
#include "utils.h"


const unsigned int DEVICE_SIZE = sizeof(struct Device);
const char* DEFAULT_OPTS = "rw,noatime,flush,users";
		//"rw,noatime,flush,uid=1000,iocharset=utf8,codepage=866,users"
const char* DEFAULT_TARGET = "/media/flash/%s";

static int copy_env(char* dst, const char* name, size_t size)
{
	char* env_str;
	if (!dst)
		return -1;
	dst[0] = '\0';

	env_str = getenv(name);
	if (!env_str)
		return -1;

	strncpy(dst, env_str, size);
	return 0;
}


int device_init_from_data(struct Device** tgt, char* data)
{
	if (!data)
		return -1;

	if (!*tgt)
		*tgt = malloc(DEVICE_SIZE);

	memcpy(*tgt, data, DEVICE_SIZE);
	return 0;
}


int device_init_from_env(struct Device* ditem)
{
	char* base;
	char* source;

	if (copy_env(ditem->source, "mount_device", DEV_MAX_PATH) < 0 ||
			ditem->source == '\0') {
		g_warning("Device not specified. Exit.");
		return -1;
	}

	if (copy_env(ditem->fstype, "mount_fstype", DEV_MAX_INFO) < 0 ||
			ditem->fstype == '\0') {
		g_warning("Device filesystem type not specified. Exit.");
		return -2;
	}

	source = strdup(ditem->source);
	base = basename(source);

	if (copy_env(ditem->name, "mount_name", DEV_MAX_INFO) < 0 ||
			ditem->name == '\0')
		strncpy(ditem->name, base, DEV_MAX_INFO);

	if (copy_env(ditem->target, "mount_target", DEV_MAX_PATH) < 0 ||
			ditem->target == '\0')
		snprintf(ditem->target, DEV_MAX_PATH, DEFAULT_TARGET, base);

	if (copy_env(ditem->options, "mount_options", DEV_MAX_PATH) < 0 ||
			ditem->options == '\0')
		strncpy(ditem->options, DEFAULT_OPTS, DEV_MAX_PATH);

	ditem->mounted = 0;
	ditem->id = -1;

	free(source);

	return 0;
}




void device_free( struct Device* tgt )
{
	if( !tgt )
		return;
	free( tgt );
}
