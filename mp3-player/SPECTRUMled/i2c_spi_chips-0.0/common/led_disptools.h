/* Header file associated with LED DISPlay TOOLS structures etc.
 * $Id: led_disptools.h,v 1.7 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef LED_DISPTOOLS_H
#define LED_DISPTOOLS_H 1

#ifdef NO_MAIN
#define TRUE       1
#define FALSE      0
#define NUL        '\0'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif /* NO_MAIN */

/* Humble Pi LED matrix #defines etc. ... */
/* Based on 2 x MCP23008 I.C.'s           */
#define ALL_ROW_CATHODES_ON   0x00
#define ALL_ROW_CATHODES_OFF  0xff
#define ALL_COLUMN_ANODES_ON  0xff
#define ALL_COLUMN_ANODES_OFF 0x00

#define RC1_ON 0x7f
#define RC2_ON 0xbf
#define RC3_ON 0xdf
#define RC4_ON 0xef
#define RC5_ON 0xf7
#define RC6_ON 0xfb
#define RC7_ON 0xfd
#define RC8_ON 0xfe

#define CA1_ON 0x10
#define CA2_ON 0x20
#define CA3_ON 0x40
#define CA4_ON 0x80
#define CA5_ON 0x01
#define CA6_ON 0x02
#define CA7_ON 0x04
#define CA8_ON 0x08

/* LC Technology 2x4-Digit 74HC164 driven Display #defines etc. */
/* Chip#1 --> Segment + DP Cathodes                             */
/* Chip#2 --> Digit (Common) Anodes                             */
#define ALL_CSEGSDP_ON  0x00
#define ALL_CSEGSDP_OFF 0xff
#define CSEGA_OFF       0x01
#define CSEGB_OFF       0x02
#define CSEGC_OFF       0x04
#define CSEGD_OFF       0x08
#define CSEGE_OFF       0x10
#define CSEGF_OFF       0x20
#define CSEGG_OFF       0x40
#define CSEGDP_OFF      0x80
#define CSEGDP_ON       0x3f 
#define ALL_ADIGITS_ON  0xff
#define ALL_ADIGITS_OFF 0x00
#define ADIGIT_U1_ON    0x01
#define ADIGIT_U2_ON    0x02
#define ADIGIT_U3_ON    0x04
#define ADIGIT_U4_ON    0x08
#define ADIGIT_L1_ON    0x10
#define ADIGIT_L2_ON    0x20
#define ADIGIT_L3_ON    0x40
#define ADIGIT_L4_ON    0x80

typedef struct
{
  int poll;
  int data;
  int dcode;
} mxdigit; /* Multi-pleXed DIGIT */

typedef struct
{
  unsigned int poll;
  int data;
  unsigned int dcode;
} mxudigit; /* Multi-pleXed Unsigned DIGIT */

typedef struct
{
  unsigned int *poll;
  int *data;
  unsigned int *dcode;
  int ncascade;
} cmxudigit; /* Cascaded Multi-pleXed Unsigned DIGIT */

typedef struct
{
  unsigned int poll;
  int data;
  unsigned int dcode;
  unsigned int lgrmask;
} mxudigitlgr; /* Multi-pleXed Unsigned DIGIT + Ln/Red/Green mask */

/* Ensure globals only declared once ... */
/* Relies on NO_MAIN being defined in libraries */
#ifndef NO_MAIN
int codes7seg0[19]; /* 16 (hex) digits + All on/off + d.p. */
int codes7seg7[19]; /* 16 (hex) digits + All on/off + d.p. */
unsigned int codes7seg0a[19]; /* 16 (hex) digits + All on/off + d.p. */
unsigned int codes7seg7a[22]; /* 16 (hex) digits + All on/off + d.p. + L1-3 */
unsigned int lgrmask;
int cathode[10];
int cath4x7[6];
unsigned int anode4d[6];
unsigned int anode4dul[10];
unsigned int anode4dlgr[10];
unsigned int rowcath[10],colanode[10]; /* 8x8 LED matrix */
mxdigit sevenseg[8];
mxdigit four7seg[4];
mxudigit four7seg0a[4],ulfour7seg0a[8];
mxudigitlgr four7seglgra[8];
#else /* !NO_MAIN */
extern int codes7seg0[19]; /* 16 (hex) digits + All on/off + d.p. */
extern int codes7seg7[19]; /* 16 (hex) digits + All on/off + d.p. */
extern unsigned int codes7seg0a[19]; /* 16 (hex) digits + All on/off + d.p. */
extern unsigned int codes7seg7a[22]; /* 16 (hex) digits + All on/off + d.p. + L1-3 */
extern unsigned int lgrmask;
extern int cathode[10];
extern int cath4x7[6];
extern unsigned int anode4d[6];
extern unsigned int anode4dul[8];
extern unsigned int anode4dlgr[10];
extern unsigned int rowcath[10],colanode[10]; /* 8x8 LED matrix */
extern mxdigit sevenseg[8];
extern mxdigit four7seg[4];
extern mxudigit four7seg0a[4],ulfour7seg0a[8];
extern mxudigitlgr four7seglgra[8];
#endif /* !NO_MAIN */

