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

#include "commandFactory.h"
#include "threadParms.h"
#include <stdexcept>
#include <SDL.h>
#include <iostream>

#define DELAY 800000
using namespace std;
CommandFactory::CommandFactory(mpd_Connection* mpd, vector<int>& volScale)
: m_keyDown(-1)
, m_keyDown2(-1)
, m_volScale(volScale)
, m_mouseMove(false)
, m_repeating(false)
, m_infiniteRepeat(false)
, m_prevX(0)
, m_prevY(0)
, m_combo(false)
{
	m_delayTimer.stop();
    ifstream configFile("keys", ios::in);
    if(configFile.fail()) {
		std::string msg = "CONFIG: Unable to open key config file: keys";
        throw runtime_error(msg.c_str());
	}
	readKeyConfigFile(configFile);
}

long CommandFactory::getHoldTime()
{
	if(m_delayTimer.active())
		return m_delayTimer.check();
	else 
		return 0;
}

int CommandFactory::keyDown(int key, int curMode)
{
	int command = 0;
	if(m_keyDown == -1) {
		m_keyDown = key;
		m_delayTimer.start();
		command = processKeyDown(curMode);			
	} else if (m_keyDown != key) {
		//do processing for key combo
		m_keyDown2 = key;
		m_combo = true;
		command = processKeyUp(curMode);
		m_keyDown = -1;
		m_keyDown2 = -1;
		m_delayTimer.stop();
		m_repeating = false;
		m_mouseMove = false;
		m_combo = false;
	} else { //same key still down
	}

	return command;
}

int CommandFactory::mouseDown(int curMode, int guiX, int guiY)
{
	int command = 0;
	if(m_keyDown == -1) {
		m_keyDown = 9999;
		m_prevX = guiX;
		m_prevY = guiY;
		m_delayTimer.start();
	}

	return command;
}


int CommandFactory::checkRepeat(int command, int prevCommand, int curMode, int& guiX, int& guiY)
{
	int rc = command;
	if(m_keyDown != -1 && m_delayTimer.active()) {
		long delayTime = m_delayTimer.check();
		if(m_keyDown == 9999) {
			SDL_GetMouseState(&guiX, &guiY);
			if(delayTime > DELAY && !m_mouseMove) {	
				rc = processDelayKey(curMode);
				m_repeating = true;
				if(prevCommand != CMD_RIGHT && prevCommand != CMD_LEFT 
						&& prevCommand != CMD_UP && prevCommand != CMD_DOWN
						&& prevCommand != CMD_VOL_UP && prevCommand != CMD_VOL_DOWN) {
					m_infiniteRepeat = false;
					//if(!m_infiniteRepeat) 
					//	m_delayTimer.stop();
				}
			} else {
				rc = processMouseMove(command, curMode, guiX, guiY);
			}
		} 
		else if(delayTime > DELAY) {
			//do processing for delaykey
			rc = processDelayKey(curMode);
			m_repeating = true;
			if(!m_infiniteRepeat) 
				m_delayTimer.stop();
		} 

	}
	
	return rc;
}

int CommandFactory::keyUp(int key, int curMode)
{
	int command = 0;
	if(m_keyDown == key && !m_repeating) {
		//do processing for key
		command = processKeyUp(curMode);
	}
	m_keyDown = -1;
	m_repeating = false;
	m_mouseMove = false;
	m_delayTimer.stop();
	return command;
}

int CommandFactory::keyPopup(int key, int curMode, int command)
{
	int rCommand = command;
	if(m_keyDown == key && !m_repeating) {
		//do processing for key
		if(checkKey("MENU_SELECT")) { 
			rCommand = CMD_POP_SELECT;
		}
	}
	m_keyDown = -1;
	m_repeating = false;
	m_mouseMove = false;
	m_delayTimer.stop();
	return rCommand;
}

