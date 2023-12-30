#*******************************************************************************
#*                              uBeeDisk Makefile                              *
#*                                                                             *
#*      A tool for converting disks/images from one to another with auto       *
#*                   detection options for Microbee disks.                     *
#*                                                                             *
#*                       Copyright (C) 2008-2023 uBee                          *
#*******************************************************************************
#
#===============================================================================
# REVISION HISTORY (Most recent at top)
#===============================================================================
# v4.0.1 - uBee 28 December 2023 Tony Sanchez
# - Added detection for MacOS systems
#===============================================================================
# v4.0.0 - uBee 29 December 2016
# - Added dos.o module.
#
# v3.1.0 - uBee 16 November 2016
# ------------------------------
# - Added entries to the 'clean' target to remove some missed packages.
# - Fixed Makefile 'r' permissions for Unices installs.
#
# v3.0.0 - uBee 5 January 2015
# ----------------------------
# - Changes for the maintaining/building packages now uses shell script.
# - Added various.o module.
#
# v2.3.0 - uBee 30 August 2013
# ----------------------------
# - Targets now use -Wl,-Bstatic and -Wl,-Bdynamic for building with static
#   libraries.
# - Changes to Windows target to make use of later mingw32 cross compiler
#   arrangement.
#
# v2.2.0 - uBee 14/05/2012
# ------------------------
# - Changes to the 'clean' target.
#
# v2.1.0 - uBee 6/10/2011
# -----------------------
# - Added microbee.o, applix.o, and fm.o modules.
#
# v1.5.0 - uBee 7/7/2010
# ----------------------
# - Added to Windows build -lbz2 and -lz libs, this was ok before without
#   including these, possibly the new win32 libdsk build I made did not
#   include the static libs when I rebuilt it or it was never detected by
#   LibDsk on the old build of it?
#
# v1.4.0 - uBee 26/2/2010
# -----------------------
# - Added some shell scripts to the install/uninstall targets.
#
# v1.3.0 - uBee 9/2/2010
# ----------------------
# - Made major changes to be more consistent with my other project
#   Makefiles.
#
# v1.0.0 - uBee 27/12/2008
# ------------------------
# - Created initial multi-platform make file to produce Linux, Win32
#   targets. The Win32 target is cross compiled on a Linux host.
#
#===============================================================================
# Overrides
#===============================================================================
.SUFFIXES:

TOPDIR=..

#===============================================================================
# Application specific definitions
#===============================================================================
APP=ubeedisk
# Append 'dev' if this is a development version, don't use any '-' or '_'
# characters as the debian packaging scripts won't work correctly.
VERSION=4.0.1

REL_NAME=$(APP)

# Set the debugging level (set to 0 to disable and strip binaries)
OPT_DEBUG?=0

# Set other system until a known one is detected
OTHER_SYSTEM=1

APP_NAME=\"$(REL_NAME)\"
APP_VER=\"$(VERSION)\"
APP_NAME_VER=\"$(REL_NAME)\-$(VERSION)\"

# Object modules
OBJC=./$(APP).o ./md5.o ./format.o ./microbee.o ./applix.o ./dos.o ./fm.o
OBJC+=./various.o ./options.o ./getopt.o ./functions.o ./strverscmp.o

DEL_XOBJC=$(OBJC:./%=build/%)
DEL_WOBJC=$(OBJC:./%=win32/%)

#===============================================================================
# Platform specific definitions
#===============================================================================
# Get the host OS name
SYSTEM?=$(shell uname -s)

# Mingw cross-compiling prefix
MINGW_PREFIX=i586-mingw32msvc

ifneq ($(TOS),win)
#===============================================================================
# Host target.  Linux, FreeBSD and 'other' builds are supported.
# Other systems will require a conditional section to be added or the 'other
# native systems' definitions will be used.
#
# This is the default build when 'make' is used by itself.
#===============================================================================

#================================ Linux build ==================================
ifeq ($(SYSTEM),Linux)
OTHER_SYSTEM=0
endif

#============================== FreeBSD build =================================
ifeq ($(SYSTEM),FreeBSD)
OTHER_SYSTEM=0
endif

