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
#include "button.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>


using namespace std;

Button::Button(string name, string imageDir)
: m_name(name)
, m_refresh(true)
, m_imageDir(imageDir)
{
}

void Button::init(Config& config)
{
	string configItem = m_imageDir;
	if(m_imageDir == "overlay")
		configItem = "sk_"+m_imageDir;;

	string btnName = "sk_"+m_name;
	if(m_name == "seek_big")
		btnName = "sk_seek";
	m_clearRect.x = config.getItemAsNum(btnName+"_x");
	m_clearRect.y = config.getItemAsNum(btnName+"_y");
	m_clearRect.w = config.getItemAsNum(btnName+"_width");
	m_clearRect.h = config.getItemAsNum(btnName+"_height");
	
	SDL_Surface * tmpBack = IMG_Load(string(m_imageDir+"s/"+config.getItem(configItem)+"/"+m_name+"_back.png").c_str());
	m_destRect = m_clearRect;
	if (!tmpBack) {
		printf("Unable to load button image: %s\n", SDL_GetError());
		m_showBack = false;
	} else {
		m_showBack = true;
		m_backImage = SDL_DisplayFormatAlpha(tmpBack);
		m_destRect.w = m_backImage->w;
		m_destRect.h = m_backImage->h;
	}
	tmpBack = IMG_Load(string(m_imageDir+"s/"+config.getItem(configItem)+"/"+m_name+".png").c_str());
	if (!tmpBack) {
		printf("Unable to load alt button image: %s\n", SDL_GetError());
		m_showFore = false;
	} else {
		m_showFore = true;
		m_foreImage = SDL_DisplayFormatAlpha(tmpBack);
		if(!m_showBack) {
			m_destRect.w = m_foreImage->w;
			m_destRect.h = m_foreImage->h;
		}
	}

	m_srcRect.x=0;
	m_srcRect.y=0;
	m_srcRect.w = m_destRect.w;
	m_srcRect.h = m_destRect.h;
}

void Button::updateInfo(string name, string imageDir)
{
	m_name = name;
	m_imageDir = imageDir;
}

void Button::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus,
						int rtmpdStatusChanged, mpd_Status* rtmpdStatus, bool forceRefresh)
{
}

void Button::draw(SDL_Surface* screen, SDL_Surface* bg, bool forceRefresh)
{
	if(forceRefresh || m_refresh) {
		//clear this portion of the screen 
		SDL_SetClipRect(screen, &m_clearRect);
		SDL_BlitSurface(bg, &m_clearRect, screen, &m_clearRect);

		if(m_showBack)
			SDL_BlitSurface(m_backImage, NULL, screen, &m_destRect );
		if(m_showFore)
			SDL_BlitSurface(m_foreImage, &m_srcRect, screen, &m_destRect );
		
		m_refresh = false;
	}
}
