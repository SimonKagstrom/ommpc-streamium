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

#ifndef __HELPBAR_H__
#define __HELPBAR_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

#include "libmpdclient.h"
#include "config.h"

class HelpBar
{
public:
	HelpBar(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, Config& config, SDL_Rect& rect);
	void updateStatus(bool mpdStatusChanged, mpd_Status * mpdStatus);	
	void draw(int curMode, bool forceUpdate);
protected:
	
	mpd_Connection* m_mpd;
	SDL_Surface* m_screen;
	SDL_Rect m_destRect;
	SDL_Rect& m_clearRect;
	SDL_Rect m_srcRect;
	Config& m_config;
	TTF_Font* m_font;
	int m_pos;
	int m_inc;
	int m_origY;

	int m_skipVal;
	bool m_refresh;
	bool m_doNotDraw;
		
	std::vector<std::vector<std::string> >m_modeCmdText;
	
	//colors
	SDL_Color m_backColor;
	SDL_Color m_itemColor;
	SDL_Color m_curItemBackColor;
	SDL_Color m_curItemColor;
	SDL_Surface * m_bg;
};

#endif
