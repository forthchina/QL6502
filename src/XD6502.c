/* ************************************************************************************
* Copyright (C) 2015-2017 Shaanxi QinWei Electroics Technologies. All rights reserved.
*
* $Date:        16. Oct 2017
* $Revision: 	V 0.4
*
* Project: 	    QL6502 - A 6502 Assembler for Game in 1990s
*
* Title:	    XD6502.c
*
* Description:	The Cross Assembly main entry for QL6502
*
* Target:		Windows 7+/Linux/ Mac OS x
*
* Author:		Zhao Yu, forthchina@163.com, forthchina@gmail.com 
*			    16/10/2017 (In dd/mm/yyyy)
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

#define ARGV_MAX	12

#include "QL6502.h"

// ************************************************************************************

typedef enum {
	T_NOP, T_IMM, T_ZER, T_ACC,
	T_ABS, T_ZPX, T_IX, T_IY,
	T_ABX, T_ABY, T_IMP, T_REL,
	T_IND, T_ZPY
}  ADDRESS_MODE_6502;

/* ............  Used For UnASMB FLAG ...........................*/

#define T_FLAG_NO_CNT   0x80
#define T_FLAG_ABS_ENT  0x40
#define T_FLAG_ABS_SUB  0x20

#define PARA_MAX		31

#define FLAG_NO         0x00
#define FLAG_DB         0x01
#define FLAG_DW         0x02
#define FLAG_SUB        0x04
#define FLAG_LOC        0x08
#define FLAG_ENT        0x10
#define FLAG_ISTR       0x20
#define FLAG_USED       0x40
#define FLAG_DO_OK      0x80

typedef struct _code_stru {
	char name[32] ;
  	int  len, cont, type, ind ;
  	int addr ;
}  CodeStruct ;

#define GetByte(pos)		(code_buf[pos])
#define GetWord(pos)		( *(unsigned short *) & code_buf[pos])
#define SetFlag(pos,flag)	(info_buf[pos] = flag)
#define GetFlag(pos)		(info_buf[pos])

struct _aux {
	unsigned int code;
	char     name[4];
	unsigned int cycles;
	unsigned int type;
};

