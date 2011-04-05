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

#ifndef __SEEKBUTTON_H__
#define __SEEKBUTTON_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

#include "libmpdclient.h"
#include "config.h"
#include <vector>
#include "button.h"

class SeekButton : public Button
{
public:
	SeekButton(std::string name);
	virtual void init(Config& config);

	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
					  int rtmpdStatusChanged, mpd_Status* rtmpdStatus, 
					  bool forceRefresh);
	int processCommand(int command, GuiPos& guiPos, mpd_Connection* mpd);
	virtual void draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh, long timePerFrame, bool inBack);
	int elapsedTime();
protected:
	std::string formattedTime(int time);
	std::string formattedRemaining(int time);
	void setDisplayNumWidth();

	int m_delay;
	int m_delayCnt;
	
	int m_width;
	int m_numWidth;
	int m_yOffset;
	int m_timeType;
	int m_xOffset;

	int m_elapsedSeconds;	
	int m_totalSeconds;
	int m_remainingSeconds;
	int m_nowPlaying;
	std::string m_total;
	std::string m_elapsed;
	std::string m_remaining;

	TTF_Font* m_font;	
	SDL_Color m_textColor;
};

#endif
