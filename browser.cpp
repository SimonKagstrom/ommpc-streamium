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

#include "browser.h"
#include "threadParms.h"
#include "commandFactory.h"
#include "config.h"
#include "guiPos.h"
#include "songDb.h"
#include "keyboard.h"
#include "playlist.h"
#include <iostream>
#include <stdexcept>
#include <SDL_image.h>

using namespace std;

Browser::Browser(mpd_Connection* mpd, SDL_Surface* screen, SDL_Surface* bg, TTF_Font* font, 
				SDL_Rect& rect,	Config& config, int skipVal, int numPerScreen, SongDb& songdb, Keyboard& kb, Playlist& pl)
: Scroller(mpd, screen, bg, font, rect, config, skipVal, numPerScreen-1)
, m_nowPlaying(0)
, m_view(0)
, m_updatingDb(false)
, m_updatingSongDb(false)
, m_songDb(songdb)
, m_keyboard(kb)
, m_pl(pl)
, m_appended(false)
, m_queued(false)
, m_showTime(0)
, m_good(false)
{
}

void Browser::initAll()
{
	m_config.getItemAsColor("sk_main_itemColor", m_itemColor.r, m_itemColor.g, m_itemColor.b);
	m_config.getItemAsColor("sk_main_curItemColor", m_curItemColor.r, m_curItemColor.g, m_curItemColor.b);
    
	string skinName = m_config.getItem("skin");
	m_drawIcons =  m_config.getItemAsNum("drawIcons");
	m_dbMsg= IMG_Load(string("skins/"+skinName+"/updatingdb.png").c_str());
	if (!m_dbMsg)
		printf("Unable to load image: %s\n", SDL_GetError());
	else 
		m_dbMsg = SDL_DisplayFormatAlpha(m_dbMsg);
	
	SDL_Surface* tmpSurface = NULL;	
	tmpSurface = IMG_Load(string("skins/"+skinName+"/iconBrowse.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("skins/default/iconBrowse.png").c_str());
	if (!tmpSurface)
		printf("Unable to load image: %s\n", SDL_GetError());
	else {
		m_iconBrowse = SDL_DisplayFormatAlpha(tmpSurface);
		SDL_FreeSurface(tmpSurface);
	}
	tmpSurface = IMG_Load(string("skins/"+skinName+"/queued.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("skins/default/queued.png").c_str());
	if (!tmpSurface)
		printf("Unable to load image: %s\n", SDL_GetError());
	else {
		m_queuedImg = SDL_DisplayFormatAlpha(tmpSurface);
		SDL_FreeSurface(tmpSurface);
	}
	tmpSurface = IMG_Load(string("skins/"+skinName+"/appended.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("skins/default/appended.png").c_str());
	if (!tmpSurface)
		printf("Unable to load image: %s\n", SDL_GetError());
	else {
		m_appendedImg = SDL_DisplayFormatAlpha(tmpSurface);
		SDL_FreeSurface(tmpSurface);
	}
	
	tmpSurface = NULL;	
	tmpSurface = IMG_Load(string("skins/"+skinName+"/iconFolder.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("skins/default/iconFolder.png").c_str());
	m_iconFolder = SDL_DisplayFormatAlpha(tmpSurface);
	SDL_FreeSurface(tmpSurface);
	
	tmpSurface = IMG_Load(string("skins/"+skinName+"/iconFile.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("skins/default/iconFile.png").c_str());
	if (!tmpSurface)
		printf("Unable to load image: %s\n", SDL_GetError());
	else { 
		m_iconFile = SDL_DisplayFormatAlpha(tmpSurface);
		SDL_FreeSurface(tmpSurface);
	}
	tmpSurface = IMG_Load(string("skins/"+skinName+"/iconFilter.png").c_str());
	if (!tmpSurface)
		tmpSurface = IMG_Load(string("skins/default/iconFilter.png").c_str());
	if (!tmpSurface)
		printf("Unable to load image: %s\n", SDL_GetError());
	else { 
		m_iconFilter = SDL_DisplayFormatAlpha(tmpSurface);
		SDL_FreeSurface(tmpSurface);
	}

	m_numPerScreen--;
	initItemIndexLookup();	
	ls("");
    //ls("tim");
    for(int i=0; i<=5; ++i)
	    m_filters.push_back("");

	m_good = true;
}

void Browser::ls(std::string item)
{
	string dir;
	if(item == "")
		m_view = VIEW_ROOT;
	else if(item == m_config.getItem("LANG_FILESYSTEM"))
		m_view = VIEW_FILES;
	else if(item == m_config.getItem("LANG_ARTISTS"))
		m_view = VIEW_ARTISTS;
	else if(item == m_config.getItem("LANG_ALBUMS"))
		m_view = VIEW_ALBUMS;
	else if(item == m_config.getItem("LANG_ALL_SONGS"))
		m_view = VIEW_SONGS;
	else if(item == m_config.getItem("LANG_GENRES"))
		m_view = VIEW_GENRES;
	
	switch(m_view) {
		case VIEW_ROOT:
			browseRoot();
		break;
		case VIEW_FILES:
			if(m_curItemType == TYPE_BACK) {
				if(!m_prevItems.empty()) {
					m_curItemNum = m_prevItems[m_prevItems.size()-1].second;
					m_curItemName = m_prevItems[m_prevItems.size()-1].first;
					makeCurItemVisible();
					m_prevItems.pop_back();
					for(prevItems_t::iterator iIter = m_prevItems.begin();
						iIter != m_prevItems.end();
						++iIter) {
						dir += (*iIter).first + "/";
					}
					dir = dir.substr(0, dir.length()-1);
				} else {
					browseRoot();
					break;
				}
			} else if(item == m_config.getItem("LANG_FILESYSTEM")) {
				m_curItemNum = 0;
				m_topItemNum = 0;
				m_curItemName = "";
				dir = "";
			} else {
				cout << "inum " << m_curItemNum << "   iname   " << m_curItemName << endl;
				m_prevItems.push_back(make_pair(m_curItemName, m_curItemNum));
				m_curItemNum = 0;
				m_topItemNum = 0;
				for(prevItems_t::iterator iIter = m_prevItems.begin();
						iIter != m_prevItems.end();
						++iIter) {
					dir += (*iIter).first + "/";
				}
				dir = dir.substr(0, dir.length()-1);
			}
			browseFileSystem(dir);
			break;
		case VIEW_ARTISTS:
			if(item == m_config.getItem("LANG_ARTISTS")) {
				m_curItemNum = 0;
				m_topItemNum = 0;
				m_curItemName = "";
				browseArtists();
			} else if(m_curItemType == TYPE_BACK) {
				if(!m_prevItems.empty()) {
					m_curItemNum = m_prevItems[m_prevItems.size()-1].second;
					m_curItemName = m_prevItems[m_prevItems.size()-1].first;
					makeCurItemVisible();
					m_prevItems.pop_back();
					if(m_prevItems.empty())
						browseArtists();
					else {
						browseAlbumsByArtist(m_prevItems[m_prevItems.size()-1].first);
					}
				} else {
					browseRoot();
				}
			} else if(m_curItemType == TYPE_ALL) {	
				m_prevItems.push_back(make_pair(m_curItemName, m_curItemNum));
				m_curItemNum = 0;
				m_topItemNum = 0;
				browseSongsByArtist(m_prevItems[m_prevItems.size()-2].first);
			} else {
				m_prevItems.push_back(make_pair(m_curItemName, m_curItemNum));
				m_curItemNum = 0;
				m_topItemNum = 0;
				if(m_prevItems.size() == 1)
					browseAlbumsByArtist(m_curItemName);
				else
					browseSongsByAlbum(m_curItemName, m_prevItems[m_prevItems.size()-2].first);
			}
			break;
		case VIEW_ALBUMS:
			if(item == m_config.getItem("LANG_ALBUMS")) {
				m_curItemNum = 0;
				m_topItemNum = 0;
				m_curItemName = "";
				browseAlbums();
			} else if(m_curItemType == TYPE_BACK) {
				if(!m_prevItems.empty()) {
					m_curItemNum = m_prevItems[m_prevItems.size()-1].second;
					m_curItemName = m_prevItems[m_prevItems.size()-1].first;
					makeCurItemVisible();
					m_prevItems.pop_back();
					if(m_prevItems.empty())
						browseAlbums();
					else {
						//browseAlbumsByArtist(m_prevItems[m_prevItems.size()-1].first);
					}
				} else {
					browseRoot();
				}
			} else {
				m_prevItems.push_back(make_pair(m_curItemName, m_curItemNum));
				m_curItemNum = 0;
				m_topItemNum = 0;
				browseSongsByAlbum(m_curItemName);
			}
			break;
		case VIEW_SONGS:
			if(item == m_config.getItem("LANG_ALL_SONGS")) {
				m_curItemNum = 0;
				m_topItemNum = 0;
				m_curItemName = "";
				browseSongs();
			} else if(m_curItemType == TYPE_BACK) {
				if(!m_prevItems.empty()) {
					m_curItemNum = m_prevItems[m_prevItems.size()-1].second;
					m_curItemName = m_prevItems[m_prevItems.size()-1].first;
					makeCurItemVisible();
					m_prevItems.pop_back();
					if(m_prevItems.empty())
						browseSongs();
					else {
						//browseAlbumsByArtist(m_prevItems[m_prevItems.size()-1].first);
					}
				} else {
					browseRoot();
				}
			} else {
				m_prevItems.push_back(make_pair(m_curItemName, m_curItemNum));
				m_curItemNum = 0;
				m_topItemNum = 0;
				browseSongs();
			}
			break;
		case VIEW_GENRES:
			if(item == m_config.getItem("LANG_GENRES")) {
				m_curItemNum = 0;
				m_topItemNum = 0;
				m_curItemName = "";
				browseGenres();
			} else if(m_curItemType == TYPE_BACK) {
				if(!m_prevItems.empty()) {
					m_curItemNum = m_prevItems[m_prevItems.size()-1].second;
					m_curItemName = m_prevItems[m_prevItems.size()-1].first;
					makeCurItemVisible();
					m_prevItems.pop_back();
					if(m_prevItems.empty())
						browseGenres();
					else if(m_prevItems.size() == 1) { //have just chosen a genre
						browseAlbumsByGenre(m_prevItems[m_prevItems.size()-1].first);	
					} else {
//						browseSongsByGenre(m_prevItems[m_prevItems.size()-2].first, m_prevItems[m_prevItems.size()-1].first);
					}
				} else {
					browseRoot();
				}
			} else {
				m_prevItems.push_back(make_pair(m_curItemName, m_curItemNum));
				m_curItemNum = 0;
				m_topItemNum = 0;
			
				if(m_prevItems.size() == 1) { //have just chosen a genre
					browseAlbumsByGenre(m_curItemName);	
				} else {
					browseSongsByGenre(m_prevItems[m_prevItems.size()-2].first, m_curItemName);
				}	
			}
			break;
	}
}

void Browser::browseRoot() {
	m_listing.clear();
	m_listing.push_back(make_pair(m_config.getItem("LANG_ARTISTS"), 0));
	m_listing.push_back(make_pair(m_config.getItem("LANG_ALBUMS"), 0));
	m_listing.push_back(make_pair(m_config.getItem("LANG_GENRES"), 0));
	m_listing.push_back(make_pair(m_config.getItem("LANG_FILESYSTEM"), 0));
	m_listing.push_back(make_pair(m_config.getItem("LANG_ALL_SONGS"), 0));
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseFileSystem(std::string dir) {
	mpd_sendLsInfoCommand(m_mpd, dir.c_str());
	m_curDir = dir;
	m_listing.clear();
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	mpd_InfoEntity* mpdItem = mpd_getNextInfoEntity(m_mpd);
	while(mpdItem != NULL) {
		std::string item = "";
		int type = mpdItem->type;
		if(type == 0) {
			item = mpdItem->info.directory->path;
		} else if(type == 1) {
			item = mpdItem->info.song->file;
		} 
		/*else if(type == 2) {
			item = mpdItem->info.playlistFile->path;
		} else {
			throw runtime_error("Unknown mpd entity");
		}
		*/
		int pos = item.rfind("/");;
		if(pos != string::npos) {
			item = item.substr(pos+1);
		}
//	cout << item << endl;
		if(!item.empty()) 
			m_listing.push_back(make_pair(item, type));
		mpd_freeInfoEntity(mpdItem);
		mpdItem = mpd_getNextInfoEntity(m_mpd);
	}
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseArtists() {
	string massagedFilter = replaceWildcard(m_filters[m_view]);
	SongDb::artists_t artists = m_songDb.getArtists(massagedFilter);
	m_listing.clear();
	m_listing.push_back(make_pair(m_config.getItem("LANG_ARTIST_FILTER")+": " + m_filters[m_view], (int)TYPE_FILTER));
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	bool hasUnknown = false;
	for(SongDb::artists_t::iterator aIter = artists.begin();
		aIter != artists.end();
		++aIter) {
		if((*aIter) != m_config.getItem("LANG_UNKNOWN"))
			m_listing.push_back(make_pair((*aIter), (int)TYPE_FOLDER));
		else 
			hasUnknown = true;
	}
	m_listing.push_back(make_pair(m_config.getItem("LANG_UNKNOWN"), (int)TYPE_FOLDER));
	m_lastItemNum = m_listing.size()-1;
/*
	mpd_sendListCommand(m_mpd, MPD_TAG_ITEM_ARTIST, NULL);

	m_listing.clear();
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	m_listing.push_back(make_pair("Artist Filter: " + m_filters[m_view], (int)TYPE_FILTER));
	char * artist = mpd_getNextArtist(m_mpd);	
	while(artist != NULL) {
		m_listing.push_back(make_pair(artist, (int)TYPE_FOLDER));
		free(artist);
		artist = mpd_getNextArtist(m_mpd);	
	}
	m_lastItemNum = m_listing.size()-1;
*/
}

void Browser::browseAlbumsByArtist(string artist) {
cout << "here" << artist << endl;
	mpd_sendListCommand(m_mpd, MPD_TAG_ITEM_ALBUM, artist.c_str());
	m_listing.clear();
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	char * album = mpd_getNextAlbum(m_mpd);	
	while(album != NULL) {
		m_listing.push_back(make_pair(album, (int)TYPE_FOLDER));
		free(album);
		album = mpd_getNextAlbum(m_mpd);	
	}
	if(m_listing.size() > 2 || m_listing.size() == 1)
		m_listing.insert(m_listing.begin()+1, make_pair(m_config.getItem("LANG_ALL_SONGS_ARTIST"), (int)TYPE_ALL));
		
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseAlbums() {
	string massagedFilter = replaceWildcard(m_filters[m_view]);
	SongDb::albums_t albums = m_songDb.getAlbums(massagedFilter);
	m_listing.clear();
	m_listing.push_back(make_pair(m_config.getItem("LANG_ALBUMS_FILTER")+": " + m_filters[m_view], (int)TYPE_FILTER));
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	bool hasUnknown = false;
	for(SongDb::albums_t::iterator aIter = albums.begin();
		aIter != albums.end();
		++aIter) {
		if((*aIter) != m_config.getItem("LANG_UNKNOWN"))
			m_listing.push_back(make_pair((*aIter), (int)TYPE_FOLDER));
		else 
			hasUnknown = true;
	}
	m_listing.push_back(make_pair(m_config.getItem("LANG_UNKNOWN"), (int)TYPE_FOLDER));
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseAlbumsByGenre(string genre)
{
	SongDb::albums_t albums = m_songDb.getAlbumsInGenre(genre);
	m_listing.clear();
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	bool hasUnknown = false;
	for(SongDb::albums_t::iterator aIter = albums.begin();
		aIter != albums.end();
		++aIter) {
		if((*aIter) != m_config.getItem("LANG_UNKNOWN"))
			m_listing.push_back(make_pair((*aIter), (int)TYPE_FOLDER));
		else 
			hasUnknown = true;
	}
	if(hasUnknown)
		m_listing.push_back(make_pair(m_config.getItem("LANG_UNKNOWN"), (int)TYPE_FOLDER));
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseSongsByGenre(string genre, string album)
{
	if(album == m_config.getItem("LANG_ALL_SONGS_GENRE"))
		album.clear();
	SongDb::songsAndPaths_t songsAndPaths = m_songDb.getSongsInGenre(genre, album);
	m_listing.clear();
	m_curSongPaths.clear();
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	for(SongDb::songsAndPaths_t::iterator sIter = songsAndPaths.begin();
		sIter != songsAndPaths.end();
		++sIter) {
		if(!(*sIter).first.empty()) {
			m_listing.push_back(make_pair((*sIter).first, (int)TYPE_FILE));
			m_curSongPaths.push_back((*sIter).second);
		}
	}
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseGenres() {
	string massagedFilter = replaceWildcard(m_filters[m_view]);
	SongDb::genres_t genres = m_songDb.getGenres(massagedFilter);
	m_listing.clear();
	m_listing.push_back(make_pair(m_config.getItem("LANG_GENRE_FILTER")+": " + m_filters[m_view], (int)TYPE_FILTER));
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	bool hasUnknown = false;
	for(SongDb::genres_t::iterator gIter = genres.begin();
		gIter != genres.end();
		++gIter) {
		if((*gIter) != m_config.getItem("LANG_UNKNOWN"))
			m_listing.push_back(make_pair((*gIter), (int)TYPE_FOLDER));
		else 
			hasUnknown = true;
	}
	m_listing.push_back(make_pair(m_config.getItem("LANG_UNKNOWN"), (int)TYPE_FOLDER));
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseSongs() {
	SongDb::songsAndPaths_t songsAndPaths = m_songDb.getSongs();
	m_listing.clear();
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	for(SongDb::songsAndPaths_t::iterator sIter = songsAndPaths.begin();
		sIter != songsAndPaths.end();
		++sIter) {
		if(!(*sIter).first.empty()) {
			m_listing.push_back(make_pair((*sIter).first, (int)TYPE_FILE));
			m_curSongPaths.push_back((*sIter).second);
		} else {
			m_listing.push_back(make_pair("FIXME filename", (int)TYPE_FILE));
			m_curSongPaths.push_back((*sIter).second);
		}
	}
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseSongsByAlbum(string album, string artist) {
	if(artist == m_config.getItem("LANG_UNKNOWN"))
		artist.clear();
	SongDb::songsAndPaths_t songsAndPaths = m_songDb.getSongsInAlbum(album, artist);
	m_listing.clear();
	m_curSongPaths.clear();
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	for(SongDb::songsAndPaths_t::iterator sIter = songsAndPaths.begin();
		sIter != songsAndPaths.end();
		++sIter) {
		if(!(*sIter).first.empty()) {
			m_listing.push_back(make_pair((*sIter).first, (int)TYPE_FILE));
			m_curSongPaths.push_back((*sIter).second);
		}
	}
	m_lastItemNum = m_listing.size()-1;
}

void Browser::browseSongsByArtist(string artist) {
	SongDb::songsAndPaths_t songsAndPaths = m_songDb.getSongsForArtist(artist);
	m_listing.clear();
	m_curSongPaths.clear();
	m_listing.push_back(make_pair("..", (int)TYPE_BACK));
	for(SongDb::songsAndPaths_t::iterator sIter = songsAndPaths.begin();
		sIter != songsAndPaths.end();
		++sIter) {
		m_listing.push_back(make_pair((*sIter).first, (int)TYPE_FILE));
		m_curSongPaths.push_back((*sIter).second);
	}
	m_lastItemNum = m_listing.size()-1;
}

std::string Browser::currentItemName()
{
	return m_curItemName;
}
std::string Browser::currentItemPath()
{
	return m_curDir+m_curItemName;

}

void Browser::updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus, bool updatingSongDb)
{
	if(mpdStatusChanged & SONG_CHG) {
		m_nowPlaying = mpdStatus->song;	
	} 
	if(mpdStatusChanged & STATE_CHG) { 
		m_curState = mpdStatus->state;
		m_refresh = true;
	}
	if(mpdStatusChanged & UPDB_CHG) {
		if(mpdStatus->updatingDb == 0) 
			m_updatingDb = false;
		else
			m_updatingDb = true;
		ls("");
		m_refresh = true;	
		cout << "updating db " << m_updatingDb<<  endl;
	}
	if(m_updatingSongDb) {
		if(!updatingSongDb) {
			m_updatingSongDb = false;
			ls("");
			m_refresh = true;
		}
	} else if(!m_updatingSongDb) {
		if(updatingSongDb) {
			m_updatingSongDb = true;
			ls("");
			m_refresh = true;
		}	
	}
}

int Browser::processCommand(int command, GuiPos& guiPos)
{
	int newMode = 0;
	if(command > 0) {
		m_refresh = true;
		if(command == CMD_CLICK || command == CMD_HOLD_CLICK) {
			if(guiPos.curY > m_clearRect.y && (guiPos.curY < m_clearRect.y + m_clearRect.h))	 {
				if(guiPos.curX < (m_clearRect.w-40)) {
					m_curItemNum = m_topItemNum + m_itemIndexLookup[guiPos.curY];	
					if(m_curItemNum < m_listing.size()) {
						m_curItemType = m_listing[m_curItemNum].second;
						m_curItemName = m_listing[m_curItemNum].first;
						if(command == CMD_CLICK)
							command = CMD_IMMEDIATE_PLAY;
						else {
							if(m_view != VIEW_ROOT)
								newMode = CMD_POP_CONTEXT;
						}
					} else {
						m_curItemNum = 0;
					}
				} else if(guiPos.curX > (m_clearRect.w-40)) {
					if(guiPos.curY < m_clearRect.y+40) {
						command = CMD_LEFT;
					} else if(guiPos.curY > m_clearRect.y + m_clearRect.h -40) {
						command = CMD_RIGHT;
					}
				}
			}
		}
		if(Scroller::processCommand(command)) {
			//scroller command...parent class processes
		} else {

			std::string dir;
			int pos;
			switch (command) {
				case CMD_IMMEDIATE_PLAY:
				case CMD_MOUSE_LEFT:
					if(m_curItemType == (int)TYPE_FOLDER || m_curItemType == (int)TYPE_BACK
							|| m_curItemType == (int)TYPE_ALL) { //directory
						ls(m_curItemName);
					} else if(m_curItemType == (int)TYPE_FILE) {
						std::string song = "";
						if(m_view == VIEW_FILES) {
							for(prevItems_t::iterator iIter = m_prevItems.begin();
									iIter != m_prevItems.end();
									++iIter) {
								dir += (*iIter).first + "/";
							}
							dir = dir.substr(0, dir.length()-1);
							if(!dir.empty())
								song = dir+"/";
							song += m_curItemName;
						} else {
							song = m_curSongPaths[m_curItemNum-1];
						}
						int id = mpd_sendAddIdCommand(m_mpd, song.c_str());
					cout << " id    " << id << endl;
					cout << " np    " << m_nowPlaying << endl;
						mpd_finishCommand(m_mpd);
						mpd_sendMoveIdCommand(m_mpd, id, m_nowPlaying+1);
						mpd_finishCommand(m_mpd);
						mpd_sendPlayCommand(m_mpd, m_nowPlaying+1);
						mpd_finishCommand(m_mpd);
						newMode = 1;
					} else if(m_curItemType == (int)TYPE_FILTER) {
						m_keyboard.init(CMD_FILTER_KEYBOARD, m_filters[m_view]);
						newMode = CMD_SHOW_KEYBOARD;
					}
					break;
				case CMD_QUEUE: 
					{
						if(m_curItemType == (int)TYPE_FILE) {
							std::string song = "";
							if(m_view == VIEW_FILES) {
								for(prevItems_t::iterator iIter = m_prevItems.begin();
										iIter != m_prevItems.end();
										++iIter) {
									dir += (*iIter).first + "/";
								}
								dir = dir.substr(0, dir.length()-1);
								if(!dir.empty())
									song = dir+"/";
								song += m_curItemName;
							} else {
								song = m_curSongPaths[m_curItemNum-1];
							}
							int pos = m_pl.lastQueued();
							if(pos == -1)
								pos = m_nowPlaying+1;
							else 
								++pos;

							int id = mpd_sendAddIdCommand(m_mpd, song.c_str());
							mpd_finishCommand(m_mpd);
							mpd_sendMoveIdCommand(m_mpd, id, pos);
							mpd_finishCommand(m_mpd);
							m_pl.lastQueued(pos);
							m_queued = true;
						}
					}
					break;
				case CMD_PREV_DIR:
				case CMD_MOUSE_RIGHT:
/*
					pos = m_curDir.rfind("/");;
					if(pos == string::npos || pos == 0) 
						dir = "";
					else
						dir = m_curDir.substr(0, pos);
*/
					m_curItemType = TYPE_BACK;
					
					if(m_prevItems.size() == 0) {
						m_topItemNum = 0;
						switch(m_view) {
							case VIEW_ARTISTS:
								m_curItemNum = 0;
							break;	
							case VIEW_ALBUMS:
								m_curItemNum = 1;
							break;	
							case VIEW_GENRES:
								m_curItemNum = 2;
							break;	
							case VIEW_FILES:
								m_curItemNum = 3;
							break;	
							case VIEW_SONGS:
								m_curItemNum = 4;
							break;	

						}
						m_curItemName = "";
					}
					ls("..");
					break;
				case CMD_ADD_TO_PL: 
					if(m_curItemType == (int)TYPE_FILE) {
						std::string song = "";
						if(m_view == VIEW_FILES) {
							for(prevItems_t::iterator iIter = m_prevItems.begin();
									iIter != m_prevItems.end();
									++iIter) {
								dir += (*iIter).first + "/";
							}
							dir = dir.substr(0, dir.length()-1);
							if(!dir.empty())
								song = dir+"/";
							song += m_curItemName;
						} else {
							song = m_curSongPaths[m_curItemNum-1];
						}
						mpd_sendAddCommand(m_mpd, song.c_str());
						mpd_finishCommand(m_mpd);
					} else if(m_curItemType == TYPE_FOLDER || m_curItemType == TYPE_ALL) {
						addFolderAsPlaylist(true);	
					}
					m_appended = true;
					break;
				case CMD_ADD_AS_PL: 
					if(m_curItemType == TYPE_FOLDER || m_curItemType == TYPE_ALL) {
						addFolderAsPlaylist();	
						newMode = 1;
					}
					break;
				case CMD_PLAY_PAUSE:
					if(m_curState == MPD_STATUS_STATE_PAUSE) {
						m_curState = MPD_STATUS_STATE_PLAY;	
						mpd_sendPauseCommand(m_mpd, 0);
						mpd_finishCommand(m_mpd);
					} else if(m_curState == MPD_STATUS_STATE_PLAY) {
						m_curState = MPD_STATUS_STATE_PAUSE;
						mpd_sendPauseCommand(m_mpd, 1);
						mpd_finishCommand(m_mpd);
					}
					break;
				case CMD_TOGGLE_VIEW:

					if(m_view == 2)
						m_view = 0;
					else
						++m_view;
					
					m_curItemNum = 0;
					m_topItemNum = 0;
					m_curItemName = "";
					m_prevItems.clear();	
					ls("");
					   
					break;
				case CMD_FILTER_KEYBOARD:
					m_filters[m_view] = m_keyboard.getText();
					std::string item;
					switch(m_view) {
						case VIEW_ARTISTS:
							item = m_config.getItem("LANG_ARTISTS");	
						break;
						case VIEW_ALBUMS:
							item = m_config.getItem("LANG_ALBUMS");	
						break;
						case VIEW_GENRES:
							item = m_config.getItem("LANG_GENRES");	
						break;
					
					}
					ls(item);
					newMode = CMD_HIDE_KEYBOARD;	
					break;
			}
		}
	}
	return newMode;
}

void Browser::draw(bool forceRefresh, long timePerFrame, bool inBack)
{
	if(!m_good)
		initAll();
	if(forceRefresh || (!inBack && m_refresh) || m_queued || m_appended) {
		//clear this portion of the screen 
		SDL_SetClipRect(m_screen, &m_clearRect);
		SDL_BlitSurface(m_bg, &m_clearRect, m_screen, &m_clearRect );

		SDL_Surface *sText;
		if(m_listing[0].first != m_config.getItem("LANG_ARTISTS") 
				|| m_listing[4].first != m_config.getItem("LANG_ALL_SONGS")) {
			switch(m_view) {
				case VIEW_ARTISTS:
					sText = TTF_RenderUTF8_Blended(m_font, m_config.getItem("LANG_BROWSE_ARTISTS").c_str(), m_itemColor);
					break;
				case VIEW_ALBUMS:
					sText = TTF_RenderUTF8_Blended(m_font, m_config.getItem("LANG_BROWSE_ALBUMS").c_str(), m_itemColor);
					break;
				case VIEW_GENRES:
					sText = TTF_RenderUTF8_Blended(m_font, m_config.getItem("LANG_BROWSE_GENRES").c_str(), m_itemColor);
					break;
				case VIEW_FILES:
					sText = TTF_RenderUTF8_Blended(m_font, m_curDir.c_str(), m_itemColor);
					break;
				case VIEW_SONGS:
					sText = TTF_RenderUTF8_Blended(m_font, "", m_itemColor);
					break;
				case VIEW_ROOT:
					sText = TTF_RenderUTF8_Blended(m_font, m_config.getItem("LANG_BROWSE_MEDIA").c_str(), m_itemColor);
					break;
				default:
					sText = TTF_RenderUTF8_Blended(m_font, m_curDir.c_str(), m_itemColor);

			}
		} else {
			sText = TTF_RenderUTF8_Blended(m_font, m_config.getItem("LANG_BROWSE_MEDIA").c_str(), m_itemColor);
		}
		if(m_drawIcons) {
			m_destRect.x = m_curItemIconRect.x + 12;
			SDL_BlitSurface(m_iconBrowse, NULL, m_screen, &m_curItemIconRect );
		}

		if(m_listing.size() == 1) {
			sText = TTF_RenderUTF8_Blended(m_font, m_config.getItem("LANG_NO_SONGS").c_str(), m_itemColor);
		}
		SDL_BlitSurface(sText,NULL, m_screen, &m_destRect );
		SDL_FreeSurface(sText);
		
		m_destRect.y += m_skipVal*2;
		m_curItemClearRect.y += m_skipVal*2;
		m_curItemIconRect.y += m_skipVal*2;

		Scroller::draw(timePerFrame, m_drawIcons);	

		if(m_showTime == 10) {
			m_showTime =0;
			m_queued = false;
			m_appended= false;
		}
		if(m_updatingDb || m_updatingSongDb) {
			SDL_Rect dstrect;
			dstrect.x = (m_screen->w - m_dbMsg->w) / 2;
			dstrect.y = (m_screen->h - m_dbMsg->h) / 2;
			dstrect.w = m_dbMsg->w;
			dstrect.h = m_dbMsg->h;

			SDL_SetClipRect(m_screen, &dstrect);
			SDL_BlitSurface(m_dbMsg, NULL, m_screen, &dstrect );
		} else if(m_queued) {
			SDL_Rect dstrect;
			dstrect.x = (m_screen->w - m_queuedImg->w) / 2;
			dstrect.y = (m_screen->h - m_queuedImg->h) / 2;
			dstrect.w = m_queuedImg->w;
			dstrect.h = m_queuedImg->h;
			SDL_SetClipRect(m_screen, &dstrect);
			SDL_BlitSurface(m_queuedImg, NULL, m_screen, &dstrect );
			++m_showTime;
		} else if(m_appended) {
			SDL_Rect dstrect;
			dstrect.x = (m_screen->w - m_appendedImg->w) / 2;
			dstrect.y = (m_screen->h - m_appendedImg->h) / 2;
			dstrect.w = m_appendedImg->w;
			dstrect.h = m_appendedImg->h;

			SDL_SetClipRect(m_screen, &dstrect);
			SDL_BlitSurface(m_appendedImg, NULL, m_screen, &dstrect );
			++m_showTime;
		}
		m_refresh = false;
	}
	
}

string Browser::replaceWildcard(string input)
{
	for(int i=0; i<input.length(); i++) {
		if(input[i] == '*')
			input.replace(i, 1, "%");	
	}
	return input;
}

void Browser::addFolderAsPlaylist(bool append)
{
	switch(m_view) {
		case VIEW_ARTISTS:		
			if(m_curItemType == TYPE_ALL) {	
				artistAsPlaylist(m_prevItems[m_prevItems.size()-1].first, append);
			} else {
				if(m_prevItems.size() == 0)
					artistAsPlaylist(m_curItemName, append);
				else if(m_prevItems.size() == 1)
					albumAsPlaylist(m_curItemName, m_prevItems[0].first, append);
			}
			break;
		case VIEW_ALBUMS:
				albumAsPlaylist(m_curItemName, "", append);
			break;
		case VIEW_GENRES:
				if(m_prevItems.size() == 0) 
					genreAsPlaylist(m_curItemName, "", append);
				else if(m_prevItems.size() == 1)
					genreAsPlaylist(m_prevItems[0].first, m_curItemName, append);
			break;
		case VIEW_FILES:
			std::string dir;
			for(prevItems_t::iterator iIter = m_prevItems.begin();
					iIter != m_prevItems.end();
					++iIter) {
				dir += (*iIter).first + "/";
			}
			dir += m_curItemName;
			folderAsPlaylist(dir, append);

			break;

	}
}

void Browser::artistAsPlaylist(string artist, bool append)
{
	if(!append) {
		mpd_sendClearCommand(m_mpd);
		mpd_finishCommand(m_mpd);
	}
	SongDb::songsAndPaths_t songsAndPaths = m_songDb.getSongsForArtist(artist);
    for(SongDb::songsAndPaths_t::iterator sIter = songsAndPaths.begin();
        sIter != songsAndPaths.end();
        ++sIter) {
		mpd_sendAddCommand(m_mpd, (*sIter).second.c_str());	
		mpd_finishCommand(m_mpd);
    }
}

void Browser::albumAsPlaylist(string album, string artist, bool append)
{
	if(!append) {
		mpd_sendClearCommand(m_mpd);
		mpd_finishCommand(m_mpd);
	}
	SongDb::songsAndPaths_t songsAndPaths = m_songDb.getSongsInAlbum(album, artist);
    for(SongDb::songsAndPaths_t::iterator sIter = songsAndPaths.begin();
        sIter != songsAndPaths.end();
        ++sIter) {
		mpd_sendAddCommand(m_mpd, (*sIter).second.c_str());	
		mpd_finishCommand(m_mpd);
    }
}

void Browser::genreAsPlaylist(string genre, string album, bool append)
{
	if(!append) {
		mpd_sendClearCommand(m_mpd);
		mpd_finishCommand(m_mpd);
	}
	SongDb::songsAndPaths_t songsAndPaths = m_songDb.getSongsInGenre(genre, album);
    for(SongDb::songsAndPaths_t::iterator sIter = songsAndPaths.begin();
        sIter != songsAndPaths.end();
        ++sIter) {
		mpd_sendAddCommand(m_mpd, (*sIter).second.c_str());	
		mpd_finishCommand(m_mpd);
    }
}

void Browser::folderAsPlaylist(string path, bool append)
{
	if(!append) {
		cout << path << endl;
		mpd_sendClearCommand(m_mpd);
		mpd_finishCommand(m_mpd);
	}
	SongDb::songsAndPaths_t songsAndPaths = m_songDb.getSongsInFolder(path);
    for(SongDb::songsAndPaths_t::iterator sIter = songsAndPaths.begin();
        sIter != songsAndPaths.end();
        ++sIter) {
		mpd_sendAddCommand(m_mpd, (*sIter).second.c_str());	
		mpd_finishCommand(m_mpd);
    }
}