static struct _aux aux_code[256] = {
//    CODE	  AUX NAME  CYCLE	MODE
	{ 0x00,   "BRK",	2,		T_IMP | T_FLAG_NO_CNT	} ,
	{ 0x01,   "ORA",	6,		T_IX					} ,
	{ 0x02,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x03,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x04,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x05,   "ORA",	3,		T_ZER					} ,
	{ 0x06,   "ASL",	5,		T_ZER					} , 
	{ 0x07,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x08,   "PHP",	3,		T_IMP					} ,
	{ 0x09,   "ORA",	2,		T_IMM					} ,
	{ 0x0a,   "ASL",	3,		T_ACC					} ,
	{ 0x0b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x0c,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x0d,   "ORA",	5,		T_ABS					} ,
	{ 0x0e,   "ASL",	6,		T_ABS					} ,
	{ 0x0f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x10,   "BPL",	0,		T_REL					} ,
	{ 0x11,   "ORA",	5,		T_IY					} ,
	{ 0x12,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x13,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x14,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x15,   "ORA",	4,		T_ZPX					} ,
	{ 0x16,   "ASL",	6,		T_ZPX					} ,
	{ 0x17,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x18,   "CLC",	2,		T_IMP					} ,
	{ 0x19,   "ORA",	4,		T_ABY					} ,
	{ 0x1a,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x1b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x1c,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x1d,   "ORA",	4,		T_ABX					} ,
	{ 0x1e,   "ASL",	7,		T_ABX					} ,
	{ 0x1f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x20,   "JSR",	6,		T_ABS | T_FLAG_ABS_SUB	} ,
	{ 0x21,   "AND",	6,		T_IX					} ,
	{ 0x22,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x23,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x24,   "BIT",	3,		T_ZER					} ,
	{ 0x25,   "AND",	3,		T_ZER					} ,
	{ 0x26,   "ROL",	5,		T_ZER					} ,
	{ 0x27,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x28,   "PLP",	4,		T_IMP					} ,
	{ 0x29,   "AND",	2,		T_IMM					} ,
	{ 0x2a,   "ROL",	2,		T_ACC					} ,
	{ 0x2b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x2c,   "BIT",	4,		T_ABS					} ,
	{ 0x2d,   "AND",	4,		T_ABS					} ,
	{ 0x2e,   "ROL",	6,		T_ABS					} ,
	{ 0x2f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x30,   "BMI",	2,		T_REL					} ,
	{ 0x31,   "AND",	5,		T_IY					} ,
	{ 0x32,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x33,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x34,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x35,   "AND",	4,		T_ZPX					} ,
	{ 0x36,   "ROL",	6,		T_ZPX					} ,
	{ 0x37,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x38,   "SEC",	2,		T_IMP					} ,
	{ 0x39,   "AND",	4,		T_ABY					} ,
	{ 0x3a,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x3b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x3c,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x3d,   "AND",	4,		T_ABX					} ,
	{ 0x3e,   "ROL",	7,		T_ABX					} ,
	{ 0x3f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x40,   "RTI",	6,		T_IMP | T_FLAG_NO_CNT	} ,
	{ 0x41,   "EOR",	6,		T_IX					} ,
	{ 0x42,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x43,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x44,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x45,   "EOR",	3,		T_ZER					} ,
	{ 0x46,   "LSR",	5,		T_ZER					} ,
	{ 0x47,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x48,   "PHA",	3,		T_IMP					} ,
	{ 0x49,   "EOR",	2,		T_IMM					} ,
	{ 0x4a,   "LSR",	2,		T_ACC					} ,
	{ 0x4b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x4c,   "JMP",	3,		T_ABS | T_FLAG_NO_CNT | T_FLAG_ABS_ENT } ,
	{ 0x4d,   "EOR",	4,		T_ABS					} ,
	{ 0x4e,   "LSR",	6,		T_ABS					} ,
	{ 0x4f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x50,   "BVC",	2,		T_REL					} ,
	{ 0x51,   "EOR",	5,		T_IY					} ,
	{ 0x52,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x53,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x54,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x55,   "EOR",	4,		T_ZPX					} ,
	{ 0x56,   "LSR",	6,		T_ZPX					} ,
	{ 0x57,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x58,   "CLI",	0,		T_IMP					} ,
	{ 0x59,   "EOR",	4,		T_ABY					} ,
	{ 0x5a,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x5b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x5c,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x5d,   "EOR",	4,		T_ABX					} ,
	{ 0x5e,   "LSR",	7,		T_ABX					} ,
	{ 0x5f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x60,   "RTS",	6,		T_IMP | T_FLAG_NO_CNT	} ,
	{ 0x61,   "ADC",	6,		T_IX					} ,
	{ 0x62,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x63,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x64,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x65,   "ADC",	3,		T_ZER					} ,
	{ 0x66,   "ROR",	5,		T_ZER					} ,
	{ 0x67,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x68,   "PLA",	4,		T_IMP					} ,
	{ 0x69,   "ADC",	2,		T_IMM					} ,
	{ 0x6a,   "ROR",	2,		T_ACC					} ,
	{ 0x6b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x6c,   "JMP",	5,		T_IND | T_FLAG_NO_CNT	} ,
	{ 0x6d,   "ADC",	4,		T_ABS					} ,
	{ 0x6e,   "ROR",	6,		T_ABS					} ,
	{ 0x6f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x70,   "BVS",	2,		T_REL					} ,
	{ 0x71,   "ADC",	5,		T_IY					} ,
	{ 0x72,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x73,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x74,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x75,   "ADC",	4,		T_ZPX					} ,
	{ 0x76,   "ROR",	6,		T_ZPX					} ,
	{ 0x77,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x78,   "SEI",	2,		T_IMP					} ,
	{ 0x79,   "ADC",	4,		T_ABY					} ,
	{ 0x7a,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x7b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x7c,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x7d,   "ADC",	7,		T_ABX					} ,
	{ 0x7e,   "ROR",	7,		T_ABX					} ,
	{ 0x7f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x80,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x81,   "STA",	6,		T_IX					} ,
	{ 0x82,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x83,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x84,   "STY",	6,		T_ZER					} ,
	{ 0x85,   "STA",	3,		T_ZER					} ,
	{ 0x86,   "STX",	3,		T_ZER					} ,
	{ 0x87,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x88,   "DEY",	2,		T_IMP					} ,
	{ 0x89,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x8a,   "TXA",	2,		T_IMP					} ,
	{ 0x8b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x8c,   "STY",	4,		T_ABS					} ,
	{ 0x8d,   "STA",	4,		T_ABS					} ,
	{ 0x8e,   "STX",	4,		T_ABS					} ,
	{ 0x8f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x90,   "BCC",	2,		T_REL					} ,
	{ 0x91,   "STA",	6,		T_IY					} ,
	{ 0x92,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x93,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x94,   "STY",	4,		T_ZPX					} ,
	{ 0x95,   "STA",	4,		T_ZPX					} ,
	{ 0x96,   "STX",	4,		T_ZPX					} ,
	{ 0x97,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x98,   "TYA",	2,		T_IMP					} ,
	{ 0x99,   "STA",	5,		T_ABY					} ,
	{ 0x9a,   "TXS",	2,		T_IMP					} ,
	{ 0x9b,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x9c,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x9d,   "STA",	5,		T_ABX					} ,
	{ 0x9e,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0x9f,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xa0,   "LDY",	2,		T_IMM					} ,
	{ 0xa1,   "LDA",	6,		T_IX					} ,
	{ 0xa2,   "LDX",	2,		T_IMM					} ,
	{ 0xa3,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xa4,   "LDY",	3,		T_ZER					} ,
	{ 0xa5,   "LDA",	3,		T_ZER					} ,
	{ 0xa6,   "LDX",	3,		T_ZER					} ,
	{ 0xa7,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xa8,   "TAY",	2,		T_IMP					} ,
	{ 0xa9,   "LDA",	3,		T_IMM					} ,
	{ 0xaa,   "TAX",	2,		T_IMP					} ,
	{ 0xab,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xac,   "LDY",	4,		T_ABS					} ,
	{ 0xad,   "LDA",	4,		T_ABS					} ,
	{ 0xae,   "LDX",	4,		T_ABS					} ,
	{ 0xaf,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xb0,   "BCS",	2,		T_REL					} ,
	{ 0xb1,   "LDA",	5,		T_IY					} ,
	{ 0xb2,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xb3,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xb4,   "LDY",	4,		T_ZPX					} ,
	{ 0xb5,   "LDA",	4,		T_ZPX					} ,
	{ 0xb6,   "LDX",	3,		T_ZPY					} ,
	{ 0xb7,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xb8,   "CLV",	2,		T_IMP					} ,
	{ 0xb9,   "LDA",	4,		T_ABY					} ,
	{ 0xba,   "TSX",	2,		T_IMP					} ,
	{ 0xbb,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xbc,   "LDY",	4,		T_ABX					} ,
	{ 0xbd,   "LDA",	4,		T_ABX					} ,
	{ 0xbe,   "LDX",	4,		T_ABY					} ,
	{ 0xbf,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xc0,   "CPY",	2,		T_IMM					} ,
	{ 0xc1,   "CMP",	6,		T_IX					} ,
	{ 0xc2,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xc3,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xc4,   "CPY",	3,		T_ZER					} ,
	{ 0xc5,   "CMP",	3,		T_ZER					} ,
	{ 0xc6,   "DEC",	3,		T_ZER					} ,
	{ 0xc7,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xc8,   "INY",	2,		T_IMP					} ,
	{ 0xc9,   "CMP",	2,		T_IMM					} ,
	{ 0xca,   "DEX",	2,		T_IMP					} ,
	{ 0xcb,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xcc,   "CPY",	4,		T_ABS					} ,
	{ 0xcd,   "CMP",	4,		T_ABS					} ,
	{ 0xce,   "DEC",	6,		T_ABS					} ,
	{ 0xcf,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xd0,   "BNE",	2,		T_REL					} ,
	{ 0xd1,   "CMP",	5,		T_IY					} ,
	{ 0xd2,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xd3,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xd4,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xd5,   "CMP",	4,		T_ZPX					} ,
	{ 0xd6,   "DEC",	6,		T_ZPX					} ,
	{ 0xd7,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xd8,   "CLD",	2,		T_IMP					} ,
	{ 0xd9,   "CMP",	4,		T_ABY					} ,
	{ 0xda,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xdb,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xdc,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xdd,   "CMP",	4,		T_ABX					} ,
	{ 0xde,   "DEC",	7,		T_ABX					} ,
	{ 0xdf,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xe0,   "CPX",	3,		T_IMM					} ,
	{ 0xe1,   "SBC",	6,		T_IX					} ,
	{ 0xe2,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xe3,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xe4,   "CPX",	3,		T_ZER					} ,
	{ 0xe5,   "SBC",	3,		T_ZER					} ,
	{ 0xe6,   "INC",	5,		T_ZER					} ,
	{ 0xe7,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xe8,   "INX",	2,		T_IMP					} ,
	{ 0xe9,   "SBC",	2,		T_IMM					} ,
	{ 0xea,   "NOP",	2,		T_IMP					} ,
	{ 0xeb,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xec,   "CPX",	4,		T_ABS					} ,
	{ 0xed,   "SBC",	4,		T_ABS					} ,
	{ 0xee,   "INC",	6,		T_ABS					} ,
	{ 0xef,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xf0,   "BEQ",	2,		T_REL					} ,
	{ 0xf1,   "SBC",	5,		T_IY					} ,
	{ 0xf2,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xf3,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xf4,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xf5,   "SBC",	4,		T_ZPX					} ,
	{ 0xf6,   "INC",	6,		T_ZPX					} ,
	{ 0xf7,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xf8,   "SED",	2,		T_IMP					} ,
	{ 0xf9,   "SBC",	4,		T_ABY					} ,
	{ 0xfa,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xfb,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xfc,   "???",	0,		T_NOP | T_FLAG_NO_CNT	} ,
	{ 0xfd,   "SBC",	4,		T_ABX					} ,
	{ 0xfe,   "INC",	4,		T_ABX					} ,
	{ 0xff,   "???",	0,		T_NOP | T_FLAG_NO_CNT	}
};

