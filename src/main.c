
#include "tray.h"
#include "mount.h"



int main(int argc, char **argv) {
	gtk_init(&argc, &argv);
	mount_init();
	tray_create_icon();
	mount_add_device(argc - 1, &argv[1]);
	gtk_main();
	mount_clean();
	return 0;
}
