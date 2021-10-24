/* Header file associated with GLCD XBM file handling TOOLS structures etc.
 * (+ some limited XPM file support)
 * $Id: glcd_xbmtools.h,v 1.3 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef GLCD_XBMTOOLS_H
#define GLCD_XBMTOOLS_H 1

#ifdef NO_MAIN
#define TRUE       1
#define FALSE      0
#define NUL        '\0'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif /* NO_MAIN */

#ifndef PARSEL_H
#include "parsel.h"
#endif /* !PARSEL_H */

/* *.xbm (xpm) file parser ... */

#define XBM_FN_LEN (FN_LEN + 7) /* extra 7 chars. of "height" */
#define XPM_FN_LEN (FN_LEN + 3) /* +3 for "*[]"               */         

typedef enum
{
  CCOMMENT,    /* /*       */
  HASHDEFKEY,  /* #define  */
  STATICKEY,   /* static   */
  UNSIGNEDKEY, /* unsigned */
  CHARKEY,     /* char     */
  KEYDATA,
  /* Extra XPM keys ...    */
  XPMKEY,      /* XPM      */
  CLRKEY,      /* c        */
  /* XPM file "support codes" ... */
  XPMCOLPIX8,  /* colpix8  */
  XPMPIXEL8,   /* Pixel8   */
  XPMUDGC,     /* UDGC     */
  XPMNIU,      /* NotInUse */
  LAST_XBMKEY,
  XBMKEYTYPES
} xbmkeynames;

typedef struct
{
  int width;
  int height;
  char wname[XBM_FN_LEN];
  char hname[XBM_FN_LEN];
  char bname[XBM_FN_LEN];
} xbm_header;

typedef struct
{
  int width;
  int height;
  int colours;
  int charperpixel;
  char back;
  char fore;
  char cname[XPM_FN_LEN];
} xpm_header;

/* QC12864B GLCD using the  ST7920 controller ... */
#define QC12864B_width  128
#define QC12864B_height  64
#define QC12864B_nblocks  4
/* Each block is 128 x 16 pixels, stored end-to-end but   ... */
/* displayed top-to-bottom ie. (in character mode) line 1 ... */
/* wraps around into line 3 (and 2 to 4 likewise)             */
#define QC12864B_nbytes ((QC12864B_width / 8) * (QC12864B_height / QC12864B_nblocks))       
/* 8-pixel values stored per byte */
#define QC12864B_xbmbytes (QC12864B_nbytes * QC12864B_nblocks)

typedef struct
{
  unsigned char block11[QC12864B_nbytes];
  unsigned char block22[QC12864B_nbytes];
  unsigned char block13[QC12864B_nbytes];
  unsigned char block24[QC12864B_nbytes];
} qc12864b_pixel8;

#define QC12864B_udgcbits 16
#define QC12864B_udgcpixc (QC12864B_udgcbits * QC12864B_udgcbits)

typedef struct
{
  unsigned char cgram_addr;
  unsigned char hbyte_rows[QC12864B_udgcbits];
  unsigned char lbyte_rows[QC12864B_udgcbits];
} qc12864b_udgc;

/* Nokia 5110 84x48 GLCD using the PCD8544 controller ...                  */
/* NB: XPM images are rotated +90deg since the GLCD is column-based. */
#define PCD8544_column_width 48
#define PCD8544_row_height   84
#define PCD8544_nblocks       6
#define PCD8544_xpmpixc      (PCD8544_column_width * PCD8544_row_height)
/* Each column block is 8 x 84 pixels, in 8-bit rows stored  */
/* bottom-to-top then left-to-right (from the rotated image) */
typedef struct
{
  unsigned char rowblock0[PCD8544_row_height]; /* Bottom row ... */
  unsigned char rowblock1[PCD8544_row_height]; 
  unsigned char rowblock2[PCD8544_row_height];
  unsigned char rowblock3[PCD8544_row_height]; 
  unsigned char rowblock4[PCD8544_row_height];
  unsigned char rowblock5[PCD8544_row_height]; /* ... Top row    */ 
} pcd8544_colpix8;

/* Ensure globals only declared once ... */
/* Relies on NO_MAIN being defined in libraries */
#ifndef NO_MAIN
/* Useful strings ... */
char xbmblank[] = " ";
char xbmerror[] = "***ERROR*** ";
char xbmwarng[] = "**WARNING** "; 
xbmkeynames xbmwhat,xbmwhich,xpmtype;
char *xbmkeylist[XBMKEYTYPES];
char xbmfile[FN_LEN],xpmfile[FN_LEN];
char xbmfopen,xpmfopen;
char xbmfread,xpmfread;
char XbmError,XpmError;
FILE *xbmfn,*xpmfn;
xbm_header this_xbm_header;
xpm_header this_xpm_header;
qc12864b_pixel8 current_qc12864b_buffer;
qc12864b_udgc cgram0000,cgram0002,cgram0004,cgram0006;
pcd8544_colpix8 bbnokia5110_buffer,fsnokia5110_buffer,nokia5110v2_buffer,nokia5110HP3_buffer;
#else /* !NO_MAIN */
extern char xbmblank[];
extern char xbmerror[];
extern char xbmwarng[]; 
extern xbmkeynames xbmwhat,xbmwhich,xpmtype;
extern char *xbmkeylist[XBMKEYTYPES];
extern char xbmfile[],xpmfile[FN_LEN];
extern char xbmfopen,xpmfopen;
extern char xbmfread,xpmfread;
extern char XbmError,XpmError;
extern FILE *xbmfn,*xpmfn;
extern xbm_header this_xbm_header;
extern xpm_header this_xpm_header;
extern qc12864b_pixel8 current_qc12864b_buffer;
extern qc12864b_udgc cgram0000,cgram0002,cgram0004,cgram0006;
extern pcd8544_colpix8 bbnokia5110_buffer,fsnokia5110_buffer,nokia5110v2_buffer,nokia5110HP3_buffer;
#endif /* !NO_MAIN */

