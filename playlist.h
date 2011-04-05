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

#ifndef __PLAYLIST_H__
#define __PLAYLIST_H__

#include <vector>
#include <string>
#include <fstream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "config.h"
#include "libmpdclient.h"
#include "scroller.h"
#include "timer.h"

class Popup;
class GuiPos;

class Playlist : public Scroller
{
public:
	typedef std::vector<std::pair<std::string, int> >listing_t;

	Playlist(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg,  TTF_Font* font, Config& config, SDL_Rect& rect, int skipVal, int numPerScreen);
	void load(std::string dir);
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus, 
							int rtmpdStatusChanged, mpd_Status* rtmpdStatus, int repeatDelay);
    int processCommand(int event, int& rtmpdStatusChanged, mpd_Status* rtmpdStatus, int repeatDelay, int volume, long delayTime, GuiPos& guiPos);
    void draw(bool force, long timePerFrame, bool inBack);
	std::string currentItemName();
	std::string currentItemPath();

	void makeNowPlayingVisible();
	bool showSaveDialog(Popup& popup, std::string name);
	void setNextNumOnSave(std::string name);
	int getRand(int max);
	void initRandomPlaylist();
	void initNewPlaylist();
	virtual void initItemIndexLookup();
	void initName(std::string name);
	void save();
	std::string nowPlayingTitle(int song=-1);
	std::string nowPlayingArtist(int song=-1);
	std::string nowPlayingFile(int song=-1);
	std::string nowPlayingFormat(int song=-1);
	void nowPlaying(int song);

	void lastQueued(int pos) { m_lastQueued = pos; }
	int lastQueued() { return m_lastQueued; }

	int getPlayingState() {return m_curState;}
protected:
	void initAll();
	bool m_good;
	std::string m_curDir;

	int m_view;
	int m_curElapsed;
	int m_nowPlaying;
	int m_random;
	bool m_otg;
	bool m_modified;	
	std::vector<std::string> m_all;	
	std::string m_name;
	int m_id;
	int m_moveFrom;
	int m_moveTo;

	int m_lastQueued;
	std::vector<int> m_used;
	Timer m_timer;
	typedef struct { 
		std::string title;
		std::string artist;
		std::string file;
	} songInfo_t;

	std::vector<songInfo_t> m_songsInfo;
};

#endif
	
