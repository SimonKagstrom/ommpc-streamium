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

#include "overlay.h"
#include "guiPos.h"
#include "commandFactory.h"
#include "playlist.h"
#include <iostream>
#include <stdexcept>
#include <SDL_image.h>

using namespace std;

Overlay::Overlay(mpd_Connection* mpd, SDL_Surface* screen, Config& config, SDL_Rect& rect,
				Playlist& playlist)
: m_mpd(mpd)
, m_screen(screen)
, m_config(config)
, m_clearRect(rect)
, m_refresh(true)
, m_playlist(playlist)
, m_good(false)
{
	m_prevRect.x = 8;
	m_prevRect.y = 8; 
	m_prevRect.w = 48;
	m_prevRect.h = 48; 
	m_nextRect.x = 264;
	m_nextRect.y = 8;
	m_nextRect.w = 48;
	m_nextRect.h = 48;

	m_rwRect.x = 8;
	m_rwRect.y = 60; 
	m_rwRect.w = 48;
	m_rwRect.h = 48; 
	m_ffRect.x = 264;
	m_ffRect.y = 60;
	m_ffRect.w = 48;
	m_ffRect.h = 48;

	m_menuRect.x = 8;
	m_menuRect.y = 184; 
	m_menuRect.w = 48;
	m_menuRect.h = 48; 
	m_exitRect.x = 264;
	m_exitRect.y = 184;
	m_exitRect.w = 48;
	m_exitRect.h = 48;
	
	m_playRect.x = 8;
	m_playRect.y = 112; 
	m_playRect.w = 48;
	m_playRect.h = 48; 
	m_stopRect.x = 264;
	m_stopRect.y = 112; 
	m_stopRect.w = 48;
	m_stopRect.h = 48; 


	m_clickRect1.x = m_config.getItemAsNum("sk_overlay_click_1_x");
	m_clickRect1.y = m_config.getItemAsNum("sk_overlay_click_1_y");
	m_clickRect1.w = m_config.getItemAsNum("sk_overlay_click_1_w");
	m_clickRect1.h = m_config.getItemAsNum("sk_overlay_click_1_h");
	m_clickRect2.x = m_config.getItemAsNum("sk_overlay_click_2_x");
	m_clickRect2.y = m_config.getItemAsNum("sk_overlay_click_2_y");
	m_clickRect2.w = m_config.getItemAsNum("sk_overlay_click_2_w");
	m_clickRect2.h = m_config.getItemAsNum("sk_overlay_click_2_h");
}
	