void init_codes7seg0(void);
/* Common cathode LED display - Anode (high-level) hex codes */
/* Bit 0 1 2 3 4 5 6 7  */
/* Seg a b c d e f g DP */

void init_codes7seg0a(void);
/* Common anode LED display - Cathode (low-level) hex codes */
/* Bit 0 1 2 3 4 5 6 7  */
/* Seg a b c d e f g DP */

void init_codes7seg7(void);
/* Common cathode LED display - Anode (high-level) hex codes */
/* Bit 7 6 5 4 3 2 1 0  */
/* Seg a b c d e f g DP */

void init_codes7seg7a(void);
/* Common anode LED display - Cathode (low-level) hex codes */
/* Bit 7 6 5 4 3 2 1 0  */
/* Seg a b c d e f g DP */

void init_cathodes(void);
/* 8 digits from left to right */
/* Bit 0 1 2 3 4 5 6 7 */
/* Dgt 8 7 6 5 4 3 2 1 */

void init_cath4x7(void);
/* 4 digits from left to right */
/* Bit 4 5 6 7 */
/* Dgt 4 3 2 1 */

void init_anode4d(void);
/* 4 digits from left to right */
/* Bit 0 1 2 3 */
/* Dgt 4 3 2 1 */

void init_anode4dul(void);
/* 2 rows of 4 digits from left to right */

void init_anode4dlgr(void);
/* 4 digits from left to right + cLock indicators & Red|Green eXternal led */
/* Bit 7 6 5 4 3 - 2 1 */
/* Dgt 4 3 2 1 L - G R */

void init_8x8ledx(void);
/* multiplexed 8 X 8 LED matriX */

void init_sevenseg(void);

void init_four7seg(void);

void init_four7seg0a(void);

void init_ulfour7seg0a(void);

void init_four7seglgra(void);

int load_sevenseg_digit(int index,
			int dpflag,
			int value);
/* LOAD index'th SEGment DIGIT */
/* dpflag = 1 (TRUE) assigns Decimal Point */

int load_four7seg_digit(int index,
			int dpflag,
			int value);
/* LOAD index'th 4 x 7-SEGment DIGIT */
/* dpflag = 1 (TRUE) assigns Decimal Point */

int load_four7seg0a_digit(int index,
			  int dpflag,
			  int value);
/* LOAD index'th 4 x 7-SEGment (common Anode) DIGIT */
/* dpflag = 1 (TRUE) assigns Decimal Point */

int load_ulfour7seg0a_digit(int index,
			    int dpflag,
			    int value);
/* LOAD index'th 2 x 4 x 7-SEGment (common Anode) DIGIT */
/* Upper row indeces 0 - 3 */
/* Lower row indeces 4 - 7 */
/* dpflag = 1 (TRUE) assigns Decimal Point */

int load_four7seglgra_digit(int index,
			    int dpflag,
			    int value,
			    unsigned int lgrmask);
/* LOAD index'th 4 x 7-SEGment (common Anode) DIGIT|Ln|GRY */
/* dpflag = 1 (TRUE) assigns Decimal Point */
/* indexed poll value combined with lgrmask to control external G|R(|Y) led */

#endif /* !LED_DISPTOOLS_H */

/* Change Log */
/*
 * $Log: led_disptools.h,v $
 * Revision 1.7  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.6  2017/09/30 18:22:31  pi
 * Interim Commit
 *
 * Revision 1.5  2017/09/28 19:23:52  pi
 * Added 74HC164 (spi-like) support
 *
 * Revision 1.4  2017/08/19 18:05:15  pi
 * Implicit definition warnings solved and code cleaned up
 *
 * Revision 1.3  2017/07/25 20:12:10  pi
 * Interim Commit
 *
 * Revision 1.2  2017/07/25 14:08:44  pi
 * Interim Commit
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.9  2015/06/25 22:56:06  pi
 * Interim commit
 *
 * Revision 1.8  2015/06/24 20:43:17  pi
 * Interim commit
 *
 * Revision 1.7  2015/06/21 21:58:10  pi
 * Interim commit
 *
 * Revision 1.6  2015/06/16 14:42:19  pi
 * More multiplexing code added
 *
 * Revision 1.5  2015/06/13 21:43:53  pi
 * Support for common anode ex-DTV display + bi-colour external led added
 *
 * Revision 1.4  2015/06/13 17:01:21  pi
 * Interim commit
 *
 * Revision 1.3  2015/06/10 22:13:28  pi
 * Multiplexed common anode support code added
 *
 * Revision 1.2  2015/06/08 19:46:48  pi
 * Some support for common anode displays added
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.1  2014/05/26 21:44:15  pi
 * Initial revision
 *
 * Revision 1.1  2013/06/03 20:58:09  pi
 * Initial revision
 *
 * Revision 1.2  2013/05/24 21:06:19  pi
 * Interim commit
 *
 * Revision 1.1  2013/05/24 13:25:18  pi
 * Initial revision
 *
 *
 *
 */
