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
#include "volButton.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

VolButton::VolButton()
: Button("vol")
, m_width(0)
, m_showBig(false)
, m_backImageBig(NULL)
, m_foreImageBig(NULL)
{
}

void VolButton::init(Config& config, vector<int>& volScale)
{
	string btnName = "sk_"+m_name;
	Button::init(config);
	m_width = m_destRect.w;	
	int from = 0;
	for(int i=0; i<=20; ++i) {
		int j;
		for(j=from; j <= volScale[i]; ++j)
			m_volLookup[j] = i;

		from = j;
	}
	if(config.getItem("softwareVolume") == "on")
		m_softVol = true;
	else
		m_softVol = false;

	SDL_Surface * tmpBack = IMG_Load(string("skins/"+config.getItem("skin")+"/"+
				m_name+"_backBig.png").c_str());
	if (!tmpBack) {
		printf("Unable to load big vol button image: %s\n", SDL_GetError());
	} else {
		m_backImageBig = SDL_DisplayFormatAlpha(tmpBack);
		m_destRectBig.w = m_backImageBig->w;
		m_destRectBig.h = m_backImageBig->h;
		m_destRectBig.x = (m_destRect.x+m_destRect.w)/2;
		m_destRectBig.y = m_destRect.y;
	}
	
	tmpBack = IMG_Load(string("skins/"+config.getItem("skin")+"/"+
				m_name+"_foreBig.png").c_str());
	if (!tmpBack) {
		printf("Unable to load big vol button image: %s\n", SDL_GetError());
	} else {
		m_foreImageBig = SDL_DisplayFormatAlpha(tmpBack);
		if(m_backImageBig == NULL) {
			m_destRectBig.w = m_foreImageBig->w;
			m_destRectBig.h = m_foreImageBig->h;
			m_destRectBig.x = m_destRect.x;
			m_destRectBig.y = m_destRect.y;
		}
	}

	m_height = m_destRectBig.h;	
	m_clearRectBig = m_destRectBig;

	m_srcRectBig.x=0;
	m_srcRectBig.y=0;
	m_srcRectBig.w = m_destRectBig.w;
	m_srcRectBig.h = m_destRectBig.h;
}

void VolButton::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
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
		if(statusChanged & VOL_CHG) { 
			int scaledVol = m_volLookup[status->volume];
			m_srcRect.w = m_width*scaledVol / 20;
			m_srcRectBig.h = m_height*scaledVol / 20;
			m_destRectBig.y = m_destRectBig.y + (m_height - m_srcRectBig.h);
			m_refresh = true;
		}
	}
}
	
int VolButton::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	if(command > 0) {

		if(command == CMD_CLICK) {
			if(guiPos.curX > m_destRect.x 
				&& guiPos.curX < m_destRect.x + m_width
				&& guiPos.curY > m_destRect.y 
				&& guiPos.curY < m_destRect.y + m_destRect.h) {
		/*	
				if(guiPos.curX < (m_destRect.x + m_width)/2)
					rCommand = CMD_VOL_DOWN;
				else
					rCommand = CMD_VOL_UP;
		*/

			}
		}
	}

	return rCommand;
}
/*
	int rCommand = command;
	if(command > 0) {

		if(command == CMD_CLICK) {
			if(!m_showBig) {
				if(guiPos.curX > m_destRect.x 
						&& guiPos.curX < m_destRect.x + m_width
						&& guiPos.curY > m_destRect.y 
						&& guiPos.curY < m_destRect.y + m_destRect.h) {

					m_showBig = !m_showBig;
					m_refresh = true;			

				}
			} else {
				if(guiPos.curX > m_destRectBig.x 
						&& guiPos.curX < m_destRectBig.x + m_width
						&& guiPos.curY > m_destRectBig.y 
						&& guiPos.curY < m_destRectBig.y + m_destRect.h) {

					if(guiPos.curY > (m_destRectBig.y + m_destRect.h)/2) 
						command = CMD_VOL_DOWN;
					else
						command = CMD_VOL_UP;
					m_refresh = true;
				}
			}
		}
	}

	return rCommand;
}
*/

void VolButton::draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh)
{
		
	Button::draw(screen, bg, forceRefresh);
/*
	if(m_showBig && (forceRefresh || m_refresh)) {
		SDL_SetClipRect(screen, &m_clearRectBig);
		SDL_BlitSurface(bg, &m_clearRectBig, screen, &m_clearRectBig);
		cout << "dst " << m_destRectBig.h << endl;
		if(m_backImageBig != NULL)
			SDL_BlitSurface(m_backImageBig, NULL, screen, &m_clearRectBig );
		if(m_foreImageBig != NULL)
			SDL_BlitSurface(m_foreImageBig, &m_srcRectBig, screen, &m_destRectBig );
 	
		m_refresh = false;
	} else {
	//clear this portion of the screen 

	}
*/
}