static unsigned char code_buf[CODE_MAX + 16], info_buf[CODE_MAX + 16];
static int LCCnt, CodeCnt, jind_no;
static CodeStruct cds;

static int de_asmb(int pc, CodeStruct * dst)
{
	unsigned int i, k, type;
	char buf[40];

	i = GetByte(pc++);
	type = aux_code[i].type;
	dst->cont = dst->ind = dst->type = dst->addr = 0;
	switch (type & 0x0f) {
		case T_NOP: 
			buf[0] = '\0';		
			dst->len = 1;			
			break;

		case T_IMM: 
			k = GetByte(pc);
			if (isprint(k))    
				sprintf(buf, "#0x%02X    ; %c", k, k);
			else
				sprintf(buf, "#0x%02X", k);
			dst->len = 2;								
			break;

		case T_ZER: 
			sprintf(buf, "0x%02X", GetByte(pc));
			dst->len = 2;							    
			break;

		case T_ACC: buf[0] = '\0';		
			dst->len = 1;			
			break;

		case T_ABS: 
			dst->addr = k = GetWord(pc);
			sprintf(buf, "L_%04X", k);
			if (type & T_FLAG_ABS_ENT) 
				dst->type = FLAG_ENT;
			if (type & T_FLAG_ABS_SUB) 
				dst->type = FLAG_SUB;
			dst->len = 3;
			break;

		case T_ZPX: 
			sprintf(buf, "0x%02X,X", GetByte(pc));
			dst->len = 2;								
			break;

		case T_IX:  
			sprintf(buf, "(0x%02X),X", GetByte(pc));
			dst->len = 2;								
			break;

		case T_IY:  
			sprintf(buf, "(0x%02X),Y", GetByte(pc));
			dst->len = 2;							 	
			break;

		case T_ABX: 
			dst->addr = k = GetWord(pc);
			dst->type = FLAG_DB;		
			sprintf(buf, "L_%04X,X", k);
			dst->len = 3;								 
			break;

		case T_ABY: 
			dst->addr = k = GetWord(pc);
			dst->type = FLAG_DB;		
			sprintf(buf, "L_%04X,Y", k);
			dst->len = 3;								 
			break;

		case T_IMP: 
			buf[0] = '\0';			
			dst->len = 1;		 
			break;

		case T_REL: 
			dst->addr = k = pc + 1 + (char)GetByte(pc);
			sprintf(buf, "L_%04X", k);	
			dst->type = FLAG_LOC;
			dst->len = 2;								 
			break;

		case T_IND: 
			sprintf(buf, "(0x%04X)", GetWord(pc));
			dst->ind = 1;			
			dst->len = 3;	 
			break;

		case T_ZPY: 
			sprintf(buf, "0x%02X,Y", GetByte(pc));
			dst->len = 2;								 
			break;

		default: printf("Internal error ...... CPU Instruction\n");
		break;
	}
	sprintf(dst->name, "%s  %s", aux_code[i].name, buf);
	dst->cont = (type & T_FLAG_NO_CNT) ? 0 : 1;
	return 1;
}

