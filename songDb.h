#ifndef __SONGDB_H__
#define  __SONGDB_H__

#include <string>
#include <iostream>
#include <vector>
#include "libmpdclient.h"
#include "sqlite3.h"

class SongDb {

public:
	typedef std::vector<std::pair<std::string, std::string> > songsAndPaths_t;
	typedef std::vector<std::string> albums_t;
	typedef std::vector<std::string> genres_t;
	typedef std::vector<std::string> artists_t;
	SongDb(std::string, int, int);
//	SongDb(mpd_Connection* mpd);
	~SongDb();

	void update();
	bool updating(){return m_updating;};
	void showAll();
	songsAndPaths_t getSongsInAlbum(std::string album, std::string artist);
	songsAndPaths_t getSongsForArtist(std::string artist);
	songsAndPaths_t getSongs();
	albums_t getAlbums(std::string filter="");
	artists_t getArtists(std::string filter="");
	genres_t getGenres(std::string filter="");
	albums_t getAlbumsInGenre(std::string genre);
	songsAndPaths_t getSongsInGenre(std::string genre, std::string album);
	songsAndPaths_t getSongsInFolder(std::string path);
	std::string cleanDoubleQuotes(std::string inStr);
	
private:
	mpd_Connection* m_mpd;
	sqlite3* m_db;
	char* m_errMsg;		
	bool m_updating;
	std::string m_host;
	int m_port;
	int m_timeout;
};

#endif
