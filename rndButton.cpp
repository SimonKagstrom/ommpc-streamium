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
#include "rndButton.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

RndButton::RndButton()
: Button("rnd")
, m_active(true)
{
}


void RndButton::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
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
		if(statusChanged & RND_CHG) { 
			if(status->random == 1)
				m_active = true;
			else 
				m_active = false;
			m_refresh = true;
		}
	}
}
	
int RndButton::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	if(command > 0) {

		if(command == CMD_CLICK) {
			if(guiPos.curX > m_destRect.x 
				&& guiPos.curX < m_destRect.x + m_destRect.w
				&& guiPos.curY > m_destRect.y 
				&& guiPos.curY < m_destRect.y + m_destRect.h) {
		
				rCommand = CMD_MODE_RANDOM;
			}
		}
	}

	return rCommand;
}

void RndButton::draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh)
{
	//clear this portion of the screen 
	if(m_refresh || forceRefresh) {
		SDL_SetClipRect(screen, &m_clearRect);
		SDL_BlitSurface(bg, &m_clearRect, screen, &m_clearRect);
		if(m_active) {
			SDL_BlitSurface(m_foreImage, NULL, screen, &m_destRect );
		} else {
			if(m_showBack)
				SDL_BlitSurface(m_backImage, NULL, screen, &m_destRect );
		}
		m_refresh = false;
	}
}