// ************************************************************************************

static void help(void)
{
	printf("\n      ......  COMMAND LIST TABLE      ...... \n\n");
	printf("B[start] [end]            Blank Area             \n");
	printf("C[T/P]   [start] [end]    Set Subroutine Table / Point\n");
	printf("D [DB/DW/DD    [start]    Dump Data              \n");
	printf("G[file]  [start] [end]    Program File Generation\n");
	printf("H                         Hex/Dec Calculate      \n");
	printf("I[start]                  List IND instruction   \n");
	printf("J[T/P]   [strat] [end]    Set Jump Table / Point \n");
	printf("L[file]  	              Load file into start   \n");
	printf("Q                         Exit to OS             \n");
	printf("S[start] [end] [data ]    Search Bytes List Data \n");
	printf("U[start] [end]            UnAsmb                 \n");
	printf("V[start] [end]            View Program  List     \n");
	printf("X                         auto De-Assembly       \n");
	printf("Z                         Zap Information        \n\n");

}

// Set CPU 6502 Entry, RESET, IRQ and NMI
static void SetKnownEntry(void)
{
	unsigned int pc;
	int 		cc;

	pc = GetWord(0xfffcL);
	cc = GetFlag(pc);
	if (!(cc & FLAG_DO_OK)) {
		SetFlag(pc, cc | FLAG_ENT | FLAG_ISTR);      		// 6502 Reset
		pc = GetWord(0xfffeL);
		cc = GetFlag(pc);
		SetFlag(pc, cc | FLAG_ENT | FLAG_ISTR);             // 6502 NMI  
		pc = GetWord(0xfffaL);
		cc = GetFlag(pc);
		SetFlag(pc, cc | FLAG_ENT | FLAG_ISTR);             // 6502 IRQ  
	}
}

