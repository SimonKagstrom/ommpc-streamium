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
#include "artButton.h"
#include "id3Button.h"
#include "guiPos.h"
#include "rptButton.h"
#include "config.h"
#include "threadParms.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

ArtButton::ArtButton(artThreadParms_t& artParms, string label, string id)
: Button(id, "overlay")
, m_active(false)
, m_label(label)
, m_sText(NULL)
, m_id(id)
, m_displayText(true)
, m_artParms(artParms)
, m_showInfo(false)		
, m_titleBtn(NULL)
, m_artistBtn(NULL)
, m_albumBtn(NULL)
, m_genreBtn(NULL)
, m_trackBtn(NULL)
, m_typeBtn(NULL)
, m_dateBtn(NULL)
, m_composerBtn(NULL)
, m_discBtn(NULL)
, m_performerBtn(NULL)
, m_commentBtn(NULL)
, m_rateBtn(NULL)
, m_freqBtn(NULL)
, m_animate(false)
{
	if(m_label.empty())
		m_displayText = false;
}


void ArtButton::init(Config& config, int command)
{
	string btnName = "sk_"+m_name;
	m_clearRect.x = config.getItemAsNum(btnName+"_x");
	m_clearRect.y = config.getItemAsNum(btnName+"_y");
	m_clearRect.w = config.getItemAsNum(btnName+"_width");
	m_clearRect.h = config.getItemAsNum(btnName+"_height");
	m_destRect = m_clearRect;
	config.getItemAsColor("sk_art_flip_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_mouseRect = m_clearRect;
	m_command = command;	
	
	string btnNames = config.getItem("sk_full_art_btn_list");
	size_t lastPos = 0;
	size_t pos;
	pos = btnNames.find(',', lastPos);
	TTF_Font * font = TTF_OpenFont(config.getItem("sk_font_id3").c_str(),
						  config.getItemAsNum("sk_font_id3_size") );
	while(pos != string::npos) {
		string btnName = "sk_" + btnNames.substr(lastPos,  pos-lastPos);
		if(btnName == "sk_artist") { 
			m_artistBtn = new ID3Button("Artist: ", "artist", font);
			m_artistBtn->init(config);
		} else if(btnName == "sk_album") {
			m_albumBtn = new ID3Button("Album: ", "album", font);
			m_albumBtn->init(config);
		} else if(btnName == "sk_genre") {
			m_genreBtn = new ID3Button("Genre: ", "genre", font);
			m_genreBtn->init(config);
		} else if(btnName == "sk_track") {
			m_trackBtn = new ID3Button("Track: ", "track", font);
			m_trackBtn->init(config);
		} else if(btnName == "sk_date") {
			m_dateBtn = new ID3Button("Date: ", "date", font);
			m_dateBtn->init(config);
		} else if(btnName == "sk_rate") {
			m_rateBtn = new ID3Button("Rate: ", "rate", font);
			m_rateBtn->init(config);
		} else if(btnName == "sk_freq") {
			m_freqBtn = new ID3Button("Freq: ", "freq", font);
			m_freqBtn->init(config);
		} else if(btnName == "sk_composer") {
			m_composerBtn = new ID3Button("Composer: ", "composer", font);
			m_composerBtn->init(config);
		} else if(btnName == "sk_performer") {
			m_performerBtn = new ID3Button("Performer: ", "performer", font);
			m_performerBtn->init(config);
		} else if(btnName == "sk_title") {
			m_titleBtn = new ID3Button("Title: ", "title", font);
			m_titleBtn->init(config);
		} else if(btnName == "sk_type") {
			m_typeBtn = new ID3Button("Type: ", "type", font);
			m_typeBtn->init(config);
		} else if(btnName == "sk_disc") {
			m_discBtn = new ID3Button("Disc: ", "disc", font);
			m_discBtn->init(config);
		} else if(btnName == "sk_comment") {
			m_commentBtn = new ID3Button("Comment: ", "comment", font);
			m_commentBtn->init(config);
		}
		lastPos = pos+1;
		pos = btnNames.find(',', lastPos);

	}
	m_moveRect.w = 160;
	m_moveRect.h = 160;
	m_moveRect.x = 0;
	m_moveRect.y = 0;
} 


bool ArtButton::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
						int rtmpdStatusChanged, mpd_Status* rtmpdStatus, bool updatingSongDb, mpd_Connection* mpd, Config& config)
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
		if(m_rateBtn && (statusChanged & RATE_CHG)) { 
			ostringstream out;
			out.str("");
			out  << status->bitRate << " kbps";
			m_rateBtn->setText(out.str());	
		}
		if(m_freqBtn && (statusChanged & FREQ_CHG)) { 
			ostringstream out;
			out.str("");
			out  << (float)status->sampleRate/1000 << " khz";
			m_freqBtn->setText(out.str());	
		}
		if(statusChanged & SONG_CHG) {
			mpd_sendCurrentSongCommand(mpd);
			mpd_InfoEntity* songEntity = mpd_getNextInfoEntity(mpd);
			if(songEntity != NULL) {
				m_artParms.songFile = config.getItem("albumArtRoot") + songEntity->info.song->file;	
				string file;	
				if(songEntity->info.song->file != NULL && m_typeBtn) {
					file = songEntity->info.song->file;
					int pos = file.rfind('.');
					string ext = file.substr(pos+1);
					if(ext == "mp3")
						ext = " MP3";
					else if(ext == "ogg")
						ext = " OGG";
					else if(ext == "mp4" || ext == "m4p" || ext == "m4a")
						ext = " AAC";
					else if (ext == "flac")
						ext = "FLAC";
					else
						ext = " ";

					m_typeBtn->setText(ext);
				}
				if(songEntity->info.song->album != NULL && m_albumBtn)
					m_albumBtn->setText(songEntity->info.song->album);
				if(songEntity->info.song->title != NULL && m_titleBtn)
					m_titleBtn->setText(songEntity->info.song->title);
				else if(songEntity->info.song->name != NULL && m_titleBtn)
					m_titleBtn->setText(songEntity->info.song->name);
				else if(songEntity->info.song->file != NULL && m_titleBtn)
					m_titleBtn->setText(songEntity->info.song->file);
				if(songEntity->info.song->artist != NULL && m_artistBtn)
					m_artistBtn->setText(songEntity->info.song->artist);
				if(songEntity->info.song->track != NULL && m_trackBtn)
					m_trackBtn->setText(songEntity->info.song->track);
				if(songEntity->info.song->date != NULL && m_dateBtn)
					m_dateBtn->setText(songEntity->info.song->date);
				if(songEntity->info.song->genre != NULL && m_genreBtn)
					m_genreBtn->setText(songEntity->info.song->genre);
				if(songEntity->info.song->composer != NULL && m_composerBtn)
					m_composerBtn->setText(songEntity->info.song->composer);
				if(songEntity->info.song->performer != NULL && m_performerBtn)
					m_performerBtn->setText(songEntity->info.song->performer);
				if(songEntity->info.song->disc != NULL && m_discBtn)
					m_discBtn->setText(songEntity->info.song->disc);
				if(songEntity->info.song->comment != NULL && m_commentBtn)
					m_commentBtn->setText(songEntity->info.song->comment);
		
				
			} else {
				m_artParms.songFile = "";
			}
			m_artParms.doArtLoad = true;
			m_refresh = true;
		}
	}	

	return m_refresh;			
}


