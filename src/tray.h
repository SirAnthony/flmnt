/*
 * tray.h
 *
 *  Created on: 10.06.2013
 *
 */

#ifndef TRAY_H_
#define TRAY_H_

#include "device.h"

int tray_init( int *argc, char ***argv );
int tray_run();

int tray_add_item( struct Device* );
int tray_remove_item( struct Device* );


#endif /* TRAY_H_ */
