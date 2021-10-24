/* C source file associated with GLCD FONT header file handling TOOLS structures etc.
 * $Id: glcd_fonttools.c,v 1.2 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "glcd_fonttools.h"

void setupfontkeys(void)
{
  fontkeylist[CCOMMENT]     = "/*";
  fontkeylist[STATICKEY]    = "static";
  fontkeylist[CONSTKEY]     = "const";
  fontkeylist[UNSIGNEDKEY]  = "unsigned";
  fontkeylist[INTKEY]       = "int";
  fontkeylist[CHARKEY]      = "char";
  fontkeylist[CHRBRKSKEY]   = "[]";
  fontkeylist[LBRACEKEY]    = "{";
  fontkeylist[RBRACESCKEY]  = "};";
  fontkeylist[KEYDATA]      = "***DATA***";
  fontkeylist[HEIGHTDKEY]   = "Height";
  fontkeylist[WIDTHDKEY]    = "Width";
  fontkeylist[LAST_FONTKEY] = "";  
}

fontkeynames whichfontkey(char *fontkeystr)
{
  fontkeynames current;
  char flag;
  char *p1,*p2;
  flag = FALSE;
  for (current=CCOMMENT; current<LAST_FONTKEY; current++)
    {
      p1 = fontkeystr;
      p2 = fontkeylist[current];
      if (*p2 == NUL) break;
      while ((*p2 == *p1) && (*p1!=NUL))	
	{
	  p1++;
	  p2++;
	}
    if ((*p1==NUL) && (*p2==NUL))
	{
	  flag = TRUE;
	  break;
	}
  }
if (flag) return(current);
else return(LAST_FONTKEY);
}

void listfontkeys(void)
{
  for (fontwhat=CCOMMENT; fontwhat<LAST_FONTKEY; fontwhat++)
    printf("fontkeylist[%2d] = %s\n", fontwhat, fontkeylist[fontwhat]);
}

void Open_Font_In_File(void)
{
  if ((fontInfn=fopen(fontInfile, "r")) == NULL)
    {
      fprintf(stderr, "<%s>\t", fontInfile);
      perror("fopen");
      printf("%s not found\n", fontInfile);
      fontInfopen = FALSE;
    }
  else 
    {
      printf("Opened %s ...\n", fontInfile);
      fontInfopen = TRUE;
    }
}

void Close_Font_In_File(void)
{
  fontInfopen = FALSE;
  fclose(fontInfn);
}

void Font_In_Error(fontkeynames error_key)
{
  switch (error_key)
    {
    default:
      {
	fprintf(stderr,
		"%s Problem with %s line!\n",
		fonterror,
		fontkeylist[error_key]);
	report_stats(fontInfile);
	break;
      }
    }
  FontInError = TRUE;
}

void Open_Font_Out_File(void)
{
  if ((fontOutfn=fopen(fontOutfile, "w")) == NULL)
    {
      fprintf(stderr, "<%s>\t", fontOutfile);
      perror("fopen");
      printf("%s not created\n", fontOutfile);
      fontOutfopen = FALSE;
    }
  else 
    {
      printf("Opened %s ...\n", fontOutfile);
      fontOutfopen = TRUE;
    }
}

void Close_Font_Out_File(void)
{
  fontOutfopen = FALSE;
  fclose(fontOutfn);
}

void Clone_Font_Header_Comments(char qv)
/* qv == Query Verbose flag */
{  
  char qcc,qvh;
  int l,m;
  if ((fontOutfopen == FALSE) && (fontOutfn != stdout)) return;
  qvh = (qv && (fontOutfn != stdout));
  do
    {
      l = fetch(buffer2, fontInfn);
      strcpy(buffer4, buffer2);
      m = parse_spt(buffer3, buffer4);
      if (strcmp(buffer3, fontkeylist[CCOMMENT]) != 0) qcc = FALSE;
      else 
	{
	  qcc = TRUE;
	  fprintf(fontOutfn, "%s\n", buffer2);
	  if (qvh) printf("%s\n", buffer2);
	}
      if (!qcc)
	{
	  if ((buffer3[0] == '/') && (buffer3[1] == '*'))
	    {
	      qcc = TRUE;
	      fprintf(fontOutfn, "%s\n", buffer2);	      
	      if (qvh) printf("%s\n", buffer2);
	    }
	}
    }
  while (qcc);
  fprintf(fontOutfn, "\n");
  if (qv) report_stats(fontInfile);
}

