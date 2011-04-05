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

#include "scroller.h"
#include "config.h"
#include "commandFactory.h"
#include <SDL_image.h>
#include <iostream>
using namespace std;

Scroller::Scroller(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, TTF_Font* font, 
		SDL_Rect& rect, Config& config, int skipVal, int numPerScreen)
	: m_mpd(mpd)
	, m_screen(screen)
	, m_font(font)
	, m_config(config)
	, m_skipVal(skipVal)
	, m_numPerScreen(numPerScreen)
	, m_topItemNum(0)
	, m_curItemNum(0)
	, m_clearRect(rect)
	, m_lastItemNum(0)
	, m_curState(0)
	, m_bg(bg)
	, m_prevX(0)
	, m_prevY(0)
	, m_skipFirstMouse(true)
	, m_refresh(true)
	, m_delay(0)
	, m_delayCnt(0)
	, m_iconFilter(NULL)
	, m_iconFile(NULL)
	, m_iconFolder(NULL)
	, m_bgNowPlaying(NULL)
	, m_goodS(false)
{
	m_destRect.x = rect.x;
	m_destRect.y = rect.y;
	m_origY = m_destRect.y;
	m_curItemClearRect = rect;
	m_curItemIconRect.x = rect.x;
	m_curItemIconRect.y = rect.y;
	m_curItemIconRect.w = 12;
	m_curItemIconRect.h = 12;
	m_upClearRect.x = rect.x + rect.w - 25;
	m_upClearRect.w = 25;
	m_upClearRect.y = rect.y;
	m_upClearRect.h = 25;
	m_downClearRect.x = rect.x + rect.w - 25;
	m_downClearRect.w = 25;
	m_downClearRect.y = rect.y+rect.h-25;
	m_downClearRect.h = 25;
	
	
	//updateDisplayList();
}

void Scroller::initAllS()
{
	m_config.getItemAsColor("sk_popup_itemColor", m_pauseItemColor.r, m_pauseItemColor.g, m_pauseItemColor.b);
	
	string skinName = m_config.getItem("skin");
	m_pauseBtn= IMG_Load(string("skins/"+skinName+"/pause.png").c_str());
	if (!m_pauseBtn)
		printf("Unable to load image: %s\n", SDL_GetError());
	else 
		m_pauseBtn = SDL_DisplayFormatAlpha(m_pauseBtn);
	m_bgCurItem = IMG_Load(string("skins/"+skinName+"/bg_curItem.png").c_str());
	if (!m_bgCurItem)
		printf("Unable to load image: %s\n", SDL_GetError());
	else 
		m_bgCurItem = SDL_DisplayFormatAlpha(m_bgCurItem);
	m_downBtn = IMG_Load(string("skins/"+skinName+"/down.png").c_str());
	if (!m_downBtn)
		printf("Unable to load image: %s\n", SDL_GetError());
	else 
		m_downBtn = SDL_DisplayFormatAlpha(m_downBtn);
	m_upBtn = IMG_Load(string("skins/"+skinName+"/up.png").c_str());
	if (!m_downBtn)
		printf("Unable to load image: %s\n", SDL_GetError());
	else 
		m_upBtn = SDL_DisplayFormatAlpha(m_upBtn);

	m_goodS = true;
}

void Scroller::initItemIndexLookup() {
	int startPos = m_clearRect.y+m_skipVal*2;
	int skipCount = 0;
	int index = 0;	
//cout <<  "skipval " << m_skipVal << "  startPos  " << startPos << endl;
	for(int i=0; i<startPos; ++i) {
		m_itemIndexLookup.push_back(0);
	}
	for(int i=startPos; i<(m_clearRect.h+m_clearRect.y); ++i) {
		m_itemIndexLookup.push_back(index);
		if(skipCount >= m_skipVal) {
			++index;
			skipCount = 0;
//		cout << "i " << i << "   index  " << index << endl;
		}
		++skipCount;
	}

}

