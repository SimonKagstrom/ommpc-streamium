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

#ifndef __BROWSER_H__
#define __BROWSER_H__

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include "libmpdclient.h"
#include "scroller.h"

class Config;
class GuiPos;
class SongDb;
class Keyboard;
class Playlist;

class Browser : public Scroller
{
public:
 	enum {PG_ARTISTS, PG_ALBUMS, PG_SONGS};
 	enum {TYPE_FOLDER, TYPE_FILE, TYPE_BACK, TYPE_ALL, TYPE_FILTER};
 	enum {VIEW_ROOT, VIEW_FILES, VIEW_ARTISTS, VIEW_ALBUMS, VIEW_SONGS, VIEW_GENRES};
	typedef std::vector<std::pair<std::string, int> > prevItems_t;
    Browser(mpd_Connection* mpd, SDL_Surface* bg, SDL_Surface* screen, TTF_Font* font, SDL_Rect& rect, Config& config, int skipVal, int numPerScreen, SongDb& songdb, Keyboard& kb, Playlist& pl);
    void ls(std::string dir="");
	
	void browseRoot();
	void browseFileSystem(std::string dir);
	void browseArtists();	
	void browseAlbums();	
	void browseSongs();
	void browseGenres();
	void browseAlbumsByGenre(std::string genre);	
	void browseSongsByGenre(std::string genre, std::string album);	
	void browseAlbumsByArtist(std::string artist);	
	void browseSongsByAlbum(std::string item, std::string artist="");
	void browseSongsByArtist(std::string artist);
	void updateStatus(int mpdStatusChanged, mpd_Status* mpdStatus, bool updatingSongDb);
	int processCommand(int command, GuiPos& guiPos);
	void draw(bool forceRefresh, long timePerFrame, bool inBack);
	std::string currentItemName();
	std::string currentItemPath();

	bool updatingDb() { return m_updatingDb;}
protected:
	void initAll();
	bool m_good;
	std::string replaceWildcard(std::string input);
	void addFolderAsPlaylist(bool append=false);
	void artistAsPlaylist(std::string artist, bool append=false);
	void albumAsPlaylist(std::string album, std::string artist, bool append=false);
	void genreAsPlaylist(std::string genre, std::string album, bool append=false);
	void folderAsPlaylist(std::string path, bool append=false);
	std::string m_curDir;
	
	int m_view;
	prevItems_t m_prevItems;
	std::vector<std::string> m_curSongPaths;
	int m_nowPlaying;
	bool m_updatingDb;
	bool m_updatingSongDb;
	int m_pageType;
	std::vector<std::string> m_filters;
	SDL_Surface* m_dbMsg;
	SDL_Surface* m_appendedImg;
	SDL_Surface* m_queuedImg;
	SDL_Surface * m_iconBrowse;
	SongDb& m_songDb;
	Keyboard& m_keyboard;
	Playlist& m_pl;
	bool m_drawIcons;
	bool m_queued;
	bool m_appended;
	int m_showTime;
};

#endif
