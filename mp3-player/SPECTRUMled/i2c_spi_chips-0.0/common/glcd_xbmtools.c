/* C source file associated with GLCD XBM file handling TOOLS structures etc.
 * (+ some limited XPM file support)
 * $Id: glcd_xbmtools.c,v 1.3 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "glcd_xbmtools.h"

void setupxbmkeys(void)
{
  xbmkeylist[CCOMMENT]    = "/*";
  xbmkeylist[HASHDEFKEY]  = "#define";
  xbmkeylist[STATICKEY]   = "static";
  xbmkeylist[UNSIGNEDKEY] = "unsigned";
  xbmkeylist[CHARKEY]     = "char";
  xbmkeylist[KEYDATA]     = "***DATA***";
  /* Extra XPM keys ...    */
  xbmkeylist[XPMKEY]      = "XPM";
  xbmkeylist[CLRKEY]      = "c";
  /* XPM file "support codes" ... */
  xbmkeylist[XPMCOLPIX8]  = "colpix8";
  xbmkeylist[XPMPIXEL8]   = "Pixel8";
  xbmkeylist[XPMUDGC]     = "UDGC";
  xbmkeylist[XPMNIU]      = "NotInUse";
  xbmkeylist[LAST_XBMKEY] = "";  
}

xbmkeynames whichxbmkey(char *xbmkeystr)
{
  xbmkeynames current;
  char flag;
  char *p1,*p2;
  flag = FALSE;
  for (current=CCOMMENT; current<LAST_XBMKEY; current++)
    {
      p1 = xbmkeystr;
      p2 = xbmkeylist[current];
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
else return(LAST_XBMKEY);
}

void listxbmkeys(void)
{
  for (xbmwhat=CCOMMENT; xbmwhat<LAST_XBMKEY; xbmwhat++)
    printf("xbmkeylist[%2d] = %s\n", xbmwhat, xbmkeylist[xbmwhat]);
}

void Open_Xbm_File(void)
{
  if ((xbmfn=fopen(xbmfile, "r")) == NULL)
    {
      fprintf(stderr, "<%s>\t", xbmfile);
      perror("fopen");
      printf("%s not found\n", xbmfile);
      xbmfopen = FALSE;
    }
  else 
    {
      printf("Opened %s ...\n", xbmfile);
      xbmfopen = TRUE;
    }
}

void Open_Xpm_File(char *mode)
{
  if ((xpmfn=fopen(xpmfile, mode)) == NULL)
    {
      fprintf(stderr, "<%s>\t", xpmfile);
      perror("fopen");
      if (mode[0] == 'r') printf("%s not found\n", xpmfile);
      if (mode[0] == 'w') printf("%s not created\n", xpmfile);
      xpmfopen = FALSE;
    }
  else 
    {
      printf("Opened %s ...\n", xpmfile);
      xpmfopen = TRUE;
    }
}

void Close_Xbm_File(void)
{
  xbmfopen = FALSE;
  fclose(xbmfn);
}

void Close_Xpm_File(void)
{
  xpmfopen = FALSE;
  fclose(xpmfn);
}

void Xbm_Error(xbmkeynames error_key)
{
  switch (error_key)
    {
    default:
      {
	fprintf(stderr,
		"%s Problem with %s line!\n",
		xbmerror,
		xbmkeylist[error_key]);
	report_stats(xbmfile);
	break;
      }
    }
  XbmError = TRUE;
}

void Xpm_Error(xbmkeynames error_key)
{
  switch (error_key)
    {
    default:
      {
	fprintf(stderr,
		"%s Problem with %s line!\n",
		xbmerror,
		xbmkeylist[error_key]);
	report_stats(xpmfile);
	break;
      }
    }
  XpmError = TRUE;
}

void Read_Xbm_Header(char qv)
/* qv == Query Verbose flag */
{
  char qcc;
  int l,m,n1,n2;
  if (xbmfopen == FALSE) return;
  XbmError = FALSE;
  /* Check for, and ignore any 'C' comment lines (all begiining with /* !) ... */
  do
    {
      l = fetch(buffer2, xbmfn);
      strcpy(buffer4, buffer2);
      m = parse_spt(buffer3, buffer4);
      if (strcmp(buffer3, xbmkeylist[CCOMMENT]) != 0) qcc = FALSE;
      else 
	{
	  qcc = TRUE;
	  if (qv) printf("%s\n", buffer2);
	}
    }
  while (qcc);
  /* xbm width */
   if (l > 7)
    {
      /* #define has 7 chars! */
      m = parse_spt(buffer3, buffer2);
      if (m < 7) Xbm_Error(HASHDEFKEY);
      else
	{
	  if (strcmp(buffer3, xbmkeylist[HASHDEFKEY]) != 0) Xbm_Error(HASHDEFKEY);
	  else
	    {
	      n1 = parse_spt(buffer3, buffer2);
	      if (n1 < 5) Xbm_Error(KEYDATA);
	      else
		{
		  strcpy(this_xbm_header.wname, buffer3);
		  if (qv) printf("xbm_header.wname  = %s\n", this_xbm_header.wname);
		  n2 = parse_spt(buffer3, buffer2);
		  if (n2 < 1) Xbm_Error(KEYDATA);
		  else
		    {
		      this_xbm_header.width = atoi(buffer3);
		      if (qv) printf("xbm_header.width  = %d\n", this_xbm_header.width);
		    }
		}
	    }
	}
    }
  else Xbm_Error(HASHDEFKEY);
  /* xbm height */
  l = fetch(buffer2, xbmfn);
  if (l > 7)
    {
      /* #define has 7 chars! */
      m = parse_spt(buffer3, buffer2);
      if (m < 7) Xbm_Error(HASHDEFKEY);
      else
	{
	  if (strcmp(buffer3, xbmkeylist[HASHDEFKEY]) != 0) Xbm_Error(HASHDEFKEY);
	  else
	    {
	      n1 = parse_spt(buffer3, buffer2);
	      if (n1 < 6) Xbm_Error(KEYDATA);
	      else
		{
		  strcpy(this_xbm_header.hname, buffer3);
		  if (qv) printf("xbm_header.hname  = %s\n", this_xbm_header.hname);
		  n2 = parse_spt(buffer3, buffer2);
		  if (n2 < 1) Xbm_Error(KEYDATA);
		  else
		    {
		      this_xbm_header.height = atoi(buffer3);
		      if (qv) printf("xbm_header.height = %d\n", this_xbm_header.height);
		    }
		}
	    }
	}
    }
  else Xbm_Error(HASHDEFKEY);
  /* xbm bits */
  l = fetch(buffer2, xbmfn);
  if (l > 6)
    {
      /* static has 6 chars! */
      m = parse_spt(buffer3, buffer2);
      if (m < 6) Xbm_Error(STATICKEY);
      else
	{
	  if (strcmp(buffer3, xbmkeylist[STATICKEY]) != 0) Xbm_Error(STATICKEY);
	  else
	    {
	      l = parse_spt(buffer3, buffer2);
	      if (l != 8) Xbm_Error(UNSIGNEDKEY);
	      else
		{
		  if (strcmp(buffer3, xbmkeylist[UNSIGNEDKEY]) != 0) Xbm_Error(UNSIGNEDKEY);
		  else
		    {
		      m = parse_spt(buffer3, buffer2);
		      if (m != 4) Xbm_Error(CHARKEY);
		      else
			{
			  if (strcmp(buffer3, xbmkeylist[CHARKEY]) != 0) 
			    Xbm_Error(CHARKEY);
			  else
			    {
			      n1 = parse_spt(buffer3, buffer2);
			      if (n1 < 6) Xbm_Error(KEYDATA);
			      else
				{
				  strcpy(this_xbm_header.bname, buffer3);
				  if (qv)
				    printf("xbm_header.bname  = %s\n",
					   this_xbm_header.bname);
				}
			    }
			}
		    }
		}
	    }
	}
    }
  else Xbm_Error(STATICKEY);
  if (qv) report_stats(xbmfile);
}

void Read_Xpm_Header(char qv)
/* qv == Query Verbose flag */
{
  int l,m,n;
  if (xpmfopen == FALSE) return;
  XpmError = FALSE;
  /* Check for 'C' commented XPM marker on first line */
  l = fetch(buffer2, xpmfn);
  strcpy(buffer4, buffer2);
  m = parse_spt(buffer3, buffer4);
  if (strcmp(buffer3, xbmkeylist[CCOMMENT]) != 0) Xpm_Error(CCOMMENT);
  else
    {
      m = parse_spt(buffer3, buffer4);
      if (strcmp(buffer3, xbmkeylist[XPMKEY]) != 0) Xpm_Error(XPMKEY);
      else
	{
	  /* xpm array name */
	  l = fetch(buffer2, xpmfn);
	  if (l > 6)
	    {
	      /* static has 6 chars! */
	      m = parse_spt(buffer3, buffer2);
	      if (m < 6) Xpm_Error(STATICKEY);
	      else
		{
		  if (strcmp(buffer3, xbmkeylist[STATICKEY]) != 0) Xpm_Error(STATICKEY);
		  else
		    {
		      m = parse_spt(buffer3, buffer2);
		      if (m != 4) Xpm_Error(CHARKEY);
		      else
			{
			  if (strcmp(buffer3, xbmkeylist[CHARKEY]) != 0) 
			    Xpm_Error(CHARKEY);
			  else
			    {
			      n = parse_spt(buffer3, buffer2);
			      if (n < 3) Xpm_Error(KEYDATA);
			      else
				{
				  strcpy(this_xpm_header.cname, buffer3);
				  if (qv)
				    printf("xpm_header.cname = %s\n",
					   this_xpm_header.cname);
				}
			    }
			}
		    }
		}
	    } 
	  else Xpm_Error(STATICKEY);
	}
    }
  if (qv) report_stats(xpmfile);
}

void Read_Xpm_Header_Data(char qv)
/* qv == Query Verbose flag */
{
  int l,m;
  if (xpmfopen == FALSE) return;
  XpmError = FALSE;
  /* Block 1 ... */
  l = fetch(buffer2, xpmfn);
  if (l < 12) 
    {
      Xpm_Error(KEYDATA);
      printf(">>>|%s|<<<\n", buffer2);
    }
  else
    {
      strcpy(buffer4, buffer2);
      remove_dquot(buffer4);
      m = parse_spt(buffer3, buffer4);
      this_xpm_header.width = atoi(buffer3);
      m = parse_spt(buffer3, buffer4);
      this_xpm_header.height = atoi(buffer3);
      m = parse_spt(buffer3, buffer4);
      this_xpm_header.colours = atoi(buffer3);
      m = parse_spt(buffer3, buffer4);
      this_xpm_header.charperpixel = atoi(buffer3);
    }
  if (qv)
    {
      report_stats(xpmfile);
      printf("this_xpm_header.width        = %2d\n", this_xpm_header.width);
      printf("this_xpm_header.height       = %2d\n", this_xpm_header.height);
      printf("this_xpm_header.colours      = %2d\n", this_xpm_header.colours);
      printf("this_xpm_header.charperpixel = %2d\n", this_xpm_header.charperpixel);
    }
  /* Block 2 ... */
  l = fetch(buffer2, xpmfn);
  if (l < 14) 
    {
      Xpm_Error(KEYDATA);
      printf(">>>|%s|<<<\n", buffer2);
    }
  else
    {
      strcpy(buffer4, buffer2);
      if (buffer4[1] == ' ')
	{
	  Xpm_Error(KEYDATA);
	  printf(">>>|%s|<<<\n", buffer2);
	}
      else
	{
	  remove_dquot(buffer4);
	  m = parse_spt(buffer3, buffer4);
	  this_xpm_header.back = buffer3[0];
	}
    }
  if (qv)
    {
      report_stats(xpmfile);
    }
  /* Block 3 ... */
  l = fetch(buffer2, xpmfn);
  if (l < 14) 
    {
      Xpm_Error(KEYDATA);
      printf(">>>|%s|<<<\n", buffer2);
    }
  else
    {
      strcpy(buffer4, buffer2);
      if (buffer4[1] == ' ')
	{
	  Xpm_Error(KEYDATA);
	  printf(">>>|%s|<<<\n", buffer2);
	}
      else
	{
	  remove_dquot(buffer4);
	  m = parse_spt(buffer3, buffer4);
	  this_xpm_header.fore = buffer3[0];
	}
    }
  if (qv)
    {
      report_stats(xpmfile);
      printf("this_xpm_header.back|fore chars = %c|%c\n",
	     this_xpm_header.back,
	     this_xpm_header.fore);
    }
}

xbmkeynames Check_Xpm_Header(void)
{
  char flag;
  xbmkeynames thisxpmtype;
  flag = TRUE;
  if (this_xpm_header.width != QC12864B_udgcbits) flag = FALSE;
  if (this_xpm_header.height != QC12864B_udgcbits) flag = FALSE;
  if (this_xpm_header.colours != 2) flag = FALSE;
  if (this_xpm_header.charperpixel != 1) flag = FALSE;
  if (flag) thisxpmtype = XPMUDGC;
  else thisxpmtype = XPMNIU;
  return(thisxpmtype);
}

xbmkeynames Check_Xpm_Header_Type(xbmkeynames this_type)
{
  char flag;
  xbmkeynames thisxpmtype;
  if (this_type == XPMUDGC) thisxpmtype = Check_Xpm_Header();
  else
    {
      flag = TRUE;
      if (this_xpm_header.width != PCD8544_column_width) flag = FALSE;
      if (this_xpm_header.height != PCD8544_row_height) flag = FALSE;
      if (this_xpm_header.colours != 2) flag = FALSE;
      if (this_xpm_header.charperpixel != 1) flag = FALSE;
      if (flag) thisxpmtype = XPMCOLPIX8;
      else thisxpmtype = XPMNIU;
    }
  return(thisxpmtype);
}

unsigned char ReverseXbmByte(unsigned char xbmbyte)
{
  unsigned char etybmbx;
  unsigned char binl,binr,lnibi,rnibi;
  rnibi   = (xbmbyte & 0xf0)/16;
  lnibi   = xbmbyte & 0x0f;
  binl    = right_nbrlut[lnibi];
  binr    = left_nbrlut[rnibi];
  etybmbx = binl + binr;
  return(etybmbx);
}

int Read_Xbm_Pixel8_Data(qc12864b_pixel8 *buffer,
			 char qv)
{
  unsigned char *ucp1,*ucp2;
  unsigned char lbyte;
  unsigned long int xbmbyte;
  int i,j,k,l,m,n;
  int npixel8;
  unsigned char xbm_buffer[QC12864B_xbmbytes];
  k = (QC12864B_width / 8) - 1;
  l = (QC12864B_height / 4);
  npixel8 = 0;
  if (xbmfopen == FALSE) return(npixel8);
  if (XbmError == TRUE)  return(npixel8);
  for (i=0; i<QC12864B_xbmbytes; i++) xbm_buffer[i] = 0x00;
  clear_pixel_store(buffer);
  /* Read into local buffer first */
  ucp1 = xbm_buffer;
  do
    {
      l = fetch(buffer2, xbmfn);
      if ((l < 6) && (l > 3)) Xbm_Error(KEYDATA);
      make_comma_white(buffer2);
      do
	{
	  m = parse_spt(buffer3, buffer2);
	  if (m == 4)
	    {
	      xbmbyte = strtoul(buffer3, NULL, 16);
	      *ucp1++ = (unsigned char)(xbmbyte);
	      npixel8++;
	      if (npixel8 == QC12864B_xbmbytes) break;
	    }
	}
      while (m > 3);
    }
  while((npixel8 < QC12864B_xbmbytes) && !feof(xbmfn));
  if (qv) 
    {
      report_stats(xbmfile);
      ucp1 = xbm_buffer;
      k = (QC12864B_width / 8) - 1;
      printf("xbm_buffer first eight row bytes ...\n");
      for (j=0; j<7; j++)
	{
	  for (i=0; i<k; i++)
	    {
	      printf("0x%02x ", *ucp1);
	      ucp1++;
	    }
	  printf("0x%02x\n", *ucp1);   
	  ucp1++;
	}
      for (i=0; i<k; i++)
	{
	  printf("0x%02x ", *ucp1);
	  ucp1++;
	}
      printf("0x%02x\n", *ucp1);   
    }
  /* Transfer data to pixel buffer */
  if (qv) printf("Transferring data to pixel buffer ...\n");
  ucp1 = xbm_buffer;
  k = (QC12864B_width / 8);
  l = (QC12864B_height / 4);
  if (qv) printf("... block11 ...\n");
  ucp2 = (*buffer).block11;
  /* for (j=0; j<l; j=j++) */
  for (j=0; j<l; j++)
   {
      for (i=0; i<k; i++)
	{
	  lbyte = *ucp1;
	  ucp1++;
	  *ucp2++ = ReverseXbmByte(lbyte);
	}
    }
  if (qv) printf("... block22 ...\n");
  ucp2 = (*buffer).block22;
  /* for (j=0; j<l; j=j++) */
  for (j=0; j<l; j++)
    {
      for (i=0; i<k; i++)
	{
	  lbyte = *ucp1;
	  ucp1++;
	  *ucp2++ = ReverseXbmByte(lbyte);
	}
    }
  if (qv) printf("... block13 ...\n");
  ucp2 = (*buffer).block13;
  /* for (j=0; j<l; j=j++) */
  for (j=0; j<l; j++)
    {
      for (i=0; i<k; i++)
	{
	  lbyte = *ucp1;
	  ucp1++;
	  *ucp2++ = ReverseXbmByte(lbyte);
	}
    }
  if (qv) printf("... block24 ...\n");
  ucp2 = (*buffer).block24;
  /* for (j=0; j<l; j=j++) */
  for (j=0; j<l; j++)
    {
      for (i=0; i<k; i++)
	{
	  lbyte = *ucp1;
	  ucp1++;
	  *ucp2++ = ReverseXbmByte(lbyte);
	}
    }
  if (qv) printf("... done! ...\n");
  return(npixel8);
}

int Read_Xpm_Udgc_Data(qc12864b_udgc *buffer,
		       unsigned char cgram_addr,			
		       char qv)
/* qv == Query Verbose flag */
{
  char *p1;
  char colour;
  unsigned char *ucp1,*ucp2,*ucp3;
  unsigned char hbyte,lbyte,jbit;
  int i,j;
  int npix,opix;
  unsigned char xpm_buffer[QC12864B_udgcpixc];
  npix = 0;
  if (xpmfopen == FALSE) return(npix);
  if (XpmError == TRUE)  return(npix);
  for (i=0; i<QC12864B_udgcpixc; i++) xpm_buffer[i] = 0;
  preset_cgram_store(buffer,
		     cgram_addr,
		     0x00,
		     0x00);
  /* Read into local buffer first */
  ucp1 = xpm_buffer;
  do
    {
      j = fetch(buffer2, xpmfn);
      strcpy(buffer4, buffer2);
      remove_dquot(buffer4);
      make_comma_white(buffer4);
      j = parse_spt(buffer3, buffer4);
      if (j != QC12864B_udgcbits)
	{
	  Xpm_Error(KEYDATA);
	  printf(">>>|%s|<<<\n", buffer4);
	}
      else
	{
	  p1 = buffer3;
	  for (i=0; i<QC12864B_udgcbits; i++)
	    {
	      colour = *p1++;
	      if (colour == this_xpm_header.back) *ucp1 = 0;
	      if (colour == this_xpm_header.fore) *ucp1 = 1;
	      ucp1++;
	      npix++;
	    }
	}
   }
  while((npix < QC12864B_udgcpixc) && !feof(xpmfn));
  if (qv) report_stats(xpmfile);
  /* Transfer data to pixel buffer */
  ucp1 = xpm_buffer;
  ucp2 = (*buffer).hbyte_rows;
  ucp3 = (*buffer).lbyte_rows;
  opix = 0;
  do
    {
      hbyte = *ucp1++;
      opix++;
      for (j=1; j<8; j++)
	{
	  jbit = *ucp1++;
	  hbyte = hbyte << 1;
	  hbyte = hbyte + jbit;
	  opix++;
	}
      *ucp2++ = hbyte;
      opix++;
      lbyte = *ucp1++;
      for (j=1; j<8; j++)
	{
	  jbit = *ucp1++;
	  lbyte = lbyte << 1;
	  lbyte = lbyte + jbit;
	  opix++;
	}
      *ucp3++ = lbyte;
    }
  while (opix < QC12864B_udgcpixc);
  return(npix);
}

int Read_Xpm_Colpix8_Data(pcd8544_colpix8 *buffer,
			  char qv)
/* qv == Query Verbose flag */
{
  char *p1;
  char colour;
  unsigned char *ucp1;
  unsigned char cbyte,jbit;
  int i,j,k;
  int npix,opix,rpix;
  unsigned char xpm_buffer[PCD8544_xpmpixc];
  npix = 0;
  if (xpmfopen == FALSE) return(npix);
  if (XpmError == TRUE)  return(npix);
  for (i=0; i<PCD8544_xpmpixc; i++) xpm_buffer[i] = 0;
  clear_colpix_store(buffer);
  /* Read into local buffer first */
  ucp1 = xpm_buffer; 
  do
    {
      j = fetch(buffer2, xpmfn);
      strcpy(buffer4, buffer2);
      remove_dquot(buffer4);
      make_comma_white(buffer4);
      j = parse_spt(buffer3, buffer4);
      if (j != PCD8544_column_width)
	{
	  Xpm_Error(KEYDATA);
	  printf(">>>|%s|<<<\n", buffer4);
	}
      else
	{
	  p1 = buffer3;
	  for (i=0; i<PCD8544_column_width; i++)
	    {
	      colour = *p1++;
	      if (colour == this_xpm_header.back) *ucp1 = 0;
	      if (colour == this_xpm_header.fore) *ucp1 = 1;
	      ucp1++;
	      npix++;
	    }
	}
    }
  while((npix < PCD8544_xpmpixc) && !feof(xpmfn));
  if (qv) report_stats(xpmfile);
  printf("First column bits ...\n");
  ucp1 = xpm_buffer;
  for (i=0; i<(PCD8544_column_width-1); i++)
    {
      printf("%1d", *ucp1);
      *ucp1++;
    }
  printf("%1d\n", *ucp1);
  printf("Last column bits ...\n");
  ucp1 = xpm_buffer + (PCD8544_xpmpixc - PCD8544_column_width);
  for (i=0; i<(PCD8544_column_width-1); i++)
    {
      printf("%1d", *ucp1);
      *ucp1++;
    }
  printf("%1d\n", *ucp1);      
  /* Transfer data to pixel buffer */
  ucp1 = xpm_buffer;
  opix = 0;
  rpix = 0;
  do
    {
      for (i=0; i<6; i++)
	{
	  cbyte = *ucp1++;
	  opix++;
	  for (j=1; j<8; j++)
	    {
	      jbit = *ucp1++;
	      cbyte = cbyte << 1;
	      cbyte = cbyte + jbit;
	      opix++;
	    }
	  cbyte = ReverseXbmByte(cbyte);
	  switch(i)
	    {
	    case 0: (*buffer).rowblock0[rpix] = cbyte; break;
	    case 1: (*buffer).rowblock1[rpix] = cbyte; break;
	    case 2: (*buffer).rowblock2[rpix] = cbyte; break;
	    case 3: (*buffer).rowblock3[rpix] = cbyte; break;
	    case 4: (*buffer).rowblock4[rpix] = cbyte; break;
	    case 5: (*buffer).rowblock5[rpix] = cbyte; break;
	    default: break;
	    }
	}
      rpix++;
     }
  while (opix < npix);
  echo_colpix_store_status(buffer);
  return(npix);
}

void clear_pixel_store(qc12864b_pixel8 *buffer)
{
  unsigned char *ucp1,*ucp2,*ucp3,*ucp4;
  int i;
  ucp1 = (*buffer).block11;
  ucp2 = (*buffer).block22;
  ucp3 = (*buffer).block13;
  ucp4 = (*buffer).block24;
  for (i=0; i<QC12864B_nbytes; i++)
    {
      *ucp1 = 0x00;
      *ucp2 = 0x00;
      *ucp3 = 0x00;
      *ucp4 = 0x00;
      ucp1++; ucp2++; ucp3++; ucp4++;
    }
}

void fill_pixel_store(qc12864b_pixel8 *buffer)
{
  unsigned char *ucp1,*ucp2,*ucp3,*ucp4;
  int i;
  ucp1 = (*buffer).block11;
  ucp2 = (*buffer).block22;
  ucp3 = (*buffer).block13;
  ucp4 = (*buffer).block24;
  for (i=0; i<QC12864B_nbytes; i++)
    {
      *ucp1 = 0xff;
      *ucp2 = 0xff;
      *ucp3 = 0xff;
      *ucp4 = 0xff;
      ucp1++; ucp2++; ucp3++; ucp4++;
    }
}

void init_pixel_store(qc12864b_pixel8 *buffer)
{
  unsigned char *ucp1,*ucp2,*ucp3,*ucp4;
  int i,j;
  ucp1 = (*buffer).block11;
  ucp2 = (*buffer).block22;
  ucp3 = (*buffer).block13;
  ucp4 = (*buffer).block24;
  for (i=0; i<QC12864B_nbytes; i++)
    {
      j = i % 2;
      if (j == 0)
	{
	  *ucp1 = 0x00;
	  *ucp2 = 0xff;
	  *ucp3 = 0x00;
	  *ucp4 = 0xff;
	}
      else
	{
	  *ucp1 = 0xff;
	  *ucp2 = 0x00;
	  *ucp3 = 0xff;
	  *ucp4 = 0x00;
	}
      ucp1++; ucp2++; ucp3++; ucp4++;
    }
}

void echo_pixel_store_status(qc12864b_pixel8 *buffer)
{
  unsigned char *ucp;
  int i,k;
  k = (QC12864B_width / 8) - 1;
  printf("qc12864b pixel store status:\n");
  printf("First row bytes ...\n");
  ucp = (*buffer).block11;
  for (i=0; i<k; i++)
    {
      printf("0x%02x ", *ucp);
      ucp++;
    }
  printf("0x%02x\n", *ucp);   
  printf("Last row bytes ...\n");
  ucp = (*buffer).block24 + (QC12864B_nbytes - k - 1);
  for (i=0; i<k; i++)
    {
      printf("0x%02x ", *ucp);
      ucp++;
    }
  printf("0x%02x\n", *ucp);   
}

void xbm_dump_pixel_store(qc12864b_pixel8 *buffer)
{
  unsigned char *ucp;
  unsigned char lbyte;
  int i,j,k,l;
  printf("#define PIXEL_STORE_width  %3d\n", QC12864B_width);
  printf("#define PIXEL_STORE_height %3d\n", QC12864B_height);
  printf("static unsigned char PIXEL_STORE_bits[] = {\n");
  k = (QC12864B_width / 8) - 1;
  l = (QC12864B_height / 4);
  ucp = (*buffer).block11;  
  for (j=0; j<l; j++)
    {
      for (i=0; i<k; i++)
	{
	  lbyte = ReverseXbmByte(*ucp);
	  ucp++;
	  printf(" 0x%02x,", lbyte);
	}
      lbyte = ReverseXbmByte(*ucp);
      ucp++;
      printf(" 0x%02x,\n", lbyte);
    }
  ucp = (*buffer).block22;  
  for (j=0; j<l; j++)
    {
      for (i=0; i<k; i++)
	{
	  lbyte = ReverseXbmByte(*ucp);
	  ucp++;
	  printf(" 0x%02x,", lbyte);
	}
      lbyte = ReverseXbmByte(*ucp);
      ucp++;
      printf(" 0x%02x,\n", lbyte);
    }
  ucp = (*buffer).block13;  
  for (j=0; j<l; j++)
    {
      for (i=0; i<k; i++)
	{
	  lbyte = ReverseXbmByte(*ucp);
	  ucp++;
	  printf(" 0x%02x,", lbyte);
	}
      lbyte = ReverseXbmByte(*ucp);
      ucp++;
      printf(" 0x%02x,\n", lbyte);
    }
  ucp = (*buffer).block24;  
  for (j=0; j<l; j++)
    {
      for (i=0; i<k; i++)
	{
	  lbyte = ReverseXbmByte(*ucp);
	  ucp++;
	  printf(" 0x%02x,", lbyte);
	}
      lbyte = ReverseXbmByte(*ucp);
      ucp++;
      printf(" 0x%02x,\n", lbyte);
    }
  printf(" };\n");
}

void xpm_dump_pixel_store(qc12864b_pixel8 *buffer)
{
  unsigned char *ucp;
  unsigned char ucbyte,bit,bitmask;
  int i,j,k,l;
  printf("/* XPM */\n");
  printf("static char *qc12864b_pixel8_buffer_xpm[] = {\n");
  printf("\"%3d %2d 2 1\",\n", QC12864B_width, QC12864B_height);
  printf("\"0	c #FFFFFF\",\n");
  printf("\"1	c #000000\",\n");
  k = (QC12864B_width / 8);
  ucp = (*buffer).block11;
  for (i=0; i<QC12864B_nbytes; i++)
    {
      j = i % k;
      if ((j == 0) && (i > 0)) printf("\",\n");
      if (j == 0) printf("\"");
      ucbyte = *ucp++;
      bitmask = 0x80;
      for (l=0; l<8; l++)
	{
	  bit = ucbyte & bitmask;
	  if (bit == 0) printf("0");
	  else printf("1");
	  bitmask = bitmask >> 1;
	}
    }
  printf("\",\n");
  ucp = (*buffer).block22;
  for (i=0; i<QC12864B_nbytes; i++)
    {
      j = i % k;
      if ((j == 0) && (i > 0)) printf("\",\n");
      if (j == 0) printf("\"");
      ucbyte = *ucp++;
      bitmask = 0x80;
      for (l=0; l<8; l++)
	{
	  bit = ucbyte & bitmask;
	  if (bit == 0) printf("0");
	  else printf("1");
	  bitmask = bitmask >> 1;
	}
    }
  printf("\",\n");
  ucp = (*buffer).block13;
  for (i=0; i<QC12864B_nbytes; i++)
    {
      j = i % k;
      if ((j == 0) && (i > 0)) printf("\",\n");
      if (j == 0) printf("\"");
      ucbyte = *ucp++;
      bitmask = 0x80;
      for (l=0; l<8; l++)
	{
	  bit = ucbyte & bitmask;
	  if (bit == 0) printf("0");
	  else printf("1");
	  bitmask = bitmask >> 1;
	}
    }
  printf("\",\n");
  ucp = (*buffer).block24;
  for (i=0; i<QC12864B_nbytes; i++)
    {
      j = i % k;
      if ((j == 0) && (i > 0)) printf("\",\n");
      if (j == 0) printf("\"");
      ucbyte = *ucp++;
      bitmask = 0x80;
      for (l=0; l<8; l++)
	{
	  bit = ucbyte & bitmask;
	  if (bit == 0) printf("0");
	  else printf("1");
	  bitmask = bitmask >> 1;
	}
    }
  printf("\"\n");
  printf("};\n");
}

void preset_cgram_store(qc12864b_udgc *buffer,
			unsigned char cgram_addr,			
			unsigned char allrows_hbyte,
			unsigned char allrows_lbyte)
{
  int l;
  (*buffer).cgram_addr = cgram_addr;
  for (l=0; l<QC12864B_udgcbits; l++)
    {
      (*buffer).hbyte_rows[l] = allrows_hbyte;
      (*buffer).lbyte_rows[l] = allrows_lbyte;
    }
}

void clear_colpix_store(pcd8544_colpix8 *buffer)
{
  unsigned char *ucp1,*ucp2,*ucp3,*ucp4,*ucp5,*ucp6;
  int i;
  ucp1 = (*buffer).rowblock0;
  ucp2 = (*buffer).rowblock1;
  ucp3 = (*buffer).rowblock2;
  ucp4 = (*buffer).rowblock3;
  ucp5 = (*buffer).rowblock4;
  ucp6 = (*buffer).rowblock5;
  for (i=0; i<PCD8544_row_height; i++)
    {
      *ucp1 = 0x00;
      *ucp2 = 0x00;
      *ucp3 = 0x00;
      *ucp4 = 0x00;
      *ucp5 = 0x00;
      *ucp6 = 0x00;
      ucp1++; ucp2++; ucp3++; ucp4++; ucp5++; ucp6++;
    }
}

void fill_colpix_store(pcd8544_colpix8 *buffer)
{
  unsigned char *ucp1,*ucp2,*ucp3,*ucp4,*ucp5,*ucp6;
  int i;
  ucp1 = (*buffer).rowblock0;
  ucp2 = (*buffer).rowblock1;
  ucp3 = (*buffer).rowblock2;
  ucp4 = (*buffer).rowblock3;
  ucp5 = (*buffer).rowblock4;
  ucp6 = (*buffer).rowblock5;
  for (i=0; i<PCD8544_row_height; i++)
    {
      *ucp1 = 0xff;
      *ucp2 = 0xff;
      *ucp3 = 0xff;
      *ucp4 = 0xff;
      *ucp5 = 0xff;
      *ucp6 = 0xff;
      ucp1++; ucp2++; ucp3++; ucp4++; ucp5++; ucp6++;
    }
}

void init_colpix_store(pcd8544_colpix8 *buffer)
{
  unsigned char *ucp1,*ucp2,*ucp3,*ucp4,*ucp5,*ucp6;
  int i,j;
  ucp1 = (*buffer).rowblock0;
  ucp2 = (*buffer).rowblock1;
  ucp3 = (*buffer).rowblock2;
  ucp4 = (*buffer).rowblock3;
  ucp5 = (*buffer).rowblock4;
  ucp6 = (*buffer).rowblock5;
  for (i=0; i<PCD8544_row_height; i++)
    {
      j = i % 12;
      if (j < 6)
	{
	  *ucp1 = 0x00;
	  *ucp2 = 0xff;
	  *ucp3 = 0x00;
	  *ucp4 = 0xff;
	  *ucp5 = 0x00;
	  *ucp6 = 0xff;
	}
      else
	{
	  *ucp1 = 0xff;
	  *ucp2 = 0x00;
	  *ucp3 = 0xff;
	  *ucp4 = 0x00;
	  *ucp5 = 0xff;
	  *ucp6 = 0x00;
	}
      ucp1++; ucp2++; ucp3++; ucp4++; ucp5++; ucp6++;
    }
}

void echo_colpix_store_status(pcd8544_colpix8 *buffer)
{
  unsigned char *ucp;
  int i,j,k;
  unsigned char colpix[PCD8544_nblocks];
  j = (PCD8544_nblocks -1);
  k = (PCD8544_row_height - 1);
  printf("pcd8544 colpix store status:\n");
  printf("First column bytes ...\n");
  colpix[0] = (*buffer).rowblock0[0];
  colpix[1] = (*buffer).rowblock1[0];
  colpix[2] = (*buffer).rowblock2[0];
  colpix[3] = (*buffer).rowblock3[0];
  colpix[4] = (*buffer).rowblock4[0];
  colpix[5] = (*buffer).rowblock5[0];
  for (i=0; i<j; i++) printf("0x%02x ", colpix[i]);
  printf("0x%02x\n", colpix[j]);
  printf("Last column bytes ...\n");
  colpix[0] = (*buffer).rowblock0[k];
  colpix[1] = (*buffer).rowblock1[k];
  colpix[2] = (*buffer).rowblock2[k];
  colpix[3] = (*buffer).rowblock3[k];
  colpix[4] = (*buffer).rowblock4[k];
  colpix[5] = (*buffer).rowblock5[k];
  for (i=0; i<j; i++) printf("0x%02x ", colpix[i]);
  printf("0x%02x\n", colpix[j]);
}

/* Change Log */
/*
 * $Log: glcd_xbmtools.c,v $
 * Revision 1.3  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.2  2017/09/10 22:40:35  pi
 * Bug-fix to XBM pixel data transfer code
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.19  2015/01/05 19:42:39  pi
 * Additional XPM file support for Nokia5110 48x84 rotated graphics images
 *
 * Revision 1.18  2015/01/05 12:20:21  pi
 * Interim commit
 *
 * Revision 1.17  2015/01/04 20:59:05  pi
 * Interim commit
 *
 * Revision 1.16  2015/01/03 22:56:48  pi
 * Interim commit
 *
 * Revision 1.15  2014/10/03 23:00:23  pi
 * Some XPM support for UDGC's
 *
 * Revision 1.14  2014/10/01 20:26:27  pi
 * Initial CGRAM character handling
 *
 * Revision 1.13  2014/09/28 16:32:25  pi
 * Some XPM support added
 *
 * Revision 1.12  2014/09/21 21:30:01  pi
 * Improved xbm data conversion
 *
 * Revision 1.11  2014/09/20 20:03:16  pi
 * Interim commit
 *
 * Revision 1.10  2014/09/15 22:33:12  pi
 * Interim comit
 *
 * Revision 1.9  2014/09/15 22:15:27  pi
 * Interim commit
 *
 * Revision 1.8  2014/09/15 18:44:01  pi
 * Interim commit
 *
 * Revision 1.7  2014/09/14 21:02:21  pi
 * Interim commit
 *
 * Revision 1.6  2014/09/10 22:39:59  pi
 * Interim commit
 *
 * Revision 1.5  2014/09/09 18:01:57  pi
 * Interim commit
 *
 * Revision 1.4  2014/09/08 21:55:26  pi
 * Interim commit
 *
 * Revision 1.3  2014/09/08 21:53:35  pi
 * Interim commit
 *
 * Revision 1.2  2014/09/08 21:50:20  pi
 * Interim commit
 *
 * Revision 1.1  2014/09/07 20:03:14  pi
 * Initial revision
 *
 *
 *
 */
