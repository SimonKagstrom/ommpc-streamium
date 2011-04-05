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
#include "buttonManager.h"
#include "volButton.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

ButtonManager::ButtonManager(mpd_Connection* mpd, SDL_mutex* lock, SDL_Surface* screen, SDL_Surface* bg, Config& config, std::vector<int>& volScale)
: m_mpd(mpd)
, m_lock(lock)
, m_screen(screen)
, m_config(config)
, m_bg(bg)
, m_volBtn(NULL)
, m_rrBtn(NULL)
, m_rndBtn(NULL)
, m_rptBtn(NULL)
, m_seekBtn(NULL)
{
	string btnNames = m_config.getItem("sk_btn_list");
	size_t lastPos = 0;
	size_t pos;
	pos = btnNames.find(',', lastPos);
	while(pos != string::npos) {
		string btnName = "sk_" + btnNames.substr(lastPos,  pos-lastPos);
		if(btnName == "sk_vol") { 
			m_volBtn = new VolButton();
			m_volBtn->init(config, volScale);
		}
		if(btnName == "sk_rndrpt") { 
			m_rrBtn = new RndRptButton();
			m_rrBtn->init(config);
		}
		if(btnName == "sk_rnd") { 
			m_rndBtn = new RndButton();
			m_rndBtn->init(config);
		}
		if(btnName == "sk_rpt") { 
			m_rptBtn = new RptButton();
			m_rptBtn->init(config);
		}
		if(btnName == "sk_seek") {
			string name = "seek";
			m_seekBtn = new SeekButton(name);
			m_seekBtn->init(config);
		}
		lastPos = pos+1;
		pos = btnNames.find(',', lastPos);
	}	
	

}

ButtonManager::~ButtonManager()
{
	if(m_rndBtn)
		delete m_rndBtn;
	if(m_rptBtn)
		delete m_rptBtn;
	if(m_rrBtn)
		delete m_rrBtn;
	if(m_volBtn)
		delete m_volBtn;
	if(m_seekBtn)
		delete m_seekBtn;
}

void ButtonManager::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
						int rtmpdStatusChanged, mpd_Status* rtmpdStatus, bool forceRefresh)
{
	if(m_volBtn)
		m_volBtn->updateStatus(mpdStatusChanged, mpdStatus, rtmpdStatusChanged, rtmpdStatus, forceRefresh);
	if(m_rrBtn)
		m_rrBtn->updateStatus(mpdStatusChanged, mpdStatus, rtmpdStatusChanged, rtmpdStatus, forceRefresh);
	if(m_rndBtn)
		m_rndBtn->updateStatus(mpdStatusChanged, mpdStatus, rtmpdStatusChanged, rtmpdStatus, forceRefresh);
	if(m_rptBtn)
		m_rptBtn->updateStatus(mpdStatusChanged, mpdStatus, rtmpdStatusChanged, rtmpdStatus, forceRefresh);
	if(m_seekBtn)
		m_seekBtn->updateStatus(mpdStatusChanged, mpdStatus, rtmpdStatusChanged, rtmpdStatus, forceRefresh);
}

int ButtonManager::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	if(command > 0) {
		if(m_volBtn)
			rCommand = m_volBtn->processCommand(command, guiPos);
		if(rCommand == command && m_rrBtn)
			rCommand = m_rrBtn->processCommand(command, guiPos);
		if(rCommand == command && m_rndBtn)
			rCommand = m_rndBtn->processCommand(command, guiPos);
		if(rCommand == command && m_rptBtn)
			rCommand = m_rptBtn->processCommand(command, guiPos);
		if(rCommand == command && m_seekBtn)
			rCommand = m_seekBtn->processCommand(command, guiPos, m_mpd);
	}

	return rCommand;
}

void ButtonManager::draw(bool forceRefresh, long timePerFrame, bool inBack)
{
	if(m_volBtn)
		m_volBtn->draw(m_screen, m_bg, forceRefresh);
	if(m_rrBtn)
		m_rrBtn->draw(m_screen, m_bg, forceRefresh);
	if(m_rndBtn)
		m_rndBtn->draw(m_screen, m_bg, forceRefresh);
	if(m_rptBtn)
		m_rptBtn->draw(m_screen, m_bg, forceRefresh);
	if(m_seekBtn)
		m_seekBtn->draw(m_screen, m_bg, forceRefresh, timePerFrame, inBack);
}

string ButtonManager::formattedElapsedTime() 
{
	ostringstream out;
	int elapsedSeconds = m_seekBtn->elapsedTime();
	int mins = elapsedSeconds/60;
	int seconds = elapsedSeconds - (mins*60);
	out << setfill('0') << setw(3) << mins << "_" << setw(2)<< seconds;
	string elapsed = out.str();
	return elapsed;
}

int ButtonManager::elapsedTime() 
{
	return m_seekBtn->elapsedTime();
}
