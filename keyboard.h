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
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

class Config;
class GuiPos;

class Keyboard {

public:
	enum {NONE,SHIFT,CAPS};
	typedef std::vector<SDL_Rect> keyPositions_t;
	typedef std::vector<std::string> keyStrings_t;
	typedef std::vector<SDL_Surface*> keys_t;
	Keyboard(SDL_Surface* screen, Config& config);
	void init(int returnCmd, std::string initialText = "");
	~Keyboard();

	int processCommand(int command, GuiPos& guiPos);
	std::string getText();
	bool draw(bool forceRefresh);
protected:
	bool inRect(int x, int y, SDL_Rect& rect);
	void initKeys();
	void initAll();
void initVector(std::vector<std::string>& myVect, std::string myArray[], int size);

private:
	bool m_good;
	int m_returnCmd;	
	Config& m_config;
	SDL_Surface* m_screen;
	std::string m_text;
	bool m_refresh;
	bool m_viewMore;
	int m_shift;	
	SDL_Surface* m_keyBack;
	SDL_Surface* m_keyboardEntry;
	SDL_Surface* m_mediumKey;
	SDL_Surface* m_largeKey;
	SDL_Surface* m_curKeyBack;
	SDL_Surface* m_curMediumKey;
	SDL_Surface* m_curLargeKey;
	TTF_Font * m_font;
	TTF_Font * m_fontBig;
	bool m_foundKey;
	std::string m_lastKey;
	SDL_Color m_itemColor;
	keyPositions_t m_keyPositions;
	keyStrings_t m_keyStrings;
	keyPositions_t m_moreKeyPositions;
	int m_counter;
	keys_t m_top;
	keys_t m_nums;
	keys_t m_qwerty;
	keys_t m_asdf;
	keys_t m_zxcv;
	keys_t m_bottomRow;

	int m_curRow;
	int m_curCol;
};

#endif
