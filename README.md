# QL6502

# CPU 6502 Assembler and Static Analysiser

## Description

The QL6502 is an assembler and static analysiser developed for the MOS 6502 CPU. It is CAXD6502 tool that the author developed in the 1990s.

The MOS 6502 is an 8-bit microprocessor developed by MOS Technologies in 1975. It became the highest performing 8-bit CPU on the market at a fraction of the price of one-sixth the price of similar chips found in major chip companies such as Motorola and Intel. It sparked a series of computer innovations and brought a personal computer revolution in the 1980s. MOS Technology initially authorized Rockwell International and Synertek as "second sources of supply" for the chip. So we used to hear the chip named R6502.

Co-born with the emerging 6502 TV game market in China in the 1990s, the CAXD6502 provides developers with static analysis of binary ROM files, disassembly, assembly and patch support. The CAXD6502 started with TurboC 2.0 from the Intel 80286 MS-DOS and was ported to the GCC compilation environment in the year 2004.

The QL6502 serves as a summary and memorial to the early development process as well as a simple development tool for FPGA CPU verification and test code.

## Features

QL6502 achieves assembly and disassembly functions for CPU 6502.

Usually an existing game program (firmware) for re-development and functional expansion, we must first understand the function of the original program. if we have no source code, the disassembly becomes the only resonable way. In early times, the code running on 8-bits processor is simple or less features, the source code itself is written in assembly language, so the disassembly analysis is relatively effective. Of course, as processors become more powerful and firmware functions become more and more complex, modern firmware is written in high-level languages such as C, and the way in which the algorithms are implemented has been detached from assembly and machine language, and the code is beyond recognition fater compiled or optimization. At this situation, disassembly is basically impossible, so the modern method we are used is usually "forward development".

The image file of a machine code contains the code and data. The function of disassembly is converting the binary code into an assembly mnemonic. But a stupid one-to-one translation does not meet the needs of our analysis either because translating the data into invalid mnemonics (marked with ???) or setting the half-word of the instruction as a jump entry results in only "junk code". In the era of a simple text editor, this kind of fault meant printing multiple, pointless source lists.

The QL6502 identifies the entire processor image file's code and data area through the simulated execution of instructions. The QL6502 first begins process the instruction analysis using the processor's well-known entry (RESET, NMI, IRQ), flows through the sequential instruction stream, identifies the identified subroutine call and branch instruction, and its destination address and identifies the other code entry. By repeatedly scanning instructions to automatically identify the entire code, this process can be understood as a virtual execution, and eventually not part of the code is executed as a virtual output data.

The QL6502 handles indirect transfer instructions based on programmer's help.

After compiling and analyzing the machine code, we can write assembly code to realize the function we need, and the tool we need is assembly language tool. Unlike common compilation tools, in addition to converting assembly mnemonics to machine code, the development of extensions requires some special features to meet our requirements. such as:

- Assembler can generate code based on the original binary image
- You can modify the original assembly instructions, such as jumping to our expansion of the entrance and so on
- Ability to include our code in the "gap" of the original image
- Simple structured, modular programming techniques

The QL6502, like a traditional assembler, processes the source file with two passes. Supported pseudo instructions  include ORG, EQU, DB, DW, +, - and other numerical operations. Conventional labels use names such as 'LABEL:'.  We implemented a new type of label 'LABEL::' (two colons) as public label. this kind of label will be see between multiple source files that make up a project, so it can be referenced throughout the project.

At the beginning of the CAXD6502 design, we implemented a complex expression computation stack to support constant expression compilation at assembly time, but we must say that during our development in the past decade, we have not encountered any of this "mathematical correct" expression calculation, so this time, we delete all these features.

The QL6502 implements the CODE directive. use

``` 
        CODE start end      ; like CODE 0x1234 0x5678
```

To define an area where user extension code can be placed (it has been analyzed before to confirm that the area is blank, invalid data area, etc.) and the subsequent code is placed sequentially in this area. Subsequent source files for a project can continue with code assembly by using only a CODE pseudo-instruction without parameters. If the user assembly code exceeds the defined area, an assembly error report is generated and the assembly is stopped.

