/*****************************************************************************************

ommpc(One More Music Player Client) - A Music Player Daemon client targetted for the gp2x

Copyright (C) 2007-2008 - Tim Temple(codertimt@gmail.com)

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

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <SDL.h>
#include <SDL_image.h>

#include "config.h"
#include <string>
#include <vector>

class Button
{
public:
	Button(std::string name, std::string imageDir="skin");
	virtual void init(Config& config);
	void updateInfo(std::string name, std::string imageDir);
	virtual void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
					  int rtmpdStatusChanged, mpd_Status* rtmpdStatus, 
					  bool forceRefresh);
	virtual void draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh);
protected:
	
	SDL_Rect m_destRect;
	SDL_Rect m_clearRect;
	SDL_Rect m_srcRect;
	bool m_showBack;	
	bool m_showFore;	
	std::string m_name;
	SDL_Surface* m_backImage;
	SDL_Surface* m_foreImage;
	bool m_refresh;
	std::string m_imageDir;

};

#endif
