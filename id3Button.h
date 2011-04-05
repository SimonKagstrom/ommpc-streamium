/*****************************************************************************************

ommpc(One More Music Player Client) - A Music Player Daemon client targetted for the gp2x

Copyright (C) 2007-2008 - Tim Temple(codertimt@gmail.com)

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

#ifndef __ID3BUTTON_H__
#define __ID3BUTTON_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

#include "libmpdclient.h"
#include "config.h"
#include <vector>
#include "button.h"

class GuiPos;

class ID3Button : public Button
{
public:
	ID3Button(std::string label, std::string id, TTF_Font* font);
	void init(Config& config);
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
					  int rtmpdStatusChanged, mpd_Status* rtmpdStatus, 
					  bool forceRefresh);
	int processCommand(int command, GuiPos& guiPos);
	virtual void draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh);
	std::string testCall() { return m_label; };
	void visible(bool vis) {m_visible = vis;};
	bool visible() {return m_visible;};
	void setText(std::string text);
protected:
	
	bool m_visible;
	std::string m_label;
	std::string m_id;
	TTF_Font* m_font;	
	SDL_Color m_textColor;
	SDL_Surface* m_sText;
	SDL_Rect m_destRectB;
	bool m_displayText;
};

#endif
