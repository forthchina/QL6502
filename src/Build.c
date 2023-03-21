/* ************************************************************************************
* Copyright (C) 1995-2017 Shaanxi QinWei Electroics Technologies. All rights reserved.
*
* $Date:        16. Oct 2017
* $Revision: 	V 0.4
*
* Project: 	    QL6502 - A 6502 Assembler for Game in 1990s
*
* Title:	    Build.c
*
* Description:	The Main Assembly for QL6502
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

#include "QL6502.h"

#define SYMB_BUF_SIZE	(128 * 1204)
#define HASH_SIZE		127

#define OP_END          0
#define OP_MUTI         1
#define OP_JMP          2
#define OP_JREL         3
#define OP_IMP          4

#define OPRAND_NO       0x0001
#define OPRAND_ABS      0x0002
#define OPRAND_ADD      0x0004
#define OPRAND_X        0x0008
#define OPRAND_Y        0x0010
#define OPRAND_IMP      0x0020

typedef struct _instruction_6502 {
	char	name[4];
	int		mode;
	int		imm, abs, zer, acc, ix, iy, zpx, abx, aby, zpy;
} INSTUCTION_6502;

typedef struct _symb_node {
	struct   _symb_node	* link;
	int		pos   ;
	char	name[1] ;
} Symbol; 

static INSTUCTION_6502 instruction_table[] = {
    // NAME		MODE		IMM,  ABS,  ZER,  ACC,  IX,   IY,   ZPX,  ABX,  ABY,  ZPY
	{ "ADC",    OP_MUTI,	0x69, 0x6d, 0x65, 0x00, 0x61, 0x71, 0x75, 0x7d, 0x79, 0x00	},
	{ "AND",    OP_MUTI,	0x29, 0x2d, 0x25, 0x00, 0x21, 0x31, 0x35, 0x3d, 0x39, 0x00	},
	{ "ASL",    OP_MUTI,	0x00, 0x0e, 0x06, 0x0a, 0x00, 0x00, 0x16, 0x1e, 0x00, 0x00	},
	{ "BCC",    OP_JREL,	0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BCS",    OP_JREL,	0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BEQ",    OP_JREL,	0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BIT",    OP_MUTI,	0x00, 0x2c, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BMI",    OP_JREL,	0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BNE",    OP_JREL,	0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BPL",    OP_JREL,	0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BRK",    OP_IMP,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BVC",    OP_JREL,	0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "BVS",    OP_JREL,	0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "CLC",    OP_IMP,		0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "CLD",    OP_IMP,		0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "CLI",    OP_IMP,		0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "CLV",    OP_IMP,		0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "CMP",    OP_MUTI,	0xc9, 0xcd, 0xc5, 0x00, 0xc1, 0xd1, 0xd5, 0xdd, 0xd9, 0x00	},
	{ "CPX",    OP_MUTI,	0xe0, 0xec, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "CPY",    OP_MUTI,	0xc0, 0xcc, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "DEC",    OP_MUTI,	0x00, 0xce, 0xc6, 0x00, 0x00, 0x00, 0xd6, 0xde, 0x00, 0x00	},
	{ "DEX",    OP_IMP,		0xca, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "DEY",    OP_IMP,		0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "EOR",    OP_MUTI,	0x49, 0x4d, 0x45, 0x00, 0x41, 0x51, 0x55, 0x5d, 0x59, 0x00	},
	{ "INC",    OP_MUTI,	0x00, 0xee, 0xe6, 0x00, 0x00, 0x00, 0xf6, 0xfe, 0x00, 0x00	},
	{ "INX",    OP_IMP,		0xe8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "INY",    OP_IMP,		0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "JMP",    OP_JMP,		0x6c, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "JSR",    OP_JMP,		0x00, 0x20,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "LDA",    OP_MUTI,	0xa9, 0xad, 0xa5, 0x00, 0xa1, 0xb1, 0xb5, 0xbd, 0xb9, 0x00	},
	{ "LDX",    OP_MUTI,	0xa2, 0xae, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbe, 0xb6	},
	{ "LDY",    OP_MUTI,	0xa0, 0xac, 0xa4, 0x00, 0x00, 0x00, 0xb4, 0xbc, 0x00, 0x00	},
	{ "LSR",    OP_MUTI,	0x00, 0x4e, 0x46, 0x4a, 0x00, 0x00, 0x56, 0x5e, 0x00, 0x00	},
	{ "NOP",    OP_IMP,		0xea, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "ORA",    OP_MUTI,	0x09, 0x0d, 0x05, 0x00, 0x01, 0x11, 0x15, 0x1d, 0x19, 0x00	},
	{ "PHA",    OP_IMP,		0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "PHP",    OP_IMP,		0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "PLA",    OP_IMP,		0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "PLP",    OP_IMP,		0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "ROL",    OP_MUTI,	0x00, 0x2e, 0x26, 0x2a, 0x00, 0x00, 0x36, 0x3e, 0x00, 0x00	},
	{ "ROR",    OP_MUTI,	0x00, 0x6e, 0x66, 0x6a, 0x00, 0x00, 0x76, 0x7e, 0x00, 0x00	},
	{ "RTI",    OP_IMP,		0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "RTS",    OP_IMP,		0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "SBC",    OP_MUTI,	0xe9, 0xed, 0xe5, 0x00, 0xe1, 0xf1, 0xf5, 0xfd, 0xf9, 0x00	},
	{ "SEC",    OP_IMP,		0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "SED",    OP_IMP,		0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "SEI",    OP_IMP,		0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "STA",    OP_MUTI,	0x00, 0x8d, 0x85, 0x00, 0x81, 0x91, 0x95, 0x9d, 0x99, 0x00	},
	{ "STX",    OP_MUTI,	0x00, 0x8e, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96	},
	{ "STY",    OP_MUTI,	0x00, 0x8c, 0x84, 0x00, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00  },
	{ "TAX",    OP_IMP,		0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "TAY",    OP_IMP,		0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	},
	{ "TSX",    OP_IMP,		0xba, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  },
	{ "TXA",    OP_IMP,		0x8a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  },
	{ "TXS",    OP_IMP,		0x9a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  },
	{ "TYA",    OP_IMP,		0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  },
	{ "" ,      OP_END, },
};

static int 	PassCnt, ErrorFlag;
static int  TotalLines, FileCnt, LineCnt, LCCnt, LCLimit, LCSave, in_code_range;
static char * ScanPtr, token[TEXT_LINE_SIZE], working_file_name[TEXT_LINE_SIZE],
			code_buf[CODE_MAX + 16], output_file_name[TEXT_LINE_SIZE],
			symb_buffer[SYMB_BUF_SIZE + 4], *symb_ptr;			
static Symbol * bucket[HASH_SIZE + 1], * CurrSymb;

// ************************************************************************************

static void error_proc(char * msg) {
	printf("Error %s @ File %s Line %d\n", msg, working_file_name, LineCnt);
	exit(-1);
}
 
// ************************************************************************************

static void gen_byte(int cc) {
	if (PassCnt == 2) {
		code_buf[LCCnt] = cc;
	}
	LCCnt++;
	if (in_code_range && LCCnt > LCLimit) {
		error_proc("Out of Code Limitation");
	}
}

static void gen_word(int ss)
{
	if (PassCnt == 2) {
		*(short *)& code_buf[LCCnt] = ss;
	}
	LCCnt += 2;
	if (in_code_range && LCCnt > LCLimit) {
		error_proc("Out of Code Limitation");
	}
}

// ************************************************************************************

static unsigned int hash_bucket(unsigned char * name) {
	unsigned int  h ;

	for (h = 0 ; * name ; h += * name ++) ;
	return h  % HASH_SIZE ;
}

static Symbol * symbol_search(char * pname) {
	char name[TEXT_LINE_SIZE + 4];
	Symbol	* spc;
	int h;

	strcpy(&name[1], pname);
	name[0] = FileCnt;
	h   = hash_bucket((unsigned char *) name);
	spc = bucket[h];
	while (spc != NULL) {
		if (strcmp(spc->name, name) == 0) {
			return spc;
		}
		spc = spc->link;
	}

	name[0] = 0xff;
	h = hash_bucket((unsigned char *)name);
	spc = bucket[h];
	while (spc != NULL) {
		if (strcmp(spc->name, name) == 0)
			return spc;
		spc = spc->link;
	}
	return NULL;
}

static void proc_label(int is_local) {
	char	name[TEXT_LINE_SIZE + 4];
	int 	size, h;
	Symbol	* spc;

	if (PassCnt == 2) return;

	strcpy(&name[1], token);
	name[0] = (is_local) ? FileCnt : 0xff;
	size = sizeof(Symbol) + strlen(name);
	if (size & 0x03) {
		size += (4 - (size & 0x03));
	}
	if ((symb_ptr + size + 1) > & symb_buffer[SYMB_BUF_SIZE]) {
		error_proc("Memory Full");
	}
	spc = (Symbol *)symb_ptr;
	symb_ptr += size;

	strcpy(spc->name, name);
	h = hash_bucket((unsigned char *)name);
	spc->link = bucket[h];
	CurrSymb = bucket[h] = spc;
	CurrSymb->pos = LCCnt;
}

// ************************************************************************************

static void scan_skip_blank(void) {
	while (*ScanPtr <= 0x20 && * ScanPtr != 0) {
		ScanPtr++;
	}
	if (*ScanPtr == ';')
		* ScanPtr = 0;
}

static int scan_get_word_name(void) {
	int idx;

	for (idx = 0; idx < TEXT_LINE_SIZE; idx++) {
		if (*ScanPtr >= 'a' && * ScanPtr <= 'z') {
			token[idx] = *ScanPtr - 'a' + 'A';
			ScanPtr++;
		}
		else if (*ScanPtr >= 'A' && * ScanPtr <= 'Z') {
			token[idx] = *ScanPtr++;
		}
		else if (*ScanPtr >= '0' && * ScanPtr <= '9') {
			token[idx] = *ScanPtr++;
		}
		else if (*ScanPtr == '_') {
			token[idx] = *ScanPtr++;
		}
		else break;
	}
	token[idx] = 0;
	return idx;
}

static int get_item(void) {
	int val, base;
	Symbol * spc;

	if (*ScanPtr >= '0' && * ScanPtr <= '9') {
		base = 10;
		if (*ScanPtr == '0' && (*(ScanPtr + 1) == 'x' || *(ScanPtr + 1) == 'X')) {
			base = 16;
		}
		val = strtol(ScanPtr, &ScanPtr, base);
		return val;
	}
	else if (!scan_get_word_name()) {
		error_proc("Expect a Word name or a number");
	}

	if ((spc = symbol_search(token)) != NULL) {
		return spc->pos;
	}
	if (PassCnt == 2) {
		error_proc("Symbol not found");
	}
	return 0;
}

static int expression(void) {
	int val;

	val = get_item();
	scan_skip_blank();
	if (*ScanPtr == '+') {
		ScanPtr++;
		scan_skip_blank();
		val += get_item();
	}
	else if (*ScanPtr == '-') {
		ScanPtr++;
		scan_skip_blank();
		val -= get_item();
	}
	scan_skip_blank();
	return val;
}

// ************************************************************************************

static INSTUCTION_6502 * get_instrcution(char * token)
{
	int i, k, m = 0, n = sizeof(instruction_table) / sizeof(INSTUCTION_6502) - 1;

	while (m <= n) {
		i = (m + n) / 2;
		k = strcmp(token, instruction_table[i].name);
		if (k > 0) 		m = i + 1;
		else if (k < 0)     n = i - 1;
		else return &instruction_table[i];
	}
	return NULL;
}

static void proc_6502_instruction(void) {
	INSTUCTION_6502 * ipc;
	int idx, oprand_type = 0, op_addr;

	if ((ipc = get_instrcution(token)) == NULL) {
		printf("Token = %s\n", token);
		error_proc("Unknown Oprator.");
	}

	if (*ScanPtr == 0) {
		oprand_type = OPRAND_NO;
	}
	else if (*ScanPtr == '#') {
		ScanPtr++;
		op_addr = expression();
		oprand_type = OPRAND_IMP;
	}
	else if (*ScanPtr == '(') {
		ScanPtr++;
		op_addr = expression();
		oprand_type = OPRAND_ADD;
		scan_skip_blank();
		if (*ScanPtr++ != ')') {
			error_proc("Expect a ) here");
		}
	}
	else {
		oprand_type = OPRAND_ABS;
		op_addr = expression();
	}

	scan_skip_blank();
	if (*ScanPtr != 0) {
		if (*ScanPtr++ != ',') {
			error_proc("Syntax error");
		}
		scan_skip_blank();
		if (*ScanPtr == 'X' || *ScanPtr == 'X') {
			oprand_type |= OPRAND_X;
		}
		else if (*ScanPtr == 'y' || *ScanPtr == 'Y') {
			oprand_type |= OPRAND_Y;
		}
		else {
			error_proc("Operand must be X or Y");
		}
		ScanPtr++;
		scan_skip_blank();
	}

	switch (ipc->mode) {
	case OP_IMP:
		if (oprand_type != OPRAND_NO) {
			error_proc("Bad operand");
		}
		gen_byte(ipc->imm);
		break;

	case OP_JREL:
		idx = op_addr - LCCnt - 2;
		if (oprand_type != OPRAND_ABS) {
			error_proc("Expect Label");
		}

		// below used for BYTE-RANGE checking 
		if (PassCnt == 2 && (idx < -127 || idx > 127)) {
			printf("JREL = from 0x%04X to 0x%04X\n", LCCnt, op_addr);
			error_proc("Out of JMP range");
		}
		gen_byte(ipc->imm);
		gen_byte(idx);
		break;

	case OP_JMP:
		if (oprand_type == OPRAND_ABS) gen_byte(ipc->abs);
		else if (oprand_type == OPRAND_ADD && ipc->imm) gen_byte(ipc->imm);
		else error_proc("Syntax");
		gen_word(op_addr);
		break;

	case OP_MUTI:
		switch (oprand_type) {
		case OPRAND_NO:
			if (ipc->acc) gen_byte(ipc->acc);
			else error_proc("Syntax");
			break;

		case OPRAND_IMP:
			if (!ipc->imm) error_proc("No this operand mode");
			gen_byte(ipc->imm);
			gen_byte(op_addr);
			break;

		case OPRAND_ABS:
			if (ipc->abs && op_addr > 0xff) {
				gen_byte(ipc->abs);
				gen_word(op_addr);
				break;
			}
			if (!ipc->zer) error_proc("No this operand mode");
			gen_byte(ipc->zer);
			gen_byte(op_addr);
			break;

		case OPRAND_ABS | OPRAND_X:
			if (ipc->zpx && op_addr <= 0xff) {
				gen_byte(ipc->zpx);
				gen_byte(op_addr);
				break;
			}
			if (!ipc->abx) error_proc("No this operand mode");
			gen_byte(ipc->abx);
			gen_word(op_addr);
			break;

		case OPRAND_ADD | OPRAND_X:
			if (!ipc->ix) error_proc("No this operand mode");
			// printf("OPADDR = 0x%04X\n", op_addr);
			if (op_addr > 0xff ) error_proc("Out of range");
			gen_byte(ipc->ix);
			gen_byte(op_addr);
			break;

		case OPRAND_ABS | OPRAND_Y:
			if (ipc->zpy && op_addr <= 0xff) {
				gen_byte(ipc->zpy);
				gen_byte(op_addr);
				break;
			}
			if (!ipc->aby) error_proc("No this operand mode");
			gen_byte(ipc->aby);
			gen_word(op_addr);
			break;

		case OPRAND_ADD | OPRAND_Y:
			// printf("OPADDR = 0x%04X\n", op_addr);
			if (!ipc->iy) error_proc("No this operand mode");
			if (op_addr > 0xff) error_proc("Out of range");
			gen_byte(ipc->iy);
			gen_byte(op_addr);
			break;
		}
		break;
	}
	if (* ScanPtr != 0) {
		error_proc("Syntax error");
	}
}

// ************************************************************************************

static void proc_db(void) {
	while (1) {
		if (*ScanPtr == '\'') {
			for (ScanPtr++; *ScanPtr != '\'' && * ScanPtr >= 0x20; ) {
				gen_byte(*ScanPtr++);
			}
			if (*ScanPtr++ != '\'') {
				error_proc("Bad String");
			}
		}
		else {
			gen_byte(expression());
		}
		scan_skip_blank();
		if (*ScanPtr != ',') break;
		ScanPtr++;
		scan_skip_blank();
	}
	if (*ScanPtr != 0) {
		error_proc("Syntax error");
	}
}

static void proc_dw(void) {
	while (1) {
		gen_word(expression());
		scan_skip_blank();
		if (*ScanPtr != ',') break;
		ScanPtr++;
		scan_skip_blank();
	}
	if (*ScanPtr != 0) {
		error_proc("Syntax error");
	}
}

static void proc_org(void) {
	LCCnt = expression();
	scan_skip_blank();
	if (*ScanPtr != 0) {
		error_proc("Syntax error");
	}
}

static void proc_equ(void) {
	if (PassCnt == 1) {
		if (CurrSymb == NULL) {
			error_proc("there should be a LABEL for this EQU.");
		}
		CurrSymb->pos = expression();
		// printf("Label %s = %d (0x%04X)\n", CurrSymb->name, val, val);
		scan_skip_blank();
		if (*ScanPtr != 0) {
			error_proc("Syntax error");
		}
	}
	CurrSymb = NULL;
}

static void proc_code(void) {
	if (*ScanPtr == 0) {
		LCCnt = LCSave;
		in_code_range = 0xff;
		// printf("Use Code = 0x%04X (%s@%d)\n", LCCnt, working_file_name, LineCnt);
	}
	else {
		LCCnt = expression();
		LCLimit = expression();
		// printf("Define Area = (0x%04X, 0x%04X), (%s@%d)\n", LCCnt, LCLimit, working_file_name, LineCnt);
		scan_skip_blank();
	}
	if (*ScanPtr != 0) {
		error_proc("Syntax error");
	}
}

// ************************************************************************************

static void scan_text_line(char * ptr) {
	ScanPtr = ptr;	
	if (* ScanPtr == ';') return;
	if (scan_get_word_name()) {
		if (* ScanPtr != ':') {
			error_proc("Bad Label Format.");
		}
		ScanPtr ++;
		if (* ScanPtr == ':') {				// public label
			ScanPtr ++;
			proc_label(0);
		} else {							// local label
			proc_label(1);
		}
	}

	scan_skip_blank();
	if (* ScanPtr == 0 || * ScanPtr == ';') { 
		return;
	}
	scan_get_word_name();	
	scan_skip_blank();

	if	(token[0] == 'D' && token[1] == 'B' && token[2] == 0) {
		proc_db();
	} else if (token[0] == 'D' && token[1] == 'W' && token[2] == 0) {
		proc_dw();
	} else if (token[0] == 'O' && token[1] == 'R' && token[2] == 'G' && token[3] == 0) {
		proc_org();
	} else if (token[0] == 'E' && token[1] == 'Q' && token[2] == 'U' && token[3] == 0) {
		proc_equ();
	} else if (token[0] == 'C' && token[1] == 'O' && token[2] == 'D' && token[3] == 'E' && token[4] == 0) {
		proc_code();
	} else {
		proc_6502_instruction();
	}
}

static void do_options(char * opt) {
	FILE * fd;

	if (PassCnt == 2) {
		if (*opt == 'L') {
			opt ++;
			if ((fd = fopen(opt, "rb")) == NULL) {
				printf("Cannot load file : %s \n", opt);
				exit(-1);
			}
			fread(code_buf, 1, CODE_MAX, fd);
			fclose(fd);
			printf("Load file : %s \n", opt);
		} else if (*opt == 'O') {
			strcpy(output_file_name, opt + 1);
		}
	}
}

static void build_from_file(char * fname) {
	char buf[TEXT_LINE_SIZE + 2], * pc;
	FILE * fd_adt, * fd_asm;

	if ((fd_adt = fopen(fname, "rt")) == NULL) {
		printf("Make File Open Error : %s \n", fname);
		return;
	}

	TotalLines = 0;
	while (fgets(working_file_name, TEXT_LINE_SIZE, fd_adt)) {
		for (pc = working_file_name; * pc > 0x20; pc++);
		* pc = 0;
		if (working_file_name[0] > 0x20) {
			if (working_file_name[0] == '-') {
				do_options(&working_file_name[1]);
			} else {
				FileCnt++;
				// printf("File Count = %d, FileName %s\n", FileCnt, working_file_name);
				if ((fd_asm = fopen(working_file_name, "rt")) == NULL) {
					printf("Source File Open Error : %s \n", buf);
					return;
				}
				LCLimit = CODE_MAX;
				LineCnt = in_code_range = 0;
				while (fgets(buf, TEXT_LINE_SIZE, fd_asm)) {
					LineCnt++;
					CurrSymb = NULL;
					scan_text_line(buf);
				}
				fclose(fd_asm);
				LCSave = LCCnt;
				TotalLines += LineCnt;
			}
		}
	}
	fclose(fd_adt);
}

// ************************************************************************************

void Build(char * fname) {
	int	 idx;
	FILE * fd;

	output_file_name[0] = 0;
	symb_ptr = symb_buffer;
	for (idx = 0 ; idx < HASH_SIZE ; idx ++) {
		bucket[idx] = NULL;
    }

	PassCnt = 1;
	FileCnt = 0;
	ErrorFlag = 0;
	build_from_file(fname);
	if (!ErrorFlag) {
		PassCnt = 2;
		FileCnt = 0;
		build_from_file(fname);
	}

	if (!ErrorFlag) {
		if (output_file_name[0] != 0) {
			if ((fd = fopen(output_file_name, "wb")) != NULL) {
				fwrite(code_buf, 1, CODE_MAX, fd);
				fclose(fd);
				printf("Write File : %s\n", output_file_name);
			} else {
				printf("Failed write to file %s.\n", output_file_name);
			}
		}
		printf("Total %d Lines in %d Files Built.\n", TotalLines, FileCnt);
	}
}