#============================== Darwin build =================================
ifeq ($(SYSTEM),Darwin)
COMOPTS+=-DDARWIN
endif

#=========================== other native systems =============================
ifeq ($(OTHER_SYSTEM),1)
endif

#========================== Unix host target build ============================
   ifeq ($(OPT_DEBUG),1)
      STRIP=
      DEBUG=-g
      OPT=
   else
      STRIP=strip build/$(APP)
      DEBUG=-g
      OPT=-O2
   endif

   XOBJC=$(OBJC:./%=build/%)
   DEPENDENCIES=$(OBJC:.o=.h) Makefile

   CC=gcc
   CFLAGS=$(DEBUG) -O2 -Wall
   CINC+= -I/usr/local/include -I/usr/include -I/opt/local/include 
   
   ifeq ($(SYSTEM),Darwin)
	     CLIB=-L/opt/local/lib -L/usr/local/lib  -Wl, -ldsk -lb2 -lz
   else	
        CLIB=-Wl,-Bstatic -ldsk -lbz2 -lz -Wl,-Bdynamic 
   endif
   
   CDEF=-D_GNU_SOURCE=1 -D_REENTRANT
   CDEF+=-DTITLESTRING=$(APP_NAME_VER) -DAPPVER=$(APP_VER) -DAPPNAME=$(APP_NAME) $(COMOPTS)

build: build/$(APP) warning

build/$(APP): $(XOBJC)
	$(CC) $(XOBJC) $(CLIB) -o build/$(APP)
	$(STRIP)

build/%.o: %.c $(DEPENDENCIES)
	@[ -d build ] || mkdir build
	$(CC) -c $(CFLAGS) $(CINC) $(CDEF) $(*).c -o build/$(*).o

endif  # ifneq $(TOS),win)

#===============================================================================
# Windows target
#===============================================================================
ifeq ($(TOS),win)

MINGW32_PATH=/usr/$(MINGW_PREFIX)
CC=$(MINGW_PREFIX)-gcc

ifeq ($(OPT_DEBUG),1)
   STRIP=
   DEBUG=-g
   OPT=
else
   STRIP=$(MINGW_PREFIX)-strip win32/$(APP).exe
   DEBUG=-g
   OPT=-O2
endif

CFLAGS=$(DEBUG) -O2 -Wall
CINC=$(USER_WIN32_INCLUDES)
LIBS=$(USER_WIN32_LIBS)
CINC+=-I$(MINGW32_PATH)/include
LIBS+=-L$(MINGW32_PATH)/lib
CLIB=-Wl,-Bstatic -ldsk -lbz2 -lz -Wl,-Bdynamic
CDEF=-D_GNU_SOURCE=1 -D_REENTRANT -DNOTWINDLL
CDEF+=-DTITLESTRING=$(APP_NAME_VER) -DAPPVER=$(APP_VER) -DAPPNAME=$(APP_NAME)

WOBJC=$(OBJC:./%=win32/%)
DEPENDENCIES=$(OBJC:.o=.h) Makefile

winx: win32/$(APP).exe warning

win32/$(APP).exe: $(WOBJC)
	$(CC) $(WOBJC) $(ICON) $(CLIB) -o win32/$(APP).exe
	$(STRIP)

win32/%.o: %.c $(DEPENDENCIES)
	@[ -d win32 ] || mkdir win32
	$(CC) -c $(CFLAGS) $(CINC) $(CDEF) $(*).c -o win32/$(*).o

endif #ifeq ($(TOS),win)

#===============================================================================
# Build the host system and Windows targets
#===============================================================================
win:
	make winx TOS=win

all:
	make
	make winx TOS=win