# Build

The QL6502 now is compiled and implemented on Windows 7 64bits, Linux, Mac OS X using the GCC compiler. Use a well-defined makefile

``` bash
make
``` 

The corresponding executable will be generated in the bin directory.

# Usage

## Static Analysis and Disassembly

Start the QL6502 in the Windows Command Window, Linux Command Line, Mac OS X Terminal Window.

``` terminal
D:\QL6502\Testcase>..\obj\QL6502.exe -X
CA6502 V4.0 An Assembly Development Tool for 6502
fortchina@163.com, 2017.07 - 2017.07, ALL RIGHTS RESERVED

This Program is based on CA6502 V3.0, Zhao Yu, Zhang WenCui, 1993.05 - 2004.09

QL6502>

```

You can now enter the command after the prompt 'QL6502>', the command is a single letter, case-insensitive. You can use ? for help.

We can try to load an image file named 'ldt512.bin', this process is easy and almost no need to think about:

``` terminal
QL6502> ldt512.bin
65536 Bytes data read into code buffer @ 0x0000
```

The 6502's total addressing space is 64K bytes, but in real systems it certainly can not all be machine code. There is a lot of memory space for I/O address mapping. We call areas outside the code "invalid areas." Before analyzing, we need to identify these areas. This should be a very complex process that may require repeated analysis of the hardware circuitry and firmware code to determine:

``` terminal
QL6502> b 0x0000 0x3fff
QL6502> b 0x8000 0xbfff
QL6502> b
000000 - 003FFF: Not Used
004000 - 007FFF: Used
008000 - 00BFFF: Not Used
00C000 - 010000: Used
QL6502>
```

We use the X command for static analysis

``` terminal
QL6502> x
    1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
    1 Indirect Entries
```

QL6502 reports it encountered an indirect jump instruction, we use the i command to view

``` terminal
QL6502> i
(001) L_D425: JMP (0x0030)
```

So we look at the code associated with this command, the u command is for disassembly, which basically is a "dumb translation," so we can set the address to the front of the prompt above.

```
    QL6502> u 0xd410
    L_D41A  BD 7B D9                LDA  L_D97B,X
    L_D41D  85 30                   STA  0x30
    L_D41F  E8                      INX
    L_D420  BD 7B D9                LDA  L_D97B,X
    L_D423  85 31                   STA  0x31
    L_D425  6C 30 00                JMP  (0x0030)
    L_D428  A5 57                   LDA  0x57
    L_D42A  29 10                   AND  #0x10
    L_D42C  F0 0F                   BEQ  L_D43D
    L_D42E  AD 4A 08                LDA  L_084A
```

This shows that starting from the address L_D97B is a jump table, we can view the contents of the table

``` terminal
    QL6502> d 0xd97b
    Scale -   0  1  2  3  4  5  6  7 -  8  9  a  b  c  d  e  f   0123456789ABCDEF

    00D97B:                                     B7 D9 69 DA 3E              ..i.>
    00D980:  D3 85 DC E4 DC 5D DD DE - DD FC DD 58 DE 8A DE 09   .....].....X....
    00D990:  DF 61 DF DF DF 04 E0 60 - E0 B7 E0 30 FF 25 E2 35   .a.....`...0.%.5
    00D9A0:  E2 90 E2 EB 4E A0 65 51 - E3 84 E4 E5 E4 0E E5 2D   ....N.eQ.......-
    00D9B0:  E5 77 E5 2D E7 A6 E7 20 - 71 C5 20 65 D7 20 28 D4   .w.-... q. e. (.
    00D9C0:  20 A3 C5 20 E8 D2 20 7C - 79 20 72 59 20 90 5A A5    .. .. |y rY .Z.
    00D9D0:  75 29 80 09 11 85 75 A9 - 00 85 C7 85 70 85 73 85   u)....u.....p.s.
    00D9E0:  76 8D BF 03 A2 0C 20 20 - CC A2 00 20 20 CC 20 D4   v.....  ...  . .
