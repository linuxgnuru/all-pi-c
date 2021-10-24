/* C source file associated with LED DISPlay TOOLS structures etc.
 * $Id: led_disptools.c,v 1.4 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "led_disptools.h"

void init_codes7seg0(void)
/* Common cathode LED display - Anode (high-level) hex codes */
/* Bit 0 1 2 3 4 5 6 7  */
/* Seg a b c d e f g DP */
{
  codes7seg0[ 0] = 0x3f;
  codes7seg0[ 1] = 0x06;
  codes7seg0[ 2] = 0x5b;
  codes7seg0[ 3] = 0x4f;
  codes7seg0[ 4] = 0x66;
  codes7seg0[ 5] = 0x6d;
  codes7seg0[ 6] = 0x7d;
  codes7seg0[ 7] = 0x07;
  codes7seg0[ 8] = 0x7f;
  codes7seg0[ 9] = 0x67;
  codes7seg0[10] = 0x77; /* A */
  codes7seg0[11] = 0x7c; /* b */
  codes7seg0[12] = 0x39; /* C */
  codes7seg0[13] = 0x5e; /* d */
  codes7seg0[14] = 0x79; /* E */
  codes7seg0[15] = 0x71; /* F */
  codes7seg0[16] = 0x00; /* All off */
  codes7seg0[17] = 0xff; /* All on */
  codes7seg0[18] = 0x80; /* Decimal Point */
}

void init_codes7seg7(void)
/* Common cathode LED display - Anode (high-level) hex codes */
/* Bit 7 6 5 4 3 2 1 0  */
/* Seg a b c d e f g DP */
{
  codes7seg7[ 0] = 0xfc;
  codes7seg7[ 1] = 0x60;
  codes7seg7[ 2] = 0xda;
  codes7seg7[ 3] = 0xf2;
  codes7seg7[ 4] = 0x66;
  codes7seg7[ 5] = 0xb6;
  codes7seg7[ 6] = 0xbe;
  codes7seg7[ 7] = 0xe0;
  codes7seg7[ 8] = 0xfe;
  codes7seg7[ 9] = 0xe6;
  codes7seg7[10] = 0xee; /* A */
  codes7seg7[11] = 0x3e; /* b */
  codes7seg7[12] = 0x9c; /* C */
  codes7seg7[13] = 0x7a; /* d */
  codes7seg7[14] = 0x9e; /* E */
  codes7seg7[15] = 0x8e; /* F */
  codes7seg7[16] = 0x00; /* All off */
  codes7seg7[17] = 0xff; /* All on */
  codes7seg7[18] = 0x01; /* Decimal Point */
}

void init_codes7seg0a(void)
/* Common anode LED display - Cathode (low-level) hex codes */
/* Bit 0 1 2 3 4 5 6 7  */
/* Seg a b c d e f g DP */
{
  codes7seg0a[ 0] = 0xc0;
  codes7seg0a[ 1] = 0xf9;
  codes7seg0a[ 2] = 0xa4;
  codes7seg0a[ 3] = 0xb0;
  codes7seg0a[ 4] = 0x99;
  codes7seg0a[ 5] = 0x92;
  codes7seg0a[ 6] = 0x82;
  codes7seg0a[ 7] = 0xf8;
  codes7seg0a[ 8] = 0x80;
  codes7seg0a[ 9] = 0x98;
  codes7seg0a[10] = 0x88; /* A */
  codes7seg0a[11] = 0x83; /* b */
  codes7seg0a[12] = 0xc6; /* C */
  codes7seg0a[13] = 0xa1; /* d */
  codes7seg0a[14] = 0x86; /* E */
  codes7seg0a[15] = 0x8e; /* F */
  codes7seg0a[16] = 0xff; /* All off */
  codes7seg0a[17] = 0x00; /* All on */
  codes7seg0a[18] = 0x7f; /* Decimal Point */
}

