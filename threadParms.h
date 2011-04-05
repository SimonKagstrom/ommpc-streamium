/*****************************************************************************************

ommpc(One More Music Player Client) - A Music Player Daemon client targetted for the gp2x

Copyright (C) 2007 - Tim Temple(codertimt@gmail.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*****************************************************************************************/

#ifndef __THREADPARMS_H__
#define __THREADPARMS_H__

#include "libmpdclient.h"
#include <SDL.h>
#include <string>
#include "songDb.h"

#if !defined(GP2X) && !defined(WIZ)
#define GP2X_VK_FY  SDLK_d
#define GP2X_VK_FX  SDLK_s
#define GP2X_VK_FB  SDLK_p
#define GP2X_VK_FA  SDLK_h
#define GP2X_VK_FR  SDLK_n
#define GP2X_VK_FL  SDLK_b
#define GP2X_VK_UP  SDLK_k
#define GP2X_VK_DOWN  SDLK_j
#define GP2X_VK_LEFT  SDLK_LEFT
#define GP2X_VK_RIGHT  SDLK_RIGHT
#define GP2X_VK_VOL_UP  SDLK_0
#define GP2X_VK_VOL_DOWN  SDLK_9
#define GP2X_VK_SELECT  SDLK_m
#define GP2X_VK_START  SDLK_c
#endif



enum { SONG_CHG=1, PL_CHG=2, RPT_CHG=4, RND_CHG=8, RATE_CHG=16, ELAPSED_CHG=32, VOL_CHG=64, STATE_CHG=128, UPDB_CHG=256, FREQ_CHG=512};

typedef struct {
	mpd_Connection* mpd;
	mpd_Status * mpdStatus;
	int mpdStatusChanged;
	bool mpdReady;
	bool pollStatusDone;
	SDL_mutex* lockConnection;
} threadParms_t;

typedef struct {
	bool doArtLoad;
	bool done;
	std::string songFile;
	SDL_Surface* artSurface;
	int destWidth;
	int destHeight;
} artThreadParms_t; 

typedef struct {
	SongDb* songDb;
	bool updating;	
} songDbThreadParms_t;
#endif
