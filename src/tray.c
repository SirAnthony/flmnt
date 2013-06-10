/*
 * tray.c
 *
 *  Created on: 10.06.2013
 *
 */

#include "tray.h"

static GtkWidget* tray_menu = NULL;
static GtkStatusIcon* tray_icon = NULL;

static void tray_menu_deactivate(GtkMenuShell *menushell, gpointer user_data)
{
	GtkWidget *attach_widget;
	(void) user_data;
	gtk_menu_popdown( GTK_MENU(menushell) );
	attach_widget = gtk_menu_get_attach_widget( GTK_MENU(menushell) );
	if( attach_widget )
		gtk_widget_set_state( attach_widget, GTK_STATE_NORMAL );
}

static gboolean tray_icon_on_click( GtkStatusIcon *status_icon,
									GdkEventButton* event,
									gpointer        user_data )
{
	(void) user_data;
	GtkMenu* menu = GTK_MENU(tray_menu);
	g_signal_connect( tray_menu, "deactivate", G_CALLBACK(tray_menu_deactivate), NULL );
	gtk_menu_popup( menu, NULL, NULL, NULL, NULL, event->button, event->time );
	gtk_widget_set_state( gtk_menu_get_attach_widget( menu ), GTK_STATE_SELECTED );
	return TRUE;
}




static void tray_item_remove( GtkMenuItem *item, gpointer user_data )
{

}


/**************************************************************************/
/***************************PUBLIC METHODS*********************************/
/**************************************************************************/


int tray_create_icon( )
{
	/* Already created */
	if( tray_icon || tray_menu )
		return 0;

	tray_menu = gtk_menu_new( );
	tray_icon = gtk_status_icon_new_from_icon_name("network-idle");

	g_signal_connect( G_OBJECT( tray_icon ), "button_press_event",
			G_CALLBACK( tray_icon_on_click ), NULL );
	gtk_status_icon_set_from_icon_name( tray_icon, GTK_STOCK_MEDIA_STOP );
	gtk_status_icon_set_tooltip( tray_icon, "Mounted devices" );
	gtk_status_icon_set_visible( tray_icon, TRUE );

	gtk_widget_show_all( tray_menu );

	return 1;
}



int tray_add_item( struct device_item* item )
{
	GtkWidget* menu_item;
	menu_item = gtk_menu_item_new_with_label( item->text );
	gtk_widget_show_all( menu_item );
	gtk_menu_shell_append( GTK_MENU_SHELL(tray_menu), menu_item );
	g_signal_connect( G_OBJECT( menu_item ), "activate",
			G_CALLBACK( tray_item_remove ),	item );
	return 1;
}


int tray_remove_item( struct device_item* item )
{
	return 0;
}