int ArtButton::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	if(command > 0) {
		if(command == CMD_CLICK || command == CMD_HOLD_CLICK) {
			if(guiPos.curX > m_mouseRect.x 
				&& guiPos.curX < m_mouseRect.x + m_mouseRect.w
				&& guiPos.curY > m_mouseRect.y 
				&& guiPos.curY < m_mouseRect.y + m_mouseRect.h) {
				if(command == CMD_CLICK) {
					rCommand = m_command;

					m_showInfo = !m_showInfo;
					m_animate = true;
					m_refresh = true;	
					m_active = true;
				} else {
					rCommand = CMD_POP_CONTEXT;
				}
			}
		} else if(command == CMD_POP_SELECT && m_active) {
			rCommand = m_command;
		} else if(command == CMD_FLIP_ART) {
					rCommand = m_command;

					m_showInfo = !m_showInfo;
					m_animate = true;
					m_refresh = true;	
					m_active = true;

		}
	
	}


	return rCommand;
}

bool ArtButton::draw2(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh)
{
	if(!m_artParms.doArtLoad && (m_refresh||forceRefresh)) {
		//clear this portion of the screen 
		SDL_SetClipRect(screen, &m_clearRect);
		SDL_BlitSurface(bg, &m_clearRect, screen, &m_clearRect );
		if(m_showInfo || m_animate) {
			if(m_titleBtn)
				m_titleBtn->draw(screen, bg, forceRefresh);
			if(m_artistBtn)
				m_artistBtn->draw(screen, bg, forceRefresh);
			if(m_albumBtn)
				m_albumBtn->draw(screen, bg, forceRefresh);
			if(m_genreBtn)
				m_genreBtn->draw(screen, bg, forceRefresh);
			if(m_trackBtn)
				m_trackBtn->draw(screen, bg, forceRefresh);
			if(m_typeBtn)
				m_typeBtn->draw(screen, bg, forceRefresh);
			if(m_dateBtn)
				m_dateBtn->draw(screen, bg, forceRefresh);
			if(m_composerBtn)
				m_composerBtn->draw(screen, bg, forceRefresh);
			if(m_discBtn)
				m_discBtn->draw(screen, bg, forceRefresh);
			if(m_performerBtn)
				m_performerBtn->draw(screen, bg, forceRefresh);
			if(m_commentBtn)
				m_commentBtn->draw(screen, bg, forceRefresh);
			if(m_rateBtn)
				m_rateBtn->draw(screen, bg, forceRefresh);
			if(m_freqBtn)
				m_freqBtn->draw(screen, bg, forceRefresh);
		}
		if(!m_showInfo || m_animate) {
			if(m_animate) {
 				if(!m_showInfo) {
					if(m_moveRect.y <= 0)
						m_animate = false;
					else
						m_moveRect.y -= 10;
				} else {
					if(m_moveRect.y >= 160)
						m_animate = false;
					else
						m_moveRect.y += 10;

				}
			} 

			SDL_BlitSurface(m_artParms.artSurface, &m_moveRect, screen, &m_destRect );
		}
		m_artParms.doArtLoad = false;
		m_refresh = false;
	}
	return m_refresh;
}

