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
#include "button.h"
#include "guiPos.h"
#include "seekButton.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

SeekButton::SeekButton(string name)
: Button(name)
, m_width(0)
, m_numWidth(0)
, m_yOffset(0)
, m_xOffset(0)
, m_totalSeconds(0)
, m_elapsedSeconds(0)
, m_remainingSeconds(0)
, m_nowPlaying(0)
, m_timeType(2)
, m_delay(0)
, m_delayCnt(0)
{
	m_elapsed = formattedTime(m_elapsedSeconds);
	m_total = formattedTime(m_totalSeconds);
	m_remaining = formattedRemaining(m_remainingSeconds);
}

void SeekButton::init(Config& config)
{
	Button::init(config);

	string btnName = "sk_"+m_name;
	if(m_name == "seek_big")
		btnName = "sk_seek";
	m_width = m_destRect.w;	
	m_yOffset = config.getItemAsNum(btnName+"_yOffset");
	m_destRect.y = m_destRect.y + m_yOffset;
	m_srcRect.w = 0;

	m_timeType = config.getItemAsNum(btnName+"_timeType");
	
	m_font = TTF_OpenFont(config.getItem("sk_seek_font").c_str(),
						  config.getItemAsNum("sk_seek_font_size"));
	config.getItemAsColor("sk_seek_textColor", m_textColor.r, m_textColor.g, m_textColor.b);

	setDisplayNumWidth();	
} 

int SeekButton::elapsedTime()
{
	return m_elapsedSeconds;
}

void SeekButton::setDisplayNumWidth()
{
	string displayText = "0:00";
	if(m_timeType == 0 || m_timeType == 1) {
		displayText = "0:00/0:00";
	} 

	if(m_totalSeconds >= 6000)  { //two digit minutes
		displayText = "000:00";
		if(m_timeType == 0 || m_timeType == 1) {
			displayText = "000:00/000:00";
		}
	} else if(m_totalSeconds >= 600) {
		displayText = "00:00";
		if(m_timeType == 0 || m_timeType == 1) {
			displayText = "00:00/00:00";
		}
	}	 

	SDL_Surface* sText = TTF_RenderText_Blended(m_font, displayText.c_str(), m_textColor);
	m_numWidth = sText->w+1;
}

void SeekButton::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
						int rtmpdStatusChanged, mpd_Status* rtmpdStatus, bool forceRefresh)
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
		if(statusChanged & SONG_CHG || statusChanged & PL_CHG) {
			m_totalSeconds = status->totalTime;
			m_total = formattedTime(m_totalSeconds);
			m_nowPlaying = mpdStatus->song;	
			if(m_timeType == 3) {
				m_remainingSeconds = m_totalSeconds - m_elapsedSeconds;
				m_remaining = formattedRemaining(m_remainingSeconds);
			}	
			setDisplayNumWidth();
		}
		if(statusChanged & ELAPSED_CHG) { 
			m_elapsedSeconds = status->elapsedTime;
			m_elapsed = formattedTime(m_elapsedSeconds);
			if(m_totalSeconds == 0) {
				m_totalSeconds = status->totalTime;
				m_total = formattedTime(m_totalSeconds);
			}
			if(m_timeType == 3) {
				m_remainingSeconds = m_totalSeconds - m_elapsedSeconds;
				m_remaining = formattedRemaining(m_remainingSeconds);
			}

			if(m_totalSeconds > 0)
				m_srcRect.w = m_width*((float)m_elapsedSeconds / (float)m_totalSeconds);
		}
		m_refresh = true; 
	}
}

string SeekButton::formattedTime(int time)
{
	ostringstream out;
	int fill = 1;
	int mins = time/60;
	int seconds = time - (mins*60);
	if(mins > 9)
		fill = 2;
	else if(mins > 99)
		fill = 3;

	out << setfill('0') << setw(fill) << mins << ":" << setw(2)<< seconds;
	return out.str();
}

string SeekButton::formattedRemaining(int time)
{
	ostringstream out;
	int fill = 1;
	int mins = time/60;
	int seconds = time - (mins*60);
	if(mins > 9)
		fill = 2;
	else if(mins > 99)
		fill = 3;

	out << "-" << setfill('0') << setw(fill) << mins << ":" << setw(2)<< seconds;
	return out.str();
}
	
int SeekButton::processCommand(int command, GuiPos& guiPos, mpd_Connection* mpd)
{
	int rCommand = command;
	if(command > 0) {

		if(command == CMD_CLICK) {
			if(guiPos.curX > m_destRect.x 
				&& guiPos.curX < m_destRect.x + m_xOffset + m_width
				&& guiPos.curY > m_destRect.y 
				&& guiPos.curY < m_destRect.y + m_destRect.h) {
	
				if(m_totalSeconds > 0) {
					int pos = ((float)(guiPos.curX - (m_destRect.x+m_xOffset)) /(float) m_width) * m_totalSeconds;
					cout << pos << endl;	
					mpd_sendSeekCommand(mpd, m_nowPlaying, pos);	
					mpd_finishCommand(mpd);
				}
			}
		}
	}

	return rCommand;
}

void SeekButton::draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh, long timePerFrame, bool inBack)
{
	if(timePerFrame > 0) 
		m_delay = 1000000 /(15 * timePerFrame) ;

	if((!inBack && m_refresh || forceRefresh) && m_delayCnt > m_delay) {
		SDL_SetClipRect(screen, &m_clearRect);
		SDL_BlitSurface(bg, &m_clearRect, screen, &m_clearRect);
		int origX = m_destRect.x;

		SDL_Surface *sText;
		if(m_timeType != 1) { //if not post seek bar	
			if(m_timeType == 0) {
				string displayText = m_elapsed + "/" + m_total;
				sText = TTF_RenderText_Blended(m_font, displayText.c_str(), m_textColor);
			} else {
				sText = TTF_RenderText_Blended(m_font, m_elapsed.c_str(), m_textColor);
			}
			SDL_BlitSurface(sText, NULL, screen, &m_destRect );
			m_xOffset = m_numWidth;
			//m_xOffset = sText->w+1;
			m_destRect.x += m_xOffset;
			SDL_FreeSurface(sText);
		}
		if(m_showBack)
			SDL_BlitSurface(m_backImage, NULL, screen, &m_destRect );
		if(m_showFore)
			SDL_BlitSurface(m_foreImage, &m_srcRect, screen, &m_destRect );

		m_destRect.x += m_width+1;
		if(m_timeType != 0) { //if not pre seek bar	
			if(m_timeType == 1) {
				string displayText = m_elapsed + "/" + m_total;
				sText = TTF_RenderText_Blended(m_font, displayText.c_str(), m_textColor);
			} else if(m_timeType == 3) {
				sText = TTF_RenderText_Blended(m_font, m_remaining.c_str(), m_textColor);
			} else {
				sText = TTF_RenderText_Blended(m_font, m_total.c_str(), m_textColor);
			}
			SDL_BlitSurface(sText, NULL, screen, &m_destRect );
			m_destRect.x += sText->w+1;
			SDL_FreeSurface(sText);
		}

		m_destRect.x = origX;
		m_refresh = false;
	} else { 
		m_delayCnt++;
	}
}

