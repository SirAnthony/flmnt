/*
 * tray.h
 *
 *  Created on: 10.06.2013
 *
 */

#ifndef TRAY_H_
#define TRAY_H_

#include <gtk/gtk.h>
#include "mount.h"

int tray_create_icon( );
int tray_add_item( struct device_item* );
int tray_remove_item( struct device_item* );


#endif /* TRAY_H_ */
