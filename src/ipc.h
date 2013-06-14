/*
 * ipc.h
 *
 *  Created on: 11.06.2013
 *
 */

#ifndef IPC_H_
#define IPC_H_

#include "device.h"

enum ipc_role {
	IPC_SERVER = 0,
	IPC_CLIENT
};


int ipc_select_role( );
int ipc_register_device( struct Device* device );
int ipc_finalize( int role );


#endif /* IPC_H_ */
