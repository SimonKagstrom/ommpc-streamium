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

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <time.h>
#include <sys/time.h>
#include <string>

class TimeStamp
{
public:
        TimeStamp();

        std::string currentTimeAsString(int format, bool getu=false);
        std::string timeAsString1(struct timeval tnow, bool getu=false);
        std::string timeAsString2(struct timeval tnow, bool getu=false);
        std::string timeAsString3(struct timeval tnow, bool getu=false);

};



#endif

