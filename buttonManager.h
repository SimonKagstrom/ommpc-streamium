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

#ifndef __BUTTONMAN_H__
#define __BUTTONMAN_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

#include "libmpdclient.h"
#include "config.h"
#include "volButton.h"
#include "rndRptButton.h"
#include "rndButton.h"
#include "rptButton.h"
#include "seekButton.h"
#include <vector>

class GuiPos;

class ButtonManager
{
public:
	ButtonManager(mpd_Connection* mpd, SDL_mutex* lock,  SDL_Surface* screen, SDL_Surface* bg, Config& config, std::vector<int>& volScale);
	~ButtonManager();

	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
					  int rtmpdStatusChanged, mpd_Status* rtmpdStatus, 
					  bool forceRefresh);
	int processCommand(int command, GuiPos& guiPos);
	void draw(bool forceRefresh, long timePerFrame, bool inBack);
	std::string formattedElapsedTime();
	int elapsedTime();
protected:
	
	mpd_Connection* m_mpd;
	SDL_mutex* m_lock;
	SDL_Surface* m_screen;
	SDL_Surface* m_bg;
	Config& m_config;

	VolButton* m_volBtn;
	RndRptButton* m_rrBtn;
	RndButton* m_rndBtn;
	RptButton* m_rptBtn;
	SeekButton* m_seekBtn;
};

#endif
