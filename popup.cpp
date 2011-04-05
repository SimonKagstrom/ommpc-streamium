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

#include "popup.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "gp2xregs.h"
#include "guiPos.h"
#include "keyboard.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <dirent.h>
#include <SDL_image.h>

using namespace std;

Popup::Popup(mpd_Connection* mpd, SDL_Surface* screen, Config& config, SDL_Rect& rect, int skipVal, int numPerScreen, GP2XRegs& gp2xRegs, Keyboard& kb)
: Scroller(mpd, 
	screen, 
	NULL, 
	TTF_OpenFont(config.getItem("sk_font_popup").c_str(), config.getItemAsNum("sk_font_popup_size")),
	rect, 
	config, 
	(int)(TTF_FontLineSkip(TTF_OpenFont(config.getItem("sk_font_popup").c_str(), config.getItemAsNum("sk_font_popup_size"))) * config.getItemAsFloat("sk_font_popup_extra_spacing")),
	numPerScreen)
, m_pos(0)
, m_type(0)
, m_bgRect(rect)
, m_borderRect(rect)
, m_gp2xRegs(gp2xRegs)
, m_keyboard(kb)
, m_globalKeys(false)
{
	m_borderRect.x = m_clearRect.x-1;
	m_borderRect.y = m_clearRect.y-1;
	m_borderRect.h = m_clearRect.h+2;
	m_borderRect.w = m_clearRect.w+2;
	
	m_config.getItemAsColor("sk_popup_backColor", m_backColor.r, m_backColor.g, m_backColor.b);
	m_config.getItemAsColor("sk_popup_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_popup_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);
	m_config.getItemAsColor("sk_popup_borderColor", m_borderColor.r, m_borderColor.g, m_borderColor.b);
	
	initItemIndexLookup();	
}

void Popup::initItemIndexLookup() {
	int startPos = m_clearRect.y+5+ m_skipVal*2;
	int skipCount = 0;
	int index = 0;	
	m_itemIndexLookup.clear();
	
	for(int i=0; i<startPos; ++i) {
		m_itemIndexLookup.push_back(0);
	}
	for(int i=startPos; i<(m_clearRect.h+m_clearRect.y-5); ++i) {
		m_itemIndexLookup.push_back(index);
		if(skipCount >= m_skipVal) {
			++index;
			skipCount = 0;
		}
		++skipCount;
	}
}

int Popup::selectedAction()
{
	int action;
	if(m_type == POPUP_LIST || m_type == POPUP_MENU || m_type == POPUP_OPTIONS) 
		action= m_listing[m_curItemNum].second;
	else 
		action = -1;
	return action;
}

string Popup::selectedText()
{
	string r;
	if(m_type == POPUP_LIST || m_type == POPUP_MENU || m_type == POPUP_OPTIONS) 
		r = m_listing[m_curItemNum].first;
	else 
		r = "";

	return r;
}

void Popup::setItemsText(Scroller::listing_t& items, int type)
{
	m_listing.clear();
	m_listing = items;
	m_curItemNum = 0;	
	m_lastItemNum = m_listing.size()-1;
	m_type = type;
	if(type == POPUP_OPTIONS)
		setOptionsText();
}

void Popup::setOptionsText()
{
	vector<string> curOption;
	vector<string>::iterator curIter;
#if defined(GP2X)
	for(int i=65; i<=280; i+=5) {
#else
	int everyThird = 1;
	for(int i=100; i<=600; i+=33) {
#endif
		ostringstream mhz;
#if !defined(GP2X)
		if(everyThird == 3) {
			i++;
			everyThird = 1;
		}
#endif
		mhz << i;
		curOption.push_back(mhz.str());
	}
	m_optionsText.push_back(curOption);
	curOption.clear();
#if defined(GP2X)
	for(int i=65; i<=280; i+=5) {
#else
	everyThird = 1;
	for(int i=100; i<=600; i+=33) {
#endif
		ostringstream mhz;
#if !defined(GP2X)
		if(everyThird == 3) {
			i++;
			everyThird = 1;
		}
#endif
		mhz << i;
		curOption.push_back(mhz.str());
	}
	m_optionsText.push_back(curOption);
	curOption.clear();
	curOption.push_back("true");
	curOption.push_back("false");
	m_optionsText.push_back(curOption);
	curOption.clear();

	DIR * udir = opendir("skins/");

	if(udir != NULL) {
		struct dirent * dirent = readdir(udir);

		bool done = false;
		while(dirent != NULL) {
			if(dirent->d_name[0] != '.') {
				string ename = "skins/";
				ename += dirent->d_name;
				struct stat s;

				if (stat(ename.c_str(), &s) < 0) {
					string msg = "error calling stat on ";
					msg += ename;
					throw runtime_error(msg.c_str());
				}

				if (S_ISDIR(s.st_mode) && strncmp(dirent->d_name, "default", 7)) 
					curOption.push_back(dirent->d_name);
			}
			dirent = readdir(udir);
		}
	}
	m_optionsText.push_back(curOption);
	
	curOption.clear();
	curOption.push_back("off");
	curOption.push_back("on");
	m_optionsText.push_back(curOption);
	curOption.clear();
	string tmpStr = m_config.getItem("programRoot");
	if(tmpStr.empty())
		tmpStr = "Undefined";
	curOption.push_back(tmpStr);
	m_optionsText.push_back(curOption);
	curOption.clear();
	tmpStr = m_config.getItem("musicRoot");
	if(tmpStr.empty())
		tmpStr = "Undefined";
	curOption.push_back(tmpStr);
	m_optionsText.push_back(curOption);
	curOption.clear();
	tmpStr = m_config.getItem("playlistRoot");
	if(tmpStr.empty())
		tmpStr = "Undefined";
	curOption.push_back(tmpStr);
	m_optionsText.push_back(curOption);
	curOption.clear();
	tmpStr = m_config.getItem("albumArtRoot");
	if(tmpStr.empty())
		tmpStr = "Undefined";
	curOption.push_back(tmpStr);
	m_optionsText.push_back(curOption);

	m_optionsIters.clear();	
	curIter = find(m_optionsText[0].begin(), m_optionsText[0].end(), m_config.getItem("cpuSpeed"));
	if(curIter == m_optionsText[0].end())
		curIter = m_optionsText[0].begin();
	m_optionsIters.push_back(curIter);
	curIter = find(m_optionsText[1].begin(), m_optionsText[1].end(), m_config.getItem("cpuSpeedLocked"));
	if(curIter == m_optionsText[1].end())
		curIter = m_optionsText[1].begin();
	m_optionsIters.push_back(curIter);

	curIter = find(m_optionsText[2].begin(), m_optionsText[2].end(), m_config.getItem("showAlbumArt"));
	if(curIter == m_optionsText[2].end())
		curIter = m_optionsText[2].begin();
	m_optionsIters.push_back(curIter);
	curIter = find(m_optionsText[3].begin(), m_optionsText[3].end(), m_config.getItem("skin"));
	if(curIter == m_optionsText[3].end())
		curIter = m_optionsText[3].begin();
	m_optionsIters.push_back(curIter);
	curIter = find(m_optionsText[4].begin(), m_optionsText[4].end(), m_config.getItem("softwareVolume"));
	if(curIter == m_optionsText[4].end())
		curIter = m_optionsText[4].begin();
	m_optionsIters.push_back(curIter);
	curIter = m_optionsText[5].begin();
	m_optionsIters.push_back(curIter);
	curIter = m_optionsText[6].begin();
	m_optionsIters.push_back(curIter);
	curIter = m_optionsText[7].begin();
	m_optionsIters.push_back(curIter);
	curIter = m_optionsText[8].begin();
	m_optionsIters.push_back(curIter);
}

void Popup::saveOptions()
{
	string oldSkin = m_config.getItem("skin");
	string oldSpeed = m_config.getItem("cpuSpeed");
	string oldSpeedLocked = m_config.getItem("cpuSpeedLocked");
	string oldSaa = m_config.getItem("showAlbumArt");
	string oldSoftwareVol = m_config.getItem("softwareVolume");
	string oldAAR = m_config.getItem("albumArtRoot");
	string oldMR = m_config.getItem("musicRoot");
	string oldPR = m_config.getItem("playlistRoot");
	string oldProgR = m_config.getItem("programRoot");
		
	int itemNum = 0;
	for(listing_t::iterator vIter = m_listing.begin();
		vIter != m_listing.end(); 
		++vIter) {
		if((*vIter).second > 20 && itemNum < m_optionsIters.size()) {
			std::string name;
			if(itemNum == 0)
				name = "cpuSpeed";
			else if(itemNum == 1)
				name = "cpuSpeedLocked";
			else if(itemNum == 2)
				name = "showAlbumArt";
			else if(itemNum == 3)
				name = "skin";
			else if(itemNum == 4)
				name = "softwareVolume";
			else if(itemNum == 5) {
				name = "programRoot";
				if((*m_optionsIters[itemNum])[(*m_optionsIters[itemNum]).length()-1] != '/')
					(*m_optionsIters[itemNum]) = (*m_optionsIters[itemNum]) + '/';
			}
			else if(itemNum == 6) {
				name = "musicRoot";
				if((*m_optionsIters[itemNum])[(*m_optionsIters[itemNum]).length()-1] != '/')
					(*m_optionsIters[itemNum]) = (*m_optionsIters[itemNum]) + '/';
			}
			else if(itemNum == 7) {
				name = "playlistRoot";
				if((*m_optionsIters[itemNum])[(*m_optionsIters[itemNum]).length()-1] != '/')
					(*m_optionsIters[itemNum]) = (*m_optionsIters[itemNum]) + '/';
			}
			else if(itemNum == 8) {
				name = "albumArtRoot";
				if((*m_optionsIters[itemNum])[(*m_optionsIters[itemNum]).length()-1] != '/')
					(*m_optionsIters[itemNum]) = (*m_optionsIters[itemNum]) + '/';
			}
			m_config.setItem(name, (*m_optionsIters[itemNum]));
		}
		++itemNum;
	}

	
	m_config.saveConfigFile();

	if(m_mpd != NULL && oldSpeed != m_config.getItem("cpuSpeed")) {
		//set cpu clock
		mpd_sendPauseCommand(m_mpd, 1);
		mpd_finishCommand(m_mpd);
		m_gp2xRegs.setClock(m_config.getItemAsNum("cpuSpeed"));
		mpd_sendPauseCommand(m_mpd, 0);
		mpd_finishCommand(m_mpd);
	}

	if(oldSoftwareVol != m_config.getItem("softwareVolume")
		|| oldMR != m_config.getItem("musicRoot")	
		|| oldPR != m_config.getItem("playlistRoot")	
		|| oldProgR != m_config.getItem("programRoot")) {
		updateMpdConf();
	}
	m_config.init();
	//reload skin file to pick up on any skin changes/album art flage changes.
	if(oldSkin == m_config.getItem("skin") || (oldSaa == m_config.getItem("showAlbumArt")))
		; //
		
}

void Popup::updateMpdConf()
{
	string softwareVol = m_config.getItem("softwareVolume");
	string aAR = m_config.getItem("albumArtRoot");
	string mR = m_config.getItem("musicRoot");
	string pR = m_config.getItem("playlistRoot");
	string progR = m_config.getItem("programRoot");
	
	char pwd[129];
	getcwd(pwd, 128);
	string pwdStr(pwd);
	ifstream in((pwdStr + "/mpd.conf").c_str(), ios::in);
	ofstream out((pwdStr + "/newmpd.conf").c_str(), ios::out);

	string line;
	while(!in.eof()) {
		getline(in, line);
	
		if(line.substr(0, 10) != "mixer_type" 
			&& line.find("_directory") == string::npos
			&& line.find("_file") == string::npos
			&& line.length() != 0) {
			out << line << endl; 
		}
	}
	if(softwareVol == "on")
		out << "mixer_type		\"software\"" << endl;

	out << "music_directory     \"" <<  mR << "\"" << endl;
	out << "playlist_directory     \"" << pR << "\"" << endl;
	out << "log_file     \"" << progR << ".mpdlog\"" << endl;
	out << "error_file     \"" << progR << ".mpderror\"" << endl;
	out << "db_file     \"" << progR << "db\"" << endl;
	out << "pid_file     \"" << progR << ".pid\"" << endl;
	out << "state_file     \"" << progR << ".mpdstate\"" << endl;

	rename((pwdStr + "/newmpd.conf").c_str(), (pwdStr + "/mpd.conf").c_str());

}

void Popup::setTitle(std::string name)
{
	m_name = name;
}

void Popup::setSize(SDL_Rect& rect)
{
	m_clearRect = rect;
	m_destRect.x = rect.x;
	m_destRect.y = rect.y+5;
	m_origY = m_destRect.y;
	
	m_curItemClearRect = m_destRect;
	m_borderRect.x = m_clearRect.x-1;
	m_borderRect.y = m_clearRect.y-1;
	m_borderRect.h = m_clearRect.h+2;
	m_borderRect.w = m_clearRect.w+2;
	
	m_numPerScreen = (rect.h-(2*m_skipVal))/m_skipVal;

	initItemIndexLookup();	

}

int Popup::processCommand(int command, GuiPos& guiPos) 
{
	int rCommand = command;

	if(command == CMD_CLICK) {
			rCommand = 0;
/*
		cout << m_clearRect.x << endl;
		cout << m_clearRect.y << endl;
		cout << m_clearRect.w << endl;
		cout << m_clearRect.h << endl;

		cout << "curx " << guiPos.curX << endl;
		cout << "cury " << guiPos.curY << endl;
*/
		int lrOffset = 40;
		int hwOffset = 40;
		if(m_type == POPUP_OPTIONS) {
			lrOffset = 150;
			hwOffset = m_skipVal;
		}

		if(guiPos.curY > m_clearRect.y && (guiPos.curY < m_clearRect.y + m_clearRect.h))	 {
			if(guiPos.curX < (m_clearRect.w+m_clearRect.x-lrOffset)) {
				m_curItemNum = m_topItemNum + m_itemIndexLookup[guiPos.curY];		
				if(m_curItemNum > m_listing.size())
					m_curItemNum = m_listing.size() -1;
				if(m_type == POPUP_OPTIONS && m_curItemNum < 5)
					rCommand = 0;
				else if(m_type == POPUP_OPTIONS && m_curItemNum < 9)
					rCommand = CMD_POP_KEYBOARD;
				else 
					rCommand = CMD_POP_SELECT;
			} else if(guiPos.curX > (m_clearRect.w+m_clearRect.x-lrOffset)) {
				if(m_type == POPUP_OPTIONS) {
					if(m_curItemNum == m_topItemNum + m_itemIndexLookup[guiPos.curY]) {	
						if(guiPos.curX < (m_clearRect.w+m_clearRect.x-(lrOffset-40 ))) 
							command = CMD_LEFT;
						else
							command = CMD_RIGHT;
					}
				} else {
					if(guiPos.curY < m_clearRect.y+hwOffset) {
						command = CMD_LEFT;
					} else if(guiPos.curY > m_clearRect.y + m_clearRect.h - hwOffset) {
						command = CMD_RIGHT;
					}
				}
			}
		} 
	}

	switch (command) {
		case CMD_POP_SELECT:
			if(m_type == POPUP_OPTIONS)	 {
				//cout << m_curItemNum << endl;
				if(m_curItemNum >4 && m_curItemNum <9) {
					rCommand = CMD_POP_KEYBOARD;
				} 
			}
			break;
		case CMD_POP_CHG_OPTION:
			(*m_optionsIters[m_curItemNum]) = m_keyboard.getText();
			break;
		case CMD_DOWN:
			++Scroller::m_curItemNum;
			if(Scroller::m_curItemNum > Scroller::m_lastItemNum) {
				Scroller::m_topItemNum = Scroller::m_curItemNum = 0;
			} else 	if(Scroller::m_curItemNum >= Scroller::m_numPerScreen+Scroller::m_topItemNum) {
				++Scroller::m_topItemNum;
			}
			rCommand = 0;
			break;
		case CMD_UP:
			if(m_curItemNum > 0) {
				--m_curItemNum;
				if(m_curItemNum <= m_topItemNum && m_topItemNum >0)
					--m_topItemNum;
			} else if(m_curItemNum == 0) {
				m_curItemNum = m_lastItemNum;
				m_topItemNum = m_curItemNum - m_numPerScreen+1;			
			}
			rCommand = 0;
			break;
		case CMD_LEFT:
			if(m_type == POPUP_OPTIONS) {
				if (m_optionsIters[m_curItemNum] == m_optionsText[m_curItemNum].begin())
					m_optionsIters[m_curItemNum] = m_optionsText[m_curItemNum].end() - 1;
				else
					m_optionsIters[m_curItemNum]--;
				rCommand = 0;
			}
			break;
		case CMD_RIGHT:
			if(m_type == POPUP_OPTIONS) {
				m_optionsIters[m_curItemNum]++;
				if (m_optionsIters[m_curItemNum] == m_optionsText[m_curItemNum].end())
					m_optionsIters[m_curItemNum] = m_optionsText[m_curItemNum].begin();
				rCommand = 0;
			}
			break;
	}

	return rCommand;

}

string Popup::getSelOptionText()
{

	return *m_optionsIters[m_curItemNum];

}


void Popup::draw()
{
	switch(m_type) {
		case POPUP_LIST: 
		case POPUP_MENU:
		case POPUP_OPTIONS:
			drawSelectList();
		break;
		case POPUP_CONFIRM:
//			drawConfirm();
		break;
//			drawMenu();
		break;
		default:
		break;
	}
}

void Popup::drawSelectList()
{
	//clear this portion of the screen 
	SDL_SetClipRect(m_screen, &m_borderRect);
	SDL_FillRect(m_screen, &m_borderRect, SDL_MapRGB(m_screen->format, m_borderColor.r, m_borderColor.g, m_borderColor.b));
	SDL_FillRect(m_screen, &m_clearRect, SDL_MapRGB(m_screen->format, m_backColor.r, m_backColor.g, m_backColor.b));

	SDL_Surface *sText;
	sText = TTF_RenderUTF8_Blended(m_font, m_name.c_str(), m_itemColor);
	SDL_BlitSurface(sText,NULL, m_screen, &m_destRect );
	SDL_FreeSurface(sText);
	m_destRect.y += m_skipVal*2;
	m_curItemClearRect.y += m_skipVal*2;
	if(m_type == POPUP_OPTIONS) {
		m_selectedOptions.clear();

		m_selectedOptions.push_back((*m_optionsIters[0]));
		m_selectedOptions.push_back((*m_optionsIters[1]));
		m_selectedOptions.push_back((*m_optionsIters[2]));
		m_selectedOptions.push_back((*m_optionsIters[3]));
		m_selectedOptions.push_back((*m_optionsIters[4]));
		m_selectedOptions.push_back((*m_optionsIters[5]));
		m_selectedOptions.push_back((*m_optionsIters[6]));
		m_selectedOptions.push_back((*m_optionsIters[7]));
		m_selectedOptions.push_back((*m_optionsIters[8]));
			
		Scroller::draw(m_selectedOptions);
	}
	else 
		Scroller::draw(0);
}


bool Popup::showPopupHelp(SDL_Surface* screen, Config& config, int curMode)
{
	bool show = false;	
	string spaces = "   ";
	Config keys("keys");	
	Scroller::listing_t items;
	int type = Popup::POPUP_MENU;
	if(!showGlobalKeys())
		items.push_back(make_pair("  "+config.getItem("LANG_GLOBAL_BINDINGS"),(int)Popup::POPUP_SHOW_GLOBAL));
	else
		items.push_back(make_pair("  "+config.getItem("LANG_BINDINGS"),(int)Popup::POPUP_SHOW_GLOBAL));
	items.push_back(make_pair("  "+config.getItem("LANG_RET_TO_PLAYER"), (int)Popup::POPUP_CANCEL));
	items.push_back(make_pair(" ", (int)Popup::POPUP_CANCEL));

	if(!showGlobalKeys()) {
		switch(curMode) {
			case 0: //Library
				items.push_back(make_pair("  "+config.getItem("BIND_LIB_SELECT")+spaces
							+keys.getItem("LIB_SELECT"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_LIB_PREV_DIR")+spaces
							+keys.getItem("LIB_PREV_DIR"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_LIB_PLAY_PAUSE")+spaces
							+keys.getItem("LIB_PLAY_PAUSE"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_LIB_STOP")+spaces
							+keys.getItem("LIB_STOP"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_LIB_ADD_TO_PL")+spaces
							+keys.getItem("LIB_ADD_TO_PL"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_LIB_ADD_AS_PL")+spaces
							+keys.getItem("LIB_ADD_AS_PL"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_LIB_QUEUE")+spaces
							+keys.getItem("LIB_QUEUE"), (int)Popup::POPUP_CANCEL)); 
				break;
			case 1: //Playlist
			case 2: //NP
				items.push_back(make_pair("  "+config.getItem("BIND_PL_PLAY_PAUSE")+spaces
							+keys.getItem("PL_PLAY_PAUSE"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_STOP")+spaces
							+keys.getItem("PL_STOP"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_NEXT")+spaces
							+keys.getItem("PL_NEXT"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_FF")+spaces
							+keys.getItem("PL_FF"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_PREV")+spaces
							+keys.getItem("PL_PREV"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_REW")+spaces
							+keys.getItem("PL_REW"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_TOGGLE_RND_RPT")+spaces
							+keys.getItem("PL_TOGGLE_RND_RPT"), (int)Popup::POPUP_CANCEL)); 
#if !defined(GP2X) || !defined(WIZ)
				items.push_back(make_pair("  "+config.getItem("BIND_PL_RND")+spaces
							+keys.getItem("PL_RND"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_RPT")+spaces
							+keys.getItem("PL_RPT"), (int)Popup::POPUP_CANCEL));
#endif 
			if(curMode == 1) {
				items.push_back(make_pair("  "+config.getItem("BIND_PL_REMOVE_FROM_PL")+spaces
							+keys.getItem("PL_REMOVE_FROM_PL"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_MOVE_IN_PL")+spaces
							+keys.getItem("PL_MOVE_IN_PL"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PL_QUEUE_NEXT")+spaces
							+keys.getItem("PL_QUEUE_NEXT"), (int)Popup::POPUP_CANCEL)); 
			}

				break;	
			case 3: //
				items.push_back(make_pair("  "+config.getItem("BIND_PLBROWSE_SELECT")+spaces
							+keys.getItem("PLBROWSE_SELECT"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PLBROWSE_PLAY_PAUSE")+spaces
							+keys.getItem("PLBROWSE_PLAY_PAUSE"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PLBROWSE_STOP")+spaces
							+keys.getItem("PLBROWSE_STOP"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PLBROWSE_APPEND")+spaces
							+keys.getItem("PLBROWSE_APPEND"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_PLBROWSE_DEL")+spaces
							+keys.getItem("PLBROWSE_DEL"), (int)Popup::POPUP_CANCEL)); 
				break;	
			case 4: //bookmarks
				items.push_back(make_pair("  "+config.getItem("BIND_BOOKMRK_SELECT")+spaces
							+keys.getItem("BOOKMRK_SELECT"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_BOOKMRK_PLAY_PAUSE")+spaces
							+keys.getItem("BOOKMRK_PLAY_PAUSE"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_BOOKMRK_STOP")+spaces
							+keys.getItem("BOOKMRK_STOP"), (int)Popup::POPUP_CANCEL)); 
				items.push_back(make_pair("  "+config.getItem("BIND_BOOKMRK_DEL")+spaces
							+keys.getItem("BOOKMRK_DEL"), (int)Popup::POPUP_CANCEL)); 
				break;	

		}	
	} else {
		items.push_back(make_pair("  "+config.getItem("BIND_RIGHT")+spaces
					+keys.getItem("RIGHT"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_LEFT")+spaces
					+keys.getItem("LEFT"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_UP")+spaces
					+keys.getItem("UP"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_DOWN")+spaces
					+keys.getItem("DOWN"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_SHOW_CONTROLS")+spaces
					+keys.getItem("SHOW_CONTROLS"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_VOL_UP")+spaces
					+keys.getItem("VOL_UP"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_VOL_DOWN")+spaces
					+keys.getItem("VOL_DOWN"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_SHOW_MENU")+spaces
					+keys.getItem("SHOW_MENU"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_LOCK")+spaces
					+keys.getItem("LOCK"), (int)Popup::POPUP_CANCEL)); 
		items.push_back(make_pair("  "+config.getItem("BIND_TOGGLE_MODE")+spaces
					+keys.getItem("TOGGLE_MODE"), (int)Popup::POPUP_CANCEL)); 
		//	items.push_back(make_pair("", type));	
	}	
	setItemsText(items, type);
	SDL_Rect popRect;
	popRect.w = 220;
	popRect.h = skipVal()*17+10;
	popRect.x = (screen->w - popRect.w) / 2;
	popRect.y = (screen->h - popRect.h) / 2;
	setSize(popRect);
	setTitle("  "+config.getItem("LANG_MENU")+"      ommpc v0.4.6");
	show = true;

	return show;
}

int Popup::processPopupCommand()
{
	int rCommand = 0;
	switch(selectedAction()) {
		case Popup::POPUP_CANCEL:
			rCommand = 0;
			break;
		case Popup::POPUP_SHOW_GLOBAL:
			toggleHelpView();
			rCommand = CMD_POP_HELP;
			break;
		case Popup::POPUP_CONTEXT:
			listingExtras_t extras = m_listingExtras[m_curItemNum];
			rCommand = extras.command;
			break;
	}
	
	return rCommand;
}

int Popup::touchContextItem()
{
	return m_contextItem;
}

bool Popup::showPopupTouch(SDL_Surface* screen, Config& config, int curMode)
{
	bool show = false;	
	string spaces = "   ";
	Scroller::listing_t items;
	int type = Popup::POPUP_MENU;
	listingExtras_t extras;
	
	items.push_back(make_pair(spaces+config.getItem("LANG_RET_TO_PLAYER"), (int)Popup::POPUP_CANCEL));
	extras.command = 0;
	m_listingExtras.push_back(extras);
	if(curMode != 5) {
		items.push_back(make_pair(spaces+config.getItem("LANG_MENU_MAIN"), (int)Popup::POPUP_CONTEXT));
		extras.command = CMD_SHOW_MENU;
		m_listingExtras.push_back(extras);
	}
	items.push_back(make_pair(" ", (int)Popup::POPUP_CANCEL));
	extras.command = 0;
	m_listingExtras.push_back(extras);
	
	switch(curMode) {
		case 0: //Library
			items.push_back(make_pair(spaces+config.getItem("BIND_LIB_PLAY_PAUSE"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_PLAY_PAUSE;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_LIB_STOP"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_STOP;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_LIB_ADD_TO_PL"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_ADD_TO_PL;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_LIB_ADD_AS_PL"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_ADD_AS_PL;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_LIB_QUEUE"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_QUEUE;
			m_listingExtras.push_back(extras);
		break;
		case 1: //Playlist
			items.push_back(make_pair(spaces+config.getItem("LANG_DISPLAY_OVERLAY"), (int)Popup::POPUP_CONTEXT));
			extras.command = CMD_SHOW_OVERLAY;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_PL_REMOVE_FROM_PL"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_DEL_FROM_PL;
			m_listingExtras.push_back(extras);
;
			items.push_back(make_pair(spaces+config.getItem("BIND_PL_MOVE_IN_PL"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_MOVE_IN_PL;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_PL_QUEUE_NEXT"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_QUEUE;
			m_listingExtras.push_back(extras);
		break;
		case 3: //PL browse
			items.push_back(make_pair(spaces+config.getItem("BIND_PLBROWSE_PLAY_PAUSE"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_PLAY_PAUSE;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_PLBROWSE_STOP"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_STOP;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_PLBROWSE_DEL"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_DEL_PL;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_PLBROWSE_APPEND"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_APPEND_PL;
			m_listingExtras.push_back(extras);
		break;
		case 4: //bookmarks
			items.push_back(make_pair(spaces+config.getItem("BIND_BOOKMRK_PLAY_PAUSE"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_PLAY_PAUSE;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_BOOKMRK_STOP"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_STOP;
			m_listingExtras.push_back(extras);
			items.push_back(make_pair(spaces+config.getItem("BIND_BOOKMRK_DEL"), 
						(int)Popup::POPUP_CONTEXT)); 
			extras.command = CMD_DEL_BKMRK;
			m_listingExtras.push_back(extras);
		break;
	}
	
	setItemsText(items, type);
	SDL_Rect popRect;
	popRect.w = 140;
	popRect.h = skipVal()*(items.size()+2)+10;
	popRect.x = (screen->w - popRect.w) / 2;
	popRect.y = (screen->h - popRect.h) / 2;
	setSize(popRect);
	setTitle(spaces+config.getItem("LANG_TS_POPUP"));
	show = true;

	return show;


}
