
#include "device.h"
#include "mount.h"
#include "ipc.h"
#include "tray.h"


int main(int argc, char **argv) {
	struct Device device;
	int role;

	role = ipc_select_role();

	// TODO: finalize on error
	if( role < 0 )
		return 1;

	if( device_init_from_env( &device ) < 0 )
		return 1;

	// Initialize server stuff
	if( role == IPC_SERVER ){
		mount_init();
		tray_init( &argc, &argv );
	}

	ipc_register_device( &device );

	ipc_finalize( IPC_CLIENT );


	// Run application
	if( role == IPC_SERVER ){
		tray_run();
		mount_clean();
		ipc_finalize( IPC_SERVER );
	}

	return 0;
}
