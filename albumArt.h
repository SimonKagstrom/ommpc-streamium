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

#ifndef __ALBUMART_H__
#define __ALBUMART_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

#include "threadParms.h"
#include "libmpdclient.h"
#include "config.h"

class AlbumArt
{
public:
	AlbumArt(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, Config& config, SDL_Rect& rect,
					artThreadParms_t& m_artParms);
	void updateStatus(int mpdStatusChanged, mpd_Status * mpdStatus,
						int rtmpdStatusChanged, mpd_Status* rtmpdStatus);
	void draw(bool forceUpdate);
protected:
	
	mpd_Connection* m_mpd;
	SDL_Surface* m_screen;
	SDL_Surface* m_bg;
	SDL_Rect m_destRect;
	SDL_Rect& m_clearRect;
	Config& m_config;
	int m_pos;
	int m_inc;
	int m_origY;

	bool m_update;
	artThreadParms_t& m_artParms;
	
	//colors
	SDL_Color m_backColor;
};

#endif
