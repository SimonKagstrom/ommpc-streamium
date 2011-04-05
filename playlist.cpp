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

#include "playlist.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "popup.h"
#include "timestamp.h"
#include "guiPos.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <time.h>
#include <SDL_image.h>

using namespace std;
#define X2DELAY 2000000
#define X4DELAY 6000000
#define X8DELAY 12000000
#define X16DELAY 25000000
#define X32DELAY 45000000
#define FFWAIT 400000

Playlist::Playlist(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, TTF_Font* font, Config& config, SDL_Rect& rect, int skipVal, int numPerScreen)
: Scroller(mpd, screen, bg, font, rect, config, skipVal, numPerScreen)
, m_curElapsed(0)
, m_view(0)
, m_random(false)
, m_otg(false)
, m_modified(false)
, m_name("")
, m_moveTo(-1)
, m_moveFrom(-1)
, m_lastQueued(-1)
, m_good(false)
{
}

void Playlist::initAll()
{
	m_origY = m_destRect.y;
	m_config.getItemAsColor("sk_main_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_main_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);
	
	string skinName = m_config.getItem("skin");
	SDL_Surface* tmpSurface = NULL;	
	tmpSurface = IMG_Load(string("skins/"+skinName+"/bg_nowPlaying.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("skins/default/bg_nowPlaying.png").c_str());
	if (!tmpSurface)
		printf("Unable to load image: %s\n", SDL_GetError());
	else { 
		m_bgNowPlaying = SDL_DisplayFormatAlpha(tmpSurface);
		SDL_FreeSurface(tmpSurface);
	}

	initItemIndexLookup();
	// centre the bitmap on screen
	load("");
	
	m_good = true;	
}

void Playlist::initItemIndexLookup() {
	int startPos = m_clearRect.y;
	int skipCount = 0;
	int index = 0;	
//cout <<  "skipval " << m_skipVal << "  startPos  " << startPos << endl;
	for(int i=0; i<m_clearRect.y; ++i) {
		m_itemIndexLookup.push_back(0);
	}
	for(int i=m_clearRect.y; i<(m_clearRect.h+m_clearRect.y); ++i) {
		m_itemIndexLookup.push_back(index);
		if(skipCount >= m_skipVal) {
			++index;
			skipCount = 0;
//		cout << "i " << i << "   index  " << index << endl;
		}
		++skipCount;
	}
}

void Playlist::load(std::string dir)
{
	if(m_mpd != NULL) {
		if(!dir.empty()) { //load playlist from disk
		}
		mpd_sendPlaylistInfoCommand(m_mpd, -1);
		//m_path = dir;
		m_listing.clear();	
		m_songsInfo.clear();
		mpd_InfoEntity* mpdItem = mpd_getNextInfoEntity(m_mpd);
		while(mpdItem != NULL) {
			std::string item = "";
			int type = mpdItem->type;
			if(type == 1) {
				if(m_view == 0 && mpdItem->info.song->title != NULL
						&& mpdItem->info.song->artist != NULL) { //Artist - Title
					item = mpdItem->info.song->title;
					item = " - " + item;
					item = mpdItem->info.song->artist + item;
				} else if(m_view ==1 && mpdItem->info.song->title != NULL) { //Title
					item = mpdItem->info.song->title;
				} else {
					item = mpdItem->info.song->file;
				}
				int pos = item.rfind("/");;
				if(pos != string::npos) {
					item = item.substr(pos+1);
				}
				m_listing.push_back(make_pair(item, type));
				songInfo_t song;
				if(mpdItem->info.song->title != NULL)
					song.title  = mpdItem->info.song->title;
				else 
					song.title  = mpdItem->info.song->file;
				if(mpdItem->info.song->artist != NULL)
					song.artist  = mpdItem->info.song->artist;
				song.file  = mpdItem->info.song->file;
				m_songsInfo.push_back(song);
				mpd_freeInfoEntity(mpdItem);
				mpdItem = mpd_getNextInfoEntity(m_mpd);
			} else {
				throw runtime_error("Unknown mpd entity for playlist");
			}
		}
	}
//	mpd_finishCommand(m_mpd);

	m_lastItemNum = m_listing.size()-1;
}

void Playlist::makeNowPlayingVisible()
{
	if(m_nowPlaying < m_topItemNum ||
		m_nowPlaying > m_topItemNum + m_numPerScreen) {

		m_topItemNum  = m_nowPlaying - (m_numPerScreen/2);
	}

}


bool Playlist::showSaveDialog(Popup& popup, string name)
{
	bool show = false;	
	
	Scroller::listing_t items;
	int type = Popup::POPUP_LIST;
	if(!m_name.empty())
		items.push_back(make_pair(m_name, type));	
			
	items.push_back(make_pair("  "+name, (int)Popup::POPUP_DO_SAVE_PL)); 
	items.push_back(make_pair("  "+m_config.getItem("LANG_CANCEL"), (int)Popup::POPUP_CANCEL)); 
	popup.setItemsText(items, type);
	SDL_Rect popRect;
	popRect.w = 180;
	popRect.h = m_skipVal*5+15;
	popRect.x = (m_screen->w - popRect.w) / 2;
	popRect.y = (m_screen->h - popRect.h) / 2;
	popup.setSize(popRect);
	popup.setTitle("  "+m_config.getItem("LANG_SAVE_PL_AS"));
	show = true;

	return show;
}

void Playlist::setNextNumOnSave(string name)
{
	int num = m_config.getItemAsNum("nextPlaylistNum");
	
	ostringstream numStr;
	numStr << num;
cout << "pl name " << name << endl;
	if(name == "playlist_"+numStr.str()) {
		++num;
		numStr.str("");
		numStr << num;
		m_config.setItem("nextPlaylistNum", numStr.str().c_str());
		m_config.saveConfigFile();
	}
}

int Playlist::getRand(int max)
{
    /* r is a random floating point value in the range [0,1) {including 0, not including 1}. Note we must convert rand() and/or RAND_MAX+1 to floating point values to avoid integer division. In addition, Sean Scanlon pointed out the possibility that RAND_MAX may be the largest positive integer the architecture can represent, so (RAND_MAX+1) may result in an overflow, or more likely the value will end up being the largest negative integer the architecture can represent, so to avoid this we convert RAND_MAX and 1 to doubles before adding. */
	double r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
   /* x is a random integer in the range [0,M) {including 0, not including M}. If M is an integer then the range is [0,M-1] {inclusive} */
    int x = (int)(r * (max));
	return x;
}

void Playlist::initRandomPlaylist()
{
	m_random = true;
	time_t t = time(NULL);
	srand(t);
	mpd_sendListallCommand(m_mpd, "");

	m_listing.clear();	
	m_all.clear();
	mpd_InfoEntity* mpdItem = mpd_getNextInfoEntity(m_mpd);
	int i = 0;
	while(mpdItem != NULL) {
		std::string item = "";
		if(mpdItem->type == 1) {
			item = mpdItem->info.song->file;
			m_all.push_back(item);	
		}	
		mpdItem = mpd_getNextInfoEntity(m_mpd);
	}

	m_used.clear();
	int size = m_all.size();
	bool makeDistinct = false;
	if(size > 65)
		makeDistinct = true;	
	int numAdded = 0;
	while(numAdded < 65) {
		int next = getRand(size);
		if(!makeDistinct || 
				(makeDistinct && find(m_used.begin(), m_used.end(), next) == m_used.end())) {
			mpd_sendAddCommand(m_mpd, m_all[next].c_str());	
			mpd_finishCommand(m_mpd);
			++numAdded;
			m_used.push_back(next);
		}
	}
	m_otg = true;
	load("");
}

void Playlist::initNewPlaylist()
{
	m_otg = true;
}

void Playlist::initName(std::string name)
{
	m_name = name;
}

std::string Playlist::currentItemName()
{
	return m_curItemName;
}
std::string Playlist::currentItemPath()
{
	return m_curDir+m_curItemName;

}

std::string Playlist::nowPlayingTitle(int song)
{
	if(song == -1) {
		song = m_nowPlaying;
	}

	if(song <= m_songsInfo.size() && !m_songsInfo.empty())
		return m_songsInfo[song].title;
	else 
		return "";
}

std::string Playlist::nowPlayingArtist(int song)
{
	if(song == -1) {
		song = m_nowPlaying;
	}

	if(song <= m_songsInfo.size() && !m_songsInfo.empty())
		return m_songsInfo[song].artist;
	else 
		return "";
}

std::string Playlist::nowPlayingFile(int song)
{
	if(song == -1) {
		song = m_nowPlaying;
	}

	if(song <= m_songsInfo.size() && !m_songsInfo.empty())
		return m_songsInfo[song].file;
	else 
		return "";
}

std::string Playlist::nowPlayingFormat(int song)
{
	if(song == -1) {
		song = m_nowPlaying;
	}
	if(song <= m_songsInfo.size() && !m_songsInfo.empty()) {
		string title =  m_songsInfo[song].file;
		int pos = title.rfind('.');
		string ext = title.substr(pos+1);
		if(ext == "mp3")
			ext = " MP3";
		else if(ext == "ogg")
			ext = " OGG";
		else if(ext == "mp4" || ext == "m4p" || ext == "m4a")
			ext = " AAC";
		else if (ext == "flac")
			ext = "FLAC";
		else if (ext.length() == 2)
			ext = "  " + ext;
		else if (ext.length() == 3)
			ext = " " + ext;
		else
			ext = "";

		return ext;
	}
	else 
		return "";
}

void Playlist::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
							int rtmpdStatusChanged, mpd_Status* rtmpdStatus, int repeatDelay)
{
	mpd_Status * status;
	int statusChanged;

	if(rtmpdStatusChanged > 0) {
		status = rtmpdStatus;
		statusChanged = rtmpdStatusChanged;
	} else {
		status = mpdStatus;
		statusChanged = mpdStatusChanged;
	}

	if(statusChanged & PL_CHG) {
		load("");
		m_refresh = true;
	}		
	if((statusChanged & ELAPSED_CHG) && repeatDelay == 0) { 
		m_curElapsed = status->elapsedTime;	
	}
	if(statusChanged & STATE_CHG) { 
		m_curState = status->state;
		m_refresh = true;
	}
	if(statusChanged & SONG_CHG) {
		m_nowPlaying = status->song;	
		m_curItemNum = m_nowPlaying;
		makeNowPlayingVisible();
		m_refresh = true;
/*
		if(m_random) {
			//append new random song
			mpd_sendDeleteCommand(m_mpd, 0);
			mpd_finishCommand(m_mpd);
			int size = m_all.size();
			int next= getRand(size);
			if(m_used.size() >= size)
				m_used.clear();
			while(find(m_used.begin(), m_used.end(), next) != m_used.end()) {
				next= getRand(size);
			}
			mpd_sendAddCommand(m_mpd, m_all[next].c_str());	
			m_used.push_back(next);
			mpd_finishCommand(m_mpd);
		}
*/

	}

}

int Playlist::processCommand(int command, int& rtmpdStatusChanged, mpd_Status* rtmpdStatus, int repeatDelay, int volume, long delayTime, GuiPos& guiPos)
{
	int rCommand = 0;
	if(command > 0) {
		m_refresh = true;
		if(command == CMD_CLICK || command == CMD_HOLD_CLICK) {
			if(guiPos.curY > m_clearRect.y && (guiPos.curY < m_clearRect.y + m_clearRect.h))	 {
				if(guiPos.curX < (m_clearRect.w-40)) {
					m_curItemNum = m_topItemNum + m_itemIndexLookup[guiPos.curY];	
					if(m_curItemNum < m_listing.size()) {
						m_curItemType = m_listing[m_curItemNum].second;
						if(command == CMD_CLICK) {
							command = CMD_PLAY_PAUSE;
						} else {
							rCommand = CMD_POP_CONTEXT;
						}
					} else {
						m_curItemNum =0;
					}
				} else if(guiPos.curX > (m_clearRect.w-40)) {
					if(guiPos.curY < m_clearRect.y+40) {
						command = CMD_LEFT;
					} else if(guiPos.curY > m_clearRect.y + m_clearRect.h -40) {
						command = CMD_RIGHT;

					} /*else {
						if(!m_skipFirstMouse) {	
							if(guiPos.curY > m_prevY) 
								command = CMD_DOWN;
							else if(guiPos.curY < m_prevY)
								command = CMD_UP;
						}
						m_skipFirstMouse = false;
						m_prevX = guiPos.curX;
						m_prevY = guiPos.curY;	
					}*/
				}
			}
		}
		if(command == CMD_HOLD_CLICK) {
			if(guiPos.curX > (m_clearRect.w-40)) {
				if(guiPos.curY < m_clearRect.y+40) {
					command = CMD_LEFT;
				} else if(guiPos.curY > m_clearRect.y + m_clearRect.h -40) {
						command = CMD_RIGHT;
				}
			}
		}
		if(m_moveFrom >= 0 && command != 0 && command != CMD_UP && command
!= CMD_DOWN && command != CMD_MOVE_IN_PL && command != CMD_RIGHT && command
!= CMD_LEFT && command != CMD_HOLD_CLICK) {
			m_moveFrom = -1;
		}	
		if(Scroller::processCommand(command)) {
			//scroller command...parent class processes
		} else {

			switch(command) {
				case CMD_PLAY_PAUSE:
				if(m_curItemType == 1) {
					if(m_curState == MPD_STATUS_STATE_PAUSE && m_nowPlaying == m_curItemNum) {
						m_curState = MPD_STATUS_STATE_PLAY;	
						mpd_sendPauseCommand(m_mpd, 0);
						mpd_finishCommand(m_mpd);
					} else if(m_curState == MPD_STATUS_STATE_PLAY && m_nowPlaying == m_curItemNum) {
						m_curState = MPD_STATUS_STATE_PAUSE;
						mpd_sendPauseCommand(m_mpd, 1);
						mpd_finishCommand(m_mpd);
					} else {
						m_curState = MPD_STATUS_STATE_PLAY;	
						mpd_sendPlayCommand(m_mpd, m_curItemNum);
						mpd_finishCommand(m_mpd);
		//				SDL_Delay(100);
		//				mpd_sendSetvolCommand( m_mpd, volume);
		//				mpd_finishCommand(m_mpd);
					}
				}
				break;
				case CMD_PAUSE:
				if(m_curState == MPD_STATUS_STATE_PAUSE) {
					m_curState = MPD_STATUS_STATE_PLAY;	
					mpd_sendPauseCommand(m_mpd, 0);
					mpd_finishCommand(m_mpd);
				} else if(m_curState == MPD_STATUS_STATE_PLAY) {
					m_curState = MPD_STATUS_STATE_PAUSE;
					mpd_sendPauseCommand(m_mpd, 1);
					mpd_finishCommand(m_mpd);
				}
				break;
				case CMD_NEXT:
				mpd_sendNextCommand(m_mpd);
				mpd_finishCommand(m_mpd);
				break;
				case CMD_PREV:
				mpd_sendPrevCommand(m_mpd);
				mpd_finishCommand(m_mpd);
				break;
				case CMD_FF:
				{ //if(repeatDelay > 0) {
					int jump = 0;
					if(delayTime > X2DELAY) {
						if(delayTime > X32DELAY)
							jump = 64;	
						else if(delayTime > X16DELAY)
							jump = 32;	
						else if(delayTime > X8DELAY)
							jump = 16;	
						else if(delayTime > X4DELAY)
							jump = 8;	
						else
							jump = 4;
					}
					else 	
						jump = 2;

					if(jump > 0) {	
						mpd_sendSeekCommand(m_mpd, m_curItemNum, m_curElapsed + jump);
						mpd_finishCommand(m_mpd);
						m_curElapsed += jump;
					}
				}
				break; 
				case CMD_RW:
				{ //	if(repeatDelay > 0) {
					int jump = 0;
					if(delayTime > X2DELAY) {
						if(delayTime > X32DELAY)
							jump = 64;	
						else if(delayTime > X16DELAY)
							jump = 32;	
						else if(delayTime > X8DELAY)
							jump = 16;	
						else if(delayTime > X4DELAY)
							jump = 8;	
						else
							jump = 4;
					}
					else 	
						jump = 2;

					if(jump > 0) {	
						mpd_sendSeekCommand(m_mpd, m_curItemNum, m_curElapsed - jump);
						mpd_finishCommand(m_mpd);
						m_curElapsed -= jump;
					}
				}
				break;
				case CMD_TOGGLE_VIEW:
				if(m_view == 2)
					m_view = 0;
				else
					++m_view;

				load("");
				break;
				case CMD_DEL_FROM_PL:
				mpd_sendDeleteCommand(m_mpd, m_curItemNum);
				mpd_finishCommand(m_mpd);
				//rtmpdStatusChanged += PL_CHG;
			//	mpd_sendStatusCommand(m_mpd);
				//rtmpdStatus = mpd_getStatus(m_mpd);
				break;
				case CMD_MOVE_IN_PL:
cout << "move " << m_moveFrom << "   to  " << endl;
				if(m_moveFrom >= 0) {
					m_moveTo = m_curItemNum;
					mpd_sendMoveCommand(m_mpd, m_moveFrom, m_moveTo);
					mpd_finishCommand(m_mpd);
					m_moveFrom = -1;
			//		rtmpdStatusChanged += PL_CHG;
			//		mpd_sendStatusCommand(m_mpd);
			//		rtmpdStatus = mpd_getStatus(m_mpd);
				} else {
					m_moveFrom = m_curItemNum;
				}
				break;
				case CMD_TOGGLE_MODE:
				break;
				case CMD_QUEUE:
				if(m_curItemNum < m_nowPlaying || m_curItemNum > m_lastQueued || m_lastQueued == -1) {
					int pos = m_lastQueued+1;
					if(m_lastQueued == -1)
						pos = m_nowPlaying+1;
					mpd_sendMoveCommand(m_mpd, m_curItemNum, pos);
					mpd_finishCommand(m_mpd);
					m_lastQueued = pos;
				} else {
					mpd_sendMoveCommand(m_mpd, m_curItemNum, m_listing.size()-1);
					mpd_finishCommand(m_mpd);
					m_lastQueued--;
					
				}
				break;
				case CMD_SHOW_OVERLAY:
					//rCommand = command;
				break;
				default:
				break;
			}
		}
	}
	return rCommand;
}

void Playlist::draw(bool forceRefresh, long timePerFrame, bool inBack)
{
	if(!m_good)
		initAll();
	if(forceRefresh || (!inBack && m_refresh)) {
		//clear this portion of the screen 
		SDL_SetClipRect(m_screen, &m_clearRect);
		SDL_BlitSurface(m_bg, &m_clearRect, m_screen, &m_clearRect );
	
		if(m_listing.size() == 0) {
			SDL_Surface *sText;
			sText = TTF_RenderUTF8_Blended(m_font, m_config.getItem("LANG_NO_SONGS_PL").c_str(), m_itemColor);
			SDL_BlitSurface(sText,NULL, m_screen, &m_destRect );
			SDL_FreeSurface(sText);
		}
		
		Scroller::draw(timePerFrame, false, m_nowPlaying, m_lastQueued);

		m_refresh = false;
	}
}

