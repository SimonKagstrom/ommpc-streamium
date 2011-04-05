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

#ifndef __mySETTINGS_H__
#define __mySETTINGS_H__

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include "libmpdclient.h"
#include "scroller.h"

class GP2XRegs;
class Config;
class GuiPos;
class Keyboard;

class PlayerSettings : public Scroller
{
public:
	typedef std::vector<std::vector<std::string> > optionsText_t;
	typedef std::vector<std::vector<std::string>::iterator > optionsTextIters_t;
	typedef std::vector<std::pair<std::string, int> >listing_t;
	PlayerSettings(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface * bg, TTF_Font* font,
			SDL_Rect& rect, Config& config, int skipVal, int numPerScreen, GP2XRegs& gp2xRegs, Keyboard& kb);
	~PlayerSettings();

	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus);
	int processCommand(int command, GuiPos& guiPos);
    void draw(bool force, long timePerFrame, bool inBack);
	
	void setItemsText(Scroller::listing_t& items, int type);
	void setOptionsText();
	void saveOptions();
	int selectedAction();
	virtual void initItemIndexLookup();
	void updateMpdConf();
	std::string getSelOptionText();
	


protected:
	void initAll();
	bool m_good;
	GP2XRegs& m_gp2xRegs;
	std::string m_curDir;
	
	//for options menu
	optionsText_t m_optionsText;
	optionsTextIters_t m_optionsIters;
	std::vector<int> m_selectedIndexes;
	std::vector<std::string> m_selectedOptions;

	Keyboard& m_keyboard;
	SDL_Surface *m_plSurfaceText;
};

#endif
