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

#include "nowPlaying.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "guiPos.h"
#include <iostream>
#include <stdexcept>
#include <SDL_image.h>

using namespace std;

NowPlaying::NowPlaying(mpd_Connection* mpd, SDL_mutex* lock, SDL_Surface* screen, SDL_Surface* bg, Config& config, SDL_Rect& rect, Playlist& pl)
: m_mpd(mpd)
, m_lock(lock)
, m_screen(screen)
, m_config(config)
, m_playlist(pl)
, m_inc(false) 
, m_delayCnt(0)
, m_delayCnt2(0)
, m_nowPlaying(0)
, m_artistNoScroll(false)
, m_noScroll(false)
, m_refresh(true)
, m_bg(bg)
, m_srcX(0)
, m_srcArtistX(0)
, m_scrollTextSurface(NULL)
, m_artistTextSurface(NULL)
, m_delay(0)
, m_scrollText("")
{

	//m_origY = m_destRect.y;
	m_font = TTF_OpenFont(config.getItem("sk_font_playing_lrg").c_str(),
						  config.getItemAsNum("sk_font_playing_lrg_size"));
	m_skipVal = TTF_FontLineSkip( m_font ) * config.getItemAsFloat("sk_font_playing_lrg_extra_spacing");
	m_destRect.x = rect.x;
	m_destRect.y = rect.y;
	m_scrollClearRect = m_destRect;
    m_scrollClearRect.w = rect.w;
	m_scrollClearRect.h = m_skipVal;
		
	m_fontSmall = TTF_OpenFont(config.getItem("sk_font_playing_sm").c_str(),
						  config.getItemAsNum("sk_font_playing_sm_size"));
	m_artistRect.x = rect.x;
	m_artistRect.y = rect.y + m_skipVal;	
	m_artistClearRect = m_artistRect;
	m_artistClearRect.w = rect.w;
	int artistSkipVal = TTF_FontLineSkip( m_fontSmall ) * config.getItemAsFloat("sk_font_playing_lrg_extra_spacing");
	m_artistClearRect.h = rect.h - m_scrollClearRect.h;

	m_config.getItemAsColor("sk_nP_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_nP_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);
	m_format = m_config.getItemAsNum("sk_nP_format");
}

void NowPlaying::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
								int rtmpdStatusChanged, mpd_Status* rtmpdStatus)
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

	if(statusChanged & SONG_CHG || statusChanged & PL_CHG) {
		m_nowPlaying = status->song;
		m_title = m_playlist.nowPlayingTitle(m_nowPlaying);
		m_artist = m_playlist.nowPlayingArtist(m_nowPlaying);
		if(m_format == 0)  {
			m_scrollText = m_artist + " - " + m_title;
		} else if(m_format == 1) {
			m_scrollText = m_title;
		} else {
			m_scrollText = m_artist + " - " + m_title;
		}
		m_srcX = 0;
		m_srcArtistX = 0;
		m_inc = false;
		m_incArtist = false;
		m_noScroll = false;
		m_artistNoScroll = false;
		m_refresh = true;	
		if(m_scrollTextSurface != NULL)
			SDL_FreeSurface(m_scrollTextSurface);
		m_scrollTextSurface = TTF_RenderUTF8_Blended(m_font, m_scrollText.c_str(), m_itemColor);
		if(m_artistTextSurface != NULL)
			SDL_FreeSurface(m_artistTextSurface);
		m_artistTextSurface = TTF_RenderUTF8_Blended(m_fontSmall, m_artist.c_str(), m_itemColor);
	}

}

void NowPlaying::songChange()
{


}


int NowPlaying::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	if(command > 0) {
		m_refresh = true;
	}
	return rCommand;
}

