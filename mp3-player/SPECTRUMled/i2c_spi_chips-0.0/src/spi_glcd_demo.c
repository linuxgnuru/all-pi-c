/* SPI Graphics LCD (nokia5110-like) DEMOnstration code
 * $Id: spi_glcd_demo.c,v 1.2 2018/03/31 21:32:45 pi Exp $
 */

#define NO_ECHO  1

#define IS_MAIN           1
#define BLK_LEN          83  /* 80 + CR, LF & NUL */
#define BUF_LEN         512
#define CMD_LEN         138  /* 2 x FN_LEN + 10   */
#define FN_LEN           64

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_WIRINGPI_H
#include <wiringPi.h>
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPISPI_H
#include <wiringPiSPI.h>
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "parsel.h"
#include "spi_chips.h"
#include "spi_wraps.h"
#include "i2c_wraps.h"
#include "glcd_xbmtools.h"
/* NB: include "PCD8544" header files last! */
#include "spi_PCD8544.h"
#ifndef PCD8544_FONT_H
#include "PCD8544_font.h"
#endif /* !PCD8544_FONT_H */

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: spi_glcd_demo.c,v $";
static char RCSId[]       = "$Id: spi_glcd_demo.c,v 1.2 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.2 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.05 $";

void setup_RCSlist(void)
{
  RCSlist[AUTHOR_RCS] = RCSAuthor;
  RCSlist[AUTHOR_SCM] = SCMAuthor;
  RCSlist[DATE]       = RCSDate;
  RCSlist[FILE_RCS]   = RCSFile;
  RCSlist[ID]         = RCSId;
  RCSlist[NAME]       = RCSName;
  RCSlist[REVISION]   = RCSRevision;
  RCSlist[STATE]      = RCSState;
  RCSlist[SVERSION]   = SCMVersion;
  RCSlist[LAST_RCS]   = "$$";
}

/* Argument list handling code etc. */

typedef enum
{
  ARGFILE,   /* -a filename[.arg]    */
  XPMFILE,   /* -xpm filename[.xpm]  */
  COMMENT,   /* * {in argfile only!} */
  BITBANG,   /* -BB                  */
  ECHO,      /* -E                   */
  FULLSPI,   /* -FS                  */
#ifdef HAVE_WIRINGPI_H
  CLRHOME,   /* -CH                  */
  INITGPIO,  /* -IG                  */
  INVERSE,   /* -inverse             */
  NORMAL,    /* -normal              */
  RESETINIT, /* -RI                  */
  RESETTST,  /* -RT                  */
  SETCON0,   /* -SC0                 */
  SETCON1,   /* -SC1                 */
  SETCON2,   /* -SC2                 */
  SETCON3,   /* -SC3                 */
  SETCON4,   /* -SC4                 */
  SETCON5,   /* -SC5                 */
  SETCON6,   /* -SC6                 */
  SETCON7,   /* -SC7                 */
  SETCON8,   /* -SC8                 */
  SHIFTTST,  /* -SOT                 */
  WCBTST,    /* -WCT                 */
  WDBTST,    /* -WDT                 */
  WFDTST,    /* -WFT                 */
  WGRTST,    /* -WGT                 */
  WMSGTST,   /* -WMT                 */
#endif /* HAVE_WIRINGPI_H */
  QUIET,     /* -Q                   */
  SYSMODE,   /* -S                   */
  TSTMODE,   /* -T                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPISPI_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPISPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,spimode,spitype;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]   = "-a";
  arglist[XPMFILE]   = "-xpm";
  arglist[COMMENT]   = "*";
  arglist[BITBANG]   = "-BB";
  arglist[ECHO]      = "-E";
  arglist[FULLSPI]   = "-FS";
#ifdef HAVE_WIRINGPI_H
  arglist[CLRHOME]   = "-CH";
  arglist[INITGPIO]  = "-IG";
  arglist[INVERSE]   = "-inverse";
  arglist[NORMAL]    = "-normal";
  arglist[RESETINIT] = "-RI";
  arglist[RESETTST]  = "-RT";
  arglist[SETCON0]   = "-SC0";
  arglist[SETCON1]   = "-SC1";
  arglist[SETCON2]   = "-SC2";
  arglist[SETCON3]   = "-SC3";
  arglist[SETCON4]   = "-SC4";
  arglist[SETCON5]   = "-SC5";
  arglist[SETCON6]   = "-SC6";
  arglist[SETCON7]   = "-SC7";
  arglist[SETCON8]   = "-SC8";
  arglist[SHIFTTST]  = "-SOT";
  arglist[WCBTST]    = "-WCT";
  arglist[WDBTST]    = "-WDT";
  arglist[WFDTST]    = "-WFT";
  arglist[WGRTST]    = "-WGT";
  arglist[WMSGTST]   = "-WMT";
#endif /* HAVE_WIRINGPI_H */
  arglist[QUIET]     = "-Q";
  arglist[SYSMODE]   = "-S";
  arglist[TSTMODE]   = "-T";
  arglist[VERBOSE]   = "-V";
#ifdef HAVE_WIRINGPISPI_H
  arglist[WPIMODE]   = "-W";
#endif /* HAVE_WIRINGPISPI_H */
  arglist[XECUTE]    = "-X";
  arglist[LAST_ARG]  = "";
}

