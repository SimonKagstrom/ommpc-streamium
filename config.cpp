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

#include "config.h"
#include <iostream>
#include <stdexcept>
#include <sys/stat.h>
#include "utf8.h"

using namespace std;

Config::Config()
{
	init();
}

Config::Config(string file)
{
    ifstream configFile(file.c_str(), ios::in);
    if(configFile.fail()) {
		std::string msg = "CONFIG: Unable to open config file: " + file;
        throw runtime_error(msg.c_str());
	}
	readConfigFile(configFile);
}

void Config::init()
{
	m_configItems.clear();
    ifstream configFile("ommpc.conf", ios::in);

    if(configFile.fail()) {
		std::string msg = "CONFIG: Unable to open config file: ommpc.conf";
        throw runtime_error(msg.c_str());
	}
	readConfigFile(configFile);

	std::string skinConfig;
	//if(getItem("showAlbumArt") == "false")
	//	skinConfig = "skin_noart.conf";
	//else
		skinConfig = "skin.conf";	
	ifstream skinFile(("skins/"+getItem("skin")+"/"+skinConfig).c_str(), ios::in);	
    
	if(skinFile.fail()) {
		std::string msg = "CONFIG: Unable to open skin config file: " + 
							getItem("skin") + "/" + skinConfig;
        throw runtime_error(msg.c_str());
	}
	readConfigFile(skinFile);

	m_configItems["realSkin"] = m_configItems["skin"];
	if(getItem("sk_resource_dir") != "") {
		m_configItems["skin"] = getItem("sk_resource_dir");
	}
		
	if(getItem("language").length() > 0) 
		readLangFile(getItem("language"));
	else 
		readLangFile("us-en");

}

void Config::readConfigFile(ifstream& configFile)
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
            m_configItems.insert(make_pair(itemName, itemValue));
        }
	}
}

void Config::readLangFile(string lang)
{
cout << "lang " << lang << endl;
	lang = "languages/"+lang;
	// Open the test file (must be UTF-8 encoded)
	ifstream fs8(lang.c_str());
	if (!fs8.is_open()) {
		cout << "Could not open " << lang << endl;
	}
    // Read the first line of the file
    unsigned line_count = 1;
    string line;
    if (!getline(fs8, line)) 
        cout << "empty lang file" << endl;
    // Look for utf-8 byte-order mark at the beginning
    if (line.size() > 2) {
        if (utf8::is_bom(line.c_str()))
            cout << "There is a byte order mark at the beginning of the file\n";
    }
    // Play with all the lines in the file
    do {
       // check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
        string::iterator endIter= utf8::find_invalid(line.begin(), line.end());
        if (endIter != line.end()) {
            cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
            cout << "This part is fine: " << string(line.begin(), endIter) << "\n";
        }
        // Get the line length (at least for the valid part)
        int length = utf8::distance(line.begin(), endIter);
        //cout << "Length of line " << line_count << " is " << length <<  "\n";
            
		int pos = line.find('=');

		if(pos != string::npos) {	
			std::string itemName = line.substr(0, pos);
			std::string itemValue = line.substr(pos+1);
			trimStr(itemName);
			trimStr(itemValue);
			m_configItems.insert(make_pair(itemName, itemValue));
		}

        getline(fs8, line);
        line_count++;
    } while (!fs8.eof());

}

void Config::saveConfigFile()
{
	ofstream configFile("ommpc.conf", ios::out|ios::trunc);

    if(configFile.fail()) {
		std::string msg = "CONFIG: Unable to open config file: ommpc.conf";
        throw runtime_error(msg.c_str());
	}

	for(std::map<std::string, std::string>::iterator cIter =  m_configItems.begin();
	cIter != m_configItems.end();
	++cIter) {
		if((*cIter).first.substr(0,3) != "sk_" 
			&& (*cIter).first.substr(0,4) != "LANG"
			&& (*cIter).first.substr(0,4) != "BIND"
			&& (*cIter).first != "realSkin") {
			configFile << (*cIter).first << "=" << (*cIter).second << endl;
		}
	}
	
	configFile.close();
	sync();
}


std::string Config::getItem(std::string itemName)
{
    std::map<std::string, std::string>::iterator mIter = m_configItems.find(itemName);

    if(mIter != m_configItems.end()) {
        return (*mIter).second;
    } else {
        return "";
    }
}

void Config::setItem(string itemName, string value) 
{
	//this is intentially this simple to allow inserting of new config item....
	m_configItems[itemName] = value;
}

int Config::getItemAsNum(std::string itemName)
{
	std::string value;
    std::map<std::string, std::string>::iterator mIter = m_configItems.find(itemName);

    if(mIter != m_configItems.end()) {
        value = (*mIter).second;
    } else {
        value = "";
    }

	return atoi(value.c_str());
}

bool Config::getItemAsBool(std::string itemName)
{
	std::string value;
    std::map<std::string, std::string>::iterator mIter = m_configItems.find(itemName);

    if(mIter != m_configItems.end()) {
        value = (*mIter).second;
    } else {
        value = "";
    }
	if(value == "true")
		return true;
	else 
		return false;
}

float Config::getItemAsFloat(std::string itemName)
{
	std::string value;
    std::map<std::string, std::string>::iterator mIter = m_configItems.find(itemName);

    if(mIter != m_configItems.end()) {
        value = (*mIter).second;
    } else {
        value = "";
    }

	return atof(value.c_str());
}


void Config::trimStr(std::string & inStr)
{
    while(inStr[0] == ' ' || inStr[0] == '\t' || inStr[0] == '\n')
        inStr = inStr.substr(1);

    while(inStr[inStr.length()-1] == ' '
            || inStr[inStr.length()-1] == '\t'
            || inStr[inStr.length()-1] == '\n')
        inStr = inStr.substr(0, inStr.length()-1);

}

void Config::getItemAsColor(std::string name, Uint8& red, Uint8& green, Uint8& blue)
{
    std::map<std::string, std::string>::iterator mIter = m_configItems.find(name);

    if(mIter != m_configItems.end()) {
       	int commaPos1 = (*mIter).second.find(',');
		red = atoi(((*mIter).second.substr(0, commaPos1)).c_str());
		int commaPos2 = (*mIter).second.find(',', commaPos1+1);
		green = atoi(((*mIter).second.substr(commaPos1+1, commaPos2-commaPos1)).c_str());;
		int commaPos3 = (*mIter).second.find(',', commaPos2+1);
		blue = atoi(((*mIter).second.substr(commaPos2+1, commaPos3-commaPos2)).c_str());
	}
}

bool Config::verifyMpdPaths()
{
	bool good = true;
	struct stat stFileInfo;
	if(stat(m_configItems["musicRoot"].c_str(),&stFileInfo) != 0)
		good &= false;
	if(stat(m_configItems["playlistRoot"].c_str(),&stFileInfo) != 0)
		good &= false;
	if(stat(m_configItems["programRoot"].c_str(),&stFileInfo) != 0)
		good &= false;

	return good;	
}

bool Config::verifyClientPaths()
{

	return false;
}