int CommandFactory::mouseUp(int curMode, int guiX, int guiY)
{
	int command = 0;
	if(m_keyDown == 9999 && !m_repeating && !m_mouseMove) {
		//do processing for key
		if(guiX < m_prevX-18) {
			;
		}else if(guiX > m_prevX+18) {
			;
		}else if(guiY < m_prevY-18) {
			;
		}else if(guiY > m_prevY+18) {
			;
		} else {
			command = CMD_CLICK;
		}
	}
	m_keyDown = -1;
	m_repeating = false;
	m_mouseMove = false;
	m_delayTimer.stop();
	return command;
}
/*
		cout << "guix x y " << guiX << "x" << guiY << endl;
		cout << "prex x y " << m_prevX << "x" << m_prevY << endl;
	//} else if(guiX == m_prevX && guiY == m_prevY){
		command = CMD_CLICK;
*/
int CommandFactory::processMouseMove(int preCommand, int curMode, int guiX, int guiY)
{
	int command = preCommand;

	if(guiX < m_prevX-40) {
		m_mouseMove = true;
		command = CMD_MOUSE_RIGHT;
		m_prevX = guiX;
	}else if(guiX > m_prevX+40) {
		m_mouseMove = true;
		m_prevX = guiX;
		command = CMD_MOUSE_LEFT;
	}else if(guiY < m_prevY-40) {
		m_mouseMove = true;
		command = CMD_MOUSE_UP;
	}else if(guiY > m_prevY+40) {
		m_mouseMove = true;
		command = CMD_MOUSE_DOWN;
	}

	return command;
}


int CommandFactory::processKeyDown(int curMode)
{
	int command = 0;
	if(checkKey("RIGHT")) {
		command = CMD_RIGHT;
	} else if(checkKey("LEFT")) {
		command = CMD_LEFT;
	} else if(checkKey("UP")) {
		command = CMD_UP;
	} else if(checkKey("DOWN")) {
		command = CMD_DOWN;
	} else if(checkKey("VOL_UP")) {
		command = CMD_VOL_UP;
	} else if(checkKey("VOL_DOWN")) {
		command = CMD_VOL_DOWN;
	}	
	return command;
	
}

