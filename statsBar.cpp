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

#include "statsBar.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "playlist.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

//#define _SHOWFPS

using namespace std;

StatsBar::StatsBar(mpd_Connection* mpd, SDL_mutex* lock, SDL_Surface* screen, SDL_Surface* bg, Config& config, SDL_Rect& rect, bool& initVolume, Playlist& pl, bool f200, vector<int>& volScale)
: m_mpd(mpd)
, m_lock(lock)
, m_screen(screen)
, m_config(config)
, m_clearRect(rect)
, m_backRect(rect)
, m_volume("")
, m_playType("")
, m_crossfade("")
, m_elapsed("")
, m_total("")
, m_bitRate("")
, m_cols(config.getItemAsNum("sk_stats_numCols"))
, m_firstPass(initVolume)
, m_playlist(pl)
, m_bg(bg)
, m_f200(f200)
, m_good(false)
{
	m_backRect = rect;
	m_destRect.x = rect.x;
	m_destRect.y = rect.y;
	m_origY = m_destRect.y;
	int from = 0;
	for(int i=0; i<=20; ++i) {
		int j;
		for(j=from; j <= volScale[i]; ++j)
			m_volLookup[j] = i;

		from = j;
	}
}

void StatsBar::initAll()
{
	m_font = TTF_OpenFont(m_config.getItem("sk_font_stats").c_str(),
						  m_config.getItemAsNum("sk_font_stats_size"));
	m_itemH = TTF_FontLineSkip( m_font );

	m_itemWidth = m_config.getItemAsNum("sk_stats_itemWidth");
	m_itemSpacing = m_config.getItemAsNum("sk_stats_itemSpacing");

	m_config.getItemAsColor("sk_stats_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_stats_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);

	if(m_config.getItem("softwareVolume") == "on")
		m_softVol = true;
	else
		m_softVol = false;

	//for(int i=0; i<=100; ++i)
	//	cout << "vol " << i << "  " << m_f200VolLookup[i] << endl;	
	m_good= true;
}

string StatsBar::formattedElapsedTime()
{
	ostringstream out;
	int mins = m_elapsedSeconds/60;
	int seconds = m_elapsedSeconds - (mins*60);
	out << setfill('0') << setw(3) << mins << "_" << setw(2)<< seconds;
	string elapsed = out.str();
	return elapsed;
}

int StatsBar::elapsedTime()
{
	return m_elapsedSeconds;
}

void StatsBar::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
							int rtmpdStatusChanged, mpd_Status* rtmpdStatus, bool forceRefresh)
{
	mpd_Status * status;
	int statusChanged;
	bool doVol = false;
	if(rtmpdStatusChanged > 0) {
		status = rtmpdStatus;
		statusChanged = rtmpdStatusChanged;
		doVol = true;
	} else {
		status = mpdStatus;
		statusChanged = mpdStatusChanged;
	}

	if(statusChanged > 0) {
		ostringstream out;
		if(statusChanged & VOL_CHG) { 
			if(doVol || m_firstPass) {
				string ws;	
				string format = m_playlist.nowPlayingFormat();
				if(status->volume < 10) {
					if(format.length() == 3)
						ws = "     ";
					else
						ws = "    ";
				} else {
					if(format.length() == 3)
						ws = "  ";
					else
						ws = " ";
				}
				int scaledVol;
				if(m_f200) {
/*
					if(status->volume > 75 && !m_softVol)
						scaledVol = 20;
					else
*/
						scaledVol = m_volLookup[status->volume];
				} else {
					scaledVol = m_volLookup[status->volume];
				}	
				out << "Vol: " << scaledVol << ws;
				if(!format.empty()) {
					out << format;	
				} else {
					out << "    ";
				}
				m_items[0] = out.str();
				m_firstPass = false;
			} 
		}

		if(statusChanged & RPT_CHG || statusChanged & RND_CHG) { 
			out.str("");
			string playType = "";
			if(status->repeat == 1)
				playType = "Rpt";
			if(status->random == 1) {
				if(!playType.empty())
					playType += "/";
				playType += "Rnd";
			}
			if(playType.empty())
				playType = " ";
			m_items[3] = playType;
		}
/*		out.str("");
		out << mpdStatus->crossfade;
		m_crossfade = out.str();
*/
		if(statusChanged & SONG_CHG || statusChanged & PL_CHG) {
			int fill = 2;
			int total = status->totalTime;
			int mins = total/60;
			int seconds = total - (mins*60);
			if(mins > 99)
				fill = 3;
			out << setfill('0') << setw(fill) << mins << ":" << setw(2)<< seconds;

			m_total = out.str();

			string volText = m_items[0];
			string format = m_playlist.nowPlayingFormat();
			if(!format.empty()) {
				cout << volText << " " << format << endl;
				volText.replace(volText.size()-4, 4, format); 
				m_items[0] = volText;
			}
		}	
			
		if(statusChanged & ELAPSED_CHG) { 
			int fill = 2;
			out.str("");
			m_elapsedSeconds = status->elapsedTime;
			int mins = m_elapsedSeconds/60;
			int seconds = m_elapsedSeconds - (mins*60);
			if(mins > 99)
				fill = 3;
			out << setfill('0') << setw(fill) << mins << ":" << setw(2)<< seconds;
			m_elapsed = out.str();
			out << "/" << m_total;
			m_items[1] = out.str();
		} 

		if(statusChanged & RATE_CHG) { 
			out.str("");
			out << status->bitRate << " kbps";
			m_items[2] = out.str();
		}
	}
}

void StatsBar::draw(bool forceRefresh, int fps)
{	
	if(!m_good)
		initAll();
	//clear this portion of the screen 
	SDL_SetClipRect(m_screen, &m_clearRect);
	SDL_BlitSurface(m_bg, &m_clearRect, m_screen, &m_clearRect );
	
	int x = m_destRect.x;
	int y = m_destRect.y;
	int curCol = 0;	
	m_curItemRect.x = x+m_itemSpacing;
	m_curItemRect.y = y+m_itemSpacing;
	m_curItemRect.w = m_itemWidth;
	m_curItemRect.h = m_itemH; 

	for(int i=0; i<4; ++i) {
		if(!m_items[0].empty()) {
			SDL_Surface *sText;
#ifdef _SHOWFPS
			char num[4];
			sprintf(num, "%d", fps);
			if(i == 2)
				sText = TTF_RenderText_Blended(m_font, num, m_itemColor);
			else	
#endif
			sText = TTF_RenderText_Blended(m_font, m_items[i].c_str(), m_itemColor);
			SDL_BlitSurface(sText, NULL, m_screen, &m_curItemRect );
			SDL_FreeSurface(sText);
		}
		++curCol;
		if(curCol == m_cols) {
			m_curItemRect.y += m_itemH+m_itemSpacing;
	//		clearRect.y += m_itemH+m_itemSpacing;
			m_curItemRect.x = x+m_itemSpacing;
	//		clearRect.x = x+m_itemSpacing;
			curCol = 0;
		} else  {
			m_curItemRect.x += m_itemWidth+(m_itemSpacing*2);
	//		clearRect.x += m_itemWidth+(m_itemSpacing*2);
		}
	}

}
