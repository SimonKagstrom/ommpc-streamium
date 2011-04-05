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

#ifndef __ARTBUTTON_H__
#define __ARTBUTTON_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

#include "libmpdclient.h"
#include "config.h"
#include "threadParms.h"
#include <vector>
#include "button.h"

class GuiPos;
class Config;
class ID3Button;

class ArtButton : public Button
{
public:
	ArtButton(artThreadParms_t& artParms, std::string label, std::string id);
	virtual void init(Config& config, int command);

	bool updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
					  int rtmpdStatusChanged, mpd_Status* rtmpdStatus, 
					  bool forceRefresh, mpd_Connection* mpd, Config& config);
	int processCommand(int command, GuiPos& guiPos);
	bool draw2(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh);
	std::string testCall() { return m_label; };
	void active(bool act) {m_active = act;};
	bool active() {return m_active;};
protected:
	
	bool m_active;
	std::string m_label;
	std::string m_id;
	int m_command;
	TTF_Font* m_font;	
	SDL_Color m_textColor;
	SDL_Color m_itemColor;
	SDL_Surface* m_sText;
	bool m_displayText;
	artThreadParms_t& m_artParms;
	bool m_showInfo;
	SDL_Rect m_moveRect;
	SDL_Rect m_mouseRect;
	bool m_animate;
	//Both the artbutton and the buttonmanager can hold any or all of our id3buttons.
	//So I think we will create a bunch of pointers to id3buttons and initialize what
	//the skin asks for and then check that button is not null when we would access it
	ID3Button * m_titleBtn;
	ID3Button * m_artistBtn;
	ID3Button * m_albumBtn;
	ID3Button * m_genreBtn;
	ID3Button * m_trackBtn;
	ID3Button * m_typeBtn;
	ID3Button * m_dateBtn;
	ID3Button * m_composerBtn;
	ID3Button * m_discBtn;
	ID3Button * m_performerBtn;
	ID3Button * m_commentBtn;
	//bitrate found in status and not with song info
	ID3Button * m_rateBtn;
	//samplerate found in status and not with song info
	ID3Button * m_freqBtn;

	
};

#endif