void NowPlaying::draw(bool forceRefresh, long timePerFrame, bool inBack)
{
	//unit to move(1px) * us/frame / 1000 / 1000 
	int m_repeatFrame = 1;
	if(timePerFrame > 0)  {
		if(15*timePerFrame < 1000000)
		m_delay = 1000000 /(15 * timePerFrame) ;
		else  {
			m_repeatFrame = (15*timePerFrame) / 1000000;
			m_delay = 0;
		}
	}


//	cout << "delay " << m_delay << "  delaycnt  " << m_delayCnt << endl;;
//	cout << "us/f " << timePerFrame << endl;
//	cout << "frames to delay " <<  m_delay << endl;
	if(m_mpd == NULL) 
		m_scrollText = m_config.getItem("LANG_MPD_NOT_STARTED");
	if((m_refresh && !inBack) || forceRefresh) {
		if(inBack)
			m_noScroll = true;
		if(m_noScroll) {
			SDL_SetClipRect(m_screen, &m_scrollClearRect);
			SDL_BlitSurface(m_bg, &m_scrollClearRect, m_screen, &m_destRect );
			SDL_BlitSurface(m_scrollTextSurface, NULL, m_screen, &m_destRect );
			m_refresh = false;
		} else if((m_delayCnt > m_delay ) && !m_noScroll) {
			for(int i=0; i < m_repeatFrame; ++i) {
			//clear this portion of the screen 
			SDL_SetClipRect(m_screen, &m_scrollClearRect);
			SDL_BlitSurface(m_bg, &m_scrollClearRect, m_screen, &m_destRect );

			if(!m_scrollText.empty()) {
				if(m_scrollTextSurface == NULL)
					m_scrollTextSurface = TTF_RenderUTF8_Blended(m_font, m_scrollText.c_str(), m_itemColor);
				if(m_scrollTextSurface->w > m_scrollClearRect.w) {
					m_srcRect.x=m_srcX; 
					m_srcRect.y=0;
					m_srcRect.w = m_scrollTextSurface->w;
					m_srcRect.h = m_scrollTextSurface->h;
				
					if(m_inc)	
						--m_srcX;
					else
						++m_srcX;
					
					if(m_srcX == 0 || m_srcX == (m_scrollTextSurface->w - m_scrollClearRect.w))
						m_inc = !m_inc;
					SDL_BlitSurface(m_scrollTextSurface, &m_srcRect,  m_screen, &m_destRect );
				} else {
					m_noScroll = true;
					m_refresh = true;
				} 
			}
			m_delayCnt = 0;
			m_refresh = true;
			}
		} else {
			++m_delayCnt;
			m_refresh = true;
		}
		if(m_artistNoScroll) {
			SDL_SetClipRect(m_screen, &m_artistClearRect);
			SDL_BlitSurface(m_bg, &m_artistClearRect, m_screen, &m_artistRect );
			SDL_BlitSurface(m_artistTextSurface, NULL, m_screen, &m_artistRect );
			m_refresh = m_refresh || false;
		} else if(m_format == 1 && (m_delayCnt2 > m_delay) && !m_artistNoScroll) {
			//clear this portion of the screen 
			SDL_SetClipRect(m_screen, &m_artistClearRect);
			SDL_BlitSurface(m_bg, &m_artistClearRect, m_screen, &m_artistRect );

			if(!m_artist.empty()) {	
				if(m_artistTextSurface == NULL)
					m_artistTextSurface = TTF_RenderUTF8_Blended(m_font, m_artist.c_str(), m_itemColor);
				if(m_artistTextSurface->w > m_artistClearRect.w) {
					m_srcRect.x=m_srcArtistX; 
					m_srcRect.y=0;
					m_srcRect.w = m_artistTextSurface->w;
					m_srcRect.h = m_artistTextSurface->h;

					if(m_incArtist)	
						--m_srcArtistX;
					else
						++m_srcArtistX;

					if(m_srcArtistX == 0 || m_srcArtistX == (m_artistTextSurface->w - m_artistClearRect.w))
						m_incArtist = !m_incArtist;

					SDL_BlitSurface(m_artistTextSurface, &m_srcRect,  m_screen, &m_artistRect );
				} else {
					m_artistNoScroll = true;
					m_refresh = true;
				}
			}
			m_delayCnt2 = 0;
			m_refresh = true;
		} else if(m_format == 1) {
			++m_delayCnt2;
			m_refresh = true;
		} else {
		//do this to clear the little area left even whent not using it
		SDL_SetClipRect(m_screen, &m_artistClearRect);
		SDL_BlitSurface(m_bg, &m_artistClearRect, m_screen, &m_artistRect );
		}
	}
}
