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

#ifndef __PLBROWSER_H__
#define __PLBROWSER_H__

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include "libmpdclient.h"
#include "scroller.h"

class Playlist;
class Config;
class GuiPos;
class Keyboard;

class PLBrowser : public Scroller
{
public:
	typedef std::vector<std::pair<std::string, int> >listing_t;
	PLBrowser(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface * bg, TTF_Font* font,
			SDL_Rect& rect, Config& config, int skipVal, int numPerScreen, Playlist& pl, Keyboard& kb);
	~PLBrowser();
    void ls(std::string dir="");

	void updateListing();
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus);
	int processCommand(int command, int curMode, GuiPos& guiPos);
    void draw(bool force, long timePerFrame, bool inBack);
	std::string currentItemName();
	std::string currentItemPath();

protected:
	void initAll();
	bool m_good;
	std::string m_curDir;

	Playlist& m_playlist;
	Keyboard& m_keyboard;
	SDL_Surface *m_plSurfaceText;
};

#endif
