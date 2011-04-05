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
#include "id3Button.h"
#include "guiPos.h"
#include "rptButton.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

ID3Button::ID3Button(string label, string id, TTF_Font* font)	
: Button(id, "skin")
, m_visible(true)
, m_label(label)
, m_sText(NULL)
, m_id(id)
, m_displayText(true)
{
	m_font = font;
	if(m_label.empty())
		m_displayText = false;
}

void ID3Button::init(Config& config)
{
	config.getItemAsColor("sk_seek_textColor", m_textColor.r, m_textColor.g, m_textColor.b);
	Button::init(config);
	
	if(m_sText != NULL)
		SDL_FreeSurface(m_sText);

	m_sText = TTF_RenderUTF8_Blended(m_font, (m_label+" ").c_str(), m_textColor);
	m_clearRect.w = m_sText->w;
	m_clearRect.h = m_sText->h;
}

void ID3Button::setText(std::string text)
{
	if(m_sText != NULL)
		SDL_FreeSurface(m_sText);
	
	m_sText = TTF_RenderUTF8_Blended(m_font, (m_label+text).c_str(), m_textColor);
	m_clearRect.w = m_sText->w;
	m_clearRect.h = m_sText->h;
}

void ID3Button::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
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
	
int ID3Button::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	if(command > 0) {
	}

	return rCommand;
}

void ID3Button::draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh)
{
	//clear this portion of the screen 
	if(m_refresh || forceRefresh) {
//		SDL_SetClipRect(screen, &m_clearRect);
//		SDL_BlitSurface(bg, &m_clearRect, screen, &m_clearRect);
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
			SDL_BlitSurface(m_sText, NULL, screen, &m_destRect );
			m_destRect.y = y;
			m_destRect.x = x;
		}
		m_refresh = false;
	}
}

