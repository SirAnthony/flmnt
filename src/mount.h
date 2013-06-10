/*
 * mount.h
 *
 *  Created on: 10.06.2013
 *
 */

#ifndef MOUNT_H_
#define MOUNT_H_

struct device_item {
	char* text;
	char* path;
	char* name;
	int id;
};

void mount_init();
void mount_clean();

int mount_add_device( int count, char** devices );
int mount_remove_device( int id );


#endif /* MOUNT_H_ */
