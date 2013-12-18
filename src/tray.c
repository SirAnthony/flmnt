/*
 * tray.c
 *
 *  Created on: 10.06.2013
 *
 */

#include "tray.h"
#include "device.h"
#include <stdlib.h>
#include <gtk/gtk.h>


static GtkWidget* tray_menu = NULL;
static GtkStatusIcon* tray_icon = NULL;
GHashTable* devices = NULL;

static struct {
	guint mount;
	guint unmount;
} signals;


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

static int tray_remove_device(GtkWidget *w, const struct Device* item)
{
	return -1;
}

static void tray_item_add(GtkWidget *w, struct Device* item)
{
	GtkWidget* menu_item;
	int* id;
	if (!item)
		return;

	menu_item = gtk_menu_item_new_with_label(item->name);
	gtk_widget_show_all(menu_item);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(tray_menu), menu_item);
	g_signal_connect(G_OBJECT( menu_item ), "activate",
			G_CALLBACK( tray_remove_device ), item);
	id = (int*)malloc(sizeof(int));
	*id = item->id;
	g_hash_table_insert(devices, id, menu_item);
}


static void tray_item_remove(GtkWidget *w, struct Device* item)
{
	GtkWidget* menu_item;
	menu_item = g_hash_table_lookup(devices, &item->id);
	if (!menu_item)
		return;
	gtk_container_remove(GTK_CONTAINER(tray_menu), menu_item);
	g_hash_table_remove(devices, &item->id);
	gtk_widget_destroy(tray_menu);
}


static int tray_create_icon( )
{
	GtkIconTheme* theme;
	GdkPixbuf* pixbuf;
	GError* error = NULL;
	int icon_size = 64;
	gchar *icons[] = { "media-flash" };

	/* Already created */
	if( tray_icon || tray_menu )
		return 1;

	tray_menu = gtk_menu_new( );

	theme = gtk_icon_theme_get_default();
	if( !gtk_icon_theme_has_icon( theme, icons[0] ) ){
		theme = gtk_icon_theme_new();
		gtk_icon_theme_set_custom_theme( theme, "Tango" );
	}

	pixbuf = gtk_icon_theme_load_icon( theme, icons[0], icon_size,
				GTK_ICON_LOOKUP_USE_BUILTIN, &error );
	if( error ){
		printf( "load icon: %s", error->message );
		tray_icon = gtk_status_icon_new( );
	}else{
		tray_icon = gtk_status_icon_new_from_pixbuf( pixbuf );
		g_object_unref( pixbuf );
	}

	g_signal_connect( G_OBJECT( tray_icon ), "button_press_event",
			G_CALLBACK( tray_icon_on_click ), NULL );
	//gtk_status_icon_set_from_icon_name( tray_icon, GTK_STOCK_MEDIA_STOP );
	gtk_status_icon_set_tooltip( tray_icon, "Mounted devices" );
	gtk_status_icon_set_visible( tray_icon, TRUE );

	gtk_widget_show_all( tray_menu );

	return 0;
}



/**************************************************************************/
/***************************PUBLIC METHODS*********************************/
/**************************************************************************/

int tray_init( int *argc, char ***argv )
{
	gtk_init( argc, argv );
	devices = g_hash_table_new_full(g_int_hash, g_int_equal, free, NULL);
	tray_create_icon( );
	signals.mount = g_signal_new("device-mounted", G_OBJECT_CLASS_TYPE(tray_menu),
									G_SIGNAL_RUN_LAST, 0, NULL, NULL,
									g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE,
									1, G_TYPE_POINTER);
	signals.unmount = g_signal_new("device-unmounted", G_OBJECT_CLASS_TYPE(tray_menu),
									G_SIGNAL_RUN_LAST, 0, NULL, NULL,
									g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE,
									1, G_TYPE_POINTER);
	g_signal_connect(tray_menu, "device-mounted", G_CALLBACK(tray_item_add), NULL);
	g_signal_connect(tray_menu, "device-unmounted", G_CALLBACK(tray_item_remove), NULL);
	return 0;
}

int tray_run()
{
	gtk_main();
	g_hash_table_destroy(devices);
	return 0;
}

void* tray_get_menu()
{
	return tray_menu;
}