argnames whicharg(char *argstr)
{
  argnames current;
  char flag;
  char *p1,*p2;
  flag = FALSE;
  /* ARGFILE is the first ARGument name in the LIST */
  for (current=ARGFILE; current<LAST_ARG; current++)
    {
      p1 = argstr;
      p2 = arglist[current];
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
  else return(LAST_ARG);
}

/* Global variables etc. */
char eflag,igflag,oflag,tflag,vflag,xflag;
char message_buffer[BUF_LEN];
unsigned int convop,dispmode;

void listargs(void)
{
  /* ARGFILE is the first ARGument name in the LIST */
  for (what=ARGFILE; what<LAST_ARG; what++)
    printf("arglist[%2d] = %s\n", what, arglist[what]);
}

void Read_Arg_File(void)
{
  static int l,k;
  do
    {
      l = fetch(buffer1, afn);
      if (l > 1)
	{
	  strcpy(buffer3, buffer1);
	  strcpy(buffer4, buffer1);
	  k = parse_sp(buffer1, buffer3);
	  k = parse_sp(buffer2, buffer3);
	  what = whicharg(buffer1);
	  switch(what)
	    {
	    case ARGFILE: break;
	    case XPMFILE: 
	      {
		make_name(xpmfile, "xpm", buffer2);
		Open_Xpm_File("r");
		break;
	      }
	    case COMMENT: printf("%s\n", buffer4); break;
#ifdef HAVE_WIRINGPI_H
	    case BITBANG:  spitype=BITBANG; break;
	    case CLRHOME:  demode=CLRHOME;  spimode=WPIMODE; break;
	    case FULLSPI:  spitype=FULLSPI; break;
	    case INITGPIO: igflag=TRUE;     break;
	    case INVERSE:
	      {
		demode=INVERSE;
		dispmode=PCD8544_DISPLAYINVERTED;
		spimode=WPIMODE;
		break;
	      }
	    case NORMAL:
	      {
		demode=NORMAL;
		dispmode=PCD8544_DISPLAYNORMAL;
		spimode=WPIMODE;
		break;
	      }
	    case RESETINIT: demode=RESETINIT; spimode=WPIMODE; break;
	    case RESETTST: demode=RESETTST; break;
	    case SETCON0:
	      {
		demode=SETCON0;
		convop=PCD8544_C0;
		spimode=WPIMODE;
		break;
	      }
	    case SETCON1:
	      {
		demode=SETCON1;
		convop=PCD8544_C1;
		spimode=WPIMODE;
		break;
	      }
	    case SETCON2:
	      {
		demode=SETCON2;
		convop=PCD8544_C2;
		spimode=WPIMODE;
		break;
	      }
	    case SETCON3:
	      {
		demode=SETCON3;
		convop=PCD8544_C3;
		spimode=WPIMODE;
		break;
	      }
	    case SETCON4:
	      {
		demode=SETCON4;
		convop=PCD8544_C4;
		spimode=WPIMODE;
		break;
	      }
	    case SETCON5:
	      {
		demode=SETCON5;
		convop=PCD8544_C5;
		spimode=WPIMODE;
		break;
	      }
	    case SETCON6:
	      {
		demode=SETCON6;
		convop=PCD8544_C6;
		spimode=WPIMODE;
		break;
	      }
	    case SETCON7:
	      {
		demode=SETCON7;
		convop=PCD8544_C7;
		spimode=WPIMODE;
		break;
	      }
	    case SETCON8:
	      {
		demode=SETCON8;
		convop=PCD8544_C8;
		spimode=WPIMODE;
		break;
	      }
	    case SHIFTTST: demode=SHIFTTST; spitype=BITBANG; break;
	    case WCBTST:  demode=WCBTST; spimode=WPIMODE; break;
	    case WDBTST:  demode=WDBTST; spimode=WPIMODE; break;
	    case WFDTST:  demode=WFDTST; spimode=WPIMODE; break;
	    case WGRTST:  demode=WGRTST; spimode=WPIMODE; break;
	    case WMSGTST: demode=WMSGTST; spimode=WPIMODE; break;
#endif /* HAVE_WIRINGPI_H */
	    case ECHO:    eflag=TRUE;       break;
	    case QUIET:   vflag=FALSE;      break;
	    case SYSMODE: spimode=SYSMODE;  break;
	    case TSTMODE: tflag=TRUE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPISPI_H
	    case WPIMODE: spimode=WPIMODE; break;
#endif /* HAVE_WIRINGPISPI_H */
	    case XECUTE:  xflag=TRUE; break;
	    case LAST_ARG: printf("* ? %s %s\n", buffer1, buffer2); break;
	    default: break;
	    }
	}
    }
  while(!feof(afn));
  fclose(afn);
  argfopen = FALSE;
  argfread = TRUE;
  printf(" ... done.\n");
}

void usage(void)
/* Set up USAGE message */
{
  strcpy(buffer1, " [Options]\n");
  strcat(buffer1, "Options: -a arg_filename[.arg],\n");
  strcat(buffer1, "         -xpm xpm_filename[.xpm] (for 48x84 rotated graphic),\n");
  strcat(buffer1, "         -BB  {use Bit-Banged** spi interface},\n");
  strcat(buffer1, "              {**NB: Prefix with sudo - root privileges reqired!},\n");
  strcat(buffer1, "         -FS  {use Full Spi interface - default},\n");
  strcat(buffer1, "         -E   {Echo command-line arguments},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -IG  {Initialise used gpio's & Gpio readall},\n");
  strcat(buffer1, "         -RI  {Reset & Initialise display - implies -W},\n");
  strcat(buffer1, "         -RT  {Reset pulse Test^^},\n");
  strcat(buffer1, "         -CH  {Clear Home - implies -W},\n");
  strcat(buffer1, "         -inverse {INVERSE display - implies -W},\n");
  strcat(buffer1, "         -normal  {NORMAL display - default, implies -W},\n");
  strcat(buffer1, "         -SCn {Set Contrast level n, 0<=n<=8 - implies -W},\n");
  strcat(buffer1, "         -SOT {Shift Out Test^^   - implies -BB},\n");
  strcat(buffer1, "         -WCT {Write Command Test - implies -W},\n");
  strcat(buffer1, "         -WDT {Write Data Test    - implies -W},\n");
  strcat(buffer1, "         -WFT {Write direct Font data Test - implies -W},\n");
  strcat(buffer1, "         -WGT {Write Graphics image data Test - implies -W},\n");
  strcat(buffer1, "         -WMT {Write Message Test - implies -W},\n");
  strcat(buffer1, "              {^^ NB: LED Test board req'd with -S},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q   {Quiet - default},\n");
  strcat(buffer1, "         -S   {use System call methods - default},\n");
  strcat(buffer1, "         -T   {using LED Test board},\n");
  strcat(buffer1, "         -V   {Verbose},\n");
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -W   {use Wiring pi methods},\n");
#endif /* HAVE_WIRINGPISPI_H */
  strcat(buffer1, "         -X   {eXecute demonstration}");
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
void C2S_GlcdReset_Test(int rst_gpio,
			char qx)
{  
  C2S_gpio_g_write(rst_gpio,
		   GPIO_HIGH,
		   qx);	    
  sleep(1);
  C2S_gpio_g_write(rst_gpio,
		   GPIO_LOW,
		   qx);	    
  sleep(1);
  C2S_gpio_g_write(rst_gpio,
		   GPIO_HIGH,
		   qx);	    
}

void C2S_BB_GlcdReset_Test(bbpcd8544 *this_bbpcd8544,
			   char qx)
{
  int lrst_wpin;
  lrst_wpin = (*this_bbpcd8544).rst_wpin;
  C2S_gpio_write(lrst_wpin, GPIO_LOW, qx);
  sleep(1);
  C2S_gpio_write(lrst_wpin, GPIO_HIGH, qx);
}

void C2S_BB_ShiftOutTest(bbpcd8544 *this_bbpcd8544,
			 int bitorder,
			 char qx)
{
  int l,lclk_wpin,ldin_wpin;
  unsigned char ldcval;
  lclk_wpin = (*this_bbpcd8544).clk_wpin;
  ldin_wpin = (*this_bbpcd8544).din_wpin;
  ldcval    = (*this_bbpcd8544).dcval;
  for (l=0; l<8; l++)
    {
      if (bitorder == LSBFIRST)
	C2S_gpio_write(ldin_wpin, !!(ldcval & (1 << l)), qx);
      else
	C2S_gpio_write(ldin_wpin, !!(ldcval & (1 << (7 - l))), qx);
      C2S_gpio_write(lclk_wpin, GPIO_HIGH, qx);
      sleep(1);
      C2S_gpio_write(lclk_wpin, GPIO_LOW, qx);
      sleep(1);
    }
}

/* Exporting the ReSeT gpio leads to spurious reset pulses so ... */
void WP_C2S_GlcdReset_Test(int rst_gpio,
			   char qx)
{
  C2S_gpio_g_write(rst_gpio,
		   GPIO_HIGH,
		   qx);	    
  delay(500);
  C2S_gpio_g_write(rst_gpio,
		   GPIO_LOW,
		   qx);	    
  delay(100);
  C2S_gpio_g_write(rst_gpio,
		   GPIO_HIGH,
		   qx);	    
}

void WP_BB_GlcdReset_Test(bbpcd8544 *this_bbpcd8544,
			  char qx)
{
  int lrst_wpin,lspeed;
  lrst_wpin = (*this_bbpcd8544).rst_wpin;
  lspeed = (*this_bbpcd8544).speed;
  WP_digitalWrite(lrst_wpin, GPIO_LOW, qx);
  delayMicroseconds(lspeed);
  WP_digitalWrite(lrst_wpin, GPIO_HIGH, qx);
}

void WP_BB_ShiftOutTest(bbpcd8544 *this_bbpcd8544,
			int bitorder,
			char qx)
{
  int l,lclk_wpin,ldin_wpin,lspeed;
  unsigned char ldcval;
  lclk_wpin = (*this_bbpcd8544).clk_wpin;
  ldin_wpin = (*this_bbpcd8544).din_wpin;
  lspeed    = (*this_bbpcd8544).speed;
  ldcval    = (*this_bbpcd8544).dcval;
  for (l=0; l<8; l++)
    {
      if (bitorder == LSBFIRST)
	WP_digitalWrite(ldin_wpin, !!(ldcval & (1 << l)), qx);
      else
	WP_digitalWrite(ldin_wpin, !!(ldcval & (1 << (7 - l))), qx);
      WP_digitalWrite(lclk_wpin, GPIO_HIGH, qx);
      delayMicroseconds(lspeed);
      WP_digitalWrite(lclk_wpin, GPIO_LOW, qx);
      delayMicroseconds(lspeed);
    }
}

void WP_BB_WriteCommandByteMSB(bbpcd8544 *this_bbpcd8544,
			       char qx)
{
  int l,lce_wpin,lclk_wpin,ldc_wpin,ldin_wpin,lspeed;
  unsigned char ldcval;
  lce_wpin  = (*this_bbpcd8544).ce_wpin;
  lclk_wpin = (*this_bbpcd8544).clk_wpin;
  ldc_wpin  = (*this_bbpcd8544).dc_wpin;
  ldin_wpin = (*this_bbpcd8544).din_wpin;
  lspeed    = (*this_bbpcd8544).speed;
  ldcval    = (*this_bbpcd8544).dcval;
  /* CE enabled */
  WP_digitalWrite(lce_wpin, GPIO_LOW, qx);
  /* Data/¬Command = Command mode */
  WP_digitalWrite(ldc_wpin, GPIO_LOW, qx);
  delayMicroseconds(lspeed);
  WP_digitalWrite(lclk_wpin, GPIO_LOW, qx);
  for (l=0; l<8; l++)
    {
      WP_digitalWrite(ldin_wpin, !!(ldcval & (1 << (7 - l))), qx);
      WP_digitalWrite(lclk_wpin, GPIO_HIGH, qx);
      delayMicroseconds(lspeed);
      WP_digitalWrite(lclk_wpin, GPIO_LOW, qx);
      delayMicroseconds(lspeed);
    }
  /* CE disabled */
  WP_digitalWrite(lce_wpin, GPIO_HIGH, qx);
  delayMicroseconds(lspeed);
}

void WP_BB_WriteDataByteMSB(bbpcd8544 *this_bbpcd8544,
			    char qx)
{
  int l,lce_wpin,lclk_wpin,ldc_wpin,ldin_wpin,lspeed;
  unsigned char ldcval;
  lce_wpin  = (*this_bbpcd8544).ce_wpin;
  lclk_wpin = (*this_bbpcd8544).clk_wpin;
  ldc_wpin  = (*this_bbpcd8544).dc_wpin;
  ldin_wpin = (*this_bbpcd8544).din_wpin;
  lspeed    = (*this_bbpcd8544).speed;
  ldcval    = (*this_bbpcd8544).dcval;
  /* CE enabled */
  WP_digitalWrite(lce_wpin, GPIO_LOW, qx);
  /* Data/¬Command = Data mode */
  WP_digitalWrite(ldc_wpin, GPIO_HIGH, qx);
  delayMicroseconds(lspeed);
  WP_digitalWrite(lclk_wpin, GPIO_LOW, qx);
  for (l=0; l<8; l++)
    {
      WP_digitalWrite(ldin_wpin, !!(ldcval & (1 << (7 - l))), qx);
      WP_digitalWrite(lclk_wpin, GPIO_HIGH, qx);
      delayMicroseconds(lspeed);
      WP_digitalWrite(lclk_wpin, GPIO_LOW, qx);
      delayMicroseconds(lspeed);
    }
  /* CE disabled */
  WP_digitalWrite(lce_wpin, GPIO_HIGH, qx);
  delayMicroseconds(lspeed);
}

void WP_BB_WriteFontDataBytesMSB(bbpcd8544 *this_bbpcd8544,
				 unsigned int ascii_code,
				 char *this8x5font,
				 char qx)
{
  unsigned char *colptr;
  char l;
  colptr = this8x5font + (ascii_code * 5);
  for (l=0; l<5; l++)
    {
      (*this_bbpcd8544).dcval = *colptr;
      WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
      colptr++;
    }
  (*this_bbpcd8544).dcval = 0x00;
  WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
}

void WP_BB_ClearHomeMSB(bbpcd8544 *this_bbpcd8544, char qx)
{
  int l,m;
  m = 84 * 6;
  (*this_bbpcd8544).dcval = PCD8544_SETXADDR;
  WP_BB_WriteCommandByteMSB(this_bbpcd8544, qx);
  (*this_bbpcd8544).dcval = PCD8544_SETYADDR;
  WP_BB_WriteCommandByteMSB(this_bbpcd8544, qx);
  (*this_bbpcd8544).dcval = 0x00;
  for (l=0; l<m; l++) WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
  (*this_bbpcd8544).dcval = PCD8544_SETXADDR;
  WP_BB_WriteCommandByteMSB(this_bbpcd8544, qx);
  (*this_bbpcd8544).dcval = PCD8544_SETYADDR;
  WP_BB_WriteCommandByteMSB(this_bbpcd8544, qx);
}

void WP_BB_WriteGraphicsDataBytesMSB(bbpcd8544 *this_bbpcd8544,
				     pcd8544_colpix8 *this_colpix8,
				     char qx)
{  
  unsigned char* ucp;
  int i;
  ucp = (*this_colpix8).rowblock0;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_bbpcd8544).dcval = *ucp;
      WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock1;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_bbpcd8544).dcval = *ucp;
      WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock2;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_bbpcd8544).dcval = *ucp;
      WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock3;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_bbpcd8544).dcval = *ucp;
      WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock4;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_bbpcd8544).dcval = *ucp;
      WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock5;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_bbpcd8544).dcval = *ucp;
      WP_BB_WriteDataByteMSB(this_bbpcd8544, qx);
      ucp++;
    }
}