// Convert XD6502 input string to number
static int string_to_value(char * pc)
{
	char * tmp;
	int	 ret_val, sign = 0;

	if (*pc == '-') {
		sign = 1;
		pc++;
	}
	if (*pc == '0' && (*(pc + 1) == 'X' || *(pc + 1) == 'x')) {
		ret_val = strtol(pc + 2, &tmp, 16);
	}
	else {
		ret_val = strtol(pc, &tmp, 10);
	}
	if (*tmp != '\0') {
		printf("Bad number : %s\n", pc);
		return 0;
	}
	return (sign) ? (-ret_val) : (ret_val);
}

// Set target Jump/Call Table entry
static void SetSubEntTable(int begin, int end, int type)
{
	int pc, cc;

	for (; begin < end; begin += 2) {
		pc = GetWord(begin);
		cc = GetFlag(pc);
		SetFlag(pc, cc | type);
	}
}

// format output text string for list 
static void gen_list(char * buf)
{
	unsigned int cc, i, px, prem;
	
	cc = GetFlag(LCCnt);
	if (cc & FLAG_ISTR) {
		de_asmb(LCCnt, &cds);
		if (cc & FLAG_SUB) {
			sprintf(buf, "L_%04X: %-25s;Subrutine ..........", LCCnt, cds.name);
		}
		else if (cc & FLAG_ENT) {
			sprintf(buf, "L_%04X: %-25s;JMP Entry ..........", LCCnt, cds.name);
		}
		else if (cc & FLAG_LOC) {
			sprintf(buf, "L_%04X: %s", LCCnt, cds.name);
		}
		else {
			sprintf(buf, "L_%04X  %s", LCCnt, cds.name);
		}
		LCCnt += cds.len;
	}
	else {
		memset(buf, ' ', 78);
		buf[79] = '\0';
		sprintf(buf, "L_%04X: DB  ", LCCnt);
		px = 12;
		prem = 56;
		buf[prem] = ';';
		prem += 2;
		for (i = 0; i < 8 && !(GetFlag(LCCnt) & FLAG_ISTR)
			&& LCCnt < CODE_MAX; i++, px += 5) {
			cc = GetByte(LCCnt++);
			sprintf(&buf[px], "0x%02X,", cc);
			buf[prem++] = (cc > 0x20 && cc <= 0x7e) ? cc : '.';
		}
		buf[px - 1] = buf[px] = ' ';
	}
}

