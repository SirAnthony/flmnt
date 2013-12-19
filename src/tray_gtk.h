/*
 * tray_gtk.h
 *
 *  Created on: 19.12.2013
 *
 */

#ifndef TRAY_GTK_H_
#define TRAY_GTK_H_

#include "device.h"
#include <stdlib.h>
#include <gtk/gtk.h>

/**
 * Screw that gtk with fucked documentation and no examples.
 * I just stole code from another project.
 */

#define TRAY_TYPE_WIDGET		(tray_widget_get_type())

typedef struct _TrayWidget	TrayWidget;
typedef struct _TrayWidgetClass	TrayWidgetClass;

struct _TrayWidget
{
	GObject parent_instance;
};

struct _TrayWidgetClass
{
	GObjectClass parent_class;

	void (* device_mounted)	(GObject *obj, struct Device* device);
	void (* device_unmounted)	(GObject *obj, struct Device* device);
};


#endif /* TRAY_GTK_H_ */