void init_codes7seg7a(void)
/* Common anode LED display - Cathode (low-level) hex codes */
/* Bit 7 6 5 4 3 2 1 0  */
/* Seg a b c d e f g DP */
{
  codes7seg7a[ 0] = 0x03;
  codes7seg7a[ 1] = 0x9f;
  codes7seg7a[ 2] = 0x25;
  codes7seg7a[ 3] = 0x0d;
  codes7seg7a[ 4] = 0x99;
  codes7seg7a[ 5] = 0x49;
  codes7seg7a[ 6] = 0x41;
  codes7seg7a[ 7] = 0x1f;
  codes7seg7a[ 8] = 0x01;
  codes7seg7a[ 9] = 0x19;
  codes7seg7a[10] = 0x11; /* A */
  codes7seg7a[11] = 0xc1; /* b */
  codes7seg7a[12] = 0x63; /* C */
  codes7seg7a[13] = 0x85; /* d */
  codes7seg7a[14] = 0x61; /* E */
  codes7seg7a[15] = 0x71; /* F */
  codes7seg7a[16] = 0xff; /* All off */
  codes7seg7a[17] = 0x00; /* All on */
  codes7seg7a[18] = 0xfe; /* Decimal Point */
  codes7seg7a[19] = 0x7f; /* Clock Indicator L1 */
  codes7seg7a[20] = 0xbf; /* Clock Indicator L2 */
  codes7seg7a[21] = 0xdf; /* Clock Indicator L3 */
}

void init_cathodes(void)
/* 8 digits from left to right */
/* Bit 0 1 2 3 4 5 6 7 */
/* Dgt 8 7 6 5 4 3 2 1 */
{
  cathode[0] = 0xff; /* All disabled */
  cathode[1] = 0x7f;
  cathode[2] = 0xbf;
  cathode[3] = 0xdf;
  cathode[4] = 0xef;
  cathode[5] = 0xf7;
  cathode[6] = 0xfb;
  cathode[7] = 0xfd;
  cathode[8] = 0xfe; /* NB: May not have a d.p. */
  cathode[9] = 0x00; /* All enabled */
}

void init_cath4x7(void)
/* 4 digits from left to right */
/* Bit 4 5 6 7 */
/* Dgt 4 3 2 1 */
{
  cath4x7[0] = 0xff; /* All disabled */
  cath4x7[1] = 0x7f;
  cath4x7[2] = 0xbf;
  cath4x7[3] = 0xdf;
  cath4x7[4] = 0xef;
  cath4x7[5] = 0x0f; /* All enabled */
}

void init_anode4d(void)
/* 4 digits from left to right */
/* Bit 0 1 2 3 */
/* Dgt 4 3 2 1 */
{
  anode4d[0] = 0x00; /* All disabled */
  anode4d[1] = 0x08;
  anode4d[2] = 0x04;
  anode4d[3] = 0x02;
  anode4d[4] = 0x01;
  anode4d[5] = 0x0f; /* All enabled */
}

void init_anode4dul(void)
/* 2 rows of 4 digits from left to right */

{
  anode4dul[0] = ALL_ADIGITS_OFF;
  /* Upper Row */
  anode4dul[1] = ADIGIT_U1_ON;
  anode4dul[2] = ADIGIT_U2_ON;
  anode4dul[3] = ADIGIT_U3_ON;
  anode4dul[4] = ADIGIT_U4_ON;
  /* Lower Row */
  anode4dul[5] = ADIGIT_L1_ON;
  anode4dul[6] = ADIGIT_L2_ON;
  anode4dul[7] = ADIGIT_L3_ON;
  anode4dul[8] = ADIGIT_L4_ON;
  anode4dul[9] = ALL_ADIGITS_ON;
}

void init_anode4dlgr(void)
/* 4 digits from left to right + cLock indicators & Red|Green eXternal led */
/* Bit 7 6 5 4 3 - 2 1 */
/* Dgt 4 3 2 1 L - G R */
{
  anode4dlgr[0] = 0x00; /* All disabled */
  anode4dlgr[1] = 0x10;
  anode4dlgr[2] = 0x20;
  anode4dlgr[3] = 0x40;
  anode4dlgr[4] = 0x80;
  anode4dlgr[5] = 0x08; /* cLock indicator | mask */
  anode4dlgr[6] = 0x02; /* Green Xled | mask */
  anode4dlgr[7] = 0x01; /* Red Xled | mask */
  anode4dlgr[8] = 0xf0; /* All digit anodes enabled */
  anode4dlgr[9] = 0x03; /* Multiplexed only, "Yellow" Xled mask */
}