// set target code information
static void blank_info(int argc, char * argv[])
{
	int 	begin, end, cc;

	if (argc == 2) {
		begin = string_to_value(argv[0]);
		end = string_to_value(argv[1]);
		for (; begin <= end; begin++) {
			SetFlag(begin, FLAG_NO);
		}
	}
	else {
		printf("000000 -- ");
		cc = GetFlag(0L) & FLAG_USED;
		for (begin = 0; begin < CODE_MAX; begin++) {
			if ((GetFlag(begin) & FLAG_USED) == cc) {
				continue;
			}
			printf("%06X : %s\n", begin - 1, (cc == 0) ? "Not Used" : "Used");
			printf("%06X -- ", begin);
			cc = GetFlag(begin) & FLAG_USED;
		}
		printf("%06X : %s\n", begin, (cc == 0) ? "Not Used" : "Used");
	}
}

// clear all target code information 
static void zap_info(void)
{
	unsigned int 	cnt;

	for (cnt = 0; cnt < CODE_MAX; cnt++) {
		SetFlag(cnt, FLAG_USED);
	}
	cnt++;
}

// dump target data
static void dump_data(int argc, char * argv[])
{
	static int DataCnt;
	unsigned char 	buf[17];
	unsigned int 	i, m = 8, k, cc;

	if (argc == 1) {
		DataCnt = string_to_value(argv[0]);
	}
	printf("Scale -   0  1  2  3  4  5  6  7 -  8  9  a  b  c  d  e  f");
	printf("   0123456789ABCDEF\n\n");
	buf[16] = '\0';
	for (i = 0; i < m && DataCnt < CODE_MAX; i++) {
		printf("%06X:  ", DataCnt);
		if ((cc = (DataCnt & 0x000f)) != 0)
			for (k = 0; k < cc; k++) {
				printf("   ");
				if (k == 8) printf("  ");
				buf[k] = ' ';
			}
		for (k = DataCnt & 0x000f; k <= 0xf; k++) {
			if (k == 8) printf("- ");
			cc = GetByte(DataCnt++);
			printf("%02X ", cc);
			buf[k] = isprint(cc) ? cc : '.';
		}
		printf("  %s\n", buf);
	}
}

