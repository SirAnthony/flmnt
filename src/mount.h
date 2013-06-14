/*
 * mount.h
 *
 *  Created on: 10.06.2013
 *
 */

#ifndef MOUNT_H_
#define MOUNT_H_

#include "device.h"

void mount_init();
void mount_clean();

int mount_add_device( struct Device* );
int mount_remove_device( int id );


#endif /* MOUNT_H_ */