void init_8x8ledx(void)
/* multiplexed 8 X 8 LED matriX */
{
  rowcath[0]  = ALL_ROW_CATHODES_OFF;
  rowcath[1]  = RC1_ON;
  rowcath[2]  = RC2_ON;
  rowcath[3]  = RC3_ON;
  rowcath[4]  = RC4_ON;
  rowcath[5]  = RC5_ON;
  rowcath[6]  = RC6_ON;
  rowcath[7]  = RC7_ON;
  rowcath[8]  = RC8_ON;
  rowcath[9]  = ALL_ROW_CATHODES_ON;
  colanode[0] = ALL_COLUMN_ANODES_OFF;
  colanode[1] = CA1_ON;
  colanode[2] = CA2_ON;
  colanode[3] = CA3_ON;
  colanode[4] = CA4_ON;
  colanode[5] = CA5_ON;
  colanode[6] = CA6_ON;
  colanode[7] = CA7_ON;
  colanode[8] = CA8_ON;
  colanode[9] = ALL_COLUMN_ANODES_ON;
}

void init_sevenseg(void)
{
  /* All blank and off */
  short int i;
  init_codes7seg0();
  init_cathodes();  
  for (i=0; i<8; i++)
    {
      sevenseg[i].poll  = cathode[0];
      sevenseg[i].data  = -1; /* ie. undefined */
      sevenseg[i].dcode = codes7seg0[16];
    }
}

void init_four7seg(void)
{
  /* All blank and off */
  short int i;
  init_codes7seg7();
  init_cath4x7();  
  for (i=0; i<4; i++)
    {
      four7seg[i].poll  = cath4x7[0];
      four7seg[i].data  = -1; /* ie. undefined */
      four7seg[i].dcode = codes7seg7[16];
    }
}

void init_four7seg0a(void)
{
  /* All blank and off */
  short int i;
  init_codes7seg0a();
  init_anode4d(); 
  for (i=0; i<4; i++)
    {
      four7seg0a[i].poll  = anode4d[0];
      four7seg0a[i].data  = -1; /* ie. undefined */
      four7seg0a[i].dcode = codes7seg0a[16];
    }
}

void init_ulfour7seg0a(void)
{
  /* All blank and off */
  short int i;
  init_codes7seg0a();
  init_anode4dul(); 
  for (i=0; i<8; i++)
    {
      ulfour7seg0a[i].poll  = ALL_ADIGITS_OFF;
      ulfour7seg0a[i].data  = -1; /* ie. undefined */
      ulfour7seg0a[i].dcode = ALL_CSEGSDP_OFF;
    }
}

void init_four7seglgra(void)
{
  /* All blank and off */
  short int i;
  init_codes7seg7a();
  init_anode4dlgr(); 
  for (i=0; i<4; i++)
    {
      four7seg0a[i].poll  = anode4dlgr[0];
      four7seg0a[i].data  = -1; /* ie. undefined */
      four7seg0a[i].dcode = codes7seg7a[16];
    }
}

int load_sevenseg_digit(int index,
			int dpflag,
			int value)
/* LOAD index'th SEGment DIGIT */
/* dpflag = 1 (TRUE) assigns Decimal Point */
{
  int lidx,nidx;
  int lval;
  lidx = (index-1) % 8;
  nidx = lidx + 1;
  sevenseg[lidx].poll = cathode[nidx];
  lval = (int)(value % 10);
  sevenseg[lidx].data  = lval;
  sevenseg[lidx].dcode = codes7seg0[lval];
  if (dpflag) sevenseg[lidx].dcode = codes7seg0[lval] + codes7seg0[18];
  nidx--;
  return(nidx);
}

int load_four7seg_digit(int index,
			int dpflag,
			int value)
/* LOAD index'th 4 x 7-SEGment DIGIT */
/* dpflag = 1 (TRUE) assigns Decimal Point */
{
  int lidx,nidx;
  int lval;
  lidx = (index-1) % 4;
  nidx = lidx + 1;
  four7seg[lidx].poll = cath4x7[nidx];
  lval = (int)(value % 16); /* Allow hex values */
  four7seg[lidx].data  = lval;
  four7seg[lidx].dcode = codes7seg7[lval];
  if (dpflag) four7seg[lidx].dcode = codes7seg7[lval] + codes7seg7[18];
  nidx--;
  return(nidx);
}
			      
