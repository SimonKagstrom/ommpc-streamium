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

#ifndef __GP2XREGS_H__
#define __GP2XREGS_H__

#include <fcntl.h>
#include <sys/mman.h>

#if defined(GP2X)
	#define OFF_GPIOH          (0x106E)
	#define OFF_GPIOL          (0x1076)
	#define PWR_TFT_BIT        (11)
	#define SYS_CLK_FREQ 7372800 
#elif defined(WIZ)
	#define SYS_CLK_FREQ 27
#endif


class GP2XRegs
{
public:
	GP2XRegs();
	~GP2XRegs();

	void setClock(unsigned int mhz);
	void toggleScreen();	
	inline bool screenIsOff() {return m_screenIsOff;}
	inline int version() {return m_version;}
	inline void vsync()
	{
#if defined(WIZ)
		while((m_memregs16[0x308C] & (1 << 10)) == 0);
		m_memregs16[0x308C] |= (1 << 10);
#endif
	};

protected:
	void initVersion();
	bool m_screenIsOff;
	int m_version;
#if defined(GP2X) || defined(WIZ)
	int m_memfd;
	unsigned long* m_memregs32;
	unsigned short* m_memregs16;
	bool m_f200;
#endif


};


#endif