int CommandFactory::processKeyUp(int curMode)
{
	int command = 0;
	if(checkKey("SHOW_CONTROLS")) {
		command = CMD_SHOW_OVERLAY;
	} else if(checkKey("DETACH_CLIENT")) {
		command = CMD_DETACH;
	} else if(checkKey("SHOW_MENU")) {
		command = CMD_SHOW_MENU;
	} else if(checkKey("LOCK")) {
		command = CMD_TOGGLE_SCREEN;
	} else if(checkKey("TOGGLE_MODE")) {
		command = CMD_TOGGLE_MODE;
	} else if(checkKey("TOGGLE_VIEW")) {
		command = CMD_TOGGLE_VIEW;
	} else if(checkKey("MENU_SELECT")) {
		command = CMD_POP_SELECT;
	} else if(checkKey("MENU_CANCEL")) {
		command = CMD_POP_CANCEL;
	} else if(checkKey("MENU_HELP")) {
		command = CMD_POP_HELP;
	}
	switch(curMode) {
		case 0: //Library
			if(checkKey("LIB_SELECT")) {
				command = CMD_IMMEDIATE_PLAY;
			} else if(checkKey("LIB_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkKey("LIB_PREV_DIR")) {
				command = CMD_PREV_DIR;
			} else if(checkKey("LIB_STOP")) {
				command = CMD_STOP;
			} else if(checkKey("LIB_ADD_TO_PL")) {
				command = CMD_ADD_TO_PL;
			} else if(checkKey("LIB_ADD_AS_PL")) {
				command = CMD_ADD_AS_PL;
			} else if(checkKey("LIB_QUEUE")) {
				command = CMD_QUEUE;
			}
		break;
		case 1: //Playlist
			if(checkKey("PL_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkKey("PL_STOP")) {
				command = CMD_STOP;
			} else if(checkKey("PL_NEXT")) {
				command = CMD_NEXT;
			} else if(checkKey("PL_FF")) {
				command = CMD_FF;
			} else if(checkKey("PL_PREV")) {
				command = CMD_PREV;
			} else if(checkKey("PL_REW")) {
				command = CMD_RW;
			} else if(checkKey("PL_TOGGLE_RND_RPT")) {
				command = CMD_RAND_RPT ;
			} else if(checkKey("PL_RND")) {
				command = CMD_MODE_RANDOM;
			} else if(checkKey("PL_RPT")) {
				command = CMD_MODE_REPEAT;
			} else if(checkKey("PL_REMOVE_FROM_PL")) {
				command = CMD_DEL_FROM_PL;
			} else if(checkKey("PL_MOVE_IN_PL")) {
				command = CMD_MOVE_IN_PL;
			} else if(checkKey("PL_QUEUE_NEXT")) {
				command = CMD_QUEUE;
			}
		break;
		case 2: //Playing
			if(checkKey("PL_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkKey("PL_STOP")) {
				command = CMD_STOP;
			} else if(checkKey("PL_NEXT")) {
				command = CMD_NEXT;
			} else if(checkKey("PL_FF")) {
				command = CMD_FF;
			} else if(checkKey("PL_PREV")) {
				command = CMD_PREV;
			} else if(checkKey("PL_REW")) {
				command = CMD_RW;
			} else if(checkKey("PL_TOGGLE_RND_RPT")) {
				command = CMD_RAND_RPT ;
			} else if(checkKey("PL_RND")) {
				command = CMD_MODE_RANDOM;
			} else if(checkKey("PL_RPT")) {
				command = CMD_MODE_REPEAT;
			} else if(checkKey("NP_FLIP_ART")) {
				command = CMD_FLIP_ART;
			}
		break;
		case 3: //PL browser
			if(checkKey("PLBROWSE_SELECT")) {
				command = CMD_LOAD_PL;
			} else if(checkKey("PLBROWSE_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkKey("PLBROWSE_STOP")) {
				command = CMD_STOP;
			} else if(checkKey("PLBROWSE_APPEND")) {
				command = CMD_APPEND_PL;
			} else if(checkKey("PLBROWSE_DEL")) {
				command = CMD_DEL_PL;
			}
		break;
		case 4: //Bookmarks
			if(checkKey("BOOKMRK_SELECT")) {
				command = CMD_LOAD_BKMRK;
			} else if(checkKey("BOOKMRK_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkKey("BOOKMRK_STOP")) {
				command = CMD_STOP;
			} else if(checkKey("PLBROWSE_DEL")) {
				command = CMD_DEL_BKMRK;
			}
		break;
		case 5: //menu
			if(checkDelayKey("MENU_SELECT")) {
				command = CMD_MENU_SELECT;
			}
		break;
	}	
	return command;
	
}

int CommandFactory::processDelayKey(int curMode)
{
	int command = 0;
	if(m_keyDown == 9999) {
		command = CMD_HOLD_CLICK;
		m_infiniteRepeat = true;
	} else if(checkDelayKey("RIGHT")) {
		command = CMD_RIGHT;
		m_infiniteRepeat = true;
	} else if(checkDelayKey("LEFT")) {
		command = CMD_LEFT;
		m_infiniteRepeat = true;
	} else if(checkDelayKey("UP")) {
		command = CMD_UP;
		m_infiniteRepeat = true;
	} else if(checkDelayKey("DOWN")) {
		command = CMD_DOWN;
		m_infiniteRepeat = true;
	} else if(checkDelayKey("VOL_UP")) {
		command = CMD_VOL_UP;
		m_infiniteRepeat = true;
	} else if(checkDelayKey("VOL_DOWN")) {
		command = CMD_VOL_DOWN;
		m_infiniteRepeat = true;
	} else if(checkDelayKey("SHOW_CONTROLS")) {
		command = CMD_SHOW_OVERLAY;
	} else if(checkDelayKey("DETACH_CLIENT")) {
		command = CMD_DETACH;
	} else if(checkDelayKey("SHOW_MENU")) {
		command = CMD_SHOW_MENU;
	} else if(checkDelayKey("LOCK")) {
		command = CMD_TOGGLE_SCREEN;
	} else if(checkDelayKey("TOGGLE_MODE")) {
		command = CMD_TOGGLE_MODE;
	} else if(checkDelayKey("TOGGLE_VIEW")) {
		command = CMD_TOGGLE_VIEW;
	} else if(checkDelayKey("MENU_SELECT")) {
		command = CMD_POP_SELECT;
	} else if(checkDelayKey("MENU_CANCEL")) {
		command = CMD_POP_CANCEL;
	}
	switch(curMode) {
		case 0: //Library
			if(checkDelayKey("LIB_SELECT")) {
				command = CMD_IMMEDIATE_PLAY;
			} else if(checkDelayKey("LIB_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkDelayKey("LIB_PREV_DIR")) {
				command = CMD_PREV_DIR;
			} else if(checkDelayKey("LIB_STOP")) {
				command = CMD_STOP;
			} else if(checkDelayKey("LIB_ADD_TO_PL")) {
				command = CMD_ADD_TO_PL;
			} else if(checkDelayKey("LIB_ADD_AS_PL")) {
				command = CMD_ADD_AS_PL;
			} else if(checkDelayKey("LIB_QUEUE")) {
				command = CMD_QUEUE;
			}
		break;
		case 1: //Playlist
		case 2: //Playing
			if(checkDelayKey("PL_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkDelayKey("PL_STOP")) {
				command = CMD_STOP;
			} else if(checkDelayKey("PL_NEXT")) {
				command = CMD_NEXT;
			} else if(checkDelayKey("PL_FF")) {
				command = CMD_FF;
				m_infiniteRepeat = true;
			} else if(checkDelayKey("PL_PREV")) {
				command = CMD_PREV;
			} else if(checkDelayKey("PL_REW")) {
				command = CMD_RW;
				m_infiniteRepeat = true;
			} else if(checkDelayKey("PL_TOGGLE_RND_RPT")) {
				command = CMD_RAND_RPT ;
			} else if(checkDelayKey("PL_RND")) {
				command = CMD_MODE_RANDOM;
			} else if(checkDelayKey("PL_RPT")) {
				command = CMD_MODE_REPEAT;
			} else if(checkDelayKey("PL_REMOVE_FROM_PL")) {
				command = CMD_DEL_FROM_PL;
			} else if(checkDelayKey("PL_MOVE_IN_PL")) {
				command = CMD_MOVE_IN_PL;
			} else if(checkDelayKey("PL_QUEUE_NEXT")) {
				command = CMD_QUEUE;
			}
		break;
		case 3: //PL browser
			if(checkDelayKey("PLBROWSE_SELECT")) {
				command = CMD_LOAD_PL;
			} else if(checkDelayKey("PLBROWSE_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkDelayKey("PLBROWSE_STOP")) {
				command = CMD_STOP;
			} else if(checkDelayKey("PLBROWSE_APPEND")) {
				command = CMD_APPEND_PL;
			} else if(checkDelayKey("PLBROWSE_DEL")) {
				command = CMD_DEL_PL;
			}
		break;
		case 4: //Bookmarks
			if(checkDelayKey("BOOKMRK_SELECT")) {
				command = CMD_LOAD_BKMRK;
			} else if(checkDelayKey("BOOKMRK_PLAY_PAUSE")) {
				command = CMD_PLAY_PAUSE;
			} else if(checkDelayKey("BOOKMRK_STOP")) {
				command = CMD_STOP;
			} else if(checkDelayKey("PLBROWSE_DEL")) {
				command = CMD_DEL_BKMRK;
			}
		break;
		case 5: //menu
			if(checkDelayKey("MENU_SELECT")) {
				command = CMD_MENU_SELECT;
			}
		break;
		
	}	
	return command;
}

int CommandFactory::processKeyCombo(int key, int curMode)
{

}

bool CommandFactory::checkDelayKey(string keyName)
{
	bool rc = false;
	map<string, vector<int> >::iterator findIter;
	findIter = m_holdKeys.find(keyName);
	if(findIter != m_holdKeys.end()) {
		for(vector<int>::iterator vIter = (*findIter).second.begin();
				vIter != (*findIter).second.end();
				++vIter) {
			if((*vIter) == m_keyDown)
				rc = true;
		}
	}
	return rc;
}

bool CommandFactory::checkKey(string keyName)
{
	bool rc = false;
	if(!m_combo) {
		map<string, vector<int> >::iterator findIter;
		findIter = m_keys.find(keyName);
		if(findIter != m_keys.end()) {
			for(vector<int>::iterator vIter = (*findIter).second.begin();
					vIter != (*findIter).second.end();
					++vIter) {
				if((*vIter) == m_keyDown)
					rc = true;
			}
		}
	} else {
		map<string, vector<pair<int, int> > >::iterator findIter;
		findIter = m_comboKeys.find(keyName);
		if(findIter != m_comboKeys.end()) {
			for(vector<pair<int, int> >::iterator vIter = (*findIter).second.begin();
					vIter != (*findIter).second.end();
					++vIter) {
				if((*vIter).first == m_keyDown 
						&& (*vIter).second == m_keyDown2 ||
					(*vIter).first == m_keyDown2 
						&& (*vIter).second == m_keyDown )
					rc = true;
			}
		}
	}
	return rc;
}

void CommandFactory::processValue(string item, string value)
{
	bool holdValue = false;
	bool comboValue = false;
	bool both = false;
	size_t lastPos = 0;
	size_t pos;
	pos = value.find(',', lastPos);
	int pass = 1;
	while(pos != string::npos || pass == 1) {
		string curValue; 
		string curValue2; 
		if(pos == string::npos)
			curValue = value;
		else	
			curValue = value.substr(lastPos,  pos-lastPos);
		int insertVal = 0;	
		int insertVal2 = 0;	
		if(curValue.substr(0, 5) == "HOLD_") {
			curValue = curValue.substr(5);
			holdValue = true;
		}
		if(curValue.find("+") != string::npos) {
			int pos = curValue.find("+");
			curValue2 = curValue.substr(pos+1);
			curValue = curValue.substr(0, pos);
			comboValue = true;
		}
		if(item == "RIGHT" || item == "LEFT" || item == "UP" || item == "DOWN"
			|| item == "VOL_UP" || item == "VOL_DOWN") {
			both = true;
		}
		if(curValue == "GAME_B") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_FB;
#endif
		} else if(curValue == "GAME_A") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_FA;
#endif
		} else if(curValue == "GAME_X") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_FX;
#endif
		} else if(curValue == "GAME_Y") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_FY;
#endif
		} else if(curValue == "GAME_L") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_FL;
#endif
		} else if(curValue == "GAME_R") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_FR;
#endif
		} else if(curValue == "GAME_START") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_START;
#endif
		} else if(curValue == "GAME_SELECT") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_SELECT;
#endif
		} else if(curValue == "GAME_VOL_UP") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_VOL_UP;
#endif
		} else if(curValue == "GAME_VOL_DOWN") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_VOL_DOWN;
#endif
		} else if(curValue == "GAME_UP") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_UP;