int load_four7seg0a_digit(int index,
			  int dpflag,
			  int value)
/* LOAD index'th 4 x 7-SEGment (common Anode) DIGIT */
/* dpflag = 1 (TRUE) assigns Decimal Point */
{
  int lidx,nidx;
  int lval;
  lidx = (index-1) % 4;
  nidx = lidx + 1;
  four7seg0a[lidx].poll = anode4d[nidx];
  lval = (int)(value % 16); /* Allow hex values */
  four7seg0a[lidx].data  = lval;
  four7seg0a[lidx].dcode = codes7seg0a[lval];
  if (dpflag) four7seg0a[lidx].dcode = codes7seg0a[lval] & codes7seg0a[18];
  nidx--;
  return(nidx);
}

int load_ulfour7seg0a_digit(int index,
			    int dpflag,
			    int value)
/* LOAD index'th 2 x 4 x 7-SEGment (common Anode) DIGIT */
/* Upper row indeces 0 - 3 */
/* Lower row indeces 4 - 7 */
/* dpflag = 1 (TRUE) assigns Decimal Point */
{
  int lidx,nidx;
  int lval;
  lidx = (index-1) % 8;
  nidx = lidx + 1;
  ulfour7seg0a[lidx].poll = anode4dul[nidx];
  lval = (int)(value % 16); /* Allow hex values */
  ulfour7seg0a[lidx].data  = lval;
  ulfour7seg0a[lidx].dcode = codes7seg0a[lval];
  if (dpflag) ulfour7seg0a[lidx].dcode = codes7seg0a[lval] & CSEGDP_ON;
  nidx--;
  return(nidx);
}

int load_four7seglgra_digit(int index,
			    int dpflag,
			    int value,
			    unsigned int lgrmask)
/* LOAD index'th 4 x 7-SEGment (common Anode) DIGIT|Ln|GRY */
/* dpflag = 1 (TRUE) assigns Decimal Point */
/* indexed poll value combined with lgrmask to control external G|R(|Y) led */
{
  int lidx,midx,nidx;
  int lval;
  if (index < 5)
    {
      /* Digit index ... */
      lidx = (index-1) % 4;
      nidx = lidx + 1;
      four7seglgra[lidx].poll = anode4dlgr[nidx];
      lval = (int)(value % 16); /* Allow hex values */
      four7seglgra[lidx].data  = lval;
      four7seglgra[lidx].dcode = codes7seg7a[lval];
      if (dpflag) four7seglgra[lidx].dcode = codes7seg7a[lval] & codes7seg7a[18];
    }
  else if (index > 4) /* cLock indicator (L1|L2|L3) index */
    {
      nidx = index;
      lidx = index - 1;
      four7seglgra[lidx].poll = anode4dlgr[5] & lgrmask;
      /* Value should be 0|1|2|3 for None|L1|L2|L3 */
      if (value > 3) lval = 16;
      else lval = value + 18;
      four7seglgra[lidx].data  = lval;
      four7seglgra[lidx].dcode = codes7seg7a[lval];
    }
  midx = nidx % 2;
  if (lgrmask == anode4dlgr[9])
    {
      if (midx == 0) four7seglgra[lidx].poll = four7seglgra[lidx].poll | anode4dlgr[6];
      else four7seglgra[lidx].poll = four7seglgra[lidx].poll | anode4dlgr[7];
    }
  else four7seglgra[lidx].poll = four7seglgra[lidx].poll | lgrmask;
  nidx--;
  return(nidx);
}

/* Change Log */
/*
 * $Log: led_disptools.c,v $
 * Revision 1.4  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.3  2017/09/30 18:22:31  pi
 * Interim Commit
 *
 * Revision 1.2  2017/07/25 20:12:10  pi
 * Interim Commit
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.11  2015/06/30 22:51:39  pi
 * Interim commit
 *
 * Revision 1.10  2015/06/24 20:43:17  pi
 * Interim commit
 *
 * Revision 1.9  2015/06/22 21:21:12  pi
 * Interim commit
 *
 * Revision 1.8  2015/06/22 20:40:15  pi
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
 *
 */