void WP_BB_WriteMessageMSB(bbpcd8544 *this_bbpcd8544,
			   char *this8x5font,
			   char qx,
			   char *message)
{
  int bite;
  char lmsg[LCDWIDTH+1],*lp1;
  strncpy(lmsg, message, LCDWIDTH);
  lmsg[LCDWIDTH] = NUL;
  lp1 = lmsg;
  while (*lp1 != NUL)
    {
      bite = toascii((int)(*lp1));
      WP_BB_WriteFontDataBytesMSB(this_bbpcd8544, bite, this8x5font, qx);
      lp1++;
    }
}			
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char spidev;
  char xpmlflag;
  int i,j,k,l;
  int mrow,nbytes,npix;
#ifdef HAVE_WIRINGPI_H
  int wPS,wPSS;
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPISPI_H
  int *myspifd;
  int wP_SPIfd[2];
#endif /* HAVE_WIRINGPISPI_H */
  bbpcd8544 bbnokia5110;
  pcd8544 nokia5110;

  setup_RCSlist();

  /* printf("getRCSVersion reports %s\n", getRCSVersion()); */

  /* Initialise argument list etc. */
  setupargs();
  setupxbmkeys();
  /* Software Configuration Management Info. & Usage */
  print_scm(getRCSFile(FALSE),
	    getSCMAuthor(),
	    getSCMVersion(),
	    getRCSState(),
	    getRCSDate());
  /* Optional package info. etc. ... */
