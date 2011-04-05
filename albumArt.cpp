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



#include "albumArt.h"
#include "threadParms.h"
#include "threadFunctions.h"
#include <iostream>
#include <stdexcept>

using namespace std;

AlbumArt::AlbumArt(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, Config& config, SDL_Rect& rect,
					artThreadParms_t& artParms)
: m_mpd(mpd)
, m_screen(screen)
, m_config(config)
, m_clearRect(rect)
, m_pos(0)
, m_update(false)
, m_artParms(artParms)
, m_bg(bg)
{
	m_destRect.x = rect.x;
	m_destRect.y = rect.y;
	m_config.getItemAsColor("sk_art_backColor", m_backColor.r, m_backColor.g, m_backColor.b);
}

void AlbumArt::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
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

	if(statusChanged > 0) {
		if(statusChanged & SONG_CHG) {
			mpd_sendCurrentSongCommand(m_mpd);
			mpd_InfoEntity* songEntity = mpd_getNextInfoEntity(m_mpd);
			if(songEntity != NULL) {
				m_artParms.songFile = m_config.getItem("albumArtRoot") + songEntity->info.song->file;		
			} else {
				m_artParms.songFile = "";
			}
			m_artParms.doArtLoad = true;
			m_update = true;
		}
	}	
			

}

void AlbumArt::draw(bool forceUpdate)
{
	if((m_update || forceUpdate) && !m_artParms.doArtLoad) {
		//clear this portion of the screen 
		SDL_SetClipRect(m_screen, &m_clearRect);
		SDL_BlitSurface(m_bg, &m_clearRect, m_screen, &m_clearRect );
		SDL_BlitSurface(m_artParms.artSurface, NULL, m_screen, &m_destRect );
		m_artParms.doArtLoad = false;
		m_update = false;
	}

}
