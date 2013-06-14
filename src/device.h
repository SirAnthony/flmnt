/*
 * device.h
 *
 *  Created on: 13.06.2013
 *
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#define DEV_MAX_PATH 128
#define DEV_MAX_INFO 32

struct Device {
	char name[DEV_MAX_INFO];
	char fstype[DEV_MAX_INFO];
	char source[DEV_MAX_PATH];
	char target[DEV_MAX_PATH];
	char options[DEV_MAX_PATH];
	short mounted;
	int id;
};

int device_init_from_data( struct Device** tgt, char* data );
int device_init_from_env( struct Device* tgt );
void device_free( struct Device* tgt );


#endif /* DEVICE_H_ */