// search bytes in target code
static void search_data(int argc, char * argv[])
{
	int 	begin, end, i, k;
	char    s_data[10];

	if (argc < 3) {
		printf("Parameter Number or Type Error.\n");
		return;
	}
	begin = string_to_value(argv[0]);
	end = string_to_value(argv[1]);
	for (i = 0, k = 2; k < argc; k++, i++) {
		s_data[i] = string_to_value(argv[k]);
	}
	for (; begin + i <= end; begin++) {
		for (k = 0; k < i; k++) {
			if (GetByte(begin + k) != s_data[k])
				break;
		}
		if (k == i) {
			printf("L_%04X  ", begin);
		}
	}
	printf("\n");
}

// scan and loop for target code
static void do_un_asmb(void)
{
	int cc, i;

	for (cds.cont = 1; cds.cont; ) {
		de_asmb(LCCnt, &cds);
		cc = GetFlag(LCCnt);
		SetFlag(LCCnt, cc | FLAG_DO_OK | FLAG_ISTR);
		for (LCCnt++, i = 1; i < cds.len; i++, LCCnt++) {
			SetFlag(LCCnt, FLAG_USED);
		}
		if (cds.ind == 1) {
			jind_no++;
			continue;
		}
		if (cds.type) {
			cc = GetFlag(cds.addr);
			if (cc & FLAG_USED) {
				SetFlag((long)cds.addr, cc | cds.type);
			}
		}
	}
}

// XD6502 de-assembly
static void un_asmb(int argc, char * argv[])
{
	int  i, k, m = 16;
	CodeStruct 		cds;

	if (argc == 1) {
		LCCnt = string_to_value(argv[0]);
	}
	for (i = 0; i < m && LCCnt < CODE_MAX; i++) {
		if (!de_asmb(LCCnt, &cds)) {
			return;
		}
		printf("L_%04X  ", LCCnt);
		for (k = 0; k < cds.len && k < 8; k++, LCCnt++) {
			printf("%02X ", GetByte(LCCnt));
		}
		for (; k < 8; k++) printf("   ");
		printf("%s\n", cds.name);
	}
}

// set target code JMP/JSR table
static void set_sub_jump_entry(int argc, char * argv[], int flag)
{
	int begin, end, cc;

	if (argc == 2) {
		begin = string_to_value(argv[0]);
		end = string_to_value(argv[1]);
		SetSubEntTable(begin, end, flag);
	}
	else if (argc == 1) {
		begin = string_to_value(argv[0]);
		cc = GetFlag(begin);
		SetFlag(begin, cc | flag);
	}
}

// display indirect access instruction
static void display_ind(void)
{
	int i, cc;

	for (LCCnt = 0, i = 1; LCCnt < CODE_MAX; ) {
		cc = GetFlag(LCCnt);
		if (!(cc & FLAG_ISTR)) { LCCnt++;   continue; }
		de_asmb(LCCnt, &cds);
		if (cds.ind == 1) {
			printf("(%03d)  L_%04X: %s\n", i, LCCnt, cds.name);
			LCCnt += cds.len;
			i++;
			continue;
		}
		LCCnt++;
	}
}

// code automatic analysis
static void auto_sat(void)
{
	int i, cc, work_flag = 1;

	SetKnownEntry();
	for (jind_no = 0, i = 1; work_flag != 0; i++) {
		printf("%4d", i);
		work_flag = 0;
		for (LCCnt = 0L; LCCnt < CODE_MAX - 1; LCCnt++) {
			cc = GetFlag(LCCnt);
			if ((cc & FLAG_USED) && !(cc & FLAG_DO_OK) &&
				((cc & FLAG_SUB) || (cc & FLAG_ENT) || (cc & FLAG_LOC))) {
				do_un_asmb();
				work_flag = 1;
			}
		}
	}
	if (jind_no == 0) {
		printf("\nOk.\n");
	}
	else {
		printf("\n%d Indirect Entries\n", jind_no);
	}
}


