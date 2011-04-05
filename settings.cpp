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

#include "settings.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "gp2xregs.h"
#include "playlist.h"
#include "keyboard.h"
#include "config.h"
#include "guiPos.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <dirent.h>
#include <algorithm>
#include <SDL_image.h>

using namespace std;

PlayerSettings::PlayerSettings(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface * bg, TTF_Font* font, SDL_Rect& rect, Config& config, int skipVal, int numPerScreen,  GP2XRegs& gp2xRegs, Keyboard& keyboard)
: Scroller(mpd, screen, bg, font, rect, config, skipVal, numPerScreen)
, m_keyboard(keyboard)
, m_plSurfaceText(NULL)
, m_gp2xRegs(gp2xRegs)
, m_good(false)
{
}

void PlayerSettings::initAll()
{	
	m_config.getItemAsColor("sk_main_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_main_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);

	m_plSurfaceText = TTF_RenderUTF8_Blended(m_font, m_config.getItem("LANG_MENU_OPTIONS").c_str(), m_itemColor);
	
	Scroller::listing_t items;
	items.push_back(make_pair("  "+m_config.getItem("LANG_CLOCK"), 21));
	items.push_back(make_pair("  "+m_config.getItem("LANG_CLOCK_LOCKED"), 25));
	items.push_back(make_pair("  "+m_config.getItem("LANG_SEL_LANG"), 22));
	items.push_back(make_pair("  "+m_config.getItem("LANG_SKIN"), 23));
	items.push_back(make_pair("  "+m_config.getItem("LANG_SOFT_VOL"), 24));
	items.push_back(make_pair("  "+m_config.getItem("LANG_INSTALL_PATH"), 26));
	items.push_back(make_pair("  "+m_config.getItem("LANG_MUSIC_PATH"), 27));
	items.push_back(make_pair("  "+m_config.getItem("LANG_PL_PATH"), 28));
	items.push_back(make_pair("  "+m_config.getItem("LANG_ART_PATH"), 29));

	items.push_back(make_pair(" ", 99));
	items.push_back(make_pair(" "+m_config.getItem("LANG_SAVE"), 8));
	items.push_back(make_pair(" "+m_config.getItem("LANG_CANCEL"), 0));
	
	setItemsText(items, 0);
	m_numPerScreen = m_numPerScreen/2;
	initItemIndexLookup();
	m_good = true;
}

PlayerSettings::~PlayerSettings()
{
	SDL_FreeSurface(m_plSurfaceText);
}

void PlayerSettings::initItemIndexLookup() {
	int startPos = m_clearRect.y+5+ m_skipVal*2;
	int skipCount = 0;
	int index = 0;	
	int skipVal = m_skipVal*2;
	m_itemIndexLookup.clear();
	
	for(int i=0; i<startPos; ++i) {
		m_itemIndexLookup.push_back(0);
	}
	for(int i=startPos; i<(m_clearRect.h+m_clearRect.y-5); ++i) {
		m_itemIndexLookup.push_back(index);
		if(index > m_optionsIters.size())
			skipVal = m_skipVal;
		if(skipCount >= (skipVal)) {
			++index;
			skipCount = 0;
		}
		++skipCount;
	}
}


void PlayerSettings::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus)
{
}

void PlayerSettings::setItemsText(Scroller::listing_t& items, int type)
{
	m_listing.clear();
	m_listing = items;
	m_curItemNum = 0;	
	m_lastItemNum = m_listing.size()-1;
	setOptionsText();
}

void PlayerSettings::setOptionsText()
{
	vector<string> curOption;
	vector<string>::iterator curIter;
#if defined(GP2X)
	for(int i=65; i<=280; i+=5) {
#else
	int everyThird = 0;
	for(int i=100; i<=600; i+=33) {
#endif
		ostringstream mhz;
#if !defined(GP2X)
		if(everyThird == 3) {
			i++;
			everyThird = 0;
		}
		everyThird++;
#endif
		mhz << i;
		curOption.push_back(mhz.str());
	}
	m_optionsText.push_back(curOption);
	curOption.clear();
#if defined(GP2X)
	for(int i=65; i<=280; i+=5) {
#else
	everyThird = 0;
	for(int i=100; i<=600; i+=33) {
#endif
		ostringstream mhz;
#if !defined(GP2X)
		if(everyThird == 3) {
			i++;
			everyThird = 0;
		}
		everyThird++;
#endif
		mhz << i;
		curOption.push_back(mhz.str());
	}
	m_optionsText.push_back(curOption);

	curOption.clear();
	DIR * udir = opendir("languages/");
	if(udir != NULL) {
		struct dirent * dirent = readdir(udir);

		bool done = false;
		while(dirent != NULL) {
			if(dirent->d_name[0] != '.') {
				string ename = "languages/";
				ename += dirent->d_name;
				struct stat s;

				if (stat(ename.c_str(), &s) < 0) {
					string msg = "error calling stat on ";
					msg += ename;
					throw runtime_error(msg.c_str());
				}

				curOption.push_back(dirent->d_name);
			}
			dirent = readdir(udir);
		}
	}
	m_optionsText.push_back(curOption);
	closedir(udir);	
	curOption.clear();

	udir = opendir("skins/");

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

				if (S_ISDIR(s.st_mode) && 
					(strncmp(dirent->d_name, "default", 7) && 
							strncmp(dirent->d_name, "fonts", 5))) 
					curOption.push_back(dirent->d_name);
			}
			dirent = readdir(udir);
		}
	}
	m_optionsText.push_back(curOption);
	closedir(udir);	
	
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

	curIter = find(m_optionsText[2].begin(), m_optionsText[2].end(), m_config.getItem("language"));
	if(curIter == m_optionsText[2].end())
		curIter = m_optionsText[2].begin();
	m_optionsIters.push_back(curIter);
	curIter = find(m_optionsText[3].begin(), m_optionsText[3].end(), m_config.getItem("realSkin"));
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

