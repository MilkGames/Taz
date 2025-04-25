
SHELL		=	/bin/sh

TOP		=	/usr/local/sce/ee
LIBDIR	=	$(TOP)/lib
INCDIR	=	$(TOP)/include
BABLIBDIR	=	/babel/ps2/lib
BABINCDIR	=	/babel/ps2/include
COMDIR	=	/usr/local/sce/common/include

TARGET	=	main
OBJS		=	crt0.o \
			$(TARGET).o \
			actors.o \
			animation.o \
			attach.o \
			breakables.o \
			camera.o \
			cartoonscaling.o \
			characters.o \
			claudescene.o \
			collectibles.o \
			collision.o \
			collisioncache.o \
			control.o \
			cutscene.o \
			debris.o \
			display.o \
			electrocution.o \
			envmap.o \
			fade.o \
			fallingobjects.o \
			files.o \
			font.o \
			health.o \
			holysmoke.o \
			invisibleObjects.o \
			lights.o \
			mailbox.o \
			mapfile.o \
			maths.o \
			objectviewer.o \
			ParseMap.o \
			paths.o \
			physics.o \
			playerstats.o \
			postcard.o \
			precipitation.o \
			projectile.o \
			scene.o \
			sfx.o \
			shadow.o \
			snow.o \
			splashscreen.o \
			springyobjects.o \
			status.o \
			steppingstone.o \
			swingingobjects.o \
			taztoproulette.o \
			textures.o \
			thoughtbubble.o \
			tiltingobjects.o \
			timer.o	\
			tonyscene.o \
			wantedposter.o \
			water.o

LCFILE	=	$(LIBDIR)/ps2.lk
LIBS		=	$(LIBDIR)/libgraph.a \
              	$(LIBDIR)/libdma.a \
			$(LIBDIR)/libkernl.a \
			$(LIBDIR)/libdev.a \
			$(LIBDIR)/libpkt.a \
			$(LIBDIR)/libvu0.a \
			$(LIBDIR)/libpad.a \
			$(LIBDIR)/libsdr.a \
			$(LIBDIR)/libmc.a \
			$(LIBDIR)/libcdvd.a


BABLIBS	=	$(BABLIBDIR)/bkernel.a \
	           	$(BABLIBDIR)/bmaths.a \
			$(BABLIBDIR)/bdisplay.a \
	            $(BABLIBDIR)/binput.a \
	            $(BABLIBDIR)/bactor.a \
			$(BABLIBDIR)/bgui.a \
			$(BABLIBDIR)/bsound.a


PREFIX      = 	ee
AS          = 	$(PREFIX)-gcc
CC          = 	$(PREFIX)-gcc
LD          = 	ps2link
#LD		= 	$(PREFIX)-gcc
AR	    	= 	$(PREFIX)-ar
DVPASM      = 	$(PREFIX)-dvp-as
OBJDUMP     = 	$(PREFIX)-objdump
RUN         = 	dsedb -r run
RM          = 	/bin/rm -f

CFLAGS	=	-O2 -Wall -Wa,-al -fno-common
#CXXFLAGS	= 	-O2 -Wall -Wa,-al -fno-exceptions -fno-common
CXXFLAGS	= 	-g -Wall -Wa,-al -fno-exceptions -fno-common -msingle-float
ASFLAGS     = 	-c -g -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = 	-g 
LDFLAGS     = 	-l $(LIBDIR) -l $(TOP)/gcc/ee/lib -l \
				$(TOP)/gcc/lib/gcc-lib/ee/2.9-ee-991111

#LDFLAGS	= -g -Wl,-Map,$(TARGET).map -nostartfiles -L$(BABLIBDIR) -L$(LIBDIR) -lm

TMPFLAGS	=

.SUFFIXES: .c .s .cc .cpp .dsm

all: $(TARGET).elf

$(TARGET).elf:	$(OBJS) $(LIBS) $(BABLIBS)
				$(LD) $(LDFLAGS) $(OBJS) $(LIBS) $(BABLIBS) \
				@$(LCFILE),$(TARGET).elf,$(TARGET).map

#$(TARGET).elf: $(OBJS) $(BABLIBS) $(LIBS)
#	$(LD) -o $@ -T $(LCFILE) $(OBJS)  $(BABLIBS) $(LIBS) $(LDFLAGS)

crt0.o:	$(LIBDIR)/crt0.s
		$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.cpp.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) -I$(INCDIR) -I$(BABINCDIR) -I$(COMDIR) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.dis *.elf
