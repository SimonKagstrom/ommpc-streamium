#-------------------------------------------------------------------------------
# Simple Makefile
#
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
# Be sure to use the cxx compiler
#-------------------------------------------------------------------------------
CC = gcc
CXX = g++


#-------------------------------------------------------------------------------
# Desired optimization level
#-------------------------------------------------------------------------------
O_FLAG = -Wall -Werror
#O_FLAG = -O1
OPTIMIZATION_FLAG = -g

#-------------------------------------------------------------------------------
# The directories to look for include files and libraries
#-------------------------------------------------------------------------------
INCDIR = -I. -I/usr/include/SDL
LIB = -L. 

#-------------------------------------------------------------------------------
# Compiler flags
#-------------------------------------------------------------------------------
CXXFLAGS      = $(OPTIMIZATION_FLAG) $(INCDIR) $(LIB)

#-------------------------------------------------------------------------------
# The .cpp files used to compile the client
#-------------------------------------------------------------------------------
.SUFFIXES: .cpp .c

.c.o:
	$(CC) $(CXXFLAGS) -c $<
.c:
	$(CC) $(CXXFLAGS) $< -o pc/$@
	
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

.cpp:
	$(CXX) $(CXXFLAGS) $< -o pc/$@

SRC     = plBrowser.cpp browser.cpp playlist.cpp main.cpp config.cpp nowPlaying.cpp statsBar.cpp commandFactory.cpp popup.cpp scroller.cpp timestamp.cpp helpBar.cpp albumArt.cpp threadFunctions.cpp bookmarks.cpp gp2xregs.cpp overlay.cpp songDb.cpp keyboard.cpp button.cpp volButton.cpp buttonManager.cpp seekButton.cpp rndButton.cpp rptButton.cpp rndRptButton.cpp menu.cpp menuButton.cpp fullPlaying.cpp artButton.cpp settings.cpp id3Button.cpp

OBJ = $(SRC:.cpp=.o) libmpdclient.o
#-------------------------------------------------------------------------------
# Libraries being linked with the application
#-------------------------------------------------------------------------------
LIBS = -pthread -lSDL -lSDLmain -lSDL_ttf -lSDL_image -lSDL_gfx /usr/lib/libsqlite3.so.0 -ldl 

#-------------------------------------------------------------------------------
# Target for creating simple client
#-------------------------------------------------------------------------------
ommpc: $(OBJ)
	@echo $(OBJ)
	@echo $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS) \
		$(SYSLIBS) $(LIBS)
	@echo "SUCCESS"

watchdog: 
	$(CXX) $(CXXFLAGS) -c watchdog.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o watchdog watchdog.o config.o libmpdclient.o

#-------------------------------------------------------------------------------
# Target for "make all"
#-------------------------------------------------------------------------------
all:
	make clean
	make ommp2x 


#-------------------------------------------------------------------------------
# Target for "make clean"
#-------------------------------------------------------------------------------
clean:
	rm -rf core *.o ommp2x *~ *.*~ .*~ \#* .in* tca.* *vti* *.idl cxx_repository


#---
#Dependecies
#---
depend:
	makedepend $(CXXFLAGS) -Y $(SRC)
# DO NOT DELETE

