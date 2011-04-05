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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <fstream>
#include <map>
#include <SDL.h>

class Config
{
public:
    Config();

	Config(std::string file);
	void init();
    std::string getItem(std::string item);
    int getItemAsNum(std::string item);
   	bool getItemAsBool(std::string item);
    float getItemAsFloat(std::string item);
	void setItem(std::string itemName, std::string value);
	void getItemAsColor(std::string name, Uint8& red, Uint8& green, Uint8& blue);
	void saveConfigFile();

	bool verifyMpdPaths();
	bool verifyClientPaths();
protected:
	void readConfigFile(std::ifstream& file);
	void readLangFile(std::string lang);
    void trimStr(std::string & inStr);
    std::map<std::string, std::string> m_configItems;
};

#endif

