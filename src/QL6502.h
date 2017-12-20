#ifndef _QL6502_HEADER_H_
#define _QL6502_HEADER_H_

#if defined(_WIN32)					// Winodws platform

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

#define strupr	_strupr

#else 

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <stdint.h>
#include <math.h>
#include <setjmp.h>

#endif

#define	TEXT_LINE_SIZE	256
#define HASH_SIZE		127
#define	ID_NAME_LEN		32
#define SYMB_MAX		0x8000
#define CODE_MAX		0x10000

/* ............  For XD/SAT .....................................*/

void Build	(char * fname);
void XD6502	(void);
void Pack	(char * op, char * src, char * dst);

#endif // _QL6502_HEADER_H_