#endif
		} else if(curValue == "GAME_DOWN") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_DOWN;
#endif
		} else if(curValue == "GAME_LEFT") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_LEFT;
#endif
		} else if(curValue == "GAME_RIGHT") {
#if defined(GP2X) || defined(WIZ)
			insertVal = GP2X_VK_RIGHT;
#endif
		} 
		else if(curValue == "Escape")
			insertVal = SDLK_ESCAPE;
		else if(curValue == "Right")
			insertVal = SDLK_RIGHT;
		else if(curValue == "Left")
			insertVal = SDLK_LEFT;
		else if(curValue == "Up")
			insertVal = SDLK_UP;
		else if(curValue == "Down")
			insertVal = SDLK_DOWN;
		else if(curValue == "Space")
			insertVal = SDLK_SPACE;
		else
			insertVal = curValue[0];
	//repeat for possible combo	
		if(curValue2 == "GAME_B") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_FB;
#endif
		} else if(curValue2 == "GAME_A") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_FA;
#endif
		} else if(curValue2 == "GAME_X") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_FX;
#endif
		} else if(curValue2 == "GAME_Y") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_FY;
#endif
		} else if(curValue2 == "GAME_L") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_FL;
#endif
		} else if(curValue2 == "GAME_R") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_FR;
#endif
		} else if(curValue2 == "GAME_START") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_START;
