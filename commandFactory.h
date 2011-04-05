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

#ifndef __COMMANDS_H__ 
#define __COMMANDS_H__

#include "libmpdclient.h"
#include "timer.h"
#include "config.h"
#include <vector>


	typedef enum {	CMD_PLAY_PAUSE=1, CMD_PAUSE, CMD_STOP, CMD_PREV, CMD_NEXT, CMD_FF, CMD_RW,
					CMD_UP, CMD_DOWN, CMD_VOL_UP, CMD_VOL_DOWN,  
					CMD_ADD_TO_PL, CMD_NEW_PL, CMD_APPEND_PL, CMD_LOAD_PL, CMD_DEL_PL,
					CMD_DEL_FROM_PL, CMD_MOVE_IN_PL, CMD_MPD_UPDATE,
					CMD_IMMEDIATE_PLAY, CMD_SAVE_PL, CMD_POP_SELECT, CMD_POP_CANCEL,
					CMD_TOGGLE_MODE, CMD_TOGGLE_VIEW, CMD_SHOW_MENU, 
					CMD_LAUNCH_APP, CMD_DETACH, CMD_TOGGLE_SCREEN, CMD_PREV_DIR,
					CMD_LOAD_BKMRK, CMD_SAVE_BKMRK, CMD_DEL_BKMRK,
					CMD_SHOW_OPTIONS, CMD_SAVE_OPTIONS, CMD_LEFT, CMD_RIGHT,
			   		CMD_MODE_RANDOM, CMD_MODE_REPEAT, CMD_RAND_RPT, CMD_QUIT,
					CMD_MPD_ADD_ALL, CMD_SHOW_OVERLAY, CMD_CLICK, CMD_LAUNCH_PROCESS,
					CMD_SAVE_PL_KEYBOARD, CMD_SAVE_BKMRK_KEYBOARD, CMD_SAVE_PL_FROM_BROWSER,
					CMD_SHOW_KEYBOARD, CMD_HIDE_KEYBOARD, CMD_POP_KEYBOARD, CMD_POP_CHG_OPTION,
					CMD_FILTER_KEYBOARD, CMD_ADD_AS_PL, CMD_QUEUE,
CMD_MOUSE_UP, CMD_MOUSE_DOWN, CMD_MOUSE_LEFT, CMD_MOUSE_RIGHT,
CMD_HOLD_CLICK, CMD_SHOW_NP, CMD_SHOW_LIB, CMD_SHOW_PL, CMD_SHOW_PLS,
CMD_SHOW_BKMRKS, CMD_MENU_SETTINGS, CMD_MENU_SELECT, CMD_MENU_EXIT,
CMD_FLIP_ART, CMD_POP_HELP, CMD_POP_CONTEXT} cmdTypes_t;

class CommandFactory
{
public:
	CommandFactory(mpd_Connection* mpd, std::vector<int>& volScale);
	int getCommand(bool keysHeld[], int curMode, int& timer, bool popupVisible, bool overlayVisible, int vol, long delayTime);
	int getCommandWhileLocked(bool keysHeld[], int curMode, int& timer, bool popupVisible, long delayTime);

	int keyDown(int key, int curMode);
	int keyPopup(int key, int curMode, int command);
	int keyUp(int key, int curMode);
	int mouseDown(int curMode, int guiX=0, int guiY=0);
	int mouseUp(int curMode, int guiX=0, int guiY=0);
	int checkRepeat(int command, int prevCommand, int curMode, int& guiX, int& guiY);
	long getHoldTime();

protected:
	int processKeyDown(int curMode);
	int processKeyUp(int curMode);
	int processDelayKey(int curMode);
	int processKeyCombo(int key, int curMode);
	int processMouseMove(int command, int curMode, int guiX, int guiY);

	void processValue(std::string item, std::string value);
	void readKeyConfigFile(std::ifstream& configFile);
	bool checkKey(std::string keyName);
	bool checkDelayKey(std::string keyName);
	void trimStr(std::string & inStr);

	mpd_Connection* m_mpd;

	int m_keyDown;
	int m_keyDown2;
	bool m_combo;
	Timer m_delayTimer;
	std::map<std::string, std::vector<int> > m_keys;
	std::map<std::string, std::vector<int> > m_holdKeys;
	std::map<std::string, std::vector<std::pair<int, int> > > m_comboKeys;
	bool m_repeating;
	bool m_infiniteRepeat;
	int m_prevX;
	int m_prevY;
	bool m_mouseMove;

	int m_volume;
	bool m_setVol;
	bool m_delayCommand;
	std::vector<int> m_volScale;
};

#endif
