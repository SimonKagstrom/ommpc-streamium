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

#include <iostream>

#include "keyboard.h"
#include "config.h"
#include "guiPos.h"
#include "commandFactory.h"
#include "SDL_image.h"

using namespace std;

Keyboard::Keyboard(SDL_Surface* screen, Config& config)
: m_config(config)
, m_screen(screen)
, m_viewMore(false)
, m_shift(0)
, m_curRow(4)
, m_curCol(3)
, m_refresh(true)
, m_foundKey(false)
, m_counter(0)
, m_good(false)
{
}

void Keyboard::initAll() 
{
	SDL_Surface* tmpSurface = NULL;	
	string keyboardName = m_config.getItem("sk_keyboard");

	tmpSurface = IMG_Load(string("keyboards/"+keyboardName+"/keyboard_entry.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("keyboards/default/keyboard_entry.png").c_str());
	m_keyboardEntry = SDL_DisplayFormatAlpha(tmpSurface);
	SDL_FreeSurface(tmpSurface);
	
	tmpSurface = IMG_Load(string("keyboards/"+keyboardName+"/key.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("keyboards/default/key.png").c_str());
	m_keyBack = SDL_DisplayFormatAlpha(tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(string("keyboards/"+keyboardName+"/curkey.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("keyboards/default/curkey.png").c_str());
	m_curKeyBack = SDL_DisplayFormatAlpha(tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(string("keyboards/"+keyboardName+"/mediumKey.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("keyboards/default/mediumKey.png").c_str());
	m_mediumKey = SDL_DisplayFormatAlpha(tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(string("keyboards/"+keyboardName+"/curmediumKey.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("keyboards/default/curmediumKey.png").c_str());
	m_curMediumKey = SDL_DisplayFormatAlpha(tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(string("keyboards/"+keyboardName+"/largeKey.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("keyboards/default/largeKey.png").c_str());
	m_largeKey = SDL_DisplayFormatAlpha(tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(string("keyboards/"+keyboardName+"/curlargeKey.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("keyboards/default/curlargeKey.png").c_str());
	m_curLargeKey = SDL_DisplayFormatAlpha(tmpSurface);
	SDL_FreeSurface(tmpSurface);
	
	initKeys();
	m_good = true;
}

void Keyboard::initVector(vector<string>& myVect, string myArray[], int size)
{
	for(int i=0; i<size; ++i) {
		myVect.push_back(myArray[i]);
	}

}

void Keyboard::initKeys()
{
		m_top.clear();
		m_keyPositions.clear();
		m_keyStrings.clear();
		m_nums.clear();
		m_qwerty.clear();
		m_asdf.clear();
		m_zxcv.clear();
		m_bottomRow.clear();
	m_config.getItemAsColor("sk_main_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	vector<string> topkeys;
	vector<string> numkeys;
	vector<string> qwertykeys;
	vector<string> asdfkeys;
	vector<string> zxcvkeys;
	vector<string> spacekeys;

	topkeys.push_back("Clear");
	topkeys.push_back("Cancel");	
	if(m_shift == NONE) {
		string tnumkeys[10] = {"1","2","3","4","5","6","7","8","9","0"};
		string tqwertykeys[10] = {"q","w","e","r","t","y","u","i","o","p"}; 
		string tasdfkeys[9] = {"a","s","d","f","g","h","j","k","l"}; 
		string tzxcvkeys[9] = {"Shift","z","x","c","v","b","n","m","Enter"}; 
		string tspacekeys[7] = {"/",",","."," ","-","'","Back"};

		initVector(numkeys, tnumkeys, 10);
		initVector(qwertykeys, tqwertykeys, 10);
		initVector(asdfkeys, tasdfkeys, 9);
		initVector(zxcvkeys, tzxcvkeys, 9);
		initVector(spacekeys, tspacekeys, 7);

	} else if(m_shift == SHIFT) {
		string tnumkeys[10] = {"!","@","#","4","%","^","&","*","(",")"};
		string tqwertykeys[10] = {"Q","W","E","R","T","Y","U","I","O","P"}; 
		string tasdfkeys[9] = {"A","S","D","F","G","H","J","K","L"}; 
		string tzxcvkeys[9] = {"Shift","Z","X","C","V","B","N","M","Enter"}; 
		string tspacekeys[7] = {"?","<",">"," ","_","\"","Back"};

		initVector(numkeys, tnumkeys, 10);
		initVector(qwertykeys, tqwertykeys, 10);
		initVector(asdfkeys, tasdfkeys, 9);
		initVector(zxcvkeys, tzxcvkeys, 9);
		initVector(spacekeys, tspacekeys, 7);
	} else if(m_shift == CAPS) {
		string tnumkeys[10] = {"!","@","#","4","%","^","&","*","(",")"};
		string tqwertykeys[10] = {"Q","W","E","R","T","Y","U","I","O","P"}; 
		string tasdfkeys[9] = {"A","S","D","F","G","H","J","K","L"}; 
		string tzxcvkeys[9] = {"CAPS","Z","X","C","V","B","N","M","Enter"}; 
		string tspacekeys[7] = {"?","<",">"," ","_","\"","Back"};

		initVector(numkeys, tnumkeys, 10);
		initVector(qwertykeys, tqwertykeys, 10);
		initVector(asdfkeys, tasdfkeys, 9);
		initVector(zxcvkeys, tzxcvkeys, 9);
		initVector(spacekeys, tspacekeys, 7);

	}

	m_font = TTF_OpenFont(m_config.getItem("sk_font_main").c_str(),
										  10);
	m_fontBig = TTF_OpenFont(m_config.getItem("sk_font_main").c_str(),
										  16);

	SDL_Surface *sText;
	SDL_Rect pos2 = {110,10,42,28};
	for(int i=0; i< 2; ++i) {
		sText = TTF_RenderText_Blended(m_font, topkeys[i].c_str(), m_itemColor);
		m_top.push_back(sText);
		m_keyPositions.push_back(pos2);
		m_keyStrings.push_back(topkeys[i]);
	
		pos2.x += 46;
	}
	SDL_Rect pos = {2,75,28,28};
	for(int i=0; i< 10; ++i) {
		sText = TTF_RenderText_Blended(m_font, numkeys[i].c_str(), m_itemColor);
		m_nums.push_back(sText);
		m_keyPositions.push_back(pos);
		m_keyStrings.push_back(numkeys[i]);
		pos.x += 32;
	}
	pos.x = 2;
	pos.y += 34;
	for(int i=0; i< 10; ++i) {
		sText = TTF_RenderText_Blended(m_font, qwertykeys[i].c_str(), m_itemColor);
		m_qwerty.push_back(sText);
		m_keyPositions.push_back(pos);
		m_keyStrings.push_back(qwertykeys[i]);
		pos.x += 32;
	}
	pos.x = 18;
	pos.y += 34;
	for(int i=0; i<9 ; ++i) {
		sText = TTF_RenderText_Blended(m_font, asdfkeys[i].c_str(), m_itemColor);
		m_asdf.push_back(sText);
		m_keyPositions.push_back(pos);
		m_keyStrings.push_back(asdfkeys[i]);
		pos.x += 32;
	}
	pos.x = 4;
	pos.y += 34;
	for(int i=0; i<9 ; ++i) {
		if(i==0) {
			pos.w = 42;
		} else if(i==8) {
			pos.w = 42;
		}
		sText = TTF_RenderText_Blended(m_font, zxcvkeys[i].c_str(), m_itemColor);
		m_zxcv.push_back(sText);
		m_keyPositions.push_back(pos);
		m_keyStrings.push_back(zxcvkeys[i]);
		if(i==0) {
			pos.w = 28;
			pos.x +=46;
		} else if(i==8) {
			pos.w = 28;
		} else {
		pos.x += 32;
		}
	}
	pos.x = 15;
	pos.y += 34;
	for(int i=0; i< 7; ++i) {
		if(i==0) 
			pos.w = 42;
		else if(i==3) 
			pos.w = 62;
		else if(i==6) 
			pos.w = 42;

		sText = TTF_RenderText_Blended(m_font, spacekeys[i].c_str(), m_itemColor);
		m_bottomRow.push_back(sText);
		m_keyPositions.push_back(pos);
		m_keyStrings.push_back(spacekeys[i]);
		if(i==0)  {
			pos.w = 28;
			pos.x += 46;
		} else if(i==3) {
			pos.w = 28;
			pos.x += 66;
		} else if(i==6) {
			pos.w = 28;
		}else {
			pos.x += 32;
		}
	}
}

Keyboard::~Keyboard()
{
	for(keys_t::iterator kIter = m_nums.begin();
		kIter != m_nums.end();
		++kIter) {
		SDL_FreeSurface((*kIter));
	}
	for(keys_t::iterator kIter = m_qwerty.begin();
		kIter != m_qwerty.end();
		++kIter) {
		SDL_FreeSurface((*kIter));
	}
	for(keys_t::iterator kIter = m_asdf.begin();
		kIter != m_asdf.end();
		++kIter) {
		SDL_FreeSurface((*kIter));
	}
	for(keys_t::iterator kIter = m_zxcv.begin();
		kIter != m_zxcv.end();
		++kIter) {
		SDL_FreeSurface((*kIter));
	}
	for(keys_t::iterator kIter = m_bottomRow.begin();
		kIter != m_bottomRow.end();
		++kIter) {
		SDL_FreeSurface((*kIter));
	}


}

void Keyboard::init(int returnCmd, string initialText)
{
	m_returnCmd = returnCmd;
	m_text = initialText;
}

std::string Keyboard::getText()
{
	return m_text;
}

bool Keyboard::inRect(int x, int y, SDL_Rect& rect)
{
	if(x > rect.x && x < rect.x+rect.w && y > rect.y && y < rect.y+rect.w) 
		return true;
	else 
		return false;
}

int Keyboard::processCommand(int command, GuiPos& guiPos)
{
	int rCommand = command;
	bool foundKey = false;
	int i=0;
	if(command > 0) {
		if(command == CMD_CLICK) {
			rCommand = 0;
			for(i=0; i<m_keyPositions.size() &&  !foundKey; ++i) {
				if(inRect(guiPos.curX,guiPos.curY, m_keyPositions[i])) {
					foundKey = true;
				}
			}

		} else if(command == CMD_UP) {
			if(m_curRow > 1)
				m_curRow--;
			else  if(m_curRow == 1) {
				m_curRow--;
				if(m_curCol > 1)
					m_curCol = 1;
			} else {
				m_curRow = 5;
				if(m_curCol == 1)
					m_curCol = 6;
			}
			rCommand = 0;
		} else if(command == CMD_DOWN) {
			if(m_curRow < 5) {
				if(m_curRow == 2) {
					if(m_curCol > 8)
					m_curCol = 8;
				} else if(m_curRow == 4) {
					if(m_curCol > 6)
					m_curCol = 6;
				} else if(m_curRow == 0) {
					if(m_curCol > 0)
					m_curCol = 9;
				}	
				m_curRow++; 
			} else  {
				m_curRow = 0;
				if(m_curCol > 0)
					m_curCol = 1;
			}
			rCommand = 0;
		} else if(command == CMD_RIGHT) {
			int maxCol = 9;
			if(m_curRow == 5)
				maxCol = 6;
			else if(m_curRow == 0)
				maxCol = 1;
			else if(m_curRow == 4 || m_curRow == 3)
				maxCol = 8;	

			if(m_curCol < maxCol)
				++m_curCol;
			else 
				m_curCol = 0;	
			rCommand = 0;
		} else if(command == CMD_LEFT) {
			int maxCol = 9;
			if(m_curRow == 5)
				maxCol = 6;
			else if(m_curRow == 0)
				maxCol = 1;
			else if(m_curRow == 4 || m_curRow == 3)
				maxCol = 8;	

			if(m_curCol > 0)
				--m_curCol;
			else 
				m_curCol = maxCol;
			rCommand = 0;
		} else if(command == CMD_LOAD_PL 
				|| command == CMD_LOAD_BKMRK
				|| command == CMD_IMMEDIATE_PLAY
				|| command == CMD_POP_SELECT
				//|| command == CMD_HIT_KEY
				|| command == CMD_PLAY_PAUSE ) {
			if(m_curRow == 0)
				i = m_curCol +1;
			else if(m_curRow == 1)
				i = 2+m_curCol+1;
			else if(m_curRow == 2)
				i = 12+m_curCol+1;
			else if(m_curRow == 3)
				i = 22+m_curCol+1;
			else if(m_curRow == 4)
				i = 31+m_curCol+1;
			else if(m_curRow == 5)
				i = 40+m_curCol+1;
		
			rCommand = 0;
			foundKey = true;	
		} else if(command == CMD_STOP
				|| command == CMD_POP_CANCEL
				|| command == CMD_PREV_DIR
				|| command == CMD_PREV_DIR ) {
			rCommand = CMD_HIDE_KEYBOARD;
			foundKey = false;
		} else if(command == CMD_SHOW_MENU) {
			rCommand = m_returnCmd;	
			foundKey = false;
		} else if(command == CMD_ADD_AS_PL
				|| command == CMD_MOVE_IN_PL) {
			rCommand = 0;
			m_text = m_text.substr(0, m_text.length()-1);
		}

		if(foundKey) {
			m_foundKey = foundKey;	
			if(m_keyStrings[i-1] == "Enter") {
				rCommand = m_returnCmd;	
			} else if(m_keyStrings[i-1] == "Back") {
				m_text = m_text.substr(0, m_text.length()-1);
			} else if(m_keyStrings[i-1] == "More") {
				m_viewMore = true;	
			} else if(m_keyStrings[i-1] == "Shift" || m_keyStrings[i-1] == "CAPS") {
				if(m_shift == NONE)
					m_shift = SHIFT;
				else if(m_shift == SHIFT)
					m_shift = CAPS;
				else
					m_shift = NONE;
				initKeys();
			} else if(m_keyStrings[i-1] == "Clear") {
				m_text.clear();	
			} else if(m_keyStrings[i-1] == "Cancel") {
				rCommand = CMD_HIDE_KEYBOARD;
			} else {
				m_lastKey = m_keyStrings[i-1];
				m_text += m_keyStrings[i-1];
				if(m_shift == SHIFT) {
					m_shift = NONE;
					initKeys();
				}
			}
		}
	}

	return rCommand;
	
}

bool Keyboard::draw(bool forceRefresh)
{
	if(!m_good)
		initAll();
	bool refreshRet = false;
	if(forceRefresh || m_refresh) {
		keyPositions_t::iterator pIter = m_keyPositions.begin();
		SDL_Rect entryRect;
		SDL_Surface *sText;
		//cout << "keyboard " << forceRefresh << m_refresh  << m_foundKey<< endl;
		if(m_counter < 10) {
			entryRect.w = 28;
			entryRect.h = 28;
			entryRect.x = 0;
			entryRect.y = 0;
			SDL_SetClipRect(m_screen, &entryRect);
			SDL_Rect letterRect = entryRect;
			SDL_BlitSurface(m_keyBack, NULL, m_screen, &entryRect );

			sText = TTF_RenderText_Blended(m_fontBig, m_lastKey.c_str(), m_itemColor);
			SDL_BlitSurface(sText, NULL, m_screen, &letterRect );
			SDL_FreeSurface(sText);
			m_foundKey = false;
			++m_counter;
			refreshRet = true;
		} else {
			m_counter = 0;
		}
		

		entryRect.w = 260;
		entryRect.h = 16;
		entryRect.x = (m_screen->w - entryRect.w) / 2;
		entryRect.y = 50;
		SDL_Rect letterRect = entryRect;
		SDL_SetClipRect(m_screen, &entryRect);
		SDL_BlitSurface(m_keyboardEntry, NULL, m_screen, &entryRect );
		sText = TTF_RenderText_Blended(m_font, m_text.c_str(), m_itemColor);
		SDL_BlitSurface(sText, NULL, m_screen, &letterRect );
		SDL_FreeSurface(sText);
		int col = 0;
		for(keys_t::iterator kIter = m_top.begin();
				kIter != m_top.end();
				++kIter) {
			SDL_Rect letterRect = (*pIter);
			letterRect.x += 2;
			letterRect.y += 2;
			SDL_SetClipRect(m_screen, &(*pIter));
			if(m_curRow == 0 && m_curCol == col)
				SDL_BlitSurface(m_curMediumKey, NULL, m_screen, &(*pIter) );
			else
				SDL_BlitSurface(m_mediumKey, NULL, m_screen, &(*pIter) );
			SDL_BlitSurface((*kIter), NULL, m_screen, &letterRect );
			++pIter;
			++col;
		}
		col = 0;
		for(keys_t::iterator kIter = m_nums.begin();
				kIter != m_nums.end();
				++kIter) {
			SDL_Rect letterRect = (*pIter);
			letterRect.x += 4;
			letterRect.y += 2;
			SDL_SetClipRect(m_screen, &(*pIter));
			if(m_curRow == 1 && m_curCol == col)
				SDL_BlitSurface(m_curKeyBack, NULL, m_screen, &(*pIter) );
			else
				SDL_BlitSurface(m_keyBack, NULL, m_screen, &(*pIter) );
			SDL_BlitSurface((*kIter), NULL, m_screen, &letterRect );
			++pIter;
			++col;
		}
		col = 0;
		for(keys_t::iterator kIter = m_qwerty.begin();
				kIter != m_qwerty.end();
				++kIter) {
			SDL_Rect letterRect = (*pIter);
			letterRect.x += 4;
			letterRect.y += 2;
			SDL_SetClipRect(m_screen, &(*pIter));
			if(m_curRow == 2 && m_curCol == col)
				SDL_BlitSurface(m_curKeyBack, NULL, m_screen, &(*pIter) );
			else
				SDL_BlitSurface(m_keyBack, NULL, m_screen, &(*pIter) );
			SDL_BlitSurface((*kIter), NULL, m_screen, &letterRect );
			++pIter;
			++col;
		}
		col = 0;
		for(keys_t::iterator kIter = m_asdf.begin();
				kIter != m_asdf.end();
				++kIter) {
			SDL_Rect letterRect = (*pIter);
			letterRect.x += 4;
			letterRect.y += 2;
			SDL_SetClipRect(m_screen, &(*pIter));
			if(m_curRow == 3 && m_curCol == col)
				SDL_BlitSurface(m_curKeyBack, NULL, m_screen, &(*pIter) );
			else
				SDL_BlitSurface(m_keyBack, NULL, m_screen, &(*pIter) );
			SDL_BlitSurface((*kIter), NULL, m_screen, &letterRect );
			++pIter;
			++col;
		}
		col = 0;
		for(keys_t::iterator kIter = m_zxcv.begin();
				kIter != m_zxcv.end();
				++kIter) {
			SDL_Rect letterRect = (*pIter);
			letterRect.x += 4;
			letterRect.y += 2;
			SDL_SetClipRect(m_screen, &(*pIter));
			if(m_curRow == 4 && m_curCol == col) {
				if(kIter == m_zxcv.begin()) 
					SDL_BlitSurface(m_curMediumKey, NULL, m_screen, &(*pIter) );
				else if(kIter == m_zxcv.end()-1) 
					SDL_BlitSurface(m_curMediumKey, NULL, m_screen, &(*pIter) );
				else 
					SDL_BlitSurface(m_curKeyBack, NULL, m_screen, &(*pIter) );
			} else {
				if(kIter == m_zxcv.begin()) 
					SDL_BlitSurface(m_mediumKey, NULL, m_screen, &(*pIter) );
				else if(kIter == m_zxcv.end()-1) 
					SDL_BlitSurface(m_mediumKey, NULL, m_screen, &(*pIter) );
				else 
					SDL_BlitSurface(m_keyBack, NULL, m_screen, &(*pIter) );
			}
			SDL_BlitSurface((*kIter), NULL, m_screen, &letterRect );

			++pIter;
			++col;
		}
		col = 0;
		for(keys_t::iterator kIter = m_bottomRow.begin();
				kIter != m_bottomRow.end();
				++kIter) {
			SDL_Rect letterRect = (*pIter);
			letterRect.x += 4;
			letterRect.y += 2;
			SDL_SetClipRect(m_screen, &(*pIter));
			if(m_curRow == 5 && m_curCol == col) {
				if(kIter == m_bottomRow.begin()) 
					SDL_BlitSurface(m_curMediumKey, NULL, m_screen, &(*pIter) );
				else if(kIter == m_bottomRow.end()-1) 
					SDL_BlitSurface(m_curMediumKey, NULL, m_screen, &(*pIter) );
				else if(kIter == m_bottomRow.begin()+3) 
					SDL_BlitSurface(m_curLargeKey, NULL, m_screen, &(*pIter) );
				else 
					SDL_BlitSurface(m_curKeyBack, NULL, m_screen, &(*pIter) );
			} else {
				if(kIter == m_bottomRow.begin()) 
					SDL_BlitSurface(m_mediumKey, NULL, m_screen, &(*pIter) );
				else if(kIter == m_bottomRow.end()-1) 
					SDL_BlitSurface(m_mediumKey, NULL, m_screen, &(*pIter) );
				else if(kIter == m_bottomRow.begin()+3) 
					SDL_BlitSurface(m_largeKey, NULL, m_screen, &(*pIter) );
				else 
					SDL_BlitSurface(m_keyBack, NULL, m_screen, &(*pIter) );
			}
			SDL_BlitSurface((*kIter), NULL, m_screen, &letterRect );
			++pIter;
			++col;
		}
		m_refresh = false;
	}

	return refreshRet;
}