#endif
		} else if(curValue2 == "GAME_SELECT") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_SELECT;
#endif
		} else if(curValue2 == "GAME_VOL_UP") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_VOL_UP;
#endif
		} else if(curValue2 == "GAME_VOL_DOWN") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_VOL_DOWN;
#endif
		} else if(curValue2 == "GAME_UP") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_UP;
#endif
		} else if(curValue2 == "GAME_DOWN") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_DOWN;
#endif
		} else if(curValue2 == "GAME_LEFT") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_LEFT;
#endif
		} else if(curValue2 == "GAME_RIGHT") {
#if defined(GP2X) || defined(WIZ)
			insertVal2 = GP2X_VK_RIGHT;
#endif
		} 
		else if(curValue2 == "Escape")
			insertVal2 = SDLK_ESCAPE;
		else if(curValue2 == "Right")
			insertVal2 = SDLK_RIGHT;
		else if(curValue2 == "Left")
			insertVal2 = SDLK_LEFT;
		else if(curValue2 == "Up")
			insertVal2 = SDLK_UP;
		else if(curValue2 == "Down")
			insertVal2 = SDLK_DOWN;
		else if(curValue2 == "Space")
			insertVal2 = SDLK_SPACE;
		else
			insertVal2 = curValue2[0];

		if(pass == 1) {
			if(!comboValue) {
				vector<int> tmp;
				tmp.push_back(insertVal);
				if(holdValue || both) {
					m_holdKeys.insert(make_pair(item, tmp));
				}
				if(!holdValue || both)
					m_keys.insert(make_pair(item, tmp));
			} else {
				vector<pair<int,int> > tmp;
				tmp.push_back(make_pair(insertVal, insertVal2));
				m_comboKeys.insert(make_pair(item, tmp));
			}	
		}
		else {
			if(!comboValue) {
				map<string, vector<int> >::iterator findIter;

				if(holdValue || both) {
					findIter = m_holdKeys.find(item);
					if(findIter != m_holdKeys.end())
						(*findIter).second.push_back(insertVal);
				}
				if(!holdValue || both){
					findIter = m_keys.find(item);
					if(findIter != m_keys.end())
						(*findIter).second.push_back(insertVal);
				}
			} else {
				map<string, vector<pair<int,int> > >::iterator findIter;
				findIter = m_comboKeys.find(item);
				if(findIter != m_comboKeys.end())
					(*findIter).second.push_back(
									make_pair(insertVal,insertVal2));

			}
		}
		++pass;	
		lastPos = pos+1;
		pos = value.find(',', lastPos);
	}


}

