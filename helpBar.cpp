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

#include "helpBar.h"
#include <iostream>
#include <stdexcept>
#include <SDL_image.h>

using namespace std;

HelpBar::HelpBar(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, Config& config, SDL_Rect& rect)
: m_mpd(mpd)
, m_screen(screen)
, m_config(config)
, m_clearRect(rect)
, m_srcRect(rect)
, m_pos(0)
, m_refresh(true)
, m_doNotDraw(false)
, m_bg(bg)
{
	if(rect.x < 0)
		m_doNotDraw = true;

	m_destRect.x = rect.x;
	if(m_clearRect.h > 20) {
		m_destRect.x += 5;	
	}
	m_destRect.y = rect.y;
	m_origY = m_destRect.y;
	m_font = TTF_OpenFont(config.getItem("sk_font_help").c_str(),
						  config.getItemAsNum("sk_font_help_size"));
	m_skipVal = TTF_FontLineSkip( m_font ) * config.getItemAsFloat("sk_font_help_extra_spacing");
	m_srcRect.x = 0;
	m_srcRect.y = 0;

	vector<string> tmp;
//#ifdef GP2X	
	//browser
	tmp.push_back("B-Play|Pause");	
	tmp.push_back("X-Go Back|Stop");	
	tmp.push_back("A-Append File/Folder");	
	tmp.push_back("Y-Add as Playlist");	
	m_modeCmdText.push_back(tmp);
	tmp.clear();
	//playlist	
	tmp.push_back("B-Play/Pause|Pause");	
	tmp.push_back("X-Stop");	
	tmp.push_back("A-Rnd/Rpt|Delete");	
	tmp.push_back("Y-Move");	
	tmp.push_back("R-Next|FF");	
	tmp.push_back("L-Prev|RWD");	
	m_modeCmdText.push_back(tmp);
	tmp.clear();
	//pl browser
	tmp.push_back("B-Load|Pause");	
	tmp.push_back("X-Stop");	
	tmp.push_back("A-Append/Delete");	
	m_modeCmdText.push_back(tmp);
	tmp.clear();
	//bookmarks
	tmp.push_back("B-Load|Pause");	
	tmp.push_back("X-Stop");	
	tmp.push_back("A-None|Delete");	
	m_modeCmdText.push_back(tmp);
/*#else
	tmp.push_back("p-Play/Pause");	
	tmp.push_back("i-Append");	
	tmp.push_back("s-Stop");	
	m_modeCmdText.push_back(tmp);
	tmp.clear();
	tmp.push_back("p-Play/Pause");	
	tmp.push_back("s-Stop");	
	tmp.push_back("n-Next");	
	tmp.push_back("p-Prev");	
	tmp.push_back("d-Delete");	
	tmp.push_back("h-Move");	
	m_modeCmdText.push_back(tmp);
	tmp.clear();
	//pl browser
	tmp.push_back("B-Load");	
	tmp.push_back("A-Append");	
	tmp.push_back("Y-Delete");	
	m_modeCmdText.push_back(tmp);
	tmp.clear();
	//bookmarks
	tmp.push_back("B-Load");	
	tmp.push_back("Y-Delete");	
	m_modeCmdText.push_back(tmp);
#endif
*/	
	m_config.getItemAsColor("sk_help_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_help_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);
}

void HelpBar::updateStatus(bool mpdStatusChanged, mpd_Status* mpdStatus)
{

}

void HelpBar::draw(int curMode, bool forceUpdate)
{
	if(m_refresh || forceUpdate || !m_doNotDraw) {
		m_destRect.x = m_clearRect.x;
		if(m_clearRect.h > 20) {
			m_destRect.x += 5;	
		}
		m_destRect.y = m_clearRect.y;
		//clear this portion of the screen 
		SDL_SetClipRect(m_screen, &m_clearRect);
		SDL_BlitSurface(m_bg, &m_clearRect, m_screen, &m_clearRect );


		SDL_Surface *sText;
		for(vector<string>::iterator vvIter = m_modeCmdText[curMode].begin();
				vvIter != m_modeCmdText[curMode].end();
				++vvIter) {
			sText = TTF_RenderUTF8_Blended(m_font, (*vvIter).c_str(), m_itemColor);
			SDL_BlitSurface(sText, NULL, m_screen, &m_destRect );
			if(m_clearRect.h < 20) {
				m_destRect.x += sText->w+3;
			} else {
				m_destRect.y += m_skipVal;
			}
			SDL_FreeSurface(sText);
		}
		m_refresh = false;
	}

}