char *get_stem_no_path(char filename[])
{
  static char stem_no_path[FN_LEN];
  char *p1,*p2,*p3;
  p1 = get_stem(filename);
  p2 = stem_no_path;
  p3 = p1;
  while (*p1 != NUL) p1++;
  while ((*p1!= '/') && (p1!=p3)) p1--;
  if (*p1 == '/') p1++;
  strcpy(stem_no_path, p1);
  return(stem_no_path);
}

void Regen_Font_HW(char qv)
/* Regen(erate) Font H(eight) & W(idth) */
/* qv == Query Verbose flag */
{
  char *p1,*p2;
  char flag,qcc,qvh;
  int l,m,n;
  if ((fontOutfopen == FALSE) && (fontOutfn != stdout)) return;
  qvh = (qv && (fontOutfn != stdout));
  FontInError = FALSE;
  /* Skip empty or too short lines */
  do
    {
      l = fetch(buffer2, fontInfn);
      /* static has 6 chars! */
      if (qvh) printf("%s\n", buffer2);
    }
  while (l < 6);
  m = parse_spt(buffer3, buffer2);
  if (qvh) printf("m=%d, %s\n", m, buffer3);
  if (m < 6) Font_In_Error(STATICKEY);
  else
    {
      if (strcmp(buffer3, fontkeylist[STATICKEY]) != 0)
	Font_In_Error(STATICKEY);
      else
	{
	  l = parse_spt(buffer3, buffer2);
	  if (l != 5) Font_In_Error(CONSTKEY);
	  else
	    {
	      if (strcmp(buffer3, fontkeylist[CONSTKEY]) != 0)
		Font_In_Error(CONSTKEY);
	      else
		{
		  m = parse_spt(buffer3, buffer2);
		  if (m != 3) Font_In_Error(INTKEY);
		  else
		    {
		      if (strcmp(buffer3, fontkeylist[INTKEY]) != 0) 
			Font_In_Error(INTKEY);
		      else
			{
			  l = parse_spt(buffer3, buffer2);
			  if (l < 9) Font_In_Error(KEYDATA);
			  /* "fontWidth" has 9 chars. */
			  else
			    {
			      p1 = get_stem_no_path(fontInfile);
			      n = strlen(p1);
			      if (qvh) printf("n=%d, %s\n", n, p1);
			      p2 = buffer3;
			      while (n > 0) 
				{
				  *p2++ = ' ';
				  n--;
				}
			      m = parse_spt(buffer4, buffer3);
			      if (qvh)
				printf("m=%d, \"%s\"\n    \"%s\"\n", m, buffer3, buffer4);
			      if (m < 5) Font_In_Error(KEYDATA);
			      if ((strcmp(buffer4, fontkeylist[HEIGHTDKEY]) != 0) &&
				  (strcmp(buffer4, fontkeylist[WIDTHDKEY]) != 0))
				Font_In_Error(KEYDATA);
			      p1 = get_stem_no_path(fontOutfile);
			      if (strcmp(buffer4, fontkeylist[HEIGHTDKEY]) == 0)
				{
				  // fprintf(fontOutfn, "static const int %sHeight = ", p1);
				  n = parse_spt(buffer3, buffer2);
				  if (n != 1) Font_In_Error(KEYDATA);
				  if (buffer3[0] != '=') Font_In_Error(KEYDATA);
				  n = parse_spt(buffer3, buffer2);
				  if (n < 1)  Font_In_Error(KEYDATA);
				  FontInHeight = atoi(buffer3);
				  // fprintf(fontOutfn, "%d;\n", FontInHeight);
				  if (FontInOrder == NOTYETFONT) FontInOrder = ROWCOLFONT;
				}
			      if (strcmp(buffer4, fontkeylist[WIDTHDKEY]) == 0)
				{
				  // fprintf(fontOutfn, "static const int %sWidth  = ", p1);
				  n = parse_spt(buffer3, buffer2);
				  if (n != 1) Font_In_Error(KEYDATA);
				  if (buffer3[0] != '=') Font_In_Error(KEYDATA);
				  n = parse_spt(buffer3, buffer2);
				  if (n < 1)  Font_In_Error(KEYDATA);
				  FontInWidth = atoi(buffer3);
				  // fprintf(fontOutfn, "%d;\n", FontInWidth);
				  if (FontInOrder == NOTYETFONT) FontInOrder = COLROWFONT;
				}			      
			    }
			}
		    }
		}
	    }
	}
    }
}