void CommandFactory::readKeyConfigFile(ifstream& configFile)
{
    std::string curItem;
    while(!configFile.eof()) {
        getline(configFile, curItem);
        if(!curItem.empty() && curItem[0] != '#') {
            int pos = curItem.find('=');
            std::string itemName = curItem.substr(0, pos);
            std::string itemValue = curItem.substr(pos+1);
            trimStr(itemName);
            trimStr(itemValue);
          	processValue(itemName, itemValue); 
        }
	}
}

int CommandFactory::getCommand(bool keysHeld[], int curMode, int& repeatDelay, bool popupVisible, bool overlayVisible, int volume, long delayTime)
{
	int command = 0;
/*
	if(repeatDelay == 1 || delayTime > DELAY) {
		//common commands
		if (keysHeld[400]) {
			if(repeatDelay == 1 || delayTime > DELAY*2)
				command = CMD_CLICK;
		}
		if (keysHeld[SDLK_ESCAPE])
			command = CMD_DETACH;
		else if(keysHeld[SDLK_o])
			command = CMD_SHOW_OVERLAY;	
		else if (keysHeld[GP2X_VK_UP] || keysHeld[SDLK_UP]||keysHeld[SDLK_k])
			command = CMD_UP;	
		else if (keysHeld[GP2X_VK_DOWN] || keysHeld[SDLK_DOWN]||keysHeld[SDLK_j])
			command = CMD_DOWN;
		else if (keysHeld[GP2X_VK_LEFT] || keysHeld[SDLK_UP]||keysHeld[SDLK_LEFT])
			command = CMD_LEFT;	
		else if (keysHeld[GP2X_VK_RIGHT] || keysHeld[SDLK_RIGHT]||keysHeld[SDLK_RIGHT])
			command = CMD_RIGHT;
		else if (keysHeld[GP2X_VK_VOL_UP] || keysHeld[SDLK_0])
			command = CMD_VOL_UP;
		else if (keysHeld[GP2X_VK_VOL_DOWN] || keysHeld[SDLK_9])
			command = CMD_VOL_DOWN;
		else if (keysHeld[GP2X_VK_SELECT] || keysHeld[SDLK_m]) {
			if(!m_delayCommand) {
				if(delayTime >= DELAY) {
					command	= CMD_TOGGLE_VIEW;
					m_select = false;
					m_delayCommand = true;
				} else if(delayTime < DELAY){
					m_select = true;
				}
			}
		} else if (keysHeld[GP2X_VK_START] || keysHeld[SDLK_c]) {
			if(!m_delayCommand) {
				if(delayTime >= DELAY*3) {
					command	= CMD_TOGGLE_SCREEN;
					m_start = false;
					m_delayCommand = true;
				} else if(delayTime < DELAY*3){
					m_start = true;
				}
			}
		} else if (keysHeld[GP2X_VK_FB] || keysHeld[SDLK_p]) {
			if(popupVisible) {
					command = CMD_POP_SELECT;
			} else {
				if(!m_delayCommand) {
					if(delayTime >= DELAY) {
						command	= CMD_PAUSE;
						m_play = false;
						m_delayCommand = true;
					} else if(delayTime < DELAY){
						m_play = true;
					}
				}
			}
		} else {
			if(popupVisible) {
				if (keysHeld[GP2X_VK_FX] || keysHeld[SDLK_s])
					command = CMD_POP_CANCEL;
				if (!keysHeld[GP2X_VK_START] && !keysHeld[SDLK_c]) {
					if(m_start) {
						command	= CMD_SHOW_MENU;
						m_start = false;
					}
					m_delayCommand = false;
				}
				if (!keysHeld[GP2X_VK_FX]) {
					if(m_prevDir) {
						command = CMD_PREV_DIR;
						m_prevDir = false;
					}
					m_delayCommand = false;
				}
				if (!keysHeld[GP2X_VK_FY] && !keysHeld[SDLK_y]) {
					if(m_move) {
						command	= CMD_MOVE_IN_PL;
						m_move = false;
					}
					m_delayCommand = false;
				}
			} else {
				switch(curMode) {
					case 0:
						{ //song browser
							if (keysHeld[GP2X_VK_FA] || keysHeld[SDLK_i])
								command = CMD_ADD_TO_PL;
							else if (keysHeld[GP2X_VK_FX] || keysHeld[SDLK_s]) {
								if(!m_delayCommand) {
									if(delayTime >= DELAY) {
										command	= CMD_STOP;
										m_prevDir = false;
										m_delayCommand = true;
									} else if(delayTime < DELAY){
										m_prevDir = true;
									}
								}
							} else if (keysHeld[GP2X_VK_FY] || keysHeld[SDLK_y]) {
								if(!m_delayCommand) {
									if(delayTime >= DELAY) {
										command	= CMD_QUEUE;
										m_addAsPl = false;
										m_delayCommand = true;
									} else if(delayTime < DELAY){
										m_addAsPl = true;
									}
								}
							}
							else {
								if (!keysHeld[GP2X_VK_FB]) {
									if(m_play) {
										command	= CMD_IMMEDIATE_PLAY;
										m_play = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_START] && !keysHeld[SDLK_c]) {
									if(m_start) {
										command	= CMD_SHOW_MENU;
										m_start = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_FX]) {
									if(m_prevDir) {
										command = CMD_PREV_DIR;
										m_prevDir = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_SELECT]) {
									if(m_select) {
										command = CMD_TOGGLE_MODE;
										m_select = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_FY] && !keysHeld[SDLK_y]) {
									if(m_addAsPl) {
										command	= CMD_ADD_AS_PL;
										m_addAsPl = false;
									}
									m_delayCommand = false;
								}
							}
						}
						break;
					case 1:
						{ //playlist
							if (keysHeld[GP2X_VK_FX])
								command = CMD_STOP;
							else if (keysHeld[GP2X_VK_FR] || keysHeld[SDLK_n]) {
								if(delayTime > DELAY) {
									if(volume != 0)
										m_volume = volume;
									m_setVol = true;
									mpd_sendSetvolCommand(m_mpd, 0);
									mpd_finishCommand(m_mpd);
									command = CMD_FF;
									m_next = false;
								}
								else
									m_next = true;
							} else if (keysHeld[GP2X_VK_FL] || keysHeld[SDLK_b]) {
								if(delayTime > DELAY) {
									if(volume != 0)
										m_volume = volume;
									m_setVol = true;
									mpd_sendSetvolCommand(m_mpd, 0);
									mpd_finishCommand(m_mpd);
									command = CMD_RW;	
									m_prev = false;
								}
								else
									m_prev = true;
							} else if (keysHeld[GP2X_VK_FA] || keysHeld[SDLK_s]) {
								if(!m_delayCommand) {
									if(delayTime >= DELAY) {
										command	= CMD_DEL_FROM_PL;
										m_rand = false;
										m_delayCommand = true;
									} else if(delayTime < DELAY){
										m_rand = true;
									}
								}
							} else if (keysHeld[GP2X_VK_FY] || keysHeld[SDLK_y]) {
								if(!m_delayCommand) {
									if(delayTime >= DELAY) {
										command	= CMD_QUEUE;
										m_move = false;
										m_delayCommand = true;
									} else if(delayTime < DELAY){
										m_move = true;
									}
								}
							} else if (keysHeld[SDLK_r]) {
								command = CMD_MODE_RANDOM;
							}  else if (keysHeld[SDLK_t]) {
								command = CMD_MODE_REPEAT;
							} else { 
								if (!keysHeld[GP2X_VK_FB]) {
									if(m_play) {
										command	= CMD_PLAY_PAUSE;
										m_play = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_FA]) {
									if(m_rand) {
										command	= CMD_RAND_RPT;
										m_rand = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_FY] && !keysHeld[SDLK_y]) {
									if(m_move) {
										command	= CMD_MOVE_IN_PL;
										m_move = false;
									}
									m_delayCommand = false;
								}
								if(!keysHeld[GP2X_VK_FR] && !keysHeld[SDLK_n]) {
									if(m_next) {
										command = CMD_NEXT;
										m_next = false;
									} else if(m_setVol){
										mpd_sendSetvolCommand(m_mpd, m_volScale[m_volume]);
										mpd_finishCommand(m_mpd);
										m_setVol = false;
									}
								}
								if (!keysHeld[GP2X_VK_FL] && !keysHeld[SDLK_b]) {
									if(m_prev) {
										command = CMD_PREV;
										m_prev = false;
									} else if(m_setVol){
										mpd_sendSetvolCommand(m_mpd, m_volScale[m_volume]);
										mpd_finishCommand(m_mpd);
										m_setVol = false;
									}
								}
								if (!keysHeld[GP2X_VK_START] && !keysHeld[SDLK_c]) {
									if(m_start) {
										command	= CMD_SHOW_MENU;
										m_start = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_SELECT]) {
									if(m_select) {
										command = CMD_TOGGLE_MODE;
										m_select = false;
									}
									m_delayCommand = false;
								}
							}
						}
						break;
					case 2: 
						{ //playlist browser
							if (keysHeld[GP2X_VK_FX])
								command = CMD_STOP;
							else if (keysHeld[GP2X_VK_FA] || keysHeld[SDLK_s]) {
								if(!m_delayCommand) {
									if(delayTime >= DELAY) {
										command	= CMD_DEL_PL;
										m_append = false;
										m_delayCommand = true;
									} else if(delayTime < DELAY){
										m_append = true;
									}
								}
							}
							else { 
								if (!keysHeld[GP2X_VK_FB]) {
									if(m_play) {
										command	= CMD_LOAD_PL;
										m_play = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_START] && !keysHeld[SDLK_c]) {
									if(m_start) {
										command	= CMD_SHOW_MENU;
										m_start = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_SELECT]) {
									if(m_select) {
										command = CMD_TOGGLE_MODE;
										m_select = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_FA]) {
									if(m_append) {
										command	= CMD_APPEND_PL;
										m_append = false;
									}
									m_delayCommand = false;
								}
							}
						}
						break;
					case 3:
						{ //bookmark browser
							if (keysHeld[GP2X_VK_FX])
								command = CMD_STOP;
							else if (keysHeld[GP2X_VK_FA] || keysHeld[SDLK_s]) {
								if(!m_delayCommand) {
									if(delayTime >= DELAY) {
										command	= CMD_DEL_BKMRK;
										m_append = false;
										m_delayCommand = true;
									} else if(delayTime < DELAY){
										m_append = true;
									}
								}
							}
							else { 
								if (!keysHeld[GP2X_VK_FB]) {
									if(m_play) {
										command	= CMD_LOAD_BKMRK;
										m_play = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_START] && !keysHeld[SDLK_c]) {
									if(m_start) {
										command	= CMD_SHOW_MENU;
										m_start = false;
									}
									m_delayCommand = false;
								}
								if (!keysHeld[GP2X_VK_SELECT]) {
									if(m_select) {
										command = CMD_TOGGLE_MODE;
										m_select = false;
									}
									m_delayCommand = false;
								}
							}

						}
						break;
					default:
						command = 0;
				}
			}
		}
	}
*/
	return command;
}


