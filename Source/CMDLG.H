// Borland C++ Windows - (C) Copyright 1992 by Borland International

// FILE : cmdlg.h

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <commdlg.h>
#include "INTEGRAT_STRUCTS.h"

// Borland C++ Windows - (C) Copyright 1992 by Borland International

// FILE : cmdlgr.h

#define RAM_REFRESH	0x109
#define RAM_CON	0x101
#define RAM_TRA	0x102
#define RAM_COD	0x103
#define RAM_BROWSE	0x104
#define RAM_SAVE	0x105
#define RAM_LOAD	0x106
#define CB_CONFILE	0x101
#define CB_TRAFILE	0x102
#define CB_CODFILE	0x103
#define CB_VSIZE	0x104
#define CB_THRESH	0x105
#define CB_DUR	0x106
#define CB_MIN	0x107
#define CB_MAX	0x108
#define CB_SIZE	0x109
#define CB_INTERVAL	0x110
#define CB_DIM	0x111
#define CB_TDESC	0x112
#define CB_ID1	0x113
#define CB_NAME1	0x114
#define CB_ID2	0x115
#define CB_NAME2	0x116
#define CB_ID3	0x117
#define CB_NAME3	0x118
#define CB_ID4	0x119
#define CB_NAME4	0x120
#define CB_ID5	0x121
#define CB_NAME5	0x122
#define CB_ID6	0x123
#define CB_NAME6	0x124
#define CB_ID7	0x125
#define CB_NAME7	0x126
#define CB_EX1	0x127
#define CB_EX2	0x128
#define CB_EX3	0x129
#define CB_EX4	0x130
#define CB_EX5	0x131
#define CB_EX6	0x132
#define CB_EX7	0x133
#define CB_CSUMM	0x134
#define CB_TITER	0x135
#define CB_NEXAM	0x136
#define CB_NID	0x137
#define CB_MORE	0x138
#define GETNUM_TEXT	0x101
#define GETNUM_NUM	0x102
#define GETNUM_OK	0x103
#define GETNUM_CAN	IDCANCEL
#define TB_SIZE	0x101
#define TB_INTERVAL	0x102
#define TB_DIM	0x103
#define TB_ACC	IDOK
#define TB_CAN	IDCANCEL
#define TB_MAX	0x105
#define ID2_NUM	0x101
#define RB_SCALE	0x101
#define FB_ALLOC	0x101
#define FB_NAME	0x102
#define SB_THRESH	0x101
#define SB_DUR	0x102
#define SB_MIN	0x103
#define SB_MAX	0x104
#define SB_CAN	IDCANCEL
#define SB_ACC	IDOK
#define IB_NUM 0x101
#define IB_NAME	0x102
#define IB_ACC	IDOK
#define IB_CAN	IDCANCEL
#define szCMDLGAPAbout "Voice to LPC Conversion Program. Use FILE to select input file"

#define CM_U_FILEOPEN               0x100
#define CM_U_COLOR                  0x101
#define CM_U_FONT                   0x102
#define CM_U_DRAWNOT				0x107
#define CM_EXIT                     0x103
#define CM_U_RUN                    0x104
#define CM_U_PLAY                   0x105
#define CM_U_WAV                   0x106
#define CM_U_DRAWNOT				0x107
#define CM_U_DRAWNOW				0x108
#define CM_U_DRAWBW					0x109

#define CM_U_SIZE		0x200

#define CM_U_FILEPLAY	0x212
#define CM_U_FILEREC	0x213
#define CM_U_FILECON	0x214
#define CM_U_FILERATE	0x215 

#define CM_U_SEGLEV	0x224
#define CM_U_SEGDUR		0x225
#define CM_U_SEGMIN		0x226
#define CM_U_SEGMAX		0x227

#define CM_U_IDNEXT		0x238
#define CM_U_IDNUM      0x239
#define CM_U_IDPAIR     0x230
#define CM_U_IDSTEP		0x237
#define CM_U_IDFILE		0x236

#define CM_U_TRANON     0x241
#define CM_U_TRANLPC    0x242
#define CM_U_TRANFFT    0x243
#define CM_U_TRANRMS    0x244
#define CM_U_TRANSIZE   0x245
#define CM_U_TRANINTERVAL	0x246
#define CM_U_TRANDIM	0x247
#define CM_U_TRANNOPITCH	0x248
#define CM_U_TRANHAMM	0x249

#define CM_U_UNTRCALC   0x258
#define CM_U_UNTRFILE   0x259
#define CM_U_UNTRSAVE   0x250
#define CM_U_UNTRPLAY   0x251

#define CM_U_CODEON     0x262
#define CM_U_CODEFILE   0x263
#define CM_U_CODEUCL    0x264
#define CM_U_CODEAUTOASSC	0x265
#define CM_U_CODESCL    0x266
#define CM_U_CODEBACK   0x267

#define CM_U_DECOON     0x278


#define CM_U_HELPABOUT  0x290

//long FAR PASCAL _export MainWndProc(HWND, unsigned, WORD, LONG);
BOOL InitApplication(HINSTANCE);