void RegenFontDefnHdr(char qv)
/* Regen(erate) Font Def(initio)n H(ea)d(e)r */
/* qv == Query Verbose flag */
{
  char *p1,*p2;
  char qvh;
  int l,m,n;
  if ((fontOutfopen == FALSE) && (fontOutfn != stdout)) return;
  qvh = (qv && (fontOutfn != stdout));
  FontInError = FALSE;
  /* Skip empty or too short lines */
  do
    {
      l = fetch(buffer2, fontInfn);
      /* static has 6 chars! */
      if (qvh) printf("%s\n", buffer2);
    }
  while (l < 6);
  m = parse_spt(buffer3, buffer2);
  if (qvh) printf("m=%d, %s\n", m, buffer3);
  if (m < 6) Font_In_Error(STATICKEY);
  else
    {
      if (strcmp(buffer3, fontkeylist[STATICKEY]) != 0)
	Font_In_Error(STATICKEY);
      else
	{
	  l = parse_spt(buffer3, buffer2);
	  if (l != 8) Font_In_Error(UNSIGNEDKEY);
	  else
	    {
	      if (strcmp(buffer3, fontkeylist[UNSIGNEDKEY]) != 0)
		Font_In_Error(UNSIGNEDKEY);
	      else
		{
		  m = parse_spt(buffer3, buffer2);
		  if (m != 4) Font_In_Error(CHARKEY);
		  else
		    {
		      if (strcmp(buffer3, fontkeylist[CHARKEY]) != 0)
			Font_In_Error(CHARKEY);
		      else
			{
			  p1 = get_stem_no_path(fontInfile);
			  n = strlen(p1);
			  if (qvh) printf("n=%d, %s\n", n, p1);
			  l = parse_spt(buffer3, buffer2);
			  if (l < n) Font_In_Error(KEYDATA);
			  else
			    {
			      if (strcmp(buffer3, p1) != 0) Font_In_Error(KEYDATA);
			      else
				{
				  m = parse_spt(buffer3, buffer2);
				  if (m != 2) Font_In_Error(CHRBRKSKEY);
				  else
				    {
				      if (strcmp(buffer3, fontkeylist[CHRBRKSKEY]) != 0)
					Font_In_Error(CHRBRKSKEY);
				      else
					{
					  l = parse_spt(buffer3, buffer2);
					  if (l != 1) Font_In_Error(CHRBRKSKEY);
					  else
					    {
					      if (buffer3[0] != '=')
						Font_In_Error(KEYDATA);
					      else
						{
						  p1 = get_stem_no_path(fontOutfile);
						  fprintf(fontOutfn, 
							  "\nstatic unsigned char %s",
							  p1);
						  fprintf(fontOutfn, 
							  " %s =\n",
							  fontkeylist[CHRBRKSKEY]);
						}
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
}

void RegenLBrace(void)
/* Regen(erate) L(eft) Brace */
{
  int l,m,n;
  if ((fontOutfopen == FALSE) && (fontOutfn != stdout)) return;
  FontInError = FALSE;
  /* Skip empty or too long lines */
  /* '{' may be indented 2 spaces */
  do
    {
      l = fetch(buffer2, fontInfn);
      m = parse_spt(buffer3, buffer2);
    }
  while (m != 1);
  if (buffer3[0] != '{') Font_In_Error(LBRACEKEY);
  else fprintf(fontOutfn, "%s\n", fontkeylist[LBRACEKEY]);
}

fontkeynames QueryCloneCharBlockComment(char qv)
/* QueryClone Char(acter) Block Comment ...
 * Returns fontkeyname of first non-blank line
 * qv == Query Verbose flag
 */
{
  char *p1,*p2;
  char qvh;
  static char lbuf[BUF_LEN];
  int l,m,n;
  if ((fontOutfopen == FALSE) && (fontOutfn != stdout)) return(LAST_FONTKEY);
  qvh = (qv && (fontOutfn != stdout));
  FontInError = FALSE;
  /* Skip empty or too short lines */
  do
    {
      l = fetch(buffer2, fontInfn);
      /* "/*" or "};" have 2 chars! */
    }
  while (l < 2);
  strcpy(lbuf, buffer2);
  m = parse_spt(buffer3, buffer2);
  if (strcmp(buffer3, fontkeylist[CCOMMENT]) == 0)
    {
      fprintf(fontOutfn, "%s\n", lbuf);
      if (qvh) printf("\t%s\n", lbuf);
      return(CCOMMENT);
    }
  if (strcmp(buffer3, fontkeylist[RBRACESCKEY]) == 0)
    {
      if (qvh) printf("%s\n", lbuf);
      return(RBRACESCKEY);
    }
  return(LAST_FONTKEY);
}

void LoadCharBlockData(unsigned int pixbyte[],
		       int index,
		       char qv)
/* Load Char(acter) Block Data ...
 * qv == Query Verbose flag
 */
{
  char *p1;
  char qvh;
  int l,m,n;
  unsigned long lpixbyte;
  static char lbuf[BUF_LEN];
  if (index > FontInHeight) return;
  qvh = (qv && (fontOutfn != stdout));
  l = fetch(buffer2, fontInfn);     
  m = parse_spt(buffer3, buffer2);
  if (qvh) printf("%s%s --> ", buffer3, buffer2);
  make_comma_white(buffer3);
  n = parse_spt(lbuf, buffer3);
  if (n == 4)
    {
      lpixbyte = strtoul(lbuf, NULL, 16);
      pixbyte[index] = (unsigned int)(lpixbyte);
    }
  else pixbyte[index] = 0xff;
  if (qvh) printf("0x%02x\n", pixbyte[index]);
}

unsigned int getPixBit(unsigned int pixbyte,
		       unsigned int mask)
{
  unsigned int lbit;
  lbit = pixbyte & mask;
  if (lbit != 0) lbit = 1;
  return(lbit);
}

void WriteCharBlockData(unsigned int pixbyte[],
			int index)
{
  int l;
  unsigned int lbit,mask;
  if ((fontOutfopen == FALSE) && (fontOutfn != stdout)) return;
  fprintf(fontOutfn, "   0x%02x, /* ", pixbyte[index]);
  mask = 0x80; /* MSB */
  for (l=0; l<8; l++)
    {
      /*
      lbit = pixbyte[index] & mask;
      if (lbit != 0) lbit = 1;
      */
      lbit = getPixBit(pixbyte[index], mask);
      fprintf(fontOutfn, "%1d", lbit);
      mask = mask >> 1;
    }
  fprintf(fontOutfn, " */\n");  
}

void Append_Empty_Change_Log(char qv)
/* qv == Query Verbose flag */
{
  static char lbuf[] = "\n/* Change Log */\n/*\n * \x024Log\x024\n *\n *\n *\n */\n";
  if ((fontOutfopen == FALSE) && (fontOutfn != stdout)) return;
  fprintf(fontOutfn, lbuf);
  if (qv && (fontOutfn != stdout))
    printf(lbuf);
}

/* Change Log */
/*
 * $Log: glcd_fonttools.c,v $
 * Revision 1.2  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.17  2015/05/19 17:23:20  pi
 * Interim commit
 *
 * Revision 1.16  2015/05/19 14:51:57  pi
 * Interim commit
 *
 * Revision 1.15  2015/05/14 19:36:28  pi
 * Interim commit
 *
 * Revision 1.14  2015/05/12 21:49:26  pi
 * Interim commit
 *
 * Revision 1.13  2015/05/11 18:09:26  pi
 * Interim commit
 *
 * Revision 1.12  2015/05/11 13:52:16  pi
 * Interim commit
 *
 * Revision 1.11  2015/05/08 19:50:25  pi
 * Interim commit
 *
 * Revision 1.10  2015/05/04 20:34:26  pi
 * Interim commit
 *
 * Revision 1.9  2015/05/04 19:33:47  pi
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