int CommandFactory::getCommandWhileLocked(bool keysHeld[], int curMode, int& repeatDelay, bool popupVisible, long delayTime)
{
	int command = 0;
/*	
	if(repeatDelay == 1 || delayTime > DELAY) {
		if (keysHeld[GP2X_VK_VOL_UP] && keysHeld[GP2X_VK_FB])
			command = CMD_VOL_UP;
		else if (keysHeld[GP2X_VK_VOL_DOWN] && keysHeld[GP2X_VK_FB])
			command = CMD_VOL_DOWN;
		else if (keysHeld[GP2X_VK_FL] && keysHeld[GP2X_VK_FB])
			command = CMD_PREV;
		else if (keysHeld[GP2X_VK_FR] && keysHeld[GP2X_VK_FB])
			command = CMD_NEXT;
		else if (keysHeld[GP2X_VK_START]) {
			if(!m_delayCommand) {
				if(delayTime >= DELAY*3) {
					command	= CMD_TOGGLE_SCREEN;
					m_start = false;
					m_delayCommand = true;
				} else if(delayTime < DELAY*3){
					m_start = true;
				}
			}
		} else {
			if (!keysHeld[GP2X_VK_START]) {
				if(m_start) {
					m_start = false;
				}
				m_delayCommand = false;
			}
		}
	}
*/
	return command;
}

void CommandFactory::trimStr(std::string & inStr)
{
    while(inStr[0] == ' ' || inStr[0] == '\t' || inStr[0] == '\n')
        inStr = inStr.substr(1);

    while(inStr[inStr.length()-1] == ' '
            || inStr[inStr.length()-1] == '\t'
            || inStr[inStr.length()-1] == '\n')
        inStr = inStr.substr(0, inStr.length()-1);

}

