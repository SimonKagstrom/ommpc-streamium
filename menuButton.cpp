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
#include "menuButton.h"
#include "guiPos.h"
#include "rptButton.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

MenuButton::MenuButton(string label, string id, TTF_Font* font)	
: Button(id, "overlay")
, m_active(false)
, m_label(label)
, m_sText(NULL)
, m_id(id)
, m_displayText(true)
{
	m_font = font;
	if(m_label.empty())
		m_displayText = false;
}

void MenuButton::init(Config& config, int command)
{
	m_command = command;
	Button::init(config);
	m_mouseRect = m_destRect;
}

void MenuButton::init(Config& config, int x, int y, string type, int command, int xSize, int ySize)
{
	config.getItemAsColor("sk_seek_textColor", m_textColor.r, m_textColor.g, m_textColor.b);
	string btnName = "sk_"+m_name;
	m_clearRect.x = x;
	m_clearRect.y = y;
	m_clearRect.w = xSize;
	m_clearRect.h = ySize;
	
	SDL_Surface * tmpBack = IMG_Load(string("skins/"+config.getItem("skin")+"/bg_menuItem.png").c_str());
	m_destRect = m_clearRect;
	m_destRectB = m_clearRect;
	if (!tmpBack) {
		printf("Unable to load button image: %s\n", SDL_GetError());
		m_showBack = false;
	} else {
		m_showBack = true;
		m_backImage = SDL_DisplayFormatAlpha(tmpBack);
		m_destRectB.x += (xSize-m_backImage->w)/2;
		m_destRectB.w = m_backImage->w;
		m_destRectB.h = m_backImage->h;
	}
		tmpBack = IMG_Load(string("overlays/"+config.getItem("sk_overlay")
													   + "/menu_"+ m_id +".png").c_str());
	if (!tmpBack) {
		printf("Unable to load alt button image: %s\n", SDL_GetError());
		m_showFore = false;
	} else {
		m_showFore = true;
		m_foreImage = SDL_DisplayFormatAlpha(tmpBack);
		m_destRect.x += (xSize-m_foreImage->w)/2;
		if(!m_showBack) {
			m_destRect.w = m_foreImage->w;
			m_destRect.h = m_foreImage->h;
		}
	}

	m_srcRect.x=0;
	m_srcRect.y=0;
	m_srcRect.w = m_destRect.w;
	m_srcRect.h = m_destRect.h;
	m_command = command;
	if(m_sText != NULL)
		SDL_FreeSurface(m_sText);

	if(!m_label.empty())
		m_sText = TTF_RenderUTF8_Blended(m_font, m_label.c_str(), m_textColor);
	m_mouseRect = m_destRect;
}


void MenuButton::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
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
	}
}
	
int MenuButton::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	if(command > 0) {
		if(command == CMD_CLICK || (command == CMD_HOLD_CLICK && (m_command == CMD_FF || m_command == CMD_RW))) {
			if(guiPos.curX > m_mouseRect.x 
				&& guiPos.curX < m_mouseRect.x + m_mouseRect.w
				&& guiPos.curY > m_mouseRect.y 
				&& guiPos.curY < m_mouseRect.y + m_mouseRect.h) {
				rCommand = m_command;
				m_active = true;
			}
		} else if(command == CMD_POP_SELECT && m_active) {
			rCommand = m_command;
		}
	}

	return rCommand;
}

void MenuButton::draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh)
{
	//clear this portion of the screen 
	if(m_refresh || forceRefresh) {
		SDL_SetClipRect(screen, &m_clearRect);
		SDL_BlitSurface(bg, &m_clearRect, screen, &m_clearRect);
		if(m_showBack && m_active)		
			SDL_BlitSurface(m_backImage, NULL, screen, &m_destRectB );
		if(m_showFore)		{
			SDL_BlitSurface(m_foreImage, NULL, screen, &m_destRect );
/*			cout << "blitx " << m_destRect.x << endl;
			cout << "blity " << m_destRect.y << endl;
			cout << "blitw " << m_destRect.w << endl;
			cout << "blith " << m_destRect.h << endl;
*/
		}
		if(m_displayText) {
			int x = m_destRect.x;
			int y = m_destRect.y;
			m_destRect.x += ((m_destRect.w-m_sText->w)/2);
			m_destRect.y += 48;
			SDL_BlitSurface(m_sText, NULL, screen, &m_destRect );
			m_destRect.y = y;
			m_destRect.x = x;
		}
		m_refresh = false;
	}
}

