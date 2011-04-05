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

#include "gp2xregs.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

GP2XRegs::GP2XRegs()
	: m_screenIsOff(false)
{
#ifdef GP2X
	m_memfd = open("/dev/mem",O_RDWR);
	m_memregs32 = (unsigned long*)mmap(0, 0x10000, PROT_READ|PROT_WRITE, MAP_SHARED, m_memfd, 0xc0000000);
	m_memregs16 = (unsigned short*)m_memregs32;

	struct stat stFileInfo;
	if(stat("/dev/touchscreen/wm97xx",&stFileInfo) == 0)
		m_f200 = true;
	else 
		m_f200 = false;
#elif defined(WIZ)
	m_memfd = open("/dev/mem",O_RDWR);
	m_memregs32 = (unsigned long*)mmap(0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, m_memfd, 0xc0000000);
    if(m_memregs32 == (unsigned long*)0xFFFFFFFF) {
        cout << "Could not mmap hardware registers!" << endl;;
    }
	m_memregs16 = (unsigned short*)m_memregs32;
#endif
	initVersion();
}

GP2XRegs::~GP2XRegs()
{
#if defined(GP2X) || defined(WIZ)
	close(m_memfd);
#endif
}

void GP2XRegs::initVersion()
{
#ifdef GP2X
    ifstream versionFile("/usr/gp2x/version", ios::in);
    if(versionFile.fail()) {
		m_version = 0;
	} else {
		string curItem;
        getline(versionFile, curItem);
		curItem.erase(3,1);
		curItem.erase(1,1);	
					
		m_version = atoi(curItem.c_str());
	}
#else
	m_version = 0;
#endif
}

void GP2XRegs::setClock(unsigned int MHZ) {
#ifdef GP2X
cout << "setting clock to " << MHZ << endl;
	unsigned int v;
	unsigned int mdiv,pdiv=3,scale=0;
	MHZ*=1000000;
	mdiv=(MHZ*pdiv)/SYS_CLK_FREQ;

	mdiv=((mdiv-8)<<8) & 0xff00;
	pdiv=((pdiv-2)<<2) & 0xfc;
	scale&=3;
	v = mdiv | pdiv | scale;

	unsigned int l = m_memregs32[0x808>>2];// Get interupt flags
	m_memregs32[0x808>>2] = 0xFF8FFFE7;   //Turn off interrupts
	m_memregs16[0x910>>1]=v;              //Set frequentie
	while(m_memregs16[0x0902>>1] & 1);    //Wait for the frequentie to be ajused
	m_memregs32[0x808>>2] = l;            //Turn on interrupts
#elif defined(WIZ)
 	unsigned  long v;
    unsigned mdiv, pdiv=9, sdiv=0;

    mdiv= (MHZ * pdiv) / SYS_CLK_FREQ;
    mdiv &= 0x3FF;
    v= pdiv<<18 | mdiv<<8 | sdiv;

	m_memregs32[0xF004>>2] = v;
	m_memregs32[0xF07C>>2] |= 0x8000;
#else
	cout << "setting clock to " << MHZ << endl;
#endif
}


void GP2XRegs::toggleScreen()
{
#ifdef GP2X
	if(m_screenIsOff) {
		//backlight
		if(m_f200)
	        m_memregs16[OFF_GPIOL >> 1] |= 0x0800;
		else
        	m_memregs16[OFF_GPIOH >> 1] |= 0x0004;
		//power to screen
        m_memregs16[OFF_GPIOH >> 1] |= 0x0002;
		m_screenIsOff = false;
	}
    else {
		if(m_f200)
			m_memregs16[OFF_GPIOL >> 1] &= ~0x0800;
		else
			m_memregs16[OFF_GPIOH >> 1] &= ~0x0004;
        
		m_memregs16[OFF_GPIOH >> 1] &= ~0x0002;
		m_screenIsOff = true;
	}
/*
	unsigned short tftState;
	if(m_screenIsOff) {
		tftState = m_memregs16[OFF_GPIOL] | (1<<PWR_TFT_BIT);
		m_memregs16[OFF_GPIOL] = tftState;
		m_screenIsOff = false;
	} else {
		tftState = m_memregs16[OFF_GPIOL] | (1<<PWR_TFT_BIT);
		tftState = tftState & (~(1<<PWR_TFT_BIT));

		m_memregs16[OFF_GPIOL] = tftState;
		m_screenIsOff = true;
	}
*/
#else
	cout << "We would be toggleing the screen now" << endl;	
	m_screenIsOff = !m_screenIsOff;
#endif

}
