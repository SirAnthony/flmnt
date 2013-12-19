/*
 * tray.h
 *
 *  Created on: 10.06.2013
 *
 */

#ifndef TRAY_H_
#define TRAY_H_

int tray_init(int *argc, char ***argv);
int tray_run();
void tray_signal_emit(const char *name, ...);

#endif /* TRAY_H_ */
