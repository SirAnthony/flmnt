/*
 * utils.h
 *
 *  Created on: 20.12.2013
 *
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <glib.h>

enum {
	CREATE_PATH_SUCCESS = 0,
	CREATE_PATH_EXISTS,
	CREATE_PATH_STAT_FAILED,
	CREATE_PATH_MKDIR_FAILED,
};

void log_init(const char *path);
void log_close();
int create_dir(const char* path);

#endif /* UTILS_H_ */
