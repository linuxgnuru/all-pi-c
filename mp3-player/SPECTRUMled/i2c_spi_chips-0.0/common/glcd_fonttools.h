/* Header file associated with GLCD FONT header file handling TOOLS structures etc.
 * $Id: glcd_fonttools.h,v 1.2 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef GLCD_FONTTOOLS_H
#define GLCD_FONTTOOLS_H 1

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

/* *font.h file parser ... */

#define NOTYETFONT      -1 /* order NOT YET known or defined */
#define CLONEFONT        0 /* CLONE input FONT order */      
#define ROWCOLFONT       1 /* Height, Width */
#define COLROWFONT       2 /* Width, Height */
#define MAX_FONT_HEIGHT 16
#define MAX_FONT_WIDTH  16

typedef enum
{
  CCOMMENT,    /* /*         */
  STATICKEY,   /* static     */
  CONSTKEY,    /* const      */
  UNSIGNEDKEY, /* unsigned   */
  INTKEY,      /* int        */
  CHARKEY,     /* char       */
  CHRBRKSKEY,  /* []         */
  LBRACEKEY,   /* {          */
  RBRACESCKEY, /* };         */
  KEYDATA,     /* ***DATA*** */
  HEIGHTDKEY,  /* Height     */
  WIDTHDKEY,   /* Width      */
  LAST_FONTKEY,
  FONTKEYTYPES
} fontkeynames;


/* Ensure globals only declared once ... */
/* Relies on NO_MAIN being defined in libraries */
#ifndef NO_MAIN
/* Useful strings ... */
char fontblank[] = " ";
char fonterror[] = "***ERROR*** ";
char fontwarng[] = "**WARNING** "; 
fontkeynames fontwhat,fontwhich,qfontkey;
char *fontkeylist[FONTKEYTYPES];
char fontInfile[FN_LEN],fontOutfile[FN_LEN];
char fontInfopen,fontOutfopen;
char fontfread;
char FontInError;
int  FontInHeight;
int  FontInOrder;
int  FontInWidth;
int  FontOutOrder;
FILE *fontInfn,*fontOutfn;
#else /* !NO_MAIN */
extern char fontblank[];
extern char fonterror[];
extern char fontwarng[]; 
extern fontkeynames fontwhat,fontwhich,qfontkey;
extern char *fontkeylist[FONTKEYTYPES];
extern char fontInfile[FN_LEN],fontOutfile[FN_LEN];
extern char fontInfopen,fontOutfopen;
extern char fontfread;
extern char FontInError;
extern int  FontInHeight;
extern int  FontInOrder;
extern int  FontInWidth;
extern int  FontOutOrder;
extern FILE *fontInfn,*fontOutfn;
#endif /* !NO_MAIN */

/* Primitives */

void setupfontkeys(void);

fontkeynames whichfontkey(char *fontkeystr);

void listfontkeys(void);

void Open_Font_In_File(void);

void Close_Font_In_File(void);

void Font_In_Error(fontkeynames error_key);

void Open_Font_Out_File(void);

void Close_Font_Out_File(void);

void Clone_Font_Header_Comments(char qv);
/* qv == Query Verbose flag */

char *get_stem_no_path(char filename[]);

void Regen_Font_HW(char qv);
/* Regen(erate) Font H(eight) & W(idth) */
/* qv == Query Verbose flag */

void RegenFontDefnHdr(char qv);
/* Regen(erate) Font Def(initio)n H(ea)d(e)r */
/* qv == Query Verbose flag */

void RegenLBrace(void);
/* Regen(erate) L(eft) Brace */

fontkeynames QueryCloneCharBlockComment(char qv);
/* QueryClone Char(acter) Block Comment ...
 * Returns fontkeyname of first non-blank line
 * qv == Query Verbose flag
 */

void LoadCharBlockData(unsigned int pixbyte[],
		       int index,
		       char qv);
/* Load Char(acter) Block Data ...
 * qv == Query Verbose flag
 */

unsigned int getPixBit(unsigned int pixbyte,
		       unsigned int mask);

void WriteCharBlockData(unsigned int pixbyte[],
			int index);

void Append_Empty_Change_Log(char qv);
/* qv == Query Verbose flag */

#endif /* !GLCD_FONTTOOLS_H */

/* Change Log */
/*
 * $Log: glcd_fonttools.h,v $
 * Revision 1.2  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.15  2015/05/19 17:23:20  pi
 * Interim commit
 *
 * Revision 1.14  2015/05/19 14:51:57  pi
 * Interim commit
 *
 * Revision 1.13  2015/05/14 19:36:28  pi
 * Interim commit
 *
 * Revision 1.12  2015/05/12 21:49:26  pi
 * Interim commit
 *
 * Revision 1.11  2015/05/11 18:09:26  pi
 * Interim commit
 *
 * Revision 1.10  2015/05/08 19:50:25  pi
 * Interim commit
 *
 * Revision 1.9  2015/05/04 20:34:26  pi
 * Interim commit
 *
 * Revision 1.8  2015/05/04 16:41:13  pi
 * Interim commit
 *
 * Revision 1.7  2015/05/03 18:17:07  pi
 * Interim commit
 *
 * Revision 1.6  2015/05/03 17:22:29  pi
 * Test commit
 *
 * Revision 1.5  2015/05/03 17:20:19  pi
 * Test commit
 *
 * Revision 1.4  2015/05/03 17:14:14  pi
 * Test commit
 *
 * Revision 1.3  2015/05/03 15:37:01  pi
 * Interim commit
 *
 * Revision 1.2  2015/05/02 21:36:15  pi
 * Interim commit
 *
 * Revision 1.1  2015/05/02 20:20:53  pi
 * Initial revision
 *
 *
 *
 */