// view code list (with formatted XD6502 text)
static void view_list(int argc, char * argv[])
{
	int i, cc, m = 20, end = CODE_MAX - 1;
	char buf[TEXT_LINE_SIZE];

	if (argc >= 1) {
		CodeCnt = string_to_value(argv[0]);
	}
	if (argc == 2) {
		end = string_to_value(argv[2]);
	}
	for (LCCnt = CodeCnt, i = 0; i < m && LCCnt <= end; i++) {
		while (!((cc = GetFlag(LCCnt)) & FLAG_USED)) {
			CodeCnt++;
			continue;
		}
		gen_list(buf);
		printf("%s\n", buf);
	}
	CodeCnt = LCCnt;
}

// output analyzed code list to file
static void gen_list_file(int argc, char * argv[])
{
	FILE * fout;
	int  end = CODE_MAX;
	char buf[TEXT_LINE_SIZE];

	if (argc != 3) {
		printf("Format: G [filename] [begin] [end].\n");
		return;
	}
	LCCnt = string_to_value(argv[1]);
	end = string_to_value(argv[2]);
	if ((fout = fopen(argv[0], "w")) == NULL) {
		printf("Error open output file : %s\n", argv[0]);
		return;
	}
	while (LCCnt <= end) {
		if (!(GetFlag(LCCnt) & FLAG_USED)) {
			LCCnt++;
			continue;
		}
		gen_list(buf);
		fprintf(fout, "%s\n", buf);
	}
	fclose(fout);
	printf("\nOK.\n");
}

// load target ROM data into XD6502 for analysis
static void load_bin_file(int argc, char * argv[])
{
	FILE 	* fd;
	int  	cc, cnt = 0;

	if (argc > 1) {
		printf("Command Format: L [filename]\n");
		return;
	}
	if ((fd = fopen(argv[0], "rb")) == NULL) {
		printf("File Open Error : %s\n", argv[0]);
	}
	else {
		cnt = fread((char *)code_buf, 1, CODE_MAX, fd);
		printf("%u Bytes data read into code buffer @0x0000\n", cnt);
		fclose(fd);
		zap_info();
	}
}

// ************************************************************************************

void XD6502(void) {
	static char keypad_in_buffer[TEXT_LINE_SIZE + 2];
	
	char * pc, cc;
	char * argv[ARGV_MAX] ;
	int i;

	while (1) {
		printf("QL6502> ");
		if (fgets(keypad_in_buffer, TEXT_LINE_SIZE, stdin) == NULL) {
			return;
		}
    
		for (pc = keypad_in_buffer; * pc <= 0x20 && * pc != '\0'; pc ++);
		if (* pc == '\0') continue;
		cc = * pc ++;
		for (i = 0; i < PARA_MAX; i++) {
			for (; * pc <= 0x20 && * pc != '\0'; pc++);
			if (*pc == '\0') {
				break;
			}
			argv[i] = pc;
			for (; * pc != ',' && * pc > ' '; pc++);
			if (* pc) * pc++ = '\0';
		}
		argv[i] = NULL;
		cc = (cc >= 'a' && cc <= 'z') ? cc - 0x20 : cc;
		switch (cc) {
			case 0:                                					break;
			case 'B':   blank_info(i, argv);      					break;
			case 'C':   set_sub_jump_entry(i, argv, FLAG_SUB); 		break;
			case 'D':   dump_data(i, argv);       					break;
			case 'G':   gen_list_file(i, argv);    					break;
			case 'I':   display_ind();              				break;
			case 'J':   set_sub_jump_entry(i, argv, FLAG_ENT); 		break;
			case 'L':   load_bin_file(i, argv);  					break;
			case 'Q':                                				return;
			case 'S':   search_data(i, argv);     					break;
			case 'U':   un_asmb(i, argv);         					break;
			case 'V':   view_list(i, argv);       					break;
			case 'X':   auto_sat();                  				break;
			case 'Z':   zap_info();									break;
			case '?':   help();                    					break;
			default:	printf("Unknown command.\n");				break;
		}
	} 
}