```

After analysis the whole program, we see that the contents from 0xd97b to 0xd9b6 is jump instruction table, so we use the j command (Jump Table) command to tell the QL6502 jump table

``` terminal
QL6502> j 0xd97b 0xd9b6
```

Then analyze again

``` terminal
    QL6502> x
    1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20
    Ok.
```

This time the analyseser did not encounter indirect jump instruction that indicating the static analysis of the entire program is completed, we can generate disassembled code files:

``` terminal
QL6502> g dt512.asm 0 0x0xffff
```

Please note that the invalid area has been identified by above B command. The document we generated looks like this in any pure text editor

```
    L_4D1B: DB  0x20,0x21,0xF0,0xA6,0x9C,0xB5,0x80,0x29     ; .!.....)
    L_4D23: DB  0x0F,0xC9,0x07,0xA9,0x31,0x8D,0x66,0x10     ; ....1.f.
    L_4D2B: DB  0xA9,0x01,0x85,0xA1,0x60                    ; ....`
    L_4D30: JSR  L_D7A7              ;Subrutine ..........
    L_4D33  LDA  0xA1
    L_4D35  BNE  L_4D3C
    L_4D37  LDA  #0x49    ; I
    L_4D39  JSR  L_F4A3
    L_4D3C: RTS
    L_4D3D: JSR  L_D8DF              ;Subrutine ..........
    L_4D40  LDA  L_2000
    L_4D43  AND  #0x80
    L_4D45  BNE  L_4D61
    L_4D47  LDA  L_0731
    L_4D4A  CMP  #0x02
    L_4D4C  BEQ  L_4D62
    L_4D4E  BCS  L_4D6D
    L_4D50  CLC
    L_4D51  LDA  L_0730
    L_4D54  ADC  #0x01
    L_4D56  STA  L_0730
    L_4D59  LDA  L_0731
    L_4D5C  ADC  #0x00
    L_4D5E  STA  L_0731
    L_4D61: RTS
```

The data is identified by DB, and the subroutine entry is indentified by a marker ';Subrutine ..........'. A local jump entry will be represented like L_4D3C: (with one colon) and L_4D51 (without a colon) means CPU address 0x4D51.

### Assembly Tools

We can use QL6502 compiling a single source file. But for a project composed of multiple source files, the most convenient way is to define a project management file, such as the ABC.MAK file, it cantains:

``` terminal
    -OOUT512.bin
    -LIN512.bin
    A.ASM
    B.ASM
    C.ASM
    D.ASM
    X.ASM
    ……
```

Each text line corresponds to a description of the project, -OOUT512.bin output ROM image file name, -LIN512.bin input image file name. * .ASM for each source file, you need to be listed all source files one by one.

We use the following command to automatically complete the assembly of the entire project

``` terminal
QL6502 ABC.MAK
```

The QL6502 will load IN512.bin and assemble the source files to form the output file OUT512.bin. Here is an example of the actual work

``` terminal
    D:\QL6502\Testcase>..\obj\ql6502 DZZB.MAK
    CA6502 V4.0 An Assembly Development Tool for 6502
    fortchina@163.com, 2017.07 - 2017.07, ALL RIGHTS RESERVED

    This Program is based on CA6502 V3.0, ZhaoYu, ZhangWenCui, 1993.05 - 2004.09

    Load file : IN512.bin
    Write File : OUT512.bin
    Total 2404 Lines in 13 Files Built.
```

# Target Hardware Platform

QL6502 does not involve a specific target hardware platform.

In actual project working, we usually firstly use EEPROM programmer to read the ROM image into a file, then compiled code to generate the target image, and use the EEPROM programmer to write image into a variety of memory such as 27C64, 7527C256, 27C512 and so on. Because different hardware designers use different address mapping schemes, so the target image may need to cut and packed before burning. In the QL6502 project, we included an actual handler for its
integrity. If we only interaested in 6502 assembler/disassembler implementation, this part can be ignored.

# Naming 

QL is the abbreviation of Qinling (Qinling Mountains). Qinling provides a natural boundary between North and South China and support a huge variety of plant and wildlife, some of which is found nowhere else on Earth.

# Agreement

The open source release of the QL6502 follows the MIT open source code agreement.