bool Scroller::processCommand(int& command)
{
	bool done = false;

	switch(command) {
		case CMD_DOWN:
		case CMD_MOUSE_DOWN:
			++m_curItemNum;
			if(m_curItemNum > m_lastItemNum) {
				m_topItemNum = m_curItemNum = 0;
			} else 	if(m_curItemNum >= m_numPerScreen+m_topItemNum) {
				++m_topItemNum;
			}
			done = true;
			break;
		case CMD_UP:
		case CMD_MOUSE_UP:
			if(m_curItemNum > 0) {
				--m_curItemNum;
				if(m_curItemNum <= m_topItemNum && m_topItemNum >0)
					--m_topItemNum;
			} else if(m_curItemNum == 0) {
				m_curItemNum = m_lastItemNum;
				m_topItemNum = m_curItemNum - m_numPerScreen+1;			

			}
			done = true;
			break;
		case CMD_LEFT:
			if(m_curItemNum -m_numPerScreen >= 0) {
				m_curItemNum -= m_numPerScreen;
				m_topItemNum -= m_numPerScreen;
			} else {
				m_curItemNum = m_topItemNum = 0;
			}
			done = true;
			break;
		case CMD_RIGHT:
			if(m_lastItemNum-m_curItemNum >= m_numPerScreen) {
				m_curItemNum += m_numPerScreen;
				m_topItemNum += m_numPerScreen;
			} else {
				m_curItemNum = m_topItemNum = m_lastItemNum;
			}
			done = true;
			break;
	}	
	//if(done)
		//updateDisplayList();
	return done;
}

int Scroller::skipVal()
{
	return m_skipVal;
}
	
void Scroller::updateDisplayList()
{
	int numProcessed = 0;
	int numDisplayed = 0;
	int type;
	for(displayList_t::iterator dlIter = m_displayList.begin();
		dlIter != m_displayList.end(); 
		++dlIter) {
		SDL_FreeSurface((*dlIter).first);
	}
	m_displayList.clear();
	for(listing_t::iterator vIter = m_listing.begin();
		vIter != m_listing.end() && (numDisplayed <= m_numPerScreen);
		++vIter) {
		if(numProcessed >= m_topItemNum) {
			if(numProcessed == m_topItemNum) 
				m_displayListOffset = numProcessed;
			if(numProcessed == m_curItemNum) {
				m_curItemName = (*vIter).first;
				m_curItemType = (*vIter).second;
				m_displayList.push_back(make_pair(TTF_RenderUTF8_Blended(m_font, (*vIter).first.c_str(), m_curItemColor), (*vIter).second));
			} else {
				m_displayList.push_back(make_pair(TTF_RenderUTF8_Blended(m_font, (*vIter).first.c_str(), m_itemColor), (*vIter).second));
			}
			++numDisplayed;
		}
		++numProcessed;
	}
}
/*
void Scroller::draw(bool drawIcons, int nowPlaying, int lastQueued) 
{
	int numProcessed = 0;
	int numDisplayed = m_displayList.size();
	int curItemNum = m_curItemNum - m_displayListOffset;
	int type;
	for(displayList_t::iterator dlIter = m_displayList.begin();
		dlIter != m_displayList.end(); 
		++dlIter) {

		if(numProcessed == m_curItemNum) {
cout << "m_curitem " << m_curItemNum << "  disploff  " << m_displayListOffset << endl;
			m_curItemClearRect.w = m_clearRect.w;
			m_curItemClearRect.h = (*dlIter).first->h;
			SDL_SetClipRect(m_screen, &m_curItemClearRect);
			SDL_BlitSurface(m_bgCurItem, NULL, m_screen, &m_curItemClearRect );
		} else if(numProcessed == nowPlaying || (lastQueued != -1 && numProcessed >= nowPlaying && numProcessed <= lastQueued)) {
			m_curItemClearRect.w = m_clearRect.w;
			m_curItemClearRect.h = (*dlIter).first->h;
			SDL_SetClipRect(m_screen, &m_curItemClearRect);
			SDL_BlitSurface(m_bgNowPlaying, NULL, m_screen, &m_curItemClearRect );
		} else {
			m_curItemClearRect.w = m_clearRect.w;
			m_curItemClearRect.h = (*dlIter).first->h;
		}

		SDL_SetClipRect(m_screen, &m_curItemClearRect);
		if(drawIcons) {
			type = (*dlIter).second;
			if(type == 0 || type == 2 || type == 3) {
				SDL_BlitSurface(m_iconFolder, NULL, m_screen, &m_curItemIconRect );
			} else if (type == 4) {
				SDL_BlitSurface(m_iconFilter, NULL, m_screen, &m_curItemIconRect );
			}
			else if(type == 1)
				SDL_BlitSurface(m_iconFile, NULL, m_screen, &m_curItemIconRect );
		}	
		SDL_BlitSurface((*dlIter).first,NULL, m_screen, &m_destRect );
		m_destRect.y += m_skipVal;
		m_curItemClearRect.y += m_skipVal;
		m_curItemIconRect.y += m_skipVal;
		++numProcessed;
	}
	m_destRect.y = m_origY;
	m_curItemClearRect.y = m_origY;
	m_curItemIconRect.y = m_origY;

	if(numDisplayed < m_listing.size()) {	
		if(m_topItemNum != 0) {
			SDL_SetClipRect(m_screen, &m_upClearRect);
			SDL_BlitSurface(m_upBtn, NULL, m_screen, &m_upClearRect );
		}
		if(m_topItemNum <= m_lastItemNum - numDisplayed) {
			SDL_SetClipRect(m_screen, &m_downClearRect);
			SDL_BlitSurface(m_downBtn, NULL, m_screen, &m_downClearRect );	
		}
	}	
	if(m_curState == MPD_STATUS_STATE_PAUSE) {
		SDL_Rect dstrect;
		dstrect.x = (m_screen->w - m_pauseBtn->w) / 2;
		dstrect.y = (m_screen->h - m_pauseBtn->h) / 2;
		dstrect.w = m_pauseBtn->w;
		dstrect.h = m_pauseBtn->h;

		SDL_SetClipRect(m_screen, &dstrect);
		SDL_BlitSurface(m_pauseBtn, NULL, m_screen, &dstrect );
	}
}
*/

void Scroller::draw(long timePerFrame, bool drawIcons, int nowPlaying, int lastQueued) 
{
	if(!m_goodS)
		initAllS();
	if(timePerFrame > 0) 
		m_delay = 1000000 /(15 * timePerFrame) ;

//	if(m_delayCnt > m_delay) {

	SDL_Surface *sText;
	int numProcessed = 0;
	int numDisplayed = 0;
	string str;
	int type;
	for(listing_t::iterator vIter = m_listing.begin();
		vIter != m_listing.end() && (numDisplayed <= m_numPerScreen);
		++vIter) {
		if(numProcessed >= m_topItemNum) {
			str = (*vIter).first;

			if(numProcessed == m_curItemNum) {
				sText = TTF_RenderUTF8_Blended(m_font, str.c_str(), m_curItemColor);
				m_curItemClearRect.w = m_clearRect.w;
				m_curItemClearRect.h = sText->h;
				SDL_SetClipRect(m_screen, &m_curItemClearRect);
				SDL_BlitSurface(m_bgCurItem, NULL, m_screen, &m_curItemClearRect );
				m_curItemName = (*vIter).first;
				m_curItemType = (*vIter).second;
			} else if(numProcessed == nowPlaying || (lastQueued != -1 && numProcessed >= nowPlaying && numProcessed <= lastQueued)) {
				sText = TTF_RenderUTF8_Blended(m_font, str.c_str(), m_itemColor);
				m_curItemClearRect.w = m_clearRect.w;
				m_curItemClearRect.h = sText->h;
				SDL_SetClipRect(m_screen, &m_curItemClearRect);
				SDL_BlitSurface(m_bgNowPlaying, NULL, m_screen, &m_curItemClearRect );
			} else {
				sText = TTF_RenderUTF8_Blended(m_font, str.c_str(), m_itemColor);
				
				m_curItemClearRect.w = m_clearRect.w;
				m_curItemClearRect.h = sText->h;
			}

			SDL_SetClipRect(m_screen, &m_curItemClearRect);
			if(drawIcons) {
				type = (*vIter).second;
				if(type == 0 || type == 2 || type == 3) {
					SDL_BlitSurface(m_iconFolder, NULL, m_screen, &m_curItemIconRect );
				} else if (type == 4) {
					SDL_BlitSurface(m_iconFilter, NULL, m_screen, &m_curItemIconRect );
				}
				else if(type == 1)
					SDL_BlitSurface(m_iconFile, NULL, m_screen, &m_curItemIconRect );
			}	
			SDL_BlitSurface(sText,NULL, m_screen, &m_destRect );
			SDL_FreeSurface(sText);
			m_destRect.y += m_skipVal;
			m_curItemClearRect.y += m_skipVal;
			m_curItemIconRect.y += m_skipVal;
			++numDisplayed;
		}
		++numProcessed;
	}
	m_destRect.y = m_origY;
	m_curItemClearRect.y = m_origY;
	m_curItemIconRect.y = m_origY;

	if(numDisplayed < m_listing.size()) {	
		if(m_topItemNum != 0) {
			SDL_SetClipRect(m_screen, &m_upClearRect);
			SDL_BlitSurface(m_upBtn, NULL, m_screen, &m_upClearRect );
		}
		if(m_topItemNum <= m_lastItemNum - numDisplayed) {
			SDL_SetClipRect(m_screen, &m_downClearRect);
			SDL_BlitSurface(m_downBtn, NULL, m_screen, &m_downClearRect );	
		}
	}	
	if(m_curState == MPD_STATUS_STATE_PAUSE) {
		SDL_Rect dstrect;
		dstrect.x = (m_screen->w - m_pauseBtn->w) / 2;
		dstrect.y = (m_screen->h - m_pauseBtn->h) / 2;
		dstrect.w = m_pauseBtn->w;
		dstrect.h = m_pauseBtn->h;

		SDL_SetClipRect(m_screen, &dstrect);
		SDL_BlitSurface(m_pauseBtn, NULL, m_screen, &dstrect );
	}
	m_delayCnt = 0;
	m_refresh = true;
//	}
	++m_delayCnt;
	m_refresh = true;
}

void Scroller::draw(vector<string>& selectedOptions) 
{
	if(!m_goodS)
		initAllS();
	SDL_Surface *sText;
	int numProcessed = 0;
	int numDisplayed = 0;
	int amount = 50;	
	int width = m_curItemClearRect.w;
	for(listing_t::iterator vIter = m_listing.begin();
		vIter != m_listing.end() && (numDisplayed <= m_numPerScreen);
		++vIter) {
		if(numProcessed >= m_topItemNum) {
			string str = (*vIter).first;
			if(numProcessed < selectedOptions.size()) 
				str +=":";
			if(numProcessed == m_curItemNum) {
				sText = TTF_RenderUTF8_Blended(m_font, str.c_str(), m_curItemColor);
				m_curItemClearRect.w = m_clearRect.w;
				m_curItemClearRect.h = sText->h;
				SDL_SetClipRect(m_screen, &m_curItemClearRect);
				SDL_BlitSurface(m_bgCurItem, NULL, m_screen, &m_curItemClearRect );
				m_curItemName = (*vIter).first;
				m_curItemType = (*vIter).second;
			} else {
				sText = TTF_RenderUTF8_Blended(m_font, str.c_str(), m_itemColor);
				m_curItemClearRect.w = sText->w;
				m_curItemClearRect.h = sText->h;
			}

			SDL_SetClipRect(m_screen, &m_curItemClearRect);
			SDL_BlitSurface(sText,NULL, m_screen, &m_curItemClearRect );
			m_curItemClearRect.w = width;

			
			SDL_FreeSurface(sText);
			if(numProcessed < selectedOptions.size()) {
				m_destRect.y += m_skipVal;
				m_curItemClearRect.y += m_skipVal;
				if(numProcessed == m_curItemNum) {
				SDL_SetClipRect(m_screen, &m_curItemClearRect);
				SDL_BlitSurface(m_bgCurItem, NULL, m_screen, &m_curItemClearRect );
					sText = TTF_RenderUTF8_Blended(m_font, selectedOptions[numProcessed].c_str(), m_curItemColor);
				}
				else
					sText = TTF_RenderUTF8_Blended(m_font, selectedOptions[numProcessed].c_str(), m_itemColor);
				m_destRect.x += amount;	
				m_destRect.w = sText->w;
				m_destRect.h = sText->h;
				SDL_SetClipRect(m_screen, &m_destRect);
				SDL_BlitSurface(sText,NULL, m_screen, &m_destRect );

				m_destRect.x -= amount;	
				SDL_FreeSurface(sText);
			} else if(numProcessed < selectedOptions.size()+3) {
				m_destRect.y += m_skipVal;
				m_curItemClearRect.y += m_skipVal;
				if(numProcessed == m_curItemNum) {
				SDL_SetClipRect(m_screen, &m_curItemClearRect);
				SDL_BlitSurface(m_bgCurItem, NULL, m_screen, &m_curItemClearRect );
					sText = TTF_RenderUTF8_Blended(m_font, " ", m_curItemColor);
				}
				else
					sText = TTF_RenderUTF8_Blended(m_font, " ", m_itemColor);
				m_destRect.x += amount;	
				m_destRect.w = sText->w;
				m_destRect.h = sText->h;
				SDL_SetClipRect(m_screen, &m_destRect);
				SDL_BlitSurface(sText,NULL, m_screen, &m_destRect );

				m_destRect.x -= amount;	
				SDL_FreeSurface(sText);
			}
			m_destRect.y += m_skipVal;
			m_curItemClearRect.y += m_skipVal;
			++numDisplayed;
		}
			++numProcessed;
	}
	m_destRect.y = m_origY;
	m_curItemClearRect.y = m_origY;

}

void Scroller::makeCurItemVisible()
{
	if(m_curItemNum < m_topItemNum ||
		m_curItemNum > m_topItemNum + m_numPerScreen) {

		m_topItemNum  = m_curItemNum - (m_numPerScreen/2);
	}

}

