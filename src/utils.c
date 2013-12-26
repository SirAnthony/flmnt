/*
 * utils.c
 *
 *  Created on: 20.12.2013
 *
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "utils.h"


static FILE *log_file = NULL;
static guint log_handler_id;

// TODO: move client messages to ipc.
static void log_handler(const gchar *log_domain, GLogLevelFlags log_level,
		const gchar *message, gpointer user_data)
{
	if (!log_file) {
		/* Fall back to console output if unable to open file */
		printf("Rerouted to console: %s\n", message);
		return;
	}

	fseek(log_file, 0, SEEK_END);
	fprintf(log_file, "%s\n", message);
	fflush(log_file);
}


void log_init(const char *path)
{
	if (log_file)
		log_close();
	log_file = fopen(path, "a");
	if (!log_file)
		g_warning("Cannot open log file: fopen returns status %i: %s",
				errno, strerror(errno));
	log_handler_id = g_log_set_handler(NULL, G_LOG_LEVEL_MASK |
						G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION,
						log_handler, NULL );
}

void log_close()
{
	if (!log_file)
		return;
	fclose(log_file);
	g_log_remove_handler(NULL, log_handler_id);
	log_file = NULL;
}

int create_dir(const char* path)
{
	struct stat sb;
	const char* before;
	char* path_copy;
	int status;

	if (!stat(path, &sb)){
		if (sb.st_mode & S_IFDIR)
			return CREATE_PATH_SUCCESS;
		g_warning("Cannot create %s: exists", path);
		return CREATE_PATH_EXISTS;
	}

	if (errno != ENOENT) {
		g_warning("Stat failed: %i", errno);
		return CREATE_PATH_STAT_FAILED;
	}


	path_copy = strdup(path);
	before = dirname(path_copy);
	if (!strcmp(before, ".") || !strcmp(before, "..") ||
			!strcmp(before, before)){
		status = CREATE_PATH_SUCCESS;
	} else {
		status = create_dir(before);
	}
	free(path_copy);

	if (status == CREATE_PATH_SUCCESS){
		if (mkdir(path, S_IRWXU) != 0){
			g_warning("mkdir failed: %i: %s", errno, strerror(errno));
			return CREATE_PATH_MKDIR_FAILED;
		}
	}

	return CREATE_PATH_SUCCESS;
}