#===============================================================================
# General maintenance
#===============================================================================
clean:
	rm -f $(DEL_XOBJC) build/$(APP) \
	$(DEL_WOBJC) win32/$(APP).exe \
	$(TOPDIR)/distributions/$(APP)-$(VERSION).tar.gz \
	$(TOPDIR)/distributions/$(APP)-$(VERSION)-win32.zip \
	$(TOPDIR)/distributions/$(APP)-$(VERSION)-win32.exe \
	$(TOPDIR)/distributions/$(APP)-$(VERSION)_armhf.tgz \
	$(TOPDIR)/distributions/$(APP)-$(VERSION)_i386.tgz \
	$(TOPDIR)/distributions/$(APP)_$(VERSION)_*.deb \
	$(TOPDIR)/distributions/$(APP)-$(VERSION)-2.*.rpm \
	$(TOPDIR)/distributions/$(APP)-$(VERSION).*.tgz \
	$(TOPDIR)/distributions/$(APP)-$(VERSION)-portable.zip \
	$(TOPDIR)/distributions/$(APP)-$(VERSION)-release_notes.txt
	rm -Rf win32/$(APP)

cleanall: clean
	rm -Rf win32/ build/

ifneq ($(OPT_DEBUG),0)
warning:
	@echo ""
	@echo "*************************************************************"
	@echo "********** CODE BEING BUILT WITH DEBUG FEATURES ON **********"
	@echo "*************************************************************"
else
warning:
endif

#===============================================================================
# Install and remove installed binaries, and shared application directory
#
# Ensure that all scripts that are intended to be run are set to executable
# for all groups (+x) or they won't be seen in the user's path.
#
# The install process must only copy the files actually required or the
# Debian build will contain unwanted files from the original source
# directories.
#===============================================================================
PREFIX=$(DESTDIR)/usr/local
BINDIR=$(PREFIX)/bin
APPDIR=$(PREFIX)/share/$(APP)
DOCDIR=$(APPDIR)/doc
CONFIGDIR=$(APPDIR)/config

makedirs:
	mkdir -p $(BINDIR)
	mkdir -p $(DOCDIR)/GPL
	mkdir -p $(CONFIGDIR)

install: makedirs
	cp \
	build/$(APP) \
	$(TOPDIR)/scripts/unix/ub-ds40-makedsk \
	$(TOPDIR)/scripts/unix/ub-ds80-makedsk \
	$(TOPDIR)/scripts/unix/ub-ds82-makedsk \
	$(TOPDIR)/scripts/unix/ub-ds84-makedsk \
	$(TOPDIR)/scripts/unix/ub-ss80-makedsk \
	$(TOPDIR)/scripts/unix/ub-ds40-makeraw \
	$(TOPDIR)/scripts/unix/ub-ds80-makeraw \
	$(TOPDIR)/scripts/unix/ub-ds82-makeraw \
	$(TOPDIR)/scripts/unix/ub-ds84-makeraw \
	$(TOPDIR)/scripts/unix/ub-ss80-makeraw \
	$(TOPDIR)/scripts/unix/ub-ds40-raw2dsk \
	$(TOPDIR)/scripts/unix/ub-ds80-raw2dsk \
	$(TOPDIR)/scripts/unix/ub-ds82-raw2dsk \
	$(TOPDIR)/scripts/unix/ub-ds84-raw2dsk \
	$(TOPDIR)/scripts/unix/ub-ss80-raw2dsk \
	$(TOPDIR)/scripts/unix/ub-ds40-dsk2raw \
	$(TOPDIR)/scripts/unix/ub-ds80-dsk2raw \
	$(TOPDIR)/scripts/unix/ub-ds82-dsk2raw \
	$(TOPDIR)/scripts/unix/ub-ds84-dsk2raw \
	$(TOPDIR)/scripts/unix/ub-ss80-dsk2raw \
	$(BINDIR)/

	cp $(TOPDIR)/doc/GPL/COPYING $(DOCDIR)/GPL

	cp \
	$(TOPDIR)/doc/ChangeLog \
	$(TOPDIR)/doc/INSTALL \
	$(TOPDIR)/doc/README \
	$(TOPDIR)/doc/license.txt \
	$(TOPDIR)/doc/microbee_disk_formats.txt \
	$(DOCDIR)/	

	cp $(TOPDIR)/config/ubeedisk.ini.sample $(CONFIGDIR)/
	
	chmod -R go+r $(APPDIR)/

