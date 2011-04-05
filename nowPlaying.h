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

#ifndef __NOWPLAYING_H__
#define __NOWPLAYING_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "libmpdclient.h"
#include "config.h"
#include "playlist.h"

class GuiPos;

class NowPlaying
{
public:
	NowPlaying(mpd_Connection* mpd, SDL_mutex* lock,  SDL_Surface* screen, SDL_Surface* bg, Config& config, SDL_Rect& rect, Playlist& playlist);
	
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
					  int rtmpdStatusChanged, mpd_Status* rtmpdStatus);
	int processCommand(int command, GuiPos& guiPos);
	void draw(bool forceRefresh, long timePerFrame, bool atBack);
	void songChange();
protected:
	
	mpd_Connection* m_mpd;
	SDL_mutex* m_lock;
	SDL_Surface* m_screen;
	SDL_Rect m_srcRect;
	SDL_Rect m_destRect;
	SDL_Rect m_artistRect;
	SDL_Rect m_scrollClearRect;
	SDL_Rect m_artistClearRect;
	Config& m_config;
	TTF_Font* m_font;
	TTF_Font* m_fontSmall;
	Playlist& m_playlist;
	std::string m_title;
	std::string m_artist;
	std::string m_scrollText;
	SDL_Surface* m_scrollTextSurface;
	SDL_Surface* m_artistTextSurface;;

	int m_format;
	int m_skipVal;
	int m_inc;
	int m_incArtist;
	int m_origY;
	int m_delay;
	int m_delayCnt;
	int m_delayCnt2;
	int m_srcX;
	int m_srcArtistX;

	bool m_artistNoScroll;
	bool m_noScroll;
	int m_nowPlaying;
	bool m_refresh;
	
	//colors
	SDL_Color m_backColor;
	SDL_Color m_itemColor;
	SDL_Color m_curItemBackColor;
	SDL_Color m_curItemColor;
	SDL_Surface * m_bg;
};

#endif
