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

#ifndef __Overlay_H__
#define __Overlay_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

#include "libmpdclient.h"
#include "config.h"

class GuiPos;
class Playlist;

class Overlay
{
public:
	Overlay(mpd_Connection* mpd, SDL_Surface* screen, Config& config, SDL_Rect& rect, Playlist& playlist);
	void updateStatus(bool mpdStatusChanged, mpd_Status * mpdStatus);	
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
							int rtmpdStatusChanged, mpd_Status* rtmpdStatus, int repeatDelay);
	void draw(bool forceUpdate);
	bool inRect(int x, int y, SDL_Rect& rect);
	int processCommand(int command, GuiPos& guiPos, bool visible, int curMode);
protected:
	void initAll();
	bool m_good;	
	mpd_Connection* m_mpd;
	SDL_Surface* m_screen;

	SDL_Rect m_playRect;
	SDL_Rect m_ffRect;
	SDL_Rect m_rwRect;
	SDL_Rect m_nextRect;
	SDL_Rect m_prevRect;
	SDL_Rect m_menuRect;
	SDL_Rect m_exitRect;
	SDL_Rect m_stopRect;

	SDL_Rect m_clickRect1;
	SDL_Rect m_clickRect2;
	
	SDL_Rect& m_clearRect;
	Playlist& m_playlist;	
	Config& m_config;

	bool m_refresh;
	
	SDL_Surface * m_ff;
	SDL_Surface * m_rw;
	SDL_Surface * m_play;
	SDL_Surface * m_pause;
	SDL_Surface * m_next;
	SDL_Surface * m_prev;
	SDL_Surface * m_exit;
	SDL_Surface * m_menu;
	SDL_Surface * m_stop;
};

#endif