#ifdef HAVE_CONFIG_H
  print_pkg_info(getPKGName(), getPKGVersion());
#endif /* HAVE_CONFIG_H */

  if (argc < 2)
    {
      usage();
      exit(EXIT_SUCCESS);
    }

  /* Set up defaults */
  eflag    = igflag = tflag = vflag = xflag = xpmlflag = FALSE;
  convop   = PCD8544_C3;
  dispmode = PCD8544_DISPLAYNORMAL;
  demode   = INITGPIO; /* Dummy mode */
  spidev   = SPI_DEV00;
  spimode  = SYSMODE;
  spitype  = FULLSPI;
#ifdef HAVE_WIRINGPISPI_H
  myspifd = &wP_SPIfd[0]; /* == SPI_DEV00 */
  wP_SPIfd[0] = wP_SPIfd[1] = -1; /* ie. as yet undefined */
#endif /* HAVE_WIRINGPISPI_H */
  init_colpix_store(&bbnokia5110_buffer);
  init_colpix_store(&fsnokia5110_buffer);  
  make_name(argfile, "arg", getRCSFile(FALSE));
  if ((i = gethostname(message_buffer, BUF_LEN)) < 0)
    {
      strcpy(message_buffer, "unknown");
      perror("gethostname");
    }

  /* Analyse the arguments (if any) */
  if (argc > 1)
    {
      for (i=1; i<argc; i++)
	{
	  /* Current argument */
	  p1   = argv[i];
	  what = whicharg(p1);
	  /* Next argument (read ahead) */
	  i++;
	  if (i < argc) p2 = argv[i];
	  else p2 = p1; /* No more to be read */
	  /* Actions */
	  switch(what)
	    {
	    case ARGFILE: 
	      {
		make_name(argfile, "arg", p2); 
		Open_Arg_File();
		if (argfopen) Read_Arg_File();
		break;
	      }
	    case XPMFILE: 
	      {
		make_name(xpmfile, "xpm", p2);
		Open_Xpm_File("r");
		break;
	      }
#ifdef HAVE_WIRINGPI_H
	    case BITBANG:  spitype=BITBANG; i--; break;
	    case CLRHOME:  demode=CLRHOME;  spimode=WPIMODE; i--; break;
	    case FULLSPI:  spitype=FULLSPI; i--; break;
	    case INITGPIO: igflag=TRUE;     i--; break;
	    case INVERSE:
	      {
		demode=INVERSE;
		dispmode=PCD8544_DISPLAYINVERTED;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case NORMAL:
	      {
		demode=NORMAL;
		dispmode=PCD8544_DISPLAYNORMAL;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case RESETINIT: demode=RESETINIT; spimode=WPIMODE; i--; break;
	    case RESETTST: demode=RESETTST; i--; break;
	    case SETCON0:
	      {
		demode=SETCON0;
		convop=PCD8544_C0;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SETCON1:
	      {
		demode=SETCON1;
		convop=PCD8544_C1;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SETCON2:
	      {
		demode=SETCON2;
		convop=PCD8544_C2;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SETCON3:
	      {
		demode=SETCON3;
		convop=PCD8544_C3;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SETCON4:
	      {
		demode=SETCON4;
		convop=PCD8544_C4;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SETCON5:
	      {
		demode=SETCON5;
		convop=PCD8544_C5;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SETCON6:
	      {
		demode=SETCON6;
		convop=PCD8544_C6;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SETCON7:
	      {
		demode=SETCON7;
		convop=PCD8544_C7;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SETCON8:
	      {
		demode=SETCON8;
		convop=PCD8544_C8;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case SHIFTTST: demode=SHIFTTST; spitype=BITBANG; i--; break;
	    case WCBTST: demode=WCBTST; spimode=WPIMODE; i--; break;
	    case WDBTST: demode=WDBTST; spimode=WPIMODE; i--; break;
	    case WFDTST: demode=WFDTST; spimode=WPIMODE; i--; break;
	    case WGRTST: demode=WGRTST; spimode=WPIMODE; i--; break;
	    case WMSGTST: demode=WMSGTST; spimode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPI_H */
	    case ECHO:    eflag=TRUE;       i--; break;
	    case QUIET:   vflag=FALSE;      i--; break;
	    case SYSMODE: spimode=SYSMODE;  i--; break;
	    case TSTMODE: tflag=TRUE; i--;  break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPISPI_H
	    case WPIMODE: spimode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPISPI_H */
	    case XECUTE:  xflag=TRUE; i--; break;
	    case LAST_ARG:
	      {
		printf("Unrecognised argument: %s\n", p1);
		eflag=TRUE;
		i--;
		break;
	      }
	    default: break; /* Do nothing */
	    }
	}
    }
  
  /* Find arguments in filename.arg, if it hasn't been read earlier */
  if (!argfread) Open_Arg_File();
  if (argfopen) Read_Arg_File();

  /* Example of Verbose Information */
  if (vflag)
    {
      listRCSnames();
      listargs();
      listxbmkeys();
    }

  /* Echo the arguments if requested */
  if (eflag)
    {
      for (i=1; i<argc; i++)
	{
	  p1 = argv[i];
	  printf("%s ", p1);
	}
      puts("");
    }

  /* Load an XPM file if declared and opened ... */
  if (xpmfopen)
    {
      Read_Xpm_Header(vflag);
      Read_Xpm_Header_Data(vflag);
      xpmtype = Check_Xpm_Header_Type(XPMCOLPIX8);
      printf("XPM file type = %s\n", xbmkeylist[xpmtype]);
      if (xpmtype == XPMCOLPIX8)
	{
	  if (spitype == BITBANG) npix = Read_Xpm_Colpix8_Data(&bbnokia5110_buffer, vflag);
	  if (spitype == FULLSPI) npix = Read_Xpm_Colpix8_Data(&fsnokia5110_buffer, vflag);
	}
      if (npix > 0) xpmlflag = TRUE;
      Close_Xpm_File();
    }

  /* -IG option leads to an early exit after the system call ... */
  if (igflag)
    {
      if (spitype == BITBANG)
	{
	  init_bbpcd8544(&bbnokia5110,
			 4,
			 3,
			 2,
			 1,
			 0,
			 GPIO_UNASSIGNED,
			 PCD8544_MEDIUM_CLOCK,
			 "BBNokia5110");
	  echo_bbpcd8544_status(bbnokia5110);
	  echo_colpix_store_status(&bbnokia5110_buffer);
#ifdef HAVE_WIRINGPI_H
	  C2S_gpio_mode(bbnokia5110.rst_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* ReSeT inactive */
	  C2S_gpio_write(bbnokia5110.rst_wpin,
			 GPIO_HIGH,
			 xflag);
	  C2S_gpio_mode(bbnokia5110.ce_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* CE disabled */
	  C2S_gpio_write(bbnokia5110.ce_wpin,
			 GPIO_HIGH,
			 xflag);
	  C2S_gpio_mode(bbnokia5110.dc_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* Data/¬Command = Command */
	  C2S_gpio_write(bbnokia5110.dc_wpin,
			 GPIO_LOW,
			 xflag);
	  C2S_gpio_mode(bbnokia5110.din_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* Set data pin high ... */
	  C2S_gpio_write(bbnokia5110.din_wpin,
			 GPIO_HIGH,
			 xflag);
	  C2S_gpio_mode(bbnokia5110.clk_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* Set clock pin high ... */
	  C2S_gpio_write(bbnokia5110.clk_wpin,
			 GPIO_HIGH,
			 xflag);
	  if (bbnokia5110.bl_wpin != GPIO_UNASSIGNED)
	    {
	      C2S_gpio_mode(bbnokia5110.bl_wpin,
			    GPIO_MODE_OUT,
			    xflag);
	      /* BackLight on */
	      C2S_gpio_write(bbnokia5110.bl_wpin,
			     GPIO_HIGH,
			     xflag);
	    }
	  C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
	}

      if (spitype == FULLSPI)
	{
	  init_pcd8544(&nokia5110,
		       25,
		       24,
		       GPIO_UNASSIGNED,
		       spidev, 
		       "Nokia5110");
	  echo_pcd8544_status(nokia5110);
	  echo_colpix_store_status(&fsnokia5110_buffer);
#ifdef HAVE_WIRINGPI_H
	  C2S_gpio_g_mode(nokia5110.rst_gpio,
			  GPIO_MODE_OUT,
			  xflag);
	  /* ReSeT inactive */
	  C2S_gpio_g_write(nokia5110.rst_gpio,
			   GPIO_HIGH,
			   xflag);
	  C2S_gpio_g_mode(nokia5110.dc_gpio,
			  GPIO_MODE_OUT,
			  xflag);
	  /* Data/¬Command = Command */
	  C2S_gpio_g_write(nokia5110.dc_gpio,
			   GPIO_LOW,
			   xflag);
	  if (nokia5110.bl_gpio != GPIO_UNASSIGNED)
	    {
	      C2S_gpio_g_mode(nokia5110.bl_gpio,
			      GPIO_MODE_OUT,
			      xflag);
	      /* BackLight on */
	      C2S_gpio_g_write(nokia5110.bl_gpio,
			       GPIO_HIGH,
			       xflag);
	    }
	  C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
	}
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  if (spimode == SYSMODE)
    {
      if (spitype == BITBANG)
	{
	  init_bbpcd8544(&bbnokia5110,
			 4,
			 3,
			 2,
			 1,
			 0,
			 GPIO_UNASSIGNED,
			 PCD8544_SLOW_CLOCK,
			 "BBNokia5110");
	  echo_bbpcd8544_status(bbnokia5110);
	  echo_colpix_store_status(&bbnokia5110_buffer);
	  printf("Using 'C' system call-based methods: ...\n");
	  C2S_gpio_mode(bbnokia5110.rst_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* ReSeT inactive */
	  C2S_gpio_write(bbnokia5110.rst_wpin, GPIO_HIGH, xflag);	    
	  C2S_gpio_mode(bbnokia5110.ce_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* CE disabled */
	  C2S_gpio_write(bbnokia5110.ce_wpin,
			 GPIO_HIGH,
			 xflag);
	  C2S_gpio_mode(bbnokia5110.dc_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* Data/¬Command = Command mode */
	  C2S_gpio_write(bbnokia5110.dc_wpin, GPIO_LOW, xflag);	    
	  C2S_gpio_mode(bbnokia5110.din_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* Set data pin low ... */
	  C2S_gpio_write(bbnokia5110.din_wpin,
			 GPIO_LOW,
			 xflag);
	  C2S_gpio_mode(bbnokia5110.clk_wpin,
			GPIO_MODE_OUT,
			xflag);
	  /* Set clock pin high ... */
	  C2S_gpio_write(bbnokia5110.clk_wpin,
			 GPIO_HIGH,
			 xflag);
	  /* Back Light on - if under software control */
	  if (bbnokia5110.bl_wpin != GPIO_UNASSIGNED)
	    {
	      C2S_gpio_mode(bbnokia5110.bl_wpin, GPIO_MODE_OUT, xflag);
	      C2S_gpio_write(bbnokia5110.bl_wpin, GPIO_HIGH, xflag);	    
	    }
	}
      
      if (spitype == FULLSPI)
	{
	  init_pcd8544(&nokia5110,
		       25,
		       24,
		       GPIO_UNASSIGNED,
		       spidev, 
		       "Nokia5110");
	  echo_pcd8544_status(nokia5110);
	  echo_colpix_store_status(&fsnokia5110_buffer);
#ifdef HAVE_WIRINGPI_H
	  C2S_gpio_g_mode(nokia5110.rst_gpio,
			  GPIO_MODE_OUT,
			  xflag);
	  /* ReSeT inactive */
	  C2S_gpio_g_write(nokia5110.rst_gpio,
			   GPIO_HIGH,
			   xflag);
	  C2S_gpio_g_mode(nokia5110.dc_gpio,
			  GPIO_MODE_OUT,
			  xflag);
	  /* Data/¬Command = Command */
	  C2S_gpio_g_write(nokia5110.dc_gpio,
			   GPIO_LOW,
			   xflag);
	  if (nokia5110.bl_gpio != GPIO_UNASSIGNED)
	    {
	      C2S_gpio_g_mode(nokia5110.bl_gpio,
			      GPIO_MODE_OUT,
			      xflag);
	      /* BackLight on */
	      C2S_gpio_g_write(nokia5110.bl_gpio,
			       GPIO_HIGH,
			       xflag);
	      if (eflag || vflag) C2S_gpio("readall", xflag);
	    }
#endif /* HAVE_WIRINGPI_H */
	}
    }

#ifdef HAVE_WIRINGPI_H
  if (spimode == WPIMODE)
    {  
      if (spitype == BITBANG)
	{
	  init_bbpcd8544(&bbnokia5110,
			 4,
			 3,
			 2,
			 1,
			 0,
			 GPIO_UNASSIGNED,
			 PCD8544_FAST_CLOCK,
			 "BBNokia5110");
	  echo_bbpcd8544_status(bbnokia5110);
	  echo_colpix_store_status(&bbnokia5110_buffer);
	  printf("Using wiringPi methods requiring root priviliges! ...\n");
	  if (xflag)
	    {
	      if ((wPS=wiringPiSetup()) < 0)
		{
		  fprintf(stderr,
			  "Could not use wiringPiSetup!");
		  perror("wiringPiSetup");
		  exit(EXIT_FAILURE);
		}
	      else printf("Executed wiringPiSetup ...\n");
	    }
	  WP_pinMode(bbnokia5110.rst_wpin, OUTPUT, xflag);
	  /* ReSeT inactive */
	  WP_digitalWrite(bbnokia5110.rst_wpin, GPIO_HIGH, xflag);
	  WP_pinMode(bbnokia5110.ce_wpin, OUTPUT, xflag);
	  /* CE disabled */
	  WP_digitalWrite(bbnokia5110.ce_wpin, GPIO_HIGH, xflag);
	  WP_pinMode(bbnokia5110.dc_wpin, OUTPUT, xflag);
	  /* Data/¬Command = Command mode */
	  WP_digitalWrite(bbnokia5110.dc_wpin, GPIO_LOW, xflag);
	  WP_pinMode(bbnokia5110.din_wpin, OUTPUT, xflag);
	  /* Set data pin low ... */
	  WP_digitalWrite(bbnokia5110.din_wpin, GPIO_LOW, xflag);
	  WP_pinMode(bbnokia5110.clk_wpin, OUTPUT, xflag);
	  /* Set clock pin high ... */
	  WP_digitalWrite(bbnokia5110.clk_wpin, GPIO_HIGH, xflag);
	  /* Back Light on - if under software control */
	  if (bbnokia5110.bl_wpin != GPIO_UNASSIGNED)
	    {
	      WP_pinMode(bbnokia5110.bl_wpin, OUTPUT, xflag);
	      WP_digitalWrite(bbnokia5110.bl_wpin, GPIO_HIGH, xflag);
	    }
	}

      if (spitype == FULLSPI)
	{
	  init_pcd8544(&nokia5110,
		       25,
		       24,
		       GPIO_UNASSIGNED,
		       spidev, 
		       "Nokia5110");
	  echo_pcd8544_status(nokia5110);
	  echo_colpix_store_status(&fsnokia5110_buffer);
	  printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
	  /* Exporting the ReSeT gpio leads to spurious reset pulses so ... */
	  /* C2S_gpio_export(nokia5110.rst_gpio, GPIO_XOUT, xflag); */
	  /* ... use SYSTEM method instead! */
	  C2S_gpio_g_mode(nokia5110.rst_gpio, GPIO_MODE_OUT, xflag);
	  C2S_gpio_export(nokia5110.dc_gpio, GPIO_XOUT, xflag);
	  if (nokia5110.bl_gpio != GPIO_UNASSIGNED)
	    C2S_gpio_export(nokia5110.bl_gpio, GPIO_XOUT, xflag);
	  if (xflag)
	    {
	      if ((wPSS=wiringPiSetupSys()) < 0)
		{
		  fprintf(stderr,
			  "Could not use wiringPiSetupSys!");
		  perror("wiringPiSetupSys");
		  C2S_gpio_unexportall(xflag);
		  exit(EXIT_FAILURE);
		}
	      else printf("Executed wiringPiSetupSys ...\n");
	    }
	  /* ReSeT disabled */
	  /* Exporting the ReSeT gpio leads to spurious reset pulses so ... */
	  /* WP_digitalWrite(nokia5110.rst_gpio, GPIO_HIGH, xflag); */
	  /* ... use SYSTEM method instead! */
	  C2S_gpio_g_write(nokia5110.rst_gpio, GPIO_HIGH, xflag);
	  /* Data/¬Command = Command mode */
	  WP_digitalWrite(nokia5110.dc_gpio, GPIO_LOW, xflag);
	  /* Back Light on - if under software control */
	  if (nokia5110.bl_gpio != GPIO_UNASSIGNED)
	    WP_digitalWrite(nokia5110.bl_gpio, GPIO_HIGH, xflag);
	  C2S_gpio("exports", xflag);
	  if (eflag || vflag) 
	    {
	      printf("ls -laF /sys/class/gpio/\n");
	      if (xflag) system("ls -laF /sys/class/gpio/");
	    }
#ifdef HAVE_WIRINGPISPI_H
	  if (spimode == WPIMODE)
	    {
	      printf("Using wiringPiSPI methods: ...\n");
	      if ((wP_SPIfd[0]=wiringPiSPISetup(0, 500000)) < 0)
		{
		  fprintf(stderr, "Could not setup wP_SPIfd[0]!\n");
		  perror("wiringPiSPISetup");
		  exit(EXIT_FAILURE);
		}
	      else printf("wP_SPIfd[0] = %d\n", wP_SPIfd[0]);
	      if ((wP_SPIfd[1]=wiringPiSPISetup(1, 500000)) < 0)
		{
		  fprintf(stderr, "Could not setup wP_SPIfd[1]!\n");
		  perror("wiringPiSPISetup");
		  exit(EXIT_FAILURE);
		}
	      else printf("wP_SPIfd[1] = %d\n", wP_SPIfd[1]);
	      if (*myspifd > 0)
		{
		  nokia5110.spifd = myspifd;
		  printf("*nokia5110.spifd = %d\n",
			 *nokia5110.spifd);
		}
	    }
#endif /* HAVE_WIRINGPISPI_H */
	}
    }
#endif /* HAVE_WIRINGPI_H */
  
  /* Demonstration ... */
  if (spimode == SYSMODE)
    {
      if (spitype == BITBANG)
	{
	  switch(demode)
	    {
	    case RESETTST:
	      {
		if (!tflag) xflag=FALSE;
		/* CE enabled */
		C2S_gpio_write(bbnokia5110.ce_wpin,
			       GPIO_LOW,
			       xflag);
		C2S_BB_GlcdReset_Test(&bbnokia5110, xflag);		
		break;
	      }
	    case SHIFTTST:
	      {
		printf("Shift Out Test ...\n");
		if (!tflag) xflag=FALSE;
		/* CE enabled */
		C2S_gpio_write(bbnokia5110.ce_wpin,
			       GPIO_LOW,
			       xflag);
		/* Set clock pin low ... */
		C2S_gpio_write(bbnokia5110.clk_wpin,
			       GPIO_LOW,
			       xflag);
		bbnokia5110.dcval = 0x55;
		printf("0x%02x ...\n", bbnokia5110.dcval);
		C2S_BB_ShiftOutTest(&bbnokia5110, MSBFIRST, xflag);
		sleep(2);
		bbnokia5110.dcval = 0x96;
		printf("0x%02x ...\n", bbnokia5110.dcval);
		C2S_BB_ShiftOutTest(&bbnokia5110, MSBFIRST, xflag);
		/* Set clock pin high ... */
		C2S_gpio_write(bbnokia5110.clk_wpin,
			       GPIO_HIGH,
			       xflag);
		/* CE disabled */
		C2S_gpio_write(bbnokia5110.ce_wpin,
			       GPIO_HIGH,
			       xflag);
		break;
	      }
	    default: break;
	    }
	}
      if (spitype == FULLSPI)
	{
	  switch(demode)
	    {
	    case RESETTST:
	      {
		if (!tflag) xflag=FALSE;
		C2S_GlcdReset_Test(nokia5110.rst_gpio, xflag);
		break;
	      }
	    default: break;
	    }
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
      if (spitype == BITBANG)
	{
	  if (tflag) bbnokia5110.speed = 1000 * bbnokia5110.speed;
	  switch(demode)
	    {
	    case CLRHOME:
	      {
		WP_BB_ClearHomeMSB(&bbnokia5110, xflag);
		break;
	      }
	    case RESETTST:
	      {
		/* CE enabled */
		WP_digitalWrite(bbnokia5110.ce_wpin, GPIO_LOW, xflag);
		WP_BB_GlcdReset_Test(&bbnokia5110, xflag);		
		break;
	      }
	    case SHIFTTST:
	      {
		printf("Shift Out Test ...\n");
		/* CE enabled */
		WP_digitalWrite(bbnokia5110.ce_wpin, GPIO_LOW, xflag);
		/* Set clock pin low ... */
		WP_digitalWrite(bbnokia5110.clk_wpin, GPIO_LOW, xflag);
		bbnokia5110.dcval = 0x55;
		printf("0x%02x ...\n", bbnokia5110.dcval);
		WP_BB_ShiftOutTest(&bbnokia5110, MSBFIRST, xflag);
		if (tflag) sleep(1);
		else delay(bbnokia5110.speed);
		bbnokia5110.dcval = 0x96;
		printf("0x%02x ...\n", bbnokia5110.dcval);
		WP_BB_ShiftOutTest(&bbnokia5110, MSBFIRST, xflag);
		/* Set clock pin high ... */
		WP_digitalWrite(bbnokia5110.clk_wpin, GPIO_HIGH, xflag);
		/* CE disabled */
		WP_digitalWrite(bbnokia5110.ce_wpin, GPIO_HIGH, xflag);
		break;
	      }
	    case RESETINIT:
	    case WCBTST:
	      {
		/* Based upon LCDInit from PCD8544,c */
		if (!tflag)
		  {
		    /* Reset ... */
		    /* CE enabled */
		    WP_digitalWrite(bbnokia5110.ce_wpin, GPIO_LOW, xflag);
		    /* ReSeT --> low */
		    WP_digitalWrite(bbnokia5110.rst_wpin, GPIO_LOW, xflag);
		    delay(500);
		    /* ReSeT --> high */
		    WP_digitalWrite(bbnokia5110.rst_wpin, GPIO_HIGH, xflag);
		    /* CE disabled */
		    WP_digitalWrite(bbnokia5110.ce_wpin, GPIO_HIGH, xflag);
		    delay(500);
		  }
		bbnokia5110.dcval = PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = PCD8544_SETBIAS | 0x04;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = PCD8544_SETVOP | convop;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = PCD8544_FUNCTIONSET;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		break;
	      }
	    case INVERSE:
	    case NORMAL:
	      {
		bbnokia5110.dcval = PCD8544_FUNCTIONSET;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = PCD8544_DISPLAYCONTROL | dispmode;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		break;
	      }
	    case SETCON0:
	    case SETCON1:
	    case SETCON2:
	    case SETCON3:
	    case SETCON4:
	    case SETCON5:
	    case SETCON6:
	    case SETCON7:
	    case SETCON8:
	      {
		bbnokia5110.dcval = PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = PCD8544_SETVOP | convop;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = PCD8544_FUNCTIONSET;
		WP_BB_WriteCommandByteMSB(&bbnokia5110, xflag);
		break;
	      }
	    case WDBTST:
	      {
		bbnokia5110.dcval = 0x1f;
		WP_BB_WriteDataByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = 0x05;
		WP_BB_WriteDataByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = 0x07;
		WP_BB_WriteDataByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = 0x00;
		WP_BB_WriteDataByteMSB(&bbnokia5110, xflag);
		bbnokia5110.dcval = 0x1f;
		WP_BB_WriteDataByteMSB(&bbnokia5110, xflag);
		break;
	      }
	    case WFDTST:
	      {
		WP_BB_ClearHomeMSB(&bbnokia5110, xflag);
		for (i=0; i<64; i++)
		  WP_BB_WriteFontDataBytesMSB(&bbnokia5110, i, font8x5, xflag);
		sleep(10);
		WP_BB_ClearHomeMSB(&bbnokia5110, xflag);
		for (i=64; i<128; i++)
		  WP_BB_WriteFontDataBytesMSB(&bbnokia5110, i, font8x5, xflag);
		sleep(10);
		WP_BB_ClearHomeMSB(&bbnokia5110, xflag);
		for (i=128; i<192; i++)
		  WP_BB_WriteFontDataBytesMSB(&bbnokia5110, i, font8x5, xflag);
		sleep(10);
		WP_BB_ClearHomeMSB(&bbnokia5110, xflag);
		for (i=192; i<256; i++)
		  WP_BB_WriteFontDataBytesMSB(&bbnokia5110, i, font8x5, xflag);
		break;
	      }
	    case WGRTST:
	      {
		WP_BB_ClearHomeMSB(&bbnokia5110, xflag);
		WP_BB_WriteGraphicsDataBytesMSB(&bbnokia5110, &bbnokia5110_buffer, xflag);
		break;
	      }
	    case WMSGTST:
	      {
		WP_BB_ClearHomeMSB(&bbnokia5110, xflag);
		sleep(1);
		strcpy(buffer4, bbnokia5110.name);
		strcat(buffer4, "   ");
		l = strlen(buffer4);
		message_buffer[LCDWIDTH - l] = NUL;
		strcat(buffer4, message_buffer);
		strcat(buffer4, getRCSFile(TRUE));
		strcat(buffer4, " Version ");
		strcat(buffer4, getSCMVersion());
		strcat(buffer4, "  ");
		strcat(buffer4, getRCSDate());
		printf("%s\n", buffer4);
		WP_BB_WriteMessageMSB(&bbnokia5110,
				      font8x5,
				      xflag,
				      buffer4);
		break;
	      }	    default: break;
	    }
	}

      if (spitype == FULLSPI)
	{
	  switch(demode)
	    {
	    case CLRHOME:
	      {
		WP_FS_ClearHome(&nokia5110, xflag);
		break;
	      }
	    case RESETTST:
	      {
		/* WP_GlcdReset_Test(nokia5110.rst_gpio, xflag); */
		WP_C2S_GlcdReset_Test(nokia5110.rst_gpio, xflag);
		break;
	      }
	    case RESETINIT:
	    case WCBTST:
	      {
		/* Based upon LCDInit from PCD8544,c */
		WP_C2S_GlcdReset_Test(nokia5110.rst_gpio, xflag);
		nokia5110.dcval = PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		nokia5110.dcval = PCD8544_SETBIAS | 0x04;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		nokia5110.dcval = PCD8544_SETVOP | convop;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		nokia5110.dcval = PCD8544_FUNCTIONSET;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		nokia5110.dcval = PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		break;
	      }
	    case INVERSE:
	    case NORMAL:
	      {
		nokia5110.dcval = PCD8544_FUNCTIONSET;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		nokia5110.dcval = PCD8544_DISPLAYCONTROL | dispmode;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		break;
	      }
	    case SETCON0:
	    case SETCON1:
	    case SETCON2:
	    case SETCON3:
	    case SETCON4:
	    case SETCON5:
	    case SETCON6:
	    case SETCON7:
	    case SETCON8:
	      {
		nokia5110.dcval = PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		nokia5110.dcval = PCD8544_SETVOP | convop;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		nokia5110.dcval = PCD8544_FUNCTIONSET;
		WP_FS_WriteCommandByte(&nokia5110, xflag);
		break;
	      }
	    case WDBTST:
	      {
		nokia5110.dcval = 0x1f;
		WP_FS_WriteDataByte(&nokia5110, xflag);
		nokia5110.dcval = 0x05;
		WP_FS_WriteDataByte(&nokia5110, xflag);
		nokia5110.dcval = 0x07;
		WP_FS_WriteDataByte(&nokia5110, xflag);
		nokia5110.dcval = 0x00;
		WP_FS_WriteDataByte(&nokia5110, xflag);
		nokia5110.dcval = 0x1f;
		WP_FS_WriteDataByte(&nokia5110, xflag);
		break;
	      }
	    case WFDTST:
	      {
		WP_FS_ClearHome(&nokia5110, xflag);
		for (i=0; i<64; i++)
		  WP_FS_WriteFontDataBytes(&nokia5110, i, font8x5, xflag);
		sleep(10);
		WP_FS_ClearHome(&nokia5110, xflag);
		for (i=64; i<128; i++)
		  WP_FS_WriteFontDataBytes(&nokia5110, i, font8x5, xflag);
		sleep(10);
		WP_FS_ClearHome(&nokia5110, xflag);
		for (i=128; i<192; i++)
		  WP_FS_WriteFontDataBytes(&nokia5110, i, font8x5, xflag);
		sleep(10);
		WP_FS_ClearHome(&nokia5110, xflag);
		for (i=192; i<256; i++)
		  WP_FS_WriteFontDataBytes(&nokia5110, i, font8x5, xflag);
		break;
	      }
	    case WGRTST:
	      {
		WP_FS_ClearHome(&nokia5110, xflag);
		WP_FS_WriteGraphicsDataBytes(&nokia5110, &fsnokia5110_buffer, xflag);
		break;
	      }
	    case WMSGTST:
	      {
		WP_FS_ClearHome(&nokia5110, xflag);
		sleep(1);
		strcpy(buffer4, nokia5110.name);
		strcat(buffer4, "     ");
		l = strlen(buffer4);
		message_buffer[LCDWIDTH - l] = NUL;
		strcat(buffer4, message_buffer);
		strcat(buffer4, getRCSFile(TRUE));
		strcat(buffer4, " Version ");
		strcat(buffer4, getSCMVersion());
		strcat(buffer4, "  ");
		strcat(buffer4, getRCSDate());
		printf("%s\n", buffer4);
		WP_FS_WriteMessage(&nokia5110,
				   font8x5,
				   xflag,
				   buffer4);
		break;
	      }
	    default: break;
	    }
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */

  if (spimode == SYSMODE)
    {
      if (spitype == BITBANG)
	{
	  /* CE disabled */
	  C2S_gpio_write(bbnokia5110.ce_wpin,
			 GPIO_HIGH,
			 xflag);
	  /* Data low */
	  C2S_gpio_write(bbnokia5110.din_wpin,
			 GPIO_LOW,
			 xflag);
	  /* Set clock pin high ... */
	  C2S_gpio_write(bbnokia5110.clk_wpin,
			 GPIO_HIGH,
			 xflag);
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
      if (spitype == BITBANG)
	{
	  /* CE disabled */
	  WP_digitalWrite(bbnokia5110.ce_wpin, GPIO_HIGH, xflag);
	  /* Data/¬Command = Command mode */
	  WP_digitalWrite(bbnokia5110.dc_wpin, GPIO_LOW, xflag);
	  /* Set data pin low ... */
	  WP_digitalWrite(bbnokia5110.din_wpin, GPIO_LOW, xflag);
	  /* Set clock pin high ... */
	  WP_digitalWrite(bbnokia5110.clk_wpin, GPIO_HIGH, xflag);
	}
      if (spitype == FULLSPI)
	{
	  /* Data/¬Command = Command mode */
	  WP_digitalWrite(nokia5110.dc_gpio, GPIO_LOW, xflag);
	  if (eflag || vflag) 
	    {
	      printf("ls -laF /sys/class/gpio/\n");
	      if (xflag) system("ls -laF /sys/class/gpio/");
	    }
	  C2S_gpio_unexportall(xflag);
	  if (wP_SPIfd[0] > 0) close(wP_SPIfd[0]);
	  if (wP_SPIfd[1] > 0) close(wP_SPIfd[1]);
	  if (eflag || vflag) 
	    {
	      printf("ls -laF /sys/class/gpio/\n");
	      if (xflag) system("ls -laF /sys/class/gpio/");
	      C2S_gpio("readall", xflag);
	    }
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: spi_glcd_demo.c,v $
 * Revision 1.2  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.22  2015/01/25 13:03:04  pi
 * -RI & -WMT options added. SCMVersion 0.05
 *
 * Revision 1.21  2015/01/16 23:29:02  pi
 * Interim commit
 *
 * Revision 1.20  2015/01/15 16:40:41  pi
 * Interim commit
 *
 * Revision 1.19  2015/01/15 14:04:58  pi
 * Beginning to move some Full Spi functions into spi_PCD8544.c
 *
 * Revision 1.18  2015/01/05 19:44:11  pi
 * -WGT option can now display XPM files imported via the -xpm option
 * SCMVersion 0.03
 *
 * Revision 1.17  2015/01/05 12:20:42  pi
 * Interim commit
 *
 * Revision 1.16  2015/01/04 20:59:28  pi
 * First test of -WGT option
 *
 * Revision 1.15  2015/01/03 22:57:15  pi
 * Interim commit
 *
 * Revision 1.14  2015/01/02 19:33:39  pi
 * All current options now working in both -BB and -FS modes.
 *
 * Revision 1.13  2015/01/01 23:06:10  pi
 * Main options now working in Full Spi mode.
 *
 * Revision 1.12  2015/01/01 22:13:41  pi
 * Interim commit. SCMVersion 0.02
 *
 * Revision 1.11  2014/12/30 16:06:20  pi
 * Interim commit
 *
 * Revision 1.10  2014/12/27 20:43:28  pi
 * Interim commit
 *
 * Revision 1.9  2014/12/23 23:17:06  pi
 * -WFT & -CH options added. Using faster clock
 *
 * Revision 1.8  2014/12/23 19:17:30  pi
 * Interim commit. SCMVersion 0.01
 *
 * Revision 1.7  2014/12/23 17:07:30  pi
 * -WCT followed by -WDT now results in a display!!!
 *
 * Revision 1.6  2014/12/23 16:44:56  pi
 * -WCT & -WDT options tested with LED test board
 *
 * Revision 1.5  2014/12/22 17:01:08  pi
 * Interim commit
 *
 * Revision 1.4  2014/12/21 21:55:15  pi
 * Interim commit
 *
 * Revision 1.3  2014/12/21 20:06:34  pi
 * Beginning to add bit-banged spi support for nokia glcd
 *
 * Revision 1.2  2014/07/28 22:14:16  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:13:46  pi
 * Initial revision
 *
 * Revision 1.7  2014/07/13 21:42:52  pi
 * Interim commit
 *
 * Revision 1.6  2014/07/13 20:42:44  pi
 * Interim commit
 *
 * Revision 1.5  2014/06/21 23:04:43  pi
 * Interim commit
 *
 * Revision 1.4  2014/06/20 20:35:04  pi
 * Interim commit
 *
 * Revision 1.3  2014/06/20 16:58:22  pi
 * Interim commit
 *
 * Revision 1.2  2014/06/19 21:57:27  pi
 * Interim commit
 *
 * Revision 1.1  2014/06/19 21:40:29  pi
 * Initial revision
 *
 *
 *
 */