void Overlay::initAll()
{
	SDL_Surface* tmpSurface = NULL;	
	string overlayName = m_config.getItem("sk_overlay");
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/play.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/play.png").c_str());
	m_play = SDL_DisplayFormatAlpha(tmpSurface);
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/pause.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/pause.png").c_str());
	m_pause = SDL_DisplayFormatAlpha(tmpSurface);
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/ff.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/ff.png").c_str());
	m_ff = SDL_DisplayFormatAlpha(tmpSurface);
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/rw.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/rw.png").c_str());
	m_rw = SDL_DisplayFormatAlpha(tmpSurface);
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/prev.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/prev.png").c_str());
	m_prev = SDL_DisplayFormatAlpha(tmpSurface);
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/next.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/next.png").c_str());
	m_next = SDL_DisplayFormatAlpha(tmpSurface);
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/menu.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/menu.png").c_str());
	m_menu = SDL_DisplayFormatAlpha(tmpSurface);
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/exit.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/exit.png").c_str());
	m_exit = SDL_DisplayFormatAlpha(tmpSurface);
	tmpSurface = IMG_Load(string("overlays/"+overlayName+"/stop.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("overlays/default/stop.png").c_str());
	m_stop = SDL_DisplayFormatAlpha(tmpSurface);
	m_good = true;
}

void Overlay::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
							int rtmpdStatusChanged, mpd_Status* rtmpdStatus, 
							int repeatDelay)
{

	
}

bool Overlay::inRect(int x, int y, SDL_Rect& rect)
{
	if(x > rect.x && x < rect.x+rect.w && y > rect.y && y < rect.y+rect.w) 
		return true;
	else 
		return false;
}

int Overlay::processCommand(int command, GuiPos& guiPos, bool visible, int curMode)
{
	int rCommand = command;
	if(command > 0 && curMode != 5 && curMode != 2) {
		if(visible) {
			m_refresh = true;

			if(command == CMD_CLICK) {
				if(inRect(guiPos.curX, guiPos.curY, m_prevRect))
					rCommand = CMD_PREV;
				else if(inRect(guiPos.curX, guiPos.curY, m_playRect))
					rCommand = CMD_PLAY_PAUSE;
				else if(inRect(guiPos.curX, guiPos.curY, m_stopRect))
					rCommand = CMD_STOP;
				else if(inRect(guiPos.curX, guiPos.curY, m_nextRect))
					rCommand = CMD_NEXT;
				else if(inRect(guiPos.curX, guiPos.curY, m_menuRect))
					rCommand = CMD_SHOW_MENU;
				else if(inRect(guiPos.curX, guiPos.curY, m_exitRect))
					rCommand = CMD_QUIT;
				else
					rCommand = CMD_SHOW_OVERLAY;
			} else if(command == CMD_HOLD_CLICK) {
				if(inRect(guiPos.curX, guiPos.curY, m_rwRect))
					rCommand = CMD_RW;
				else if(inRect(guiPos.curX, guiPos.curY, m_ffRect))
					rCommand = CMD_FF;
			}
		
		} else {
			if(command == CMD_CLICK) {
				if(guiPos.curY > m_clickRect1.y 
					&& (guiPos.curY < m_clickRect1.y + m_clickRect1.h)
					&& guiPos.curX > m_clickRect1.x 
					&& (guiPos.curX < m_clickRect1.x + m_clickRect1.w)) {
					rCommand = CMD_SHOW_OVERLAY;
				} else if(guiPos.curY > m_clickRect2.y 
					&& (guiPos.curY < m_clickRect2.y + m_clickRect2.h)
					&& guiPos.curX > m_clickRect2.x 
					&& (guiPos.curX < m_clickRect2.x + m_clickRect2.w)) {
					rCommand = CMD_SHOW_OVERLAY;
				}
			}
		}
	}
	return rCommand;
}

void Overlay::draw(bool forceUpdate)
{
	if(!m_good)
		initAll();
	if(forceUpdate) {
		cout << "should be after " << endl;
		SDL_SetClipRect(m_screen, &m_prevRect);
		SDL_BlitSurface(m_prev, NULL, m_screen, &m_prevRect );
		
		SDL_SetClipRect(m_screen, &m_nextRect);
		SDL_BlitSurface(m_next, NULL, m_screen, &m_nextRect );
	
		SDL_SetClipRect(m_screen, &m_rwRect);
		SDL_BlitSurface(m_rw, NULL, m_screen, &m_rwRect );

		SDL_SetClipRect(m_screen, &m_ffRect);
		SDL_BlitSurface(m_ff, NULL, m_screen, &m_ffRect );
	
		SDL_SetClipRect(m_screen, &m_playRect);
		cout << "ps " << m_playlist.getPlayingState() << endl;
		if(m_playlist.getPlayingState() == MPD_STATUS_STATE_PLAY)
			SDL_BlitSurface(m_pause, NULL, m_screen, &m_playRect );
		else
			SDL_BlitSurface(m_play, NULL, m_screen, &m_playRect );
		
		SDL_SetClipRect(m_screen, &m_stopRect);
		SDL_BlitSurface(m_stop, NULL, m_screen, &m_stopRect );
		
		SDL_SetClipRect(m_screen, &m_menuRect);
		SDL_BlitSurface(m_menu, NULL, m_screen, &m_menuRect );
		
		SDL_SetClipRect(m_screen, &m_exitRect);
		SDL_BlitSurface(m_exit, NULL, m_screen, &m_exitRect );
	}	
}