/* XBM data is "byte-bit-reversed       ... */
/* "Nibble Bit Reversal Look-Up Tables" ... */
static unsigned char right_nbrlut[] = {
  0x00, /* 0x00 */
  0x80, /* 0x01 */
  0x40, /* 0x02 */
  0xc0, /* 0x03 */
  0x20, /* 0x04 */
  0xa0, /* 0x05 */
  0x60, /* 0x06 */
  0xe0, /* 0x07 */
  0x10, /* 0x08 */
  0x90, /* 0x09 */
  0x50, /* 0x0a */
  0xd0, /* 0x0b */
  0x30, /* 0x0c */
  0xb0, /* 0x0d */
  0x70, /* 0x0e */
  0xf0, /* 0x0f */ };

static unsigned char left_nbrlut[] = {
  0x00, /* 0x00 */
  0x08, /* 0x10 */
  0x04, /* 0x20 */
  0x0c, /* 0x30 */
  0x02, /* 0x40 */
  0x0a, /* 0x50 */
  0x06, /* 0x60 */
  0x0e, /* 0x70 */
  0x01, /* 0x80 */
  0x09, /* 0x90 */
  0x05, /* 0xa0 */
  0x0d, /* 0xb0 */
  0x03, /* 0xc0 */
  0x0b, /* 0xd0 */
  0x07, /* 0xe0 */
  0x0f, /* 0xf0 */ };

/* Primitives */

void setupxbmkeys(void);

xbmkeynames whichxbmkey(char *xbmkeystr);

void listxbmkeys(void);

void Open_Xbm_File(void);

void Open_Xpm_File(char *mode);

void Close_Xbm_File(void);

void Close_Xpm_File(void);

void Xbm_Error(xbmkeynames error_key);

void Xpm_Error(xbmkeynames error_key);

void Read_Xbm_Header(char qv);
/* qv == Query Verbose flag */

void Read_Xpm_Header(char qv);
/* qv == Query Verbose flag */

void Read_Xpm_Header_Data(char qv);
/* qv == Query Verbose flag */

xbmkeynames Check_Xpm_Header(void);

xbmkeynames Check_Xpm_Header_Type(xbmkeynames this_type);

unsigned char ReverseXbmByte(unsigned char xbmbyte);

int Read_Xbm_Pixel8_Data(qc12864b_pixel8 *buffer,
			 char qv);
/* qv == Query Verbose flag */

int Read_Xpm_Udgc_Data(qc12864b_udgc *buffer,
		       unsigned char cgram_addr,			
		       char qv);
/* qv == Query Verbose flag */

int Read_Xpm_Colpix8_Data(pcd8544_colpix8 *buffer,
			  char qv);
/* qv == Query Verbose flag */

void clear_pixel_store(qc12864b_pixel8 *buffer);

void fill_pixel_store(qc12864b_pixel8 *buffer);

void init_pixel_store(qc12864b_pixel8 *buffer);

void echo_pixel_store_status(qc12864b_pixel8 *buffer);

void xbm_dump_pixel_store(qc12864b_pixel8 *buffer);

void xpm_dump_pixel_store(qc12864b_pixel8 *buffer);

void preset_cgram_store(qc12864b_udgc *buffer,
			unsigned char cgram_addr,			
			unsigned char allrows_hbyte,
			unsigned char allrows_lbyte);

void clear_colpix_store(pcd8544_colpix8 *buffer);

void fill_colpix_store(pcd8544_colpix8 *buffer);

void init_colpix_store(pcd8544_colpix8 *buffer);

void echo_colpix_store_status(pcd8544_colpix8 *buffer);

#endif /* !GLCD_XBMTOOLS_H */

/* Change Log */
/*
 * $Log: glcd_xbmtools.h,v $
 * Revision 1.3  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.2  2017/08/31 15:50:59  pi
 * Minor bug-fix (another buffer added)
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.16  2015/01/16 23:28:17  pi
 * Interim commit
 *
 * Revision 1.15  2015/01/05 12:20:21  pi
 * Interim commit
 *
 * Revision 1.14  2015/01/03 22:56:48  pi
 * Interim commit
 *
 * Revision 1.13  2014/10/03 23:00:23  pi
 * Some XPM support for UDGC's
 *
 * Revision 1.12  2014/10/01 20:26:27  pi
 * Initial CGRAM character handling
 *
 * Revision 1.11  2014/09/28 16:32:25  pi
 * Some XPM support added
 *
 * Revision 1.10  2014/09/21 21:30:01  pi
 * Improved xbm data conversion
 *
 * Revision 1.9  2014/09/20 20:03:16  pi
 * Interim commit
 *
 * Revision 1.8  2014/09/15 22:15:27  pi
 * Interim commit
 *
 * Revision 1.7  2014/09/15 18:44:01  pi
 * Interim commit
 *
 * Revision 1.6  2014/09/14 21:02:21  pi
 * Interim commit
 *
 * Revision 1.5  2014/09/10 22:39:59  pi
 * Interim commit
 *
 * Revision 1.4  2014/09/09 18:01:57  pi
 * Interim commit
 *
 * Revision 1.3  2014/09/08 21:50:20  pi
 * Interim commit
 *
 * Revision 1.2  2014/09/07 20:48:27  pi
 * Interim commit
 *
 * Revision 1.1  2014/09/07 20:03:14  pi
 * Initial revision
 *
 *
 *
 */