uninstall:
	rm \
	$(BINDIR)/$(APP) \
	$(BINDIR)/ub-ds40-makedsk \
	$(BINDIR)/ub-ds80-makedsk \
	$(BINDIR)/ub-ds82-makedsk \
	$(BINDIR)/ub-ds84-makedsk \
	$(BINDIR)/ub-ss80-makedsk \
	$(BINDIR)/ub-ds40-makeraw \
	$(BINDIR)/ub-ds80-makeraw \
	$(BINDIR)/ub-ds82-makeraw \
	$(BINDIR)/ub-ds84-makeraw \
	$(BINDIR)/ub-ss80-makeraw \
	$(BINDIR)/ub-ds40-raw2dsk \
	$(BINDIR)/ub-ds80-raw2dsk \
	$(BINDIR)/ub-ds82-raw2dsk \
	$(BINDIR)/ub-ds84-raw2dsk \
	$(BINDIR)/ub-ss80-raw2dsk \
	$(BINDIR)/ub-ds40-dsk2raw \
	$(BINDIR)/ub-ds80-dsk2raw \
	$(BINDIR)/ub-ds82-dsk2raw \
	$(BINDIR)/ub-ds84-dsk2raw \
	$(BINDIR)/ub-ss80-dsk2raw

	rm -Rf $(APPDIR)

#===============================================================================
# Test ubeedisk host build under Linux Gnome X Windows
#
# Change mate-terminal to gnome-terminal or other as required.
#===============================================================================
debug: build/$(APP)
	mate-terminal -x gdb --args ./build/ubeedisk

#===============================================================================
# Help
#===============================================================================
help:
	@echo "This is the $(APP) v$(VERSION) GNU makefile"
	@echo ""
	@echo "Host system (uname -s): $(SYSTEM)"
	@echo ""
	@echo "Specify one of the following:"
	@echo "make                  native host build"
	@echo "make win              cross compiled Windows build"
	@echo "make all              make native host and Windows builds"
	@echo "make clean            removes build/win32 and distribution files"
	@echo "make cleanall         cleans and removes all generated files"
	@echo "make install          install files on the host system"
	@echo "make uninstall        uninstall files on the host system"
	@echo "make help             this help information"
	@echo ""
	@echo "Packaging system only:"	
	@echo "make srcdist          make source distribution"
	@echo "make alldist          make all Unix and win32 binary distributions"
	@echo "make winzipdist       make win32 ZIP binary distribution"
	@echo "make winexedist       make win32 executable installer distribution"
	@echo "make winalldist       make win32 EXE and ZIP binary distributions"
	@echo "make rpidist          make Raspberry Pi distributions"
	@echo "make unixdist         make host Unix binary distributions"
	@echo "make upload           upload distribution packages"

#===============================================================================
# Create source distribution.
#===============================================================================
srcdist:
	cd $(TOPDIR) && ./packaging.sh $@ $(APP) $(VERSION)
	
#===============================================================================
# Create all distributions.
#===============================================================================
alldist:
	cd $(TOPDIR) && ./packaging.sh $@ $(APP) $(VERSION)

#===============================================================================
# Create a win32 binary ZIP file distribution.
#===============================================================================
winzipdist:
	cd $(TOPDIR) && ./packaging.sh $@ $(APP) $(VERSION)

#===============================================================================
# Create a win32 binary installer distribution.
#===============================================================================
winexedist: 
	cd $(TOPDIR) && ./packaging.sh $@ $(APP) $(VERSION)

#===============================================================================
# Create the ZIP and win32 binary installer distributions.
#===============================================================================
winalldist:
	cd $(TOPDIR) && ./packaging.sh $@ $(APP) $(VERSION)

#===============================================================================
# Create the Raspberry pi binary installer distributions.
#===============================================================================
rpidist:
	cd $(TOPDIR) && ./packaging.sh $@ $(APP) $(VERSION)

#===============================================================================
# Create Unix binary distributions.
#===============================================================================
unixdist:
	cd $(TOPDIR) && ./packaging.sh $@ $(APP) $(VERSION)	

#===============================================================================
# Upload all the distribution files.
#===============================================================================
upload:
	cd $(TOPDIR) && ./packaging.sh $@ $(APP) $(VERSION)