void PlayerSettings::saveOptions()
{
	string oldSkin = m_config.getItem("realSkin");
	string oldSpeed = m_config.getItem("cpuSpeed");
	string oldSpeedLocked = m_config.getItem("cpuSpeedLocked");
	string oldLang = m_config.getItem("language");
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
				name = "language";
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
	if(oldSkin == m_config.getItem("realSkin"))
		; //
		
}

void PlayerSettings::updateMpdConf()
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

int PlayerSettings::selectedAction()
{
	int action;
	action= m_listing[m_curItemNum].second;
	return action;
}


int PlayerSettings::processCommand(int command, GuiPos& guiPos) 
{
	if(!m_good)
		initAll();
	int rCommand = command;
	int selAction = selectedAction();

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
		if(guiPos.curY > m_clearRect.y && (guiPos.curY < m_clearRect.y + m_clearRect.h))	 {
			if(guiPos.curX < (m_clearRect.w+m_clearRect.x)) {
				int curcur = m_curItemNum;
				m_curItemNum = m_topItemNum + m_itemIndexLookup[guiPos.curY];		
				if(m_curItemNum > m_listing.size())
					m_curItemNum = m_listing.size() -1;
				if(m_curItemNum < 5) {
					if(curcur == m_curItemNum) {
						if(guiPos.curX < (m_clearRect.x+55)) 
							command = CMD_LEFT;
						else
							command = CMD_RIGHT;	
					}	
					rCommand = 0;
					m_refresh = true;
				} else if(m_curItemNum < 9) {
					m_keyboard.init(CMD_POP_CHG_OPTION, getSelOptionText());
					rCommand = CMD_SHOW_KEYBOARD;
					m_refresh = true;
				} else  {
					command = CMD_POP_SELECT;
					m_refresh = true;
				}	
			} else if(guiPos.curX > (m_clearRect.w+m_clearRect.x)) {
					if(m_curItemNum == m_topItemNum + m_itemIndexLookup[guiPos.curY]) {	
						if(guiPos.curX < (m_clearRect.w+m_clearRect.x)) 
							command = CMD_LEFT;
						else
							command = CMD_RIGHT;
						m_refresh = true;
					}
			}
		} 
	}
	switch (command) {
		case CMD_POP_SELECT:
				if(m_curItemNum >=4 && m_curItemNum <9) {
					m_keyboard.init(CMD_POP_CHG_OPTION, getSelOptionText());
					rCommand = CMD_SHOW_KEYBOARD;
				} else if(selectedAction() == 0) {
					m_topItemNum = 0;
					m_curItemNum = 0;
					rCommand = CMD_MENU_SETTINGS;
				} else if(selectedAction() == 8) {
					saveOptions();
					m_topItemNum = 0;
					m_curItemNum = 0;
					rCommand = CMD_MENU_SETTINGS;
				}	
			break;
		case CMD_POP_CHG_OPTION:
			(*m_optionsIters[m_curItemNum]) = m_keyboard.getText();
			m_refresh = true;
			break;
		case CMD_DOWN:
			++Scroller::m_curItemNum;
			if(Scroller::m_curItemNum > Scroller::m_lastItemNum) {
				Scroller::m_topItemNum = Scroller::m_curItemNum = 0;
			} else 	if(Scroller::m_curItemNum >= Scroller::m_numPerScreen+Scroller::m_topItemNum) {
				++Scroller::m_topItemNum;
			}
			m_refresh = true;
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
			m_refresh = true;
			rCommand = 0;
			break;
		case CMD_LEFT:
				if (m_optionsIters[m_curItemNum] == m_optionsText[m_curItemNum].begin())
					m_optionsIters[m_curItemNum] = m_optionsText[m_curItemNum].end() - 1;
				else
					m_optionsIters[m_curItemNum]--;
				rCommand = 0;
			m_refresh = true;
			break;
		case CMD_RIGHT:
				m_optionsIters[m_curItemNum]++;
				if (m_optionsIters[m_curItemNum] == m_optionsText[m_curItemNum].end())
					m_optionsIters[m_curItemNum] = m_optionsText[m_curItemNum].begin();
				rCommand = 0;
			m_refresh = true;
			break;
	}

	return rCommand;

}

string PlayerSettings::getSelOptionText()
{

	return *m_optionsIters[m_curItemNum];

}


void PlayerSettings::draw(bool forceRefresh, long timePerFrame, bool inBack)
{	
	if(!m_good)
		initAll();
	/*clear this portion of the screen 
	SDL_SetClipRect(m_screen, &m_borderRect);
	SDL_FillRect(m_screen, &m_borderRect, SDL_MapRGB(m_screen->format, m_borderColor.r, m_borderColor.g, m_borderColor.b));
	SDL_FillRect(m_screen, &m_clearRect, SDL_MapRGB(m_screen->format, m_backColor.r, m_backColor.g, m_backColor.b));
	*/
	if(forceRefresh || (!inBack && m_refresh)) {
		SDL_SetClipRect(m_screen, &m_clearRect);
		SDL_BlitSurface(m_bg, &m_clearRect, m_screen, &m_clearRect );
		SDL_BlitSurface(m_plSurfaceText,NULL, m_screen, &m_destRect );
		m_destRect.y += m_skipVal*2;
		m_curItemClearRect.y += m_skipVal*2;
		{
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
		m_refresh = false;
	}
}

