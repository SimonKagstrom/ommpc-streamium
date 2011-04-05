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

#ifndef __STATSBAR_H__
#define __STATSBAR_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "libmpdclient.h"
#include "config.h"
#include "playlist.h"
#include <vector>
class StatsBar
{
public:
	StatsBar(mpd_Connection* mpd, SDL_mutex* lock,  SDL_Surface* screen, SDL_Surface* bg, Config& config, SDL_Rect& rect, bool&, Playlist& pl, bool f200, std::vector<int>& volScale);

	std::string formattedElapsedTime();
	int elapsedTime();
	
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
							int rtmpdStatusChanged, mpd_Status* rtmpdStatus, bool forceRefresh);
	void draw(bool forceRefresh, int fps);
protected:
	void initAll();
	bool m_good;
	
	mpd_Connection* m_mpd;
	SDL_mutex* m_lock;
	SDL_Surface* m_screen;
	SDL_Surface* m_bg;
	SDL_Rect m_destRect;
	SDL_Rect& m_clearRect;
	SDL_Rect& m_backRect;
	SDL_Rect m_curItemClearRect;
	SDL_Rect m_curItemRect;
	Config& m_config;
	TTF_Font* m_font;
	
	std::string m_name;
	std::string m_volume;
	std::string m_playType;
	std::string m_crossfade;
	std::string m_elapsed;
	std::string m_total;
	std::string m_bitRate;
	std::string m_items[4];

	int m_elapsedSeconds;	
	int m_cols;
	int m_itemH;
	int m_pos;
	int m_inc;
	int m_origY;
	
	int m_numCols;
	int m_itemWidth;
	int m_itemSpacing;

	int m_curVol;	
	bool& m_firstPass;

	//colors
	SDL_Color m_backColor;
	SDL_Color m_itemColor;
	SDL_Color m_curItemBackColor;
	SDL_Color m_curItemColor;

	Playlist& m_playlist;
	bool m_f200;
	int m_volLookup[101];
	bool m_softVol;
};

#endif
