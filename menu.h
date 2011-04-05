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

#ifndef __MENU_H__
#define __MENU_H__

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include "libmpdclient.h"
#include "menuButton.h"
#include "scroller.h"

class Config;
class GuiPos;
class SongDb;
class Keyboard;
class Playlist;

class Menu
{
public:
 	enum {TYPE_FOLDER, TYPE_FILE, TYPE_BACK, TYPE_ALL, TYPE_FILTER};
    Menu(mpd_Connection* mpd, SDL_Surface* bg, SDL_Surface* screen, TTF_Font* font, SDL_Rect& rect, Config& config, int skipVal, int numPerScreen, SongDb& songdb, Keyboard& kb, Playlist& pl);
	
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus, bool updatingSongDb);
	int processCommand(int command, GuiPos& guiPos);
	void draw(bool forceRefresh, long timePerFrame, bool inBack);

protected:
	void initAll();
	void initItems(int command);
	void switchMenu(int command);
	SDL_Surface * m_iconBrowse;
	SongDb& m_songDb;
	Keyboard& m_keyboard;
	Playlist& m_pl;
	bool m_drawIcons;
	std::vector<MenuButton> m_buttonsMenu;
	std::vector<MenuButton> m_buttonsSet;
	std::vector<MenuButton> m_buttonsExit;
	std::vector<MenuButton> m_buttons;
	int m_view;
	int m_menu1Active;
	int m_menu2Active;
	int m_numPerRow;
	int m_rowHeight;
	int m_colWidth;
	int m_2ndRowOffset;
	int m_1stRowOffset2;
	int m_1stRowOffset3;
	int m_ySize1;
	int m_ySize2;
	int m_ySize3;
	TTF_Font* m_font;	
	mpd_Connection* m_mpd;

	SDL_Surface* m_screen;
	SDL_Rect m_destRect;
	SDL_Rect& m_clearRect;
	Config& m_config;
	
	int m_skipVal;
	int m_numPerScreen;
	SDL_Color m_itemColor;
	SDL_Color m_curItemColor;
	bool m_refresh;
	int m_curState;
	SDL_Surface * m_bg;
	bool m_good;
};

#endif
