#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>

#include "config.h"
#include "libmpdclient.h"

using namespace std;

int main(int argc, char** argv)
{
	Config config;
	
	bool pollOnly = false;
	if(argc > 1) {
		if(strcmp(argv[1], "-pollonly") == 0) {
			cout << "********WATCHDOG******* poll only mode" << endl;
			pollOnly = true;
		}
	}

	
	bool done = false;
	unsigned int oldFreq, newFreq;
	mpd_Connection* mpd = mpd_newConnection(config.getItem("host").c_str(), 
					config.getItemAsNum("port"),
					config.getItemAsNum("timeout"));
	if(mpd->error == 0) {
		mpd_sendStatusCommand(mpd);
		mpd_Status* rtmpdStatus = mpd_getStatus(mpd);
		mpd_finishCommand(mpd);
		int oldVolume = rtmpdStatus->volume;
		int newVolume = oldVolume;
		cout << "init vol " << oldVolume << endl;

		sleep(1);
		while(!done) {
			mpd_sendStatusCommand(mpd);
			mpd_Status* rtmpdStatus = mpd_getStatus(mpd);
			mpd_finishCommand(mpd);
			newVolume = rtmpdStatus->volume;
			if(!pollOnly) {
				if(oldVolume != newVolume && (oldVolume > newVolume+5 || oldVolume < newVolume -5)) {
					mpd_sendSetvolCommand(mpd, oldVolume);
					mpd_finishCommand(mpd);

					cout << "*******WATCHDOG****** Volume adjust" << endl;
					cout << "old vol " << oldVolume << " new vol " << newVolume << endl;
					newVolume = oldVolume;
				} else {
					oldVolume = newVolume;
				} 	
			}
			sleep(1);
		}
	} else {
		cout << "problem connecting to mpd daemon" << endl;
	}
 
}