plBrowser.o: plBrowser.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
plBrowser.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
plBrowser.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
plBrowser.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
plBrowser.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
plBrowser.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
plBrowser.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
plBrowser.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
plBrowser.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
plBrowser.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
plBrowser.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
plBrowser.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
plBrowser.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
plBrowser.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h scroller.h
plBrowser.o: threadParms.h songDb.h sqlite3.h commandFactory.h timer.h
plBrowser.o: config.h playlist.h keyboard.h guiPos.h
plBrowser.o: /usr/include/SDL/SDL_image.h
browser.o: browser.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
browser.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
browser.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
browser.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
browser.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
browser.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
browser.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
browser.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
browser.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
browser.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
browser.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
browser.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
browser.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
browser.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h scroller.h threadParms.h
browser.o: songDb.h sqlite3.h commandFactory.h timer.h config.h guiPos.h
browser.o: keyboard.h playlist.h /usr/include/SDL/SDL_image.h
playlist.o: playlist.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
playlist.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
playlist.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
playlist.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
playlist.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
playlist.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
playlist.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
playlist.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
playlist.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
playlist.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
playlist.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
playlist.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
playlist.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
playlist.o: /usr/include/SDL/SDL_ttf.h config.h libmpdclient.h scroller.h
playlist.o: timer.h threadParms.h songDb.h sqlite3.h commandFactory.h popup.h
playlist.o: timestamp.h guiPos.h /usr/include/SDL/SDL_image.h
main.o: /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
main.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
main.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
main.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
main.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
main.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
main.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
main.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
main.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
main.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
main.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
main.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
main.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
main.o: /usr/include/SDL/SDL_ttf.h /usr/include/SDL/SDL_image.h config.h
main.o: browser.h libmpdclient.h scroller.h plBrowser.h bookmarks.h
main.o: playlist.h timer.h settings.h albumArt.h threadParms.h songDb.h
main.o: sqlite3.h overlay.h nowPlaying.h fullPlaying.h menuButton.h button.h
main.o: artButton.h buttonManager.h volButton.h rndRptButton.h rndButton.h
main.o: rptButton.h seekButton.h threadFunctions.h commandFactory.h
main.o: statsBar.h menu.h popup.h gp2xregs.h guiPos.h keyboard.h
config.o: config.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
config.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
config.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
config.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
config.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
config.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
config.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
config.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
config.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
config.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
config.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
config.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
config.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h utf8.h
config.o: utf8/checked.h utf8/core.h utf8/unchecked.h
nowPlaying.o: nowPlaying.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
nowPlaying.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
nowPlaying.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
nowPlaying.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
nowPlaying.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
nowPlaying.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
nowPlaying.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
nowPlaying.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
nowPlaying.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
nowPlaying.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
nowPlaying.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
nowPlaying.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
nowPlaying.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
nowPlaying.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
nowPlaying.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
nowPlaying.o: commandFactory.h guiPos.h /usr/include/SDL/SDL_image.h
statsBar.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
statsBar.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
statsBar.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
statsBar.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
statsBar.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
statsBar.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
statsBar.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
statsBar.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
statsBar.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
statsBar.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
statsBar.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
statsBar.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
statsBar.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
statsBar.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
statsBar.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
statsBar.o: commandFactory.h
commandFactory.o: commandFactory.h libmpdclient.h timer.h config.h
commandFactory.o: /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
commandFactory.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
commandFactory.o: /usr/include/SDL/SDL_platform.h
commandFactory.o: /usr/include/SDL/begin_code.h /usr/include/SDL/close_code.h
commandFactory.o: /usr/include/SDL/SDL_audio.h /usr/include/SDL/SDL_error.h
commandFactory.o: /usr/include/SDL/SDL_endian.h /usr/include/SDL/SDL_mutex.h
commandFactory.o: /usr/include/SDL/SDL_thread.h /usr/include/SDL/SDL_rwops.h
commandFactory.o: /usr/include/SDL/SDL_cdrom.h /usr/include/SDL/SDL_cpuinfo.h
commandFactory.o: /usr/include/SDL/SDL_events.h /usr/include/SDL/SDL_active.h
commandFactory.o: /usr/include/SDL/SDL_keyboard.h
commandFactory.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
commandFactory.o: /usr/include/SDL/SDL_video.h
commandFactory.o: /usr/include/SDL/SDL_joystick.h /usr/include/SDL/SDL_quit.h
commandFactory.o: /usr/include/SDL/SDL_loadso.h /usr/include/SDL/SDL_timer.h
commandFactory.o: /usr/include/SDL/SDL_version.h threadParms.h songDb.h
commandFactory.o: sqlite3.h
popup.o: popup.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
popup.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
popup.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
popup.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
popup.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
popup.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
popup.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
popup.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
popup.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
popup.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
popup.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
popup.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
popup.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
popup.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
popup.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h commandFactory.h
popup.o: gp2xregs.h guiPos.h keyboard.h /usr/include/SDL/SDL_image.h
scroller.o: scroller.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
scroller.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
scroller.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
scroller.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
scroller.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
scroller.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
scroller.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
scroller.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
scroller.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
scroller.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
scroller.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
scroller.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
scroller.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
scroller.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h
scroller.o: commandFactory.h timer.h /usr/include/SDL/SDL_image.h
timestamp.o: timestamp.h
helpBar.o: helpBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
helpBar.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
helpBar.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
helpBar.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
helpBar.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
helpBar.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
helpBar.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
helpBar.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
helpBar.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
helpBar.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
helpBar.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
helpBar.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
helpBar.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
helpBar.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h
helpBar.o: /usr/include/SDL/SDL_image.h
albumArt.o: albumArt.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
albumArt.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
albumArt.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
albumArt.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
albumArt.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
albumArt.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
albumArt.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
albumArt.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
albumArt.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
albumArt.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
albumArt.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
albumArt.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
albumArt.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
albumArt.o: /usr/include/SDL/SDL_ttf.h threadParms.h libmpdclient.h songDb.h
albumArt.o: sqlite3.h config.h threadFunctions.h
threadFunctions.o: threadFunctions.h /usr/include/SDL/SDL.h
threadFunctions.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_stdinc.h
threadFunctions.o: /usr/include/SDL/SDL_config.h
threadFunctions.o: /usr/include/SDL/SDL_platform.h
threadFunctions.o: /usr/include/SDL/begin_code.h
threadFunctions.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
threadFunctions.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
threadFunctions.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
threadFunctions.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
threadFunctions.o: /usr/include/SDL/SDL_cpuinfo.h
threadFunctions.o: /usr/include/SDL/SDL_events.h
threadFunctions.o: /usr/include/SDL/SDL_active.h
threadFunctions.o: /usr/include/SDL/SDL_keyboard.h
threadFunctions.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
threadFunctions.o: /usr/include/SDL/SDL_video.h
threadFunctions.o: /usr/include/SDL/SDL_joystick.h
threadFunctions.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
threadFunctions.o: /usr/include/SDL/SDL_timer.h
threadFunctions.o: /usr/include/SDL/SDL_version.h
threadFunctions.o: /usr/include/SDL/SDL_image.h
threadFunctions.o: /usr/include/SDL/SDL_rotozoom.h threadParms.h
threadFunctions.o: libmpdclient.h songDb.h sqlite3.h
bookmarks.o: bookmarks.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
bookmarks.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
bookmarks.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
bookmarks.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
bookmarks.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
bookmarks.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
bookmarks.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
bookmarks.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
bookmarks.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
bookmarks.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
bookmarks.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
bookmarks.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
bookmarks.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
bookmarks.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h scroller.h
bookmarks.o: threadParms.h songDb.h sqlite3.h commandFactory.h timer.h
bookmarks.o: config.h playlist.h keyboard.h buttonManager.h
bookmarks.o: /usr/include/SDL/SDL_image.h volButton.h button.h rndRptButton.h
bookmarks.o: rndButton.h rptButton.h seekButton.h statsBar.h guiPos.h
gp2xregs.o: gp2xregs.h
overlay.o: overlay.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
overlay.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
overlay.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
overlay.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
overlay.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
overlay.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
overlay.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
overlay.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
overlay.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
overlay.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
overlay.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
overlay.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
overlay.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
overlay.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h guiPos.h
overlay.o: commandFactory.h timer.h playlist.h scroller.h
overlay.o: /usr/include/SDL/SDL_image.h
songDb.o: songDb.h libmpdclient.h sqlite3.h
keyboard.o: keyboard.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
keyboard.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
keyboard.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
keyboard.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
keyboard.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
keyboard.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
keyboard.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
keyboard.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
keyboard.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
keyboard.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
keyboard.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
keyboard.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
keyboard.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
keyboard.o: /usr/include/SDL/SDL_ttf.h config.h guiPos.h commandFactory.h
keyboard.o: libmpdclient.h timer.h /usr/include/SDL/SDL_image.h
button.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
button.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
button.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
button.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
button.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
button.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
button.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
button.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
button.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
button.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
button.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
button.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
button.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
button.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
button.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
button.o: commandFactory.h button.h /usr/include/SDL/SDL_image.h
volButton.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
volButton.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
volButton.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
volButton.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
volButton.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
volButton.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
volButton.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
volButton.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
volButton.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
volButton.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
volButton.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
volButton.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
volButton.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
volButton.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
volButton.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
volButton.o: commandFactory.h button.h /usr/include/SDL/SDL_image.h guiPos.h
volButton.o: volButton.h
buttonManager.o: statsBar.h /usr/include/SDL/SDL.h
buttonManager.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_stdinc.h
buttonManager.o: /usr/include/SDL/SDL_config.h
buttonManager.o: /usr/include/SDL/SDL_platform.h
buttonManager.o: /usr/include/SDL/begin_code.h /usr/include/SDL/close_code.h
buttonManager.o: /usr/include/SDL/SDL_audio.h /usr/include/SDL/SDL_error.h
buttonManager.o: /usr/include/SDL/SDL_endian.h /usr/include/SDL/SDL_mutex.h
buttonManager.o: /usr/include/SDL/SDL_thread.h /usr/include/SDL/SDL_rwops.h
buttonManager.o: /usr/include/SDL/SDL_cdrom.h /usr/include/SDL/SDL_cpuinfo.h
buttonManager.o: /usr/include/SDL/SDL_events.h /usr/include/SDL/SDL_active.h
buttonManager.o: /usr/include/SDL/SDL_keyboard.h
buttonManager.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
buttonManager.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
buttonManager.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
buttonManager.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
buttonManager.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h
buttonManager.o: playlist.h scroller.h timer.h threadParms.h songDb.h
buttonManager.o: sqlite3.h commandFactory.h buttonManager.h
buttonManager.o: /usr/include/SDL/SDL_image.h volButton.h button.h
buttonManager.o: rndRptButton.h rndButton.h rptButton.h seekButton.h
seekButton.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
seekButton.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
seekButton.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
seekButton.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
seekButton.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
seekButton.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
seekButton.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
seekButton.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
seekButton.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
seekButton.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
seekButton.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
seekButton.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
seekButton.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
seekButton.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
seekButton.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
seekButton.o: commandFactory.h button.h /usr/include/SDL/SDL_image.h guiPos.h
seekButton.o: seekButton.h
rndButton.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
rndButton.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
rndButton.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
rndButton.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
rndButton.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
rndButton.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
rndButton.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
rndButton.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
rndButton.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
rndButton.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
rndButton.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
rndButton.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
rndButton.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
rndButton.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
rndButton.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
rndButton.o: commandFactory.h button.h /usr/include/SDL/SDL_image.h guiPos.h
rndButton.o: rndButton.h
rptButton.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
rptButton.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
rptButton.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
rptButton.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
rptButton.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
rptButton.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
rptButton.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
rptButton.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
rptButton.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
rptButton.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
rptButton.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
rptButton.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
rptButton.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
rptButton.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
rptButton.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
rptButton.o: commandFactory.h button.h /usr/include/SDL/SDL_image.h guiPos.h
rptButton.o: rptButton.h
rndRptButton.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
rndRptButton.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
rndRptButton.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
rndRptButton.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
rndRptButton.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
rndRptButton.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
rndRptButton.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
rndRptButton.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
rndRptButton.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
rndRptButton.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
rndRptButton.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
rndRptButton.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
rndRptButton.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
rndRptButton.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
rndRptButton.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
rndRptButton.o: commandFactory.h button.h /usr/include/SDL/SDL_image.h
rndRptButton.o: guiPos.h rndRptButton.h
menu.o: menu.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
menu.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
menu.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
menu.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
menu.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
menu.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
menu.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
menu.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
menu.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
menu.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
menu.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
menu.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
menu.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
menu.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h menuButton.h
menu.o: /usr/include/SDL/SDL_image.h config.h button.h scroller.h
menu.o: threadParms.h songDb.h sqlite3.h commandFactory.h timer.h guiPos.h
menu.o: keyboard.h playlist.h
menuButton.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
menuButton.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
menuButton.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
menuButton.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
menuButton.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
menuButton.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
menuButton.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
menuButton.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
menuButton.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
menuButton.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
menuButton.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
menuButton.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
menuButton.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
menuButton.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
menuButton.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
menuButton.o: commandFactory.h menuButton.h /usr/include/SDL/SDL_image.h
menuButton.o: button.h guiPos.h rptButton.h
fullPlaying.o: fullPlaying.h /usr/include/SDL/SDL.h
fullPlaying.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_stdinc.h
fullPlaying.o: /usr/include/SDL/SDL_config.h /usr/include/SDL/SDL_platform.h
fullPlaying.o: /usr/include/SDL/begin_code.h /usr/include/SDL/close_code.h
fullPlaying.o: /usr/include/SDL/SDL_audio.h /usr/include/SDL/SDL_error.h
fullPlaying.o: /usr/include/SDL/SDL_endian.h /usr/include/SDL/SDL_mutex.h
fullPlaying.o: /usr/include/SDL/SDL_thread.h /usr/include/SDL/SDL_rwops.h
fullPlaying.o: /usr/include/SDL/SDL_cdrom.h /usr/include/SDL/SDL_cpuinfo.h
fullPlaying.o: /usr/include/SDL/SDL_events.h /usr/include/SDL/SDL_active.h
fullPlaying.o: /usr/include/SDL/SDL_keyboard.h /usr/include/SDL/SDL_keysym.h
fullPlaying.o: /usr/include/SDL/SDL_mouse.h /usr/include/SDL/SDL_video.h
fullPlaying.o: /usr/include/SDL/SDL_joystick.h /usr/include/SDL/SDL_quit.h
fullPlaying.o: /usr/include/SDL/SDL_loadso.h /usr/include/SDL/SDL_timer.h
fullPlaying.o: /usr/include/SDL/SDL_version.h /usr/include/SDL/SDL_ttf.h
fullPlaying.o: libmpdclient.h menuButton.h /usr/include/SDL/SDL_image.h
fullPlaying.o: config.h button.h artButton.h threadParms.h songDb.h sqlite3.h
fullPlaying.o: scroller.h commandFactory.h timer.h guiPos.h keyboard.h
fullPlaying.o: playlist.h
artButton.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
artButton.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
artButton.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
artButton.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
artButton.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
artButton.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
artButton.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
artButton.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
artButton.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
artButton.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
artButton.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
artButton.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
artButton.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
artButton.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
artButton.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
artButton.o: commandFactory.h artButton.h /usr/include/SDL/SDL_image.h
artButton.o: button.h id3Button.h guiPos.h rptButton.h
settings.o: settings.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
settings.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
settings.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
settings.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
settings.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
settings.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
settings.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
settings.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
settings.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
settings.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
settings.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
settings.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
settings.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
settings.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h scroller.h
settings.o: threadParms.h songDb.h sqlite3.h commandFactory.h timer.h
settings.o: config.h gp2xregs.h playlist.h keyboard.h guiPos.h
settings.o: /usr/include/SDL/SDL_image.h
id3Button.o: statsBar.h /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
id3Button.o: /usr/include/SDL/SDL_stdinc.h /usr/include/SDL/SDL_config.h
id3Button.o: /usr/include/SDL/SDL_platform.h /usr/include/SDL/begin_code.h
id3Button.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_audio.h
id3Button.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_endian.h
id3Button.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_thread.h
id3Button.o: /usr/include/SDL/SDL_rwops.h /usr/include/SDL/SDL_cdrom.h
id3Button.o: /usr/include/SDL/SDL_cpuinfo.h /usr/include/SDL/SDL_events.h
id3Button.o: /usr/include/SDL/SDL_active.h /usr/include/SDL/SDL_keyboard.h
id3Button.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
id3Button.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_joystick.h
id3Button.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_loadso.h
id3Button.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_version.h
id3Button.o: /usr/include/SDL/SDL_ttf.h libmpdclient.h config.h playlist.h
id3Button.o: scroller.h timer.h threadParms.h songDb.h sqlite3.h
id3Button.o: commandFactory.h menuButton.h /usr/include/SDL/SDL_image.h
id3Button.o: button.h guiPos.h rptButton.h
