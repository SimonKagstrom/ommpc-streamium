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

#include "statsBar.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "playlist.h"
#include "button.h"
#include "guiPos.h"
#include "rndRptButton.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

RndRptButton::RndRptButton()
: Button("rndrpt")
, m_good(true)
, m_mode(0)
{
}

void RndRptButton::init(Config& config)
{
	Button::init(config);
	string btnName = "sk_"+m_name;
	
	SDL_Surface * tmpBack = IMG_Load(string("skins/"+config.getItem("skin")+"/"+
									config.getItem(btnName+"_foreImage2")).c_str());
	if (!tmpBack) {
		printf("Unable to load alt 2 button image: %s\n", SDL_GetError());
	} else {
		m_foreImage2 = SDL_DisplayFormatAlpha(tmpBack);
	}

	if(m_backImage == NULL || m_foreImage == NULL || m_foreImage2 == NULL)
		m_good = false;	
}

void RndRptButton::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
						int rtmpdStatusChanged, mpd_Status* rtmpdStatus, bool forceRefresh)
{
	mpd_Status * status;
	int statusChanged;
	bool doVol = false;
	if(rtmpdStatusChanged > 0) {
		status = rtmpdStatus;
		statusChanged = rtmpdStatusChanged;
		doVol = true;
	} else {
		status = mpdStatus;
		statusChanged = mpdStatusChanged;
	}
	if(statusChanged > 0) {
		if(statusChanged & RPT_CHG || statusChanged & RND_CHG) { 
			m_mode = 0;
			if(status->repeat == 1)
				m_mode = 1;
			if(status->random == 1) {
				if(m_mode == 1)
					m_mode = 2;
				else
					m_mode = 3;
			}
			m_refresh = true;
		}
	}
}
	
int RndRptButton::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	if(command > 0) {

		if(command == CMD_CLICK) {
			if(guiPos.curX > m_destRect.x 
				&& guiPos.curX < m_destRect.x + m_destRect.w
				&& guiPos.curY > m_destRect.y 
				&& guiPos.curY < m_destRect.y + m_destRect.h) {
		
				rCommand = CMD_RAND_RPT;
			}
		}
	}

	return rCommand;
}

void RndRptButton::draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh)
{
	//clear this portion of the screen 
	if(m_refresh || forceRefresh) {
		SDL_SetClipRect(screen, &m_clearRect);
		SDL_BlitSurface(bg, &m_clearRect, screen, &m_clearRect);
		if(m_good) {
			switch(m_mode) {
				case 0:
					break;
				case 1:
					SDL_BlitSurface(m_backImage, NULL, screen, &m_destRect );
					break;
				case 2:
					SDL_BlitSurface(m_backImage, NULL, screen, &m_destRect );
					SDL_BlitSurface(m_foreImage, NULL, screen, &m_destRect );
					break;
				case 3:
					SDL_BlitSurface(m_foreImage, NULL, screen, &m_destRect );
					break;
			}
		}
		m_refresh = false;
	}
}

