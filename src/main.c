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

static int ql_console_handler(int arg) {
	return 1;
}

// ************************************************************************************

int main(int argc, char * argv[]) {

	printf("QL6502 V1.0 An Assembly Development Tool for 6502\n");
	printf("fortchina@163.com, 2017.07 - 2024.07, ALL RIGHTS RESERVED\n\n");
	printf("This Program is based on CA6502 V3.0, ZhaoYu, ZhangWenCui, 1993.05 - 2004.09\n\n") ;


#ifdef WIN32
	setvbuf(stdout, NULL, _IONBF, 0); 
	SetConsoleCtrlHandler((PHANDLER_ROUTINE) ql_console_handler, TRUE);
#else	
	signal(SIGINT,  (void *) ql_console_handler);
	signal(SIGTERM, (void *) ql_console_handler);
	signal(SIGPIPE, SIG_IGN);
#endif	

	if (argc == 2) {
		if (strcmp(argv[1], "-X") == 0 || strcmp(argv[1], "-x") == 0) {
			XD6502();
		} else {
			Build(argv[1]);
		}
	} else if (argc == 4) {
		Pack(argv[1], argv[2], argv[3]);
	} else {
		printf("Usage:  QL6502 file.mak <CR> \n");
		printf("            Build ADT program\n\n");
		printf("        QL6502 [opt] [src-file] [dst-file] <CR> \n");
		printf("            [opt] -p1 U512.bin P256.bin  -> Pack 2000 Program.\n");
		printf("                  -p2 U512.bin P512.bin  -> Pack 2017 Latest.\n\n");
		printf("                  -u1 P256.bin U512.bin  -> UnPack 2000 Program.\n");
		printf("                  -u2 P512.bin U512.bin  -> UnPack 2017 Latest.\n");
		printf("                  -img IMG128.bin IMG256.bin  -> Resize image file.\n\n");
		printf("        QL6502 -X <CR>\n");
		printf("            Enter interpret mode for XD6502 command\n\n");    
	}
	return 0;		
}
