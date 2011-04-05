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

#include "timestamp.h"
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

TimeStamp::TimeStamp()
{
}

std::string TimeStamp::currentTimeAsString(int format, bool getu)
{
    struct timeval tnow;
    struct timezone tz;

    gettimeofday(&tnow, &tz);

    std::string timeStr;
    switch(format) {
        case 1:
            timeStr = timeAsString1(tnow, getu);
        break;
        case 2:
            timeStr = timeAsString2(tnow, getu);
        break;
        case 3:
            timeStr = timeAsString3(tnow, getu);
        break;
        default:
            timeStr = timeAsString1(tnow, getu);
    }
    return timeStr;
}

std::string TimeStamp::timeAsString1(struct timeval tnow, bool getu)
{
    struct tm* now = localtime(&tnow.tv_sec);

    stringstream timeStr;
    timeStr.fill('0');
    timeStr << setw(4) << now->tm_year+1900 << "/"
            << setw(2) << now->tm_mon+1 << "/"
            << setw(2) << now->tm_mday << "-"
            << setw(2) << now->tm_hour << ":"
            << setw(2) << now->tm_min << ":"
            << setw(2) << now->tm_sec;

    return timeStr.str();
}

std::string TimeStamp::timeAsString2(struct timeval tnow, bool getu)
{
    struct tm* now = localtime(&tnow.tv_sec);

    stringstream timeStr;
    timeStr.fill('0');
    timeStr << setw(4) << now->tm_year+1900 << "-"
            << setw(2) << now->tm_mon+1 << "-"
            << setw(2) << now->tm_mday << "T"
            << setw(2) << now->tm_hour << ":"
            << setw(2) << now->tm_min << ":"
            << setw(2) << now->tm_sec;

    return timeStr.str();
}

std::string TimeStamp::timeAsString3(struct timeval tnow, bool getu)
{
    struct tm* now = localtime(&tnow.tv_sec);

    stringstream timeStr;
    timeStr.fill('0');
    timeStr << setw(4) << now->tm_year+1900 << "_"
            << setw(2) << now->tm_mon+1 << "_"
            << setw(2) << now->tm_mday << "-"
            << setw(2) << now->tm_hour << "_"
            << setw(2) << now->tm_min << "_"
            << setw(2) << now->tm_sec;

    if(getu)
        timeStr << "_" << tnow.tv_usec;

    return timeStr.str();
}

