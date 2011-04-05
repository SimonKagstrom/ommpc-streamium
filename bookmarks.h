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

#ifndef __BOOKMARKS_H__
#define __BOOKMARKS_H__

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include "libmpdclient.h"
#include "scroller.h"

class Playlist;
class Config;
class StatsBar;
class GuiPos;
class Keyboard;
class ButtonManager;

class Bookmarks : public Scroller
{
public:
	typedef std::vector<std::pair<std::string, int> >listing_t;
    Bookmarks(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, TTF_Font* font, SDL_Rect& rect, int skipVal, int numPerScreen, Playlist& pl, Config& config, StatsBar& sb, ButtonManager& bm, bool csb, Keyboard& kb);
    void ls(std::string dir="");

	void updateListing();
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus);
	int processCommand(int command, GuiPos& guiPos);
	void draw(bool forceRefresh, long timePerFrame, bool inBack);
	std::string currentItemName();
	std::string currentItemPath();

	void doSave();
protected:
	void initAll();
	bool m_good;
	std::string m_curDir;

	int m_nowPlaying;
	Playlist& m_playlist;
	StatsBar& m_sb;
	ButtonManager& m_bm;
	Keyboard& m_keyboard;
	bool m_classicStatsBar;
};

#endif
