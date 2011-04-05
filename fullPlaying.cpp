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

#include "fullPlaying.h"
#include "button.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "config.h"
#include "guiPos.h"
#include "keyboard.h"
#include "playlist.h"
#include <iostream>
#include <stdexcept>
#include <SDL_image.h>

#define X2DELAY 2000000
#define X4DELAY 6000000
#define X8DELAY 12000000
#define X16DELAY 25000000
#define X32DELAY 45000000
#define FFWAIT 400000

using namespace std;

FullPlaying::FullPlaying(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, TTF_Font* font, 
				SDL_Rect& rect,	Config& config, int skipVal, int numPerScreen, Keyboard& kb, artThreadParms_t& artParms)
: Scroller(mpd, screen, bg, font, rect, config, skipVal, numPerScreen-1)
, m_keyboard(kb)
, m_artParms(artParms)
, m_curElapsed(0)
, m_good(false)
{
	m_artBtn = new ArtButton(m_artParms, "", "full_art");;
	m_artBtn->init(m_config, CMD_FLIP_ART);
	//	initAll();
}

void FullPlaying::initAll()
{
	m_config.getItemAsColor("sk_main_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_main_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);
	m_playBtn = new MenuButton("", "play", m_font);
	m_playBtn->init(m_config, CMD_PLAY_PAUSE);
	m_pauseBtn = new MenuButton("", "pause", m_font);
	m_pauseBtn->init(m_config, CMD_PLAY_PAUSE);
	m_stopBtn = new MenuButton("", "stop", m_font);
	m_stopBtn->init(m_config, CMD_STOP);
	m_rwBtn = new MenuButton("", "rw", m_font);
	m_rwBtn->init(m_config, CMD_RW);
	m_ffBtn = new MenuButton("", "ff", m_font);
	m_ffBtn->init(m_config, CMD_FF);
	m_prevBtn = new MenuButton("", "prev", m_font);
	m_prevBtn->init(m_config, CMD_PREV);
	m_nextBtn = new MenuButton("", "next", m_font);
	m_nextBtn->init(m_config, CMD_NEXT);

	
	m_good = true;
}

void FullPlaying::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
						int rtmpdStatusChanged, mpd_Status* rtmpdStatus, bool updatingSongDb,
						int repeatDelay)
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


	m_refresh = m_artBtn->updateStatus(mpdStatusChanged, mpdStatus, rtmpdStatusChanged, rtmpdStatus, updatingSongDb, m_mpd, m_config);
	if((statusChanged & ELAPSED_CHG) && repeatDelay == 0) { 
		m_curElapsed = status->elapsedTime;	
	}
	if(statusChanged & STATE_CHG) { 
		m_curState = status->state;
		m_refresh = true;
	}
	if(statusChanged & SONG_CHG) {
		m_curItemNum = status->song;
	}
}


int FullPlaying::processCommand(int command, GuiPos& guiPos, int delayTime)
{
	int rCommand = command;
	bool done = false;
	int curItem = 0;
	if(command > 0) {
		
		rCommand = m_playBtn->processCommand(command, guiPos);
		if(rCommand != command) 
			rCommand = m_pauseBtn->processCommand(command, guiPos);
		if(rCommand == command) 
			rCommand = m_stopBtn->processCommand(command, guiPos);
		if(rCommand == command) 
			rCommand = m_rwBtn->processCommand(command, guiPos);
		if(rCommand == command) 
			rCommand = m_ffBtn->processCommand(command, guiPos);
		if(rCommand == command) 
			rCommand = m_prevBtn->processCommand(command, guiPos);
		if(rCommand == command) 
			rCommand = m_nextBtn->processCommand(command, guiPos);
		if(rCommand == command) 
			rCommand = m_artBtn->processCommand(command, guiPos);
			
			switch(rCommand) {
				case CMD_PLAY_PAUSE:
					if(m_curState == MPD_STATUS_STATE_PAUSE) {
						m_curState = MPD_STATUS_STATE_PLAY;	
						mpd_sendPauseCommand(m_mpd, 0);
						mpd_finishCommand(m_mpd);
					} else if(m_curState == MPD_STATUS_STATE_PLAY) {
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
					case CMD_STOP:
						mpd_sendStopCommand(m_mpd);
						mpd_finishCommand(m_mpd);
						break;
				case CMD_FLIP_ART:
					m_refresh = true;
				break;	
				default:
				break;
		}
	}
	return rCommand;
}

void FullPlaying::draw(bool forceRefresh, long timePerFrame, bool inBack)
{
m_refresh = true;
	if(!m_good)
		initAll();
	if(forceRefresh || (!inBack && m_refresh)) {
		//clear this portion of the screen 
		SDL_SetClipRect(m_screen, &m_clearRect);
		SDL_BlitSurface(m_bg, &m_clearRect, m_screen, &m_clearRect );
		if(m_curState == MPD_STATUS_STATE_PLAY) 
			m_pauseBtn->draw(m_screen, m_bg, forceRefresh||m_refresh);
		else
			m_playBtn->draw(m_screen, m_bg, forceRefresh||m_refresh);
		m_stopBtn->draw(m_screen, m_bg, forceRefresh||m_refresh);
		m_ffBtn->draw(m_screen, m_bg, forceRefresh||m_refresh);
		m_rwBtn->draw(m_screen, m_bg, forceRefresh||m_refresh);
		m_prevBtn->draw(m_screen, m_bg, forceRefresh||m_refresh);
		m_nextBtn->draw(m_screen, m_bg, forceRefresh||m_refresh);
		m_refresh = m_artBtn->draw2(m_screen, m_bg, forceRefresh||m_refresh);
	}
	
}
