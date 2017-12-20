/* ************************************************************************************
* Copyright (C) 2015-2017 Shaanxi QinWei Electroics Technologies. All rights reserved.
*
* $Date:        9. May 2017
* $Revision: 	V 0.4
*
* Project: 	    QL6502 - A 6502 Assembler for Game in 1990s
*
* Title:	    main.c
*
* Description:	The Win32 console program main entry for QLForth
*
* Target:		Windows 7+
*
* Author:		Zhao Yu, forthchina@163.com, forthchina@gmail.com 
*			    24/03/2017 (In dd/mm/yyyy)
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of author nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* ********************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "QL6502.h"

#define BUFFER_SIZE		0x10000

static unsigned char m2367_data[] = {
	0x5a, 0x5a, 0xaa, 0xaa, 0x55, 0x55, 0xa5, 0xa5,
	0x5a, 0x5a, 0x55, 0x55, 0x5a, 0x5a, 0xaa, 0xaa
};

static unsigned char mask_bit[] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static unsigned char src_buf[BUFFER_SIZE + 16], dst_buf[BUFFER_SIZE + 16];
static int addr;

static int unpack_mask0(int code)
{
	unsigned int m;

	switch (code & 0x5a) {
		case 0x00:	m = 0x5a;	break;
		case 0x02:	m = 0x1a;	break;
		case 0x08:	m = 0x58;	break;
		case 0x0a:  m = 0x18;	break;
		case 0x10:	m = 0x52;	break;
		case 0x12:	m = 0x12;	break;
		case 0x18:	m = 0x50;	break;
		case 0x1a:  m = 0x10;	break;
		case 0x40:	m = 0x4a;	break;
		case 0x42:	m = 0x0a;	break;
		case 0x48:	m = 0x48;	break;
		case 0x4a:  m = 0x08;	break;
		case 0x50:	m = 0x42;	break;
		case 0x52:	m = 0x02;	break;
		case 0x58:	m = 0x40;	break;
		case 0x5a:  m = 0x00;	break;
		default: 	printf("Error At Address : L_%04X\n", addr);	exit(-1);
	}
	return m;
}

static int pack_mask0(int code)
{
	unsigned int m ;

	switch(code & 0x5a) {
		case 0x00 :		m = 0x5a ;	break ;
		case 0x02 :		m = 0x52 ;	break ;
		case 0x08 :		m = 0x4a ;	break ;
		case 0x0a :     m = 0x42 ;	break ;
		case 0x10 :		m = 0x1a ;	break ;
		case 0x12 :		m = 0x12 ;	break ;
		case 0x18 :		m = 0x0a ;	break ;
		case 0x1a :     m = 0x02 ;	break ;
		case 0x40 :		m = 0x58 ;	break ;
		case 0x42 :		m = 0x50 ;	break ;
		case 0x48 :		m = 0x48 ;	break ;
		case 0x4a :     m = 0x40 ;	break ;
		case 0x50 :		m = 0x18 ;	break ;
		case 0x52 :		m = 0x10 ;	break ;
		case 0x58 :		m = 0x08 ;	break ;
		case 0x5a :     m = 0x00 ;	break ;
		default   : 	printf("Error At Address : L_%04X\n", addr) ;

  	}
  	return m ;
}

static int unpack_mask1(int code)
{
	unsigned int m;

	switch (code & 0x5a) {
		case 0x00:	m = 0x18;	break;
		case 0x02:	m = 0x10;	break;
		case 0x08:	m = 0x08;	break;
		case 0x0a:  m = 0x00;	break;
		case 0x10:	m = 0x58;	break;
		case 0x12:	m = 0x50;	break;
		case 0x18:	m = 0x48;	break;
		case 0x1a:  m = 0x40;	break;
		case 0x40:	m = 0x1a;	break;
		case 0x42:	m = 0x12;	break;
		case 0x48:	m = 0x0a;	break;
		case 0x4a:  m = 0x02;	break;
		case 0x50:	m = 0x5a;	break;
		case 0x52:	m = 0x52;	break;
		case 0x58:	m = 0x4a;	break;
		case 0x5a:  m = 0x42;	break;
		default: 	printf("Error At Address : L_%04X\n", addr);	exit(-1);
	}
	return m;
}

static int pack_mask1(int code)
{
	unsigned int m ;

	switch(code & 0x5a) {
		case 0x00 :		m = 0x0a ;	break ;
		case 0x02 :		m = 0x4a ;	break ;
		case 0x08 :		m = 0x08 ;	break ;
		case 0x0a :     m = 0x48 ;	break ;
		case 0x10 :		m = 0x02 ;	break ;
		case 0x12 :		m = 0x42 ;	break ;
		case 0x18 :		m = 0x00 ;	break ;
		case 0x1a :     m = 0x40 ;	break ;
		case 0x40 :		m = 0x1a ;	break ;
		case 0x42 :		m = 0x5a ;	break ;
		case 0x48 :		m = 0x18 ;	break ;
		case 0x4a :     m = 0x58 ;	break ;
		case 0x50 :		m = 0x12 ;	break ;
		case 0x52 :		m = 0x52 ;	break ;
		case 0x58 :		m = 0x10 ;	break ;
		case 0x5a :     m = 0x50 ;	break ;
		default   : 	printf("Error At Address : L_%04X\n", addr) ;
  	}
  	return m ;
}

static int unpack_mask4(int code)
{
	unsigned int m;

	switch (code & 0x5a) { 
		case 0x00:	m = 0x58;	break;
		case 0x02:	m = 0x48;	break;
		case 0x08:	m = 0x5a;	break;
		case 0x0a:  m = 0x4a;	break;
		case 0x10:	m = 0x50;	break;
		case 0x12:	m = 0x40;	break;
		case 0x18:	m = 0x52;	break;
		case 0x1a:  m = 0x42;	break;
		case 0x40:	m = 0x18;	break;
		case 0x42:	m = 0x08;	break;
		case 0x48:	m = 0x1a;	break;
		case 0x4a:  m = 0x0a;	break;
		case 0x50:	m = 0x10;	break;
		case 0x52:	m = 0x00;	break;
		case 0x58:	m = 0x12;	break;
		case 0x5a:  m = 0x02;	break;
		default: 	printf("Error At Address : L_%04X\n", addr);	exit(-1);
	}
	return m;
}

static int pack_mask4(int code)
{
	unsigned int m ;

	switch(code & 0x5a) {
		case 0x00 :		m = 0x52 ;	break ;
		case 0x02 :		m = 0x5a ;	break ;
		case 0x08 :		m = 0x42 ;	break ;
		case 0x0a :     m = 0x4a ;	break ;
		case 0x10 :		m = 0x50 ;	break ;
		case 0x12 :		m = 0x58 ;	break ;
		case 0x18 :		m = 0x40 ;	break ;
		case 0x1a :		m = 0x48 ;	break ;
		case 0x40 :		m = 0x12 ;	break ;
		case 0x42 :		m = 0x1a ;	break ;
		case 0x48 :		m = 0x02 ;	break ;
		case 0x4a :		m = 0x0a ;	break ;
		case 0x50 :		m = 0x10 ;	break ;
		case 0x52 :		m = 0x18 ;	break ;
		case 0x58 :		m = 0x00 ;	break ;
		case 0x5a :		m = 0x08 ;	break ;
		default   : 	printf("Error At Address : L_%04X\n", addr) ;
  	}
  	return m ;
}

static int unpack_mask5(int code)
{
	unsigned int m;

	switch (code & 0x5a) {
		case 0x00:	m = 0x1a;	break;
		case 0x02:	m = 0x5a;	break;
		case 0x08:	m = 0x0a;	break;
		case 0x0a:  m = 0x4a;	break;
		case 0x10:	m = 0x12;	break;
		case 0x12:	m = 0x52;	break;
		case 0x18:	m = 0x02;	break;
		case 0x1a:  m = 0x42;	break;
		case 0x40:	m = 0x18;	break;
		case 0x42:	m = 0x58;	break;
		case 0x48:	m = 0x08;	break;
		case 0x4a:  m = 0x48;	break;
		case 0x50:	m = 0x10;	break;
		case 0x52:	m = 0x50;	break;
		case 0x58:	m = 0x00;	break;
		case 0x5a:  m = 0x40;	break;
		default: 	printf("Error At Address : L_%04X\n", addr);	exit(-1);
	}
	return m;
}

static int pack_mask5(int code)
{
	unsigned int m ;

	switch(code & 0x5a) {
		case 0x00 :		m = 0x58 ;	break ;
		case 0x02 :		m = 0x18 ;	break ;
		case 0x08 :		m = 0x48 ;	break ;
		case 0x0a :     m = 0x08 ;	break ;
		case 0x10 :		m = 0x50 ;	break ;
		case 0x12 :		m = 0x10 ;	break ;
		case 0x18 :		m = 0x40 ;	break ;
		case 0x1a :     m = 0x00 ;	break ;
		case 0x40 :		m = 0x5a ;	break ;
		case 0x42 :		m = 0x1a ;	break ;
		case 0x48 :		m = 0x4a ;	break ;
		case 0x4a :     m = 0x0a ;	break ;
		case 0x50 :		m = 0x52 ;	break ;
		case 0x52 :		m = 0x12 ;	break ;
		case 0x58 :		m = 0x42 ;	break ;
		case 0x5a :     m = 0x02 ;	break ;
		default   : 	printf("Error At Address : L_%04X\n", addr) ;
	}
	return m ;
}

static int unpack_mask_g1(int code)
{
	unsigned int m;

	switch (code & 0x5a) {
		case 0x00:	m = 0x58;	break;
		case 0x02:	m = 0x50;	break;
		case 0x08:	m = 0x18;	break;
		case 0x0a:  m = 0x10;	break;
		case 0x10:	m = 0x5a;	break;
		case 0x12:	m = 0x52;	break;
		case 0x18:	m = 0x1a;	break;
		case 0x1a:  m = 0x12;	break;
		case 0x40:	m = 0x48;	break;
		case 0x42:	m = 0x40;	break;
		case 0x48:	m = 0x08;	break;
		case 0x4a:  m = 0x00;	break;
		case 0x50:	m = 0x4a;	break;
		case 0x52:	m = 0x42;	break;
		case 0x58:	m = 0x0a;	break;
		case 0x5a:  m = 0x02;	break;
		default: 	printf("Error At Address : L_%04X\n", addr);	exit(-1);
	}
	return m;
}

static int pack_mask_g1(int code)
{
	unsigned int m ;

	switch(code & 0x5a) {
		case 0x00 :		m = 0x4a ;	break ;
		case 0x02 :		m = 0x5a ;	break ;
		case 0x08 :		m = 0x48 ;	break ;
		case 0x0a :     m = 0x58 ;	break ;
		case 0x10 :		m = 0x0a ;	break ;
		case 0x12 :		m = 0x1a ;	break ;
		case 0x18 :		m = 0x08 ;	break ;
		case 0x1a :     m = 0x18 ;	break ;
		case 0x40 :		m = 0x42 ;	break ;
		case 0x42 :		m = 0x52 ;	break ;
		case 0x48 :		m = 0x40 ;	break ;
		case 0x4a :     m = 0x50 ;	break ;
		case 0x50 :		m = 0x02 ;	break ;
		case 0x52 :		m = 0x12 ;	break ;
		case 0x58 :		m = 0x00 ;	break ;
		case 0x5a :     m = 0x10 ;	break ;
		default   : 	printf("Error At Address : L_%04X\n", addr) ;
	}
	return m ;
}

static int unpack_mask_g2(int code)
{
	unsigned int m;

	switch (code & 0x5a) {
		case 0x00:	m = 0x52;	break;
		case 0x02:	m = 0x42;	break;
		case 0x08:	m = 0x50;	break;
		case 0x0a:  m = 0x40;	break;
		case 0x10:	m = 0x5a;	break;
		case 0x12:	m = 0x4a;	break;
		case 0x18:	m = 0x58;	break;
		case 0x1a:  m = 0x48;	break;
		case 0x40:	m = 0x12;	break;
		case 0x42:	m = 0x02;	break;
		case 0x48:	m = 0x10;	break;
		case 0x4a:  m = 0x00;	break;
		case 0x50:	m = 0x1a;	break;
		case 0x52:	m = 0x0a;	break;
		case 0x58:	m = 0x18;	break;
		case 0x5a:  m = 0x08;	break;
		default: 	printf("Error At Address : L_%04X\n", addr);	exit(-1);
	}
	return m;
}

static int pack_mask_g2(int code)
{
	unsigned int m ;

	switch(code & 0x5a) {
		case 0x00 :		m = 0x4a ;	break ;
		case 0x02 :		m = 0x42 ;	break ;
		case 0x08 :		m = 0x5a ;	break ;
		case 0x0a :     m = 0x52 ;	break ;
		case 0x10 :		m = 0x48 ;	break ;
		case 0x12 :		m = 0x40 ;	break ;
		case 0x18 :		m = 0x58 ;	break ;
		case 0x1a :     m = 0x50 ;	break ;
		case 0x40 :		m = 0x0a ;	break ;
		case 0x42 :		m = 0x02 ;	break ;
		case 0x48 :		m = 0x1a ;	break ;
		case 0x4a :     m = 0x12 ;	break ;
		case 0x50 :		m = 0x08 ;  break ;
		case 0x52 :		m = 0x00 ;	break ;
		case 0x58 :		m = 0x18 ;	break ;
		case 0x5a :     m = 0x10 ;	break ;
		default   : 	printf("Error At Address : L_%04X\n", addr) ;
	}
	return m ;
}

static int get_m2367(unsigned int addr)
{
	int hi, low, off, byte;

	hi	= (addr & 0x38) >> 3;	
	low = addr & 0x07;
	off = (addr & 0x0100) ? 0x08 : 0x00;
	byte = m2367_data[off + hi]; 
	return (byte & mask_bit[low]);
}

static void unpack_proc_2367(unsigned int begin)
{
	unsigned int i, flag;

	for (addr = begin, i = 0; i <= 0x0fff; i++) {
		flag = get_m2367(addr);
		if (flag) {
			src_buf[addr] = (src_buf[addr] & 0xa5) | unpack_mask_g2(src_buf[addr]);
		}
		else {
			src_buf[addr] = (src_buf[addr] & 0xa5) | unpack_mask_g1(src_buf[addr]);
		}
		addr++;
	}
}

static void pack_proc_2367(unsigned int begin)
{
	unsigned int i, flag;

	for (addr = begin, i = 0; i <= 0x0fff; i++) {
		flag = get_m2367(addr);
		if (flag) {
			src_buf[addr] = (src_buf[addr] & 0xa5) | pack_mask_g2(src_buf[addr]);
		}
		else {
			src_buf[addr] = (src_buf[addr] & 0xa5) | pack_mask_g1(src_buf[addr]);
		}
		addr++;
	}
}

static void unpack_2000(void)
{
	unsigned int addr;

	for (addr = 0; addr <= 0x0fff; addr++) {
		src_buf[addr] = (src_buf[addr] & 0xa5) | unpack_mask0(src_buf[addr]);
	}

	for (addr = 0x1000; addr <= 0x1fff; addr++) {
		src_buf[addr] = (src_buf[addr] & 0xa5) | unpack_mask1(src_buf[addr]);
	}

	for (addr = 0x4000; addr <= 0x4fff; addr++) {
		src_buf[addr] = (src_buf[addr] & 0xa5) | unpack_mask4(src_buf[addr]);
	}

	for (addr = 0x5000; addr <= 0x5fff; addr++) {
		src_buf[addr] = (src_buf[addr] & 0xa5) | unpack_mask5(src_buf[addr]);
	}
	unpack_proc_2367(0x2000);		
	unpack_proc_2367(0x3000);
	unpack_proc_2367(0x6000);		
	unpack_proc_2367(0x7000);

	memmove(dst_buf, src_buf, 0x8000);
}

static void pack_2000(void)
{
	unsigned int addr ;

	for (addr = 0 ; addr <= 0x0fff; addr ++)
		src_buf[addr] = (src_buf[addr] & 0xa5) | pack_mask0(src_buf[addr]) ;
	for (addr = 0x1000 ; addr <= 0x1fff; addr ++)
		src_buf[addr] = (src_buf[addr] & 0xa5) | pack_mask1(src_buf[addr]) ;
	for (addr = 0x4000 ; addr <= 0x4fff; addr ++)
		src_buf[addr] = (src_buf[addr] & 0xa5) | pack_mask4(src_buf[addr]) ;
	for (addr = 0x5000 ; addr <= 0x5fff; addr ++)
		src_buf[addr] = (src_buf[addr] & 0xa5) | pack_mask5(src_buf[addr]) ;
	pack_proc_2367(0x2000) ;
	pack_proc_2367(0x3000) ;
	pack_proc_2367(0x6000) ;
	pack_proc_2367(0x7000) ;

	memmove(dst_buf, src_buf, 0x8000);
}

static void UnPack_ROM_2000(char * src_name, char * dst_name) {
	FILE 	* fd;

	printf("Unpacking %s -> %s\n", src_name, dst_name);

	if ((fd = fopen(src_name, "rb")) == NULL) {
		printf("Can not Open Input File : %s.\n", src_name);
		exit(-1);
	}

	fread(src_buf, 1, BUFFER_SIZE, fd);
	fclose(fd);

	unpack_2000();

	if ((fd = fopen(dst_name, "wb")) == NULL) {
		printf("Can not Create Output File : %s.\n", dst_name);
		exit(-1);
	}
	
	memmove(dst_buf + 0xC000, dst_buf + 0x4000, 0x4000);
	memmove(dst_buf + 0x4000, dst_buf, 0x4000);
	memset(dst_buf, 0xff, 0x4000);
	memset(dst_buf + 0x8000, 0xff, 0x4000);

	fwrite(dst_buf, 1, 0x10000, fd);
	fclose(fd);
	printf("Ok.\n");
}

static void Pack_ROM_2000(char * src_name, char * dst_name) {
	FILE 	* fd;

	printf("Packing 2000 type ROM file : %s -> %s\n", src_name, dst_name);

	if ((fd = fopen(src_name, "rb")) == NULL) {
		printf("Can not Open Input File : %s.\n", src_name);
		exit(-1);
	}

	fread(src_buf, 1, BUFFER_SIZE, fd);
	fclose(fd);

	memmove(&src_buf[0x0000], &src_buf[0x4000], 0x4000);
	memmove(&src_buf[0x4000], &src_buf[0xc000], 0x4000);

	pack_2000();

	if ((fd = fopen(dst_name, "wb")) == NULL) {
		printf("Can not Create Output File : %s.\n", dst_name);
		exit(-1);
	}
	fwrite(dst_buf, 1, 0x8000, fd);
	fclose(fd);

	printf("Ok.\n");

}

// ....................................................................................

static void relocation_27512(void) {
	memmove(dst_buf, dst_buf + 0x5000, 0x1000);
	memmove(dst_buf + 0x5000, dst_buf + 0xC000, 0x1000);
	memmove(dst_buf + 0xC000, dst_buf, 0x1000);

	memmove(dst_buf, dst_buf + 0x7000, 0x1000);
	memmove(dst_buf + 0x7000, dst_buf + 0xE000, 0x1000);
	memmove(dst_buf + 0xE000, dst_buf, 0x1000);
	
	memset(dst_buf, 0xff, 0x4000);
	memset(dst_buf + 0x8000, 0xff, 0x4000);
}

static void un_pack_27512(void) {
	int idx, pc, dd, data;

	for (idx = 0; idx < BUFFER_SIZE; idx ++) {
		pc = idx;

		if ((idx & 0x0003) == 0x0001) {
			pc = (idx & 0xFFFC) + 0x02;
		}
		if ((idx & 0x0003) == 0x02) {
			pc = (idx & 0xFFFC) + 0x0001;
		}

		dd = src_buf[pc];

		data = 0;
		if (dd & 0x01) data |= 0x80;
		if (dd & 0x02) data |= 0x01;
		if (dd & 0x04) data |= 0x40;
		if (dd & 0x08) data |= 0x02;
		if (dd & 0x10) data |= 0x08;
		if (dd & 0x20) data |= 0x04;
		if (dd & 0x40) data |= 0x10;
		if (dd & 0x80) data |= 0x20;
		dst_buf[idx] = data;
	}
}

static void pack_27512(void)
{
	int idx, pc, dd, data;

	for (idx = 0; idx < BUFFER_SIZE; idx++) {
		pc = idx;

		if ((idx & 0x0003) == 0x0001) {
			pc = (idx & 0xFFFC) + 0x02;
		}
		if ((idx & 0x0003) == 0x02) {
			pc = (idx & 0xFFFC) + 0x0001;
		}

		dd = src_buf[pc];

		data = 0;
		if (dd & 0x80) data |= 0x01;
		if (dd & 0x01) data |= 0x02;
		if (dd & 0x40) data |= 0x04;
		if (dd & 0x02) data |= 0x08;
		if (dd & 0x08) data |= 0x10;
		if (dd & 0x04) data |= 0x20;
		if (dd & 0x10) data |= 0x40;
		if (dd & 0x20) data |= 0x80;
		dst_buf[idx] = data;
	}
}

static void UnPack_ROM_27512(char * src_name, char * dst_name) {
	FILE 	* fd;

	printf("Unpacking %s -> %s\n", src_name, dst_name);

	if ((fd = fopen(src_name, "rb")) == NULL) {
		printf("Can not Open Input File : %s.\n", src_name);
		exit(-1);
	}

	fread(src_buf, 1, BUFFER_SIZE, fd);
	fclose(fd);

	un_pack_27512();
	relocation_27512();

	if ((fd = fopen(dst_name, "wb")) == NULL) {
		printf("Can not Create Output File : %s.\n", dst_name);
		exit(-1);
	}
	fwrite(dst_buf, 1, BUFFER_SIZE, fd);
	fclose(fd);

	printf("Ok.\n");
}

static void Pack_ROM_27512(char * src_name, char * dst_name) {
	FILE 	* fd;

	printf("Packing %s -> %s\n", src_name, dst_name);

	if ((fd = fopen(src_name, "rb")) == NULL) {
		printf("Can not Open Input File : %s.\n", src_name);
		exit(-1);
	}

	fread(src_buf, 1, BUFFER_SIZE, fd);
	fclose(fd);

	pack_27512();
	relocation_27512();
	if ((fd = fopen(dst_name, "wb")) == NULL) {
		printf("Can not Create Output File : %s.\n", dst_name);
		exit(-1);
	}
	fwrite(dst_buf, 1, BUFFER_SIZE, fd);
	fclose(fd);

	printf("Ok.\n");
}

// ....................................................................................

static void image_pack(char * src_name, char * dst_name) {
	FILE 	* fd;

	printf("Build Image %s -> %s\n", src_name, dst_name);

	if ((fd = fopen(src_name, "rb")) == NULL) {
		printf("Can not Open Input File : %s.\n", src_name);
		exit(-1);
	}

	memset(src_buf, 0, BUFFER_SIZE);
	fread(&src_buf[0x4000], 1, 16384, fd);
	fclose(fd);

	if ((fd = fopen(dst_name, "wb")) == NULL) {
		printf("Can not Create Output File : %s.\n", dst_name);
		exit(-1);
	}

	fwrite(src_buf, 1, 32768, fd);
	fclose(fd);

	printf("Ok.\n");
}

// ************************************************************************************

void Pack(char * op, char * src, char * dst) {
	if (strcmp(op, "-p1") == 0) {
		Pack_ROM_2000(src, dst);
	}
	else if (strcmp(op, "-p2") == 0) {
		Pack_ROM_27512(src, dst);
	}
	else  if (strcmp(op, "-u1") == 0) {
		UnPack_ROM_2000(src, dst);
	}
	else if (strcmp(op, "-u2") == 0) {
		UnPack_ROM_27512(src, dst);
	}
	else if (strcmp(op, "-img") == 0) {
		image_pack(src, dst);
	}
	else {
		printf("Unknown command : %s\n", op);
	}
}
