
rm=/bin/rm -f
CC= gcc
DEFS= 
INCLUDES= -I/usr/include $(shell pkg-config --cflags gtk+-2.0)
LIBS= $(shell pkg-config --libs gtk+-2.0)
DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -g -O0 -Wall $(DEFINES)
PROGNAME= flmnt
OBJDIR= obj/
OUTDIR= bin/
VPATH= src/


SRCS = utils.c device.c ipc.c mount.c tray.c main.c 
OBJ = $(SRCS:.c=.o)
OBJS = $(addprefix $(OBJDIR), $(OBJ:.c=.o)) 

.PHONY: all clean

rebuild: cleanprog all


$(OBJDIR)%.o: %.c
	$(rm) $@
	$(CC) $(CFLAGS) -c $^ -o $@


all: $(PROGNAME)


$(PROGNAME) : dirs $(OBJS)
	$(CC) $(CFLAGS)  -o $(OUTDIR)$(PROGNAME) $(OBJS) $(LIBS)


dirs:
	mkdir -p $(OBJDIR) $(OUTDIR)


clean: cleanobjs cleanprog cleandirs


cleanobjs:
	$(rm) $(OBJS)


cleanprog:
	$(rm) -r $(OUTDIR)*


cleandirs:
	$(rm) -r $(OBJDIR)*