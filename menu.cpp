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

#include "menu.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "config.h"
#include "guiPos.h"
#include "songDb.h"
#include "keyboard.h"
#include "playlist.h"
#include <iostream>
#include <stdexcept>
#include <SDL_image.h>

using namespace std;

Menu::Menu(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, TTF_Font* font, 
				SDL_Rect& rect,	Config& config, int skipVal, int numPerScreen, SongDb& songdb, Keyboard& kb, Playlist& pl)
: m_songDb(songdb)
, m_keyboard(kb)
, m_pl(pl)
, m_menu1Active(0)
, m_menu2Active(0)
, m_view(0)
, m_config(config)
, m_destRect(rect)
, m_clearRect(rect)
, m_screen(screen)
, m_bg(bg)
, m_mpd(mpd)
, m_skipVal(skipVal)
, m_numPerScreen(numPerScreen-1)
, m_good(false)
{
}

void Menu::initAll()
{
	m_config.getItemAsColor("sk_main_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_main_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);
   
	int width = m_config.getItemAsNum("sk_main_width"); 
	int height = m_config.getItemAsNum("sk_main_height");
	m_numPerRow = m_config.getItemAsNum("sk_menu_numPerRow");
	m_rowHeight = m_config.getItemAsNum("sk_menu_rowHeight");
	m_colWidth = m_config.getItemAsNum("sk_menu_colWidth");


	//CHANGE HERE WHEN ADDING ITEMS, right now it's 2 items
	m_2ndRowOffset = (width-(m_colWidth*3))/2;
	m_1stRowOffset2 = (width-(m_colWidth*3))/2;
	m_1stRowOffset3 = (width-(m_colWidth*3))/2;

	m_ySize1 = (height-(m_rowHeight*2))/2;
	m_ySize2 = (height-(m_rowHeight))/2;
	m_ySize3 = (height-(m_rowHeight))/2;
 
	m_drawIcons =  m_config.getItemAsNum("drawIcons");

	initItems(CMD_SHOW_MENU);
	m_good = true;
}

void Menu::switchMenu(int command)
{
	switch(command) {
		case CMD_SHOW_MENU:
				m_view = 0;
			m_buttons = m_buttonsMenu;
			m_buttons[m_menu1Active].active(true);
			break;
		case CMD_MENU_SETTINGS:
				m_view = 1;
			m_buttons = m_buttonsSet;
				m_buttons[m_menu2Active].active(true);
			break;
		case CMD_MENU_EXIT:
				m_view = 1;
			m_buttons = m_buttonsExit;
				m_buttons[m_menu2Active].active(true);
			break;
	}
}
void Menu::initItems(int command)
{
	//m_buttons.clear();
	//switch(command) {
	//	case CMD_SHOW_MENU:
	m_font = TTF_OpenFont(m_config.getItem("sk_font_menu").c_str(),
						  m_config.getItemAsNum("sk_font_menu_size") );
			{
				m_view = 0;
				int xOffset = 0;
				int yOffset = m_ySize1;
				MenuButton butt(m_config.getItem("LANG_MENU_NP"), "np", m_font);
				butt.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_SHOW_NP, m_colWidth, m_rowHeight);
				m_buttonsMenu.push_back(butt);

				MenuButton butt1(m_config.getItem("LANG_MENU_PL"), "cur_pl", m_font);
				xOffset += m_colWidth;
				butt1.init(m_config, m_destRect.x+xOffset, m_destRect.y+yOffset, 
								"np", CMD_SHOW_PL, m_colWidth, m_rowHeight);
				m_buttonsMenu.push_back(butt1);

				MenuButton butt2(m_config.getItem("LANG_MENU_LIB"), "lib", m_font);
				xOffset += m_colWidth;
				butt2.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, 
						  		"np", CMD_SHOW_LIB, m_colWidth, m_rowHeight);
				m_buttonsMenu.push_back(butt2);

				MenuButton butt3(m_config.getItem("LANG_MENU_PLS"), "pls", m_font);
				xOffset += m_colWidth;
				butt3.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, 
							"np", CMD_SHOW_PLS, m_colWidth, m_rowHeight); 
				m_buttonsMenu.push_back(butt3);

				xOffset = m_2ndRowOffset;
				yOffset += m_rowHeight;
				MenuButton butt4(m_config.getItem("LANG_MENU_BKMRKS"), "bkmrks", m_font);
				butt4.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, 
							"np", CMD_SHOW_BKMRKS, m_colWidth, m_rowHeight);
				m_buttonsMenu.push_back(butt4);

				xOffset += m_colWidth;
				MenuButton butt5(m_config.getItem("LANG_MENU_SET"), "settings", m_font);
				butt5.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_MENU_SETTINGS, m_colWidth, m_rowHeight);
				m_buttonsMenu.push_back(butt5);
				
				xOffset += m_colWidth;
				MenuButton butt6(m_config.getItem("LANG_MENU_EXIT"), "exit", m_font);
				butt6.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_MENU_EXIT, m_colWidth, m_rowHeight);
				m_buttonsMenu.push_back(butt6);

			}
	//		break;
	//	case CMD_MENU_SETTINGS:
			{
				m_view = 1;
				int xOffset = m_1stRowOffset2;
				int yOffset = m_ySize2;
				MenuButton butt0(m_config.getItem("LANG_MENU_MAIN"),  "main", m_font);
				butt0.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_SHOW_MENU, m_colWidth, m_rowHeight);
				m_buttonsSet.push_back(butt0);

				xOffset += m_colWidth;
				MenuButton butt1(m_config.getItem("LANG_MENU_UPDATE"), "update", m_font);
				butt1.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_MPD_UPDATE, m_colWidth, m_rowHeight);
				m_buttonsSet.push_back(butt1);

				MenuButton butt2(m_config.getItem("LANG_MENU_OPTIONS"), "options", m_font);
				xOffset += m_colWidth;
				butt2.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_SHOW_OPTIONS, m_colWidth, m_rowHeight);
				m_buttonsSet.push_back(butt2);

			}
	//		break;
	//	case CMD_MENU_EXIT:
			{
				m_view = 1;
				int xOffset = m_1stRowOffset3;
				int yOffset = m_ySize3;
				MenuButton butt0(m_config.getItem("LANG_MENU_MAIN"), "main", m_font);
				butt0.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_SHOW_MENU, m_colWidth, m_rowHeight);
				m_buttonsExit.push_back(butt0);

				xOffset += m_colWidth;
				MenuButton butt1(m_config.getItem("LANG_MENU_DETACH"),  "detach", m_font);
				butt1.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_DETACH, m_colWidth, m_rowHeight); 
				m_buttonsExit.push_back(butt1); 
				MenuButton butt2(m_config.getItem("LANG_MENU_EXIT"), "exit", m_font);
				xOffset += m_colWidth;
				butt2.init(m_config, m_destRect.x+xOffset,m_destRect.y+yOffset, "np", CMD_QUIT, m_colWidth, m_rowHeight);
				m_buttonsExit.push_back(butt2);

			}
	//		break;
	//	}
	
	switchMenu(command);
}

void Menu::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus, bool updatingSongDb)
{
	if(mpdStatusChanged & STATE_CHG) { 
		m_curState = mpdStatus->state;
		m_refresh = true;
	}
}

int Menu::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	bool done = false;
	int curItem = 0;
	if(command > 0) {
		for(vector<MenuButton>::iterator bIter = m_buttons.begin();
			bIter != m_buttons.end() && !done;
			++bIter) {
			rCommand = (*bIter).processCommand(command, guiPos);
			if(rCommand != command) {
				done = true;
				if(command == CMD_CLICK) {
					if(m_view == 0) {
						m_buttons[m_menu1Active].active(false);
						m_menu1Active = curItem;
					} else {
						m_buttons[m_menu2Active].active(false);
						m_menu2Active = curItem;
					}
					m_refresh = true;
				} else if(command == CMD_HOLD_CLICK) {
					
				}
			}
			++curItem;
		}
		
		if(m_view == 0) 
			curItem = m_menu1Active;
		else
			curItem = m_menu2Active;	
	
		switch(command) {
			case CMD_DOWN:
				m_buttons[curItem].active(false);
				if(curItem < m_numPerRow && m_buttons.size() > m_numPerRow)
					curItem = m_numPerRow;
				m_buttons[curItem].active(true);
				m_refresh = true;
				break;
			case CMD_UP:
				m_buttons[curItem].active(false);
				if(curItem >= m_numPerRow)
					curItem = 0;
				m_buttons[curItem].active(true);
				m_refresh = true;
				break;
			case CMD_LEFT:
				m_buttons[curItem].active(false);
				if(curItem -1 >= 0)
					curItem--;
				m_buttons[curItem].active(true);
				m_refresh = true;
				break;
			case CMD_RIGHT:
				m_buttons[curItem].active(false);
				if(curItem+1 < m_buttons.size())
					curItem++;
				m_buttons[curItem].active(true);
				m_refresh = true;
				break;
		}	
		if(m_view == 0) 
			m_menu1Active = curItem;
		else
			m_menu2Active = curItem;

		
		switch(rCommand) {
			case CMD_SHOW_MENU:
			case CMD_MENU_SETTINGS:
			case CMD_MENU_EXIT:
				switchMenu(rCommand);
				m_refresh = true;
				break;
		}
	}
	return rCommand;
}

void Menu::draw(bool forceRefresh, long timePerFrame, bool inBack)
{
	if(!m_good)
		initAll();
	if(forceRefresh || (!inBack && m_refresh)) {
		//clear this portion of the screen 
		SDL_SetClipRect(m_screen, &m_clearRect);
		SDL_BlitSurface(m_bg, &m_clearRect, m_screen, &m_clearRect );
		SDL_Surface *sText;
		for(vector<MenuButton>::iterator bIter = m_buttons.begin();
			bIter != m_buttons.end();
			++bIter) {
			(*bIter).draw(m_screen, m_bg, forceRefresh||m_refresh);
		}
		m_refresh = false;
	}
	
}
