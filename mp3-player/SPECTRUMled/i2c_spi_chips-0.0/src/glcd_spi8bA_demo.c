/* GLCD (128x64 pixels - 12864B/ST7920), SPI-to-parallel 8-Bit 
 * (74HC595-based) interface via Adapter board, DEMOnstration code
 * $Id: glcd_spi8bA_demo.c,v 1.7 2018/03/31 21:32:45 pi Exp $
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
#include <strings.h>
#include <unistd.h>
#include <sys/utsname.h>
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
#include "lcd_disptools.h"
#include "glcd_xbmtools.h"

/* Uncomment to enable ICON RAM TEST */
/* #define ICONRAMTEST 1             */

/* RCS info blocks */
static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: glcd_spi8bA_demo.c,v $";
static char RCSId[]       = "$Id: glcd_spi8bA_demo.c,v 1.7 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.7 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.00 $";

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
  XBMFILE,   /* -xbm filename[.xbm]  */
  XPMFILE,   /* -xpm filename[.xpm]  */
  XBMDUMP,   /* +XBM                 */
  XPMDUMP,   /* +XPM                 */
  COMMENT,   /* * {in argfile only!} */
  ECHO,      /* -E                   */
  FONT0816,  /* -F0816               */
  FONT1616,  /* -F1616               */
  GFNT1616,  /* -FG1616              */
  GLCDNIDX,  /* -glcd# n             */
  IFNT1616,  /* -FI1616              */
  UCGRAM0,   /* +UCG0                */
  UCGRAM2,   /* +UCG2                */
  UCGRAM4,   /* +UCG4                */
  UCGRAM6,   /* +UCG6                */
#ifdef HAVE_WIRINGPI_H
  FSI8BTST,  /* -F8BT                */
  INITGPIO,  /* -IG                  */
  STRBTST,   /* -ST                  */
  WDTATST,   /* -WDT                 */
  WRGRTST,   /* -WRT                 */
#ifdef HAVE_WIRINGPISPI_H
  CLRHOME,   /* -CH                  */
  FSI8XTST,  /* -F8XT                */
  GDSWTST,   /* -GDST                */
#ifdef ICONRAMTEST
  IRSWTST,   /* -IRST                */
#endif /* ICONRAMTEST */
  REVLINE1,  /* -R1                  */
  REVLINE2,  /* -R2                  */
  WLINTST,   /* -WLT                 */
  WMSGTST,   /* -WMT                 */
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  QUIET,     /* -Q                   */
  SYSMODE,   /* -S                   */
  TSTMODE,   /* -T                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPI_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,spimode,udgcmode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[XBMFILE]  = "-xbm";
  arglist[XPMFILE]  = "-xpm";
  arglist[XBMDUMP]  = "+XBM";
  arglist[XPMDUMP]  = "+XPM";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[FONT0816] = "-F0816";
  arglist[FONT1616] = "-F1616";
  arglist[GFNT1616] = "-FG1616";
  arglist[GLCDNIDX] = "-glcd#";
  arglist[IFNT1616] = "-FI1616";
  arglist[UCGRAM0]  = "+UCG0";
  arglist[UCGRAM2]  = "+UCG2";
  arglist[UCGRAM4]  = "+UCG4";
  arglist[UCGRAM6]  = "+UCG6";
#ifdef HAVE_WIRINGPI_H
  arglist[FSI8BTST] = "-F8BT";
  arglist[INITGPIO] = "-IG";
  arglist[STRBTST]  = "-ST";
  arglist[WDTATST]  = "-WDT";
  arglist[WRGRTST]  = "-WRT";
#ifdef HAVE_WIRINGPISPI_H
  arglist[CLRHOME]  = "-CH";
  arglist[FSI8XTST] = "-F8XT";
  arglist[GDSWTST]  = "-GDST";
#ifdef ICONRAMTEST
  arglist[IRSWTST]  = "-IRST";
#endif /* ICONRAMTEST */
  arglist[REVLINE1] = "-R1";
  arglist[REVLINE2] = "-R2";
  arglist[WLINTST]  = "-WLT";
  arglist[WMSGTST]  = "-WMT";
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  arglist[QUIET]    = "-Q";
  arglist[SYSMODE]  = "-S";
  arglist[TSTMODE]  = "-T";
  arglist[VERBOSE]  = "-V";
#ifdef HAVE_WIRINGPI_H
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPI_H */
  arglist[XECUTE]   = "-X";
  arglist[LAST_ARG] = "";
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
char eflag,igflag,oflag,tflag,vflag,xflag,xisflag;
char message_buffer[BUF_LEN];
int glcdhash;

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
	    case XBMFILE: 
	      {
		make_name(xbmfile, "xbm", buffer2);
		Open_Xbm_File();
		break;
	      }
	    case XPMFILE: 
	      {
		make_name(xpmfile, "xpm", buffer2);
		Open_Xpm_File("r");
		break;
	      }
	    case XBMDUMP: demode=XBMDUMP;   break;
	    case XPMDUMP: demode=XPMDUMP;   break;
	    case COMMENT: printf("%s\n", buffer4); break;
	    case ECHO:    eflag=TRUE;       break;
	    case FONT0816: glcdfont=HC8X16; break;
	    case FONT1616: glcdfont=C16X16; break;
	    case GFNT1616: glcdfont=G16X16; break;
	    case IFNT1616: glcdfont=I16X16; break;
	    case UCGRAM0: udgcmode=UCGRAM0; break;
	    case UCGRAM2: udgcmode=UCGRAM2; break;
	    case UCGRAM4: udgcmode=UCGRAM4; break;
	    case UCGRAM6: udgcmode=UCGRAM6; break;
#ifdef HAVE_WIRINGPI_H
	    case FSI8BTST: demode=FSI8BTST; break;
	    case INITGPIO: igflag=TRUE;     break;
	    case STRBTST:  demode=STRBTST;  break;
	    case WDTATST:  demode=WDTATST;  break;
	    case WRGRTST:  demode=WRGRTST;  break;
#ifdef HAVE_WIRINGPISPI_H
	    case CLRHOME:  demode=CLRHOME;  spimode=WPIMODE; break;
	    case FSI8XTST: demode=FSI8XTST; spimode=WPIMODE; break;
	    case GDSWTST:  demode=GDSWTST;  spimode=WPIMODE; break;
#ifdef ICONRAMTEST
	    case IRSWTST:  demode=IRSWTST;  spimode=WPIMODE; break;
#endif /* ICONRAMTEST */
	    case REVLINE1: demode=REVLINE1; spimode=WPIMODE; break;
	    case REVLINE2: demode=REVLINE2; spimode=WPIMODE; break;
	    case WLINTST:
	      {
		glcdfont=HC8X16;
		demode=WLINTST;
		spimode=WPIMODE;
		break;
	      }
	    case WMSGTST:
	      {
		glcdfont=HC8X16;
		demode=WMSGTST;
		spimode=WPIMODE;
		break;
	      }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;      break;
	    case SYSMODE: spimode=SYSMODE;  break;
	    case TSTMODE: tflag=TRUE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPI_H
	    case WPIMODE: spimode=WPIMODE; break;
#endif /* HAVE_WIRINGPI_H */
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
  strcat(buffer1, "         -xbm xbm_filename[.xbm] (for 128x64 graphic),\n");
  strcat(buffer1, "         -xpm xpm_filename[.xpm] (for 16x16 CGRAM char.),\n");
  strcat(buffer1, "         +XBM     {XBM dump of pixel store},\n");
  strcat(buffer1, "         +XPM     {XPM dump of pixel store},\n");
  strcat(buffer1, "         -E       {Echo command-line arguments},\n");
  strcat(buffer1, "         -F0816   {use half-character, 8x16 Font (default)},\n");
  strcat(buffer1, "         -F1616   {use 16x16 Font#},\n");
  strcat(buffer1, "         -FG1616  {use user-defined Graphic 16x16 Font#},\n");
  strcat(buffer1, "         -FI1616  {use chinese Icon 16x16 Font#},\n");
  strcat(buffer1, "                  {# for -WDT option only},\n");
  strcat(buffer1, "         -glcd# n {Use GLCD model name #n (0 < n < 4)},\n");
  strcat(buffer1, "         +UCGn    {Use CGram address 000n (n=0,2,4 or 6) ...},\n");
  strcat(buffer1, "                  {... for xpm file data},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -F8BT {Func set init 8-bit, Basic instruction set Test*},\n");
  strcat(buffer1, "         -IG   {Initialise used gpio's & Gpio readall},\n");
  strcat(buffer1, "         -ST   {Strobe Test*},\n");
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -WDT  {Write Data Test*},\n");
  strcat(buffer1, "         -WRT  {Write Register Test*},\n");
  strcat(buffer1, "               {*NB: LED Test board req'd with -S},\n");
  strcat(buffer1, "         -CH   {Clear display & return Home - implies -W},\n");
  strcat(buffer1,
	 "         -F8XT {Func set init 8-bit, eXtended instr. set Test - implies -W},\n");
  strcat(buffer1, "         -GDST {Graphics Display Switch Test - implies -W},\n");
#ifdef ICONRAMTEST
  strcat(buffer1, "         -IRST {IRam Switch Test - implies -W},\n");
#endif /* ICONRAMTEST */
  strcat(buffer1, "         -Rn   {Reverse line n (n=1|2) - implies -W},\n");
  strcat(buffer1, "         -WLT  {Write Line info. Test - implies -W & -F0816},\n");
  strcat(buffer1, "         -WMT  {Write Message Test - implies -W & -F0816},\n");
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q    {Quiet - default},\n");
#ifndef NO_SUDO_ECHO_TO_SPI
  strcat(buffer1, "         -S    {use System call methods** - default},\n");
#else /* NO_SUDO_ECHO_TO_SPI */
  strcat(buffer1, "         -S    {use System call methods - default},\n");
#endif /* NO_SUDO_ECHO_TO_SPI */
  strcat(buffer1, "         -T    {using LED Test board},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -W    {use Wiring pi spi methods},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.\n");
#ifndef NO_SUDO_ECHO_TO_SPI
  strcat(buffer1, " ** NB: Uses \"sudo echo ...\" to the SPI device!");
#endif /* NO_SUDO_ECHO_TO_SPI */
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
void C2S_Spi2pGlcdStrobe_Test(glcd_spi2p_8bit any_spi2p_8bit,
			      char qx)
{
  sleep(1);
  /* Enable enabled */
  C2S_gpio_g_write(any_spi2p_8bit.glcd_e_gpio,
		   GPIO_HIGH,
		   qx);
  sleep(1);
  /* Enable disabled */
  C2S_gpio_g_write(any_spi2p_8bit.glcd_e_gpio,
		   GPIO_LOW,
		   qx);
}

void C2S_Spi2pGlcdWriteRegister_Test(glcd_spi2p_8bit any_spi2p_8bit,
				     unsigned int reg,
				     char qx)
{
  C2S_gpio_g_write(any_spi2p_8bit.glcd_rs_gpio,
		   GPIO_LOW,
		   qx);
  C2S_sechob2spidev(any_spi2p_8bit.glcd_data.spidev,
		    reg,
		    qx);
  C2S_Spi2pGlcdStrobe_Test(any_spi2p_8bit, qx);
}

void C2S_Spi2pGlcdWriteData_Test(glcd_spi2p_8bit any_spi2p_8bit,
				 unsigned int hdata,
				 unsigned int ldata,
				 char qx)
/* NB: 16-bit data ie. two bytes */
{
  C2S_gpio_g_write(any_spi2p_8bit.glcd_rs_gpio,
		   GPIO_HIGH,
		   qx);
  C2S_sechob2spidev(any_spi2p_8bit.glcd_data.spidev,
		    hdata,
		    qx);
  C2S_Spi2pGlcdStrobe_Test(any_spi2p_8bit, qx);
  delay(any_spi2p_8bit.glcd_elmin);  
  C2S_sechob2spidev(any_spi2p_8bit.glcd_data.spidev,
		    ldata,
		    qx);
  C2S_Spi2pGlcdStrobe_Test(any_spi2p_8bit, qx);
}

void WP_Spi2pGlcdStrobe(glcd_spi2p_8bit *any_spi2p_8bit,
			char qx)
{
  unsigned int lwait;
  char uorm;
  uorm = TRUE;
  /* if ((*any_spi2p_8bit).glcd_wait > (*any_spi2p_8bit).glcd_ehmin) */
  if ((*any_spi2p_8bit).glcd_wait > (*any_spi2p_8bit).glcd_elmin)
    lwait = (*any_spi2p_8bit).glcd_wait;
  /* else lwait = (*any_spi2p_8bit).glcd_ehmin; */
  else lwait = (*any_spi2p_8bit).glcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  WP_digitalWrite((*any_spi2p_8bit).glcd_e_gpio, GPIO_HIGH, qx);
  delayMicroseconds((*any_spi2p_8bit).glcd_ehmin);
  WP_digitalWrite((*any_spi2p_8bit).glcd_e_gpio, GPIO_LOW, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

#ifdef HAVE_WIRINGPISPI_H
void WP_Spi2pGlcdWriteRegister(glcd_spi2p_8bit *any_spi2p_8bit,
			       unsigned int reg,
			       char qx)
{
  unsigned int old_glcd_wait;
  /* ¬Register Select ... */
  WP_digitalWrite((*any_spi2p_8bit).glcd_rs_gpio, GPIO_LOW, qx);
  /* Write REGister command to 74HC595 ... */
  WP_write_byte(*(*any_spi2p_8bit).glcd_data.spifd, reg, qx);
  /* Enable 74HC595 outputs ... */
  WP_digitalWrite((*any_spi2p_8bit).glcd_data.goe_gpio,
		  GPIO_LOW,
		  qx);
  if (reg == GLCD_HOME)
    {
      old_glcd_wait = (*any_spi2p_8bit).glcd_wait;
      /* (*any_spi2p_8bit).glcd_wait = 1600 + old_glcd_wait; */
      /* (*any_spi2p_8bit).glcd_wait = 1920 + old_glcd_wait; /* + 20% */
      (*any_spi2p_8bit).glcd_wait = (*any_spi2p_8bit).glcd_elmax; 
    }
  WP_Spi2pGlcdStrobe(any_spi2p_8bit, qx);
  /* Reset glcd_wait? */
  if (reg == GLCD_HOME) (*any_spi2p_8bit).glcd_wait = old_glcd_wait;
  /* Tr-state the 74HC595 outputs ... */
  WP_digitalWrite((*any_spi2p_8bit).glcd_data.goe_gpio,
		  GPIO_HIGH,
		  qx);
}

void WP_Spi2pGlcdWriteData(glcd_spi2p_8bit *any_spi2p_8bit,
			   unsigned int hdata,
			   unsigned int ldata,
			   char qx)
/* NB: 16-bit data ie. two bytes */
{
  /* data mode ... */ 
  WP_digitalWrite((*any_spi2p_8bit).glcd_rs_gpio, GPIO_HIGH, qx);
  /* Write DATA to 74HC595 ... */
  WP_write_byte(*(*any_spi2p_8bit).glcd_data.spifd, hdata, qx);
  /* Enable 74HC595 outputs ... */
  WP_digitalWrite((*any_spi2p_8bit).glcd_data.goe_gpio,
		  GPIO_LOW,
		  qx);
  WP_Spi2pGlcdStrobe(any_spi2p_8bit, qx);
  /* Tr-state the 74HC595 outputs ... */
  WP_digitalWrite((*any_spi2p_8bit).glcd_data.goe_gpio,
		  GPIO_HIGH,
		  qx);
  delayMicroseconds((*any_spi2p_8bit).glcd_elmin);
  /* Write DATA to 74HC595 ... */
  WP_write_byte(*(*any_spi2p_8bit).glcd_data.spifd, ldata, qx);
  /* Enable 74HC595 outputs ... */
  WP_digitalWrite((*any_spi2p_8bit).glcd_data.goe_gpio,
		  GPIO_LOW,
		  qx);
  WP_Spi2pGlcdStrobe(any_spi2p_8bit, qx);
  /* Tr-state the 74HC595 outputs ... */
  WP_digitalWrite((*any_spi2p_8bit).glcd_data.goe_gpio,
		  GPIO_HIGH,
		  qx);
}

void WP_Spi2pGlcdHomeClear(glcd_spi2p_8bit any_spi2p_8bit,
			   char qx)
{ 
  unsigned int lregr;
  lregr = GLCD_DISPLAY_OFF;
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
  lregr = GLCD_DISPLAY_ON_CURSOR_BLINK;
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
  lregr = GLCD_CLEAR;
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
  lregr = GLCD_HOME;
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
  lregr = GLCD_SET_DDRAM;
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
  lregr = GLCD_CURSOR_UP_NO_SHIFT;
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
}

void WP_Spi2pGlcdCursorShiftRight(glcd_spi2p_8bit any_spi2p_8bit,
				  int nshift,
				  char qx)
{
  int l,n;
  unsigned int spiregr;
  spiregr = GLCD_CURSOR_SHIFT_RIGHT;
  if (nshift > 64) n = nshift % 64;
  else n = nshift;
  if (n > 0)
    {
      for (l=0; l<n; l++)
	{
	  /* delay(10); */
	  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, spiregr, qx);
	}
    }
}

void WP_Spi2pGlcdMoveToLine(glcd_spi2p_8bit any_spi2p_8bit,
			    int nline,
			    char qx)
/* NB: Should be preceded by a call to WP_Spi2pGlcdHomeClear() */
{
  unsigned int lregr;
  switch(nline)
    {
    case 1:
      {
	lregr = GLCD_SET_DDRAM + 
	  (any_spi2p_8bit.glcd_ddram_idx[1] & GLCD_DDRAM_ADDR_MASK);
	WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
	break;
      }
    case 2:
      {
	lregr = GLCD_SET_DDRAM + 
	  (any_spi2p_8bit.glcd_ddram_idx[2] & GLCD_DDRAM_ADDR_MASK);
	WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
	WP_Spi2pGlcdCursorShiftRight(any_spi2p_8bit, 8, qx);
	break;
      }
    case 3:
      {
	lregr = GLCD_SET_DDRAM + 
	  (any_spi2p_8bit.glcd_ddram_idx[3] & GLCD_DDRAM_ADDR_MASK);
	WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
	WP_Spi2pGlcdCursorShiftRight(any_spi2p_8bit, 8, qx);
	break;
      }
    case 0:
    default: break;
    }
}

void WP_Spi2pGlcdWriteMessage(glcd_spi2p_8bit any_spi2p_8bit,
			      char qx,
			      char *message)
{
  int i,k1,k2,k3;
  unsigned int hdata,ldata,lregr;
  char lmsg[65],*lp1;
  strncpy(lmsg, message, 64);
  lmsg[64] = NUL;
  lp1 = lmsg;
  k1 = any_spi2p_8bit.glcd_nc;
  k2 = k1 + any_spi2p_8bit.glcd_nc;
  k3 = k2 + any_spi2p_8bit.glcd_nc;
  i = 0;
  while (*lp1 != NUL)
    {
      hdata = toascii((int)(*lp1));
      lp1++;
      i++;
      ldata = toascii((int)(*lp1));
      WP_Spi2pGlcdWriteData(&any_spi2p_8bit, hdata, ldata, qx);
      lp1++;
      i++;
      if (i == k1)
	{
	  lregr = 
	    GLCD_SET_DDRAM + (any_spi2p_8bit.glcd_ddram_idx[1] & GLCD_DDRAM_ADDR_MASK);
	  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
	}
      if (i == k2)
	{
	  lregr = 
	    GLCD_SET_DDRAM + (any_spi2p_8bit.glcd_ddram_idx[2] & GLCD_DDRAM_ADDR_MASK);
	  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
	  WP_Spi2pGlcdCursorShiftRight(any_spi2p_8bit, 8, qx);
	}
      if (i == k3)
	{
	  lregr = 
	    GLCD_SET_DDRAM + (any_spi2p_8bit.glcd_ddram_idx[3] & GLCD_DDRAM_ADDR_MASK);
	  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit, lregr, qx);
	  WP_Spi2pGlcdCursorShiftRight(any_spi2p_8bit, 8, qx);
	}
    }
}

void WP_Spi2pGlcdWriteGraphicsData(glcd_spi2p_8bit any_spi2p_8bit,
				   qc12864b_pixel8 gdbuffer,
				   char qx)
{
  unsigned char *ucp1,*ucp2;
  unsigned int hdata,ldata,lregr;
  int i,j,k;
  k = (QC12864B_width / 8);
  ucp1 = gdbuffer.block11;
  ucp2 = gdbuffer.block13;
  for (j=0; j<16; j++)
    {
      /* Vertical */
      lregr = GLCDX_SET_GRAM_ADDR + j;
      WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit,
				lregr,
				qx);
      /* Horizontal */
      lregr = GLCDX_SET_GRAM_ADDR;
      WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit,
				lregr,
				qx);
      for (i=0; i<k; i=i+2)
	{
	  hdata = *ucp1;
	  ucp1++;
	  ldata = *ucp1;
	  ucp1++;
	  WP_Spi2pGlcdWriteData(&any_spi2p_8bit, hdata, ldata, qx);
	}
      for (i=0; i<k; i=i+2)
	{
	  hdata = *ucp2;
	  ucp2++;
	  ldata = *ucp2;
	  ucp2++;
	  WP_Spi2pGlcdWriteData(&any_spi2p_8bit, hdata, ldata, qx);
	}
    }
  ucp1 = current_qc12864b_buffer.block22;
  ucp2 = current_qc12864b_buffer.block24;
  for (j=16; j<32; j++)
    {
      /* Vertical */
      lregr = GLCDX_SET_GRAM_ADDR + j;
      WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit,
				lregr,
				qx);
      /* Horizontal */
      lregr = GLCDX_SET_GRAM_ADDR;
      WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit,
				lregr,
				qx);
      for (i=0; i<k; i=i+2)
	{
	  hdata = *ucp1;
	  ucp1++;
	  ldata = *ucp1;
	  ucp1++;
	  WP_Spi2pGlcdWriteData(&any_spi2p_8bit, hdata, ldata, qx);
	}
      for (i=0; i<k; i=i+2)
	{
	  hdata = *ucp2;
	  ucp2++;
	  ldata = *ucp2;
	  ucp2++;
	  WP_Spi2pGlcdWriteData(&any_spi2p_8bit, hdata, ldata, qx);
	}
    }
}

void WP_Spi2pGlcdLoadCgramData(glcd_spi2p_8bit any_spi2p_8bit,
			       qc12864b_udgc cgram_buffer,
			       char qx)
{
  int l;
  /* First ensure that VSCROLL is disabled ... */
  /* Extended mode ... */
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit,
			    GLCD_FUNC_SET_8BIT_EXTENDED,
			    qx);
  /* Allow IRAM or CGRAM (VSCROLL disabled) */
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit,
			    GLCDX_IRAM_ADDR_SET,
			    qx);
  /* Back to basic mode */
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit,
			    GLCD_FUNC_SET_8BIT_BASIC,
			    qx);
  delay(10);
  /* Set CGRAM address ... */
  WP_Spi2pGlcdWriteRegister(&any_spi2p_8bit,
			    cgram_buffer.cgram_addr,
			    qx);
  /* ... and write 16 rows ... */
  for (l=0; l<16; l++)
    WP_Spi2pGlcdWriteData(&any_spi2p_8bit, 
			  cgram_buffer.hbyte_rows[l], 
			  cgram_buffer.lbyte_rows[l], 
			  qx);
}
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  unsigned char *ucp1,*ucp2;
  char *p1,*p2;
  char spidev;
  char xpmlflag;
  int i,j,k,l;
  int npix,npix8;
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPISPI_H
  int *myspifd;
  int wP_SPIfd[2];
  unsigned int datah,datal,spidata,spiregr;
#endif /* HAVE_WIRINGPISPI_H */

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
  eflag    = igflag = tflag = vflag = xflag = xisflag = xpmlflag = FALSE;
  demode   = INITGPIO; /* Dummy mode */
  spidev   = SPI_DEV00;
  spimode  = SYSMODE;
  glcdfont = HC8X16;
  glcdhash = -1; /* Not (yet) defined */
  udgcmode = UCGRAM0;
  init_pixel_store(&current_qc12864b_buffer);
#ifdef HAVE_WIRINGPISPI_H
  myspifd = &wP_SPIfd[0]; /* == SPI_DEV00 */
  wP_SPIfd[0] = wP_SPIfd[1] = -1; /* ie. as yet undefined */
#endif /* HAVE_WIRINGPISPI_H */
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
	    case XBMFILE: 
	      {
		make_name(xbmfile, "xbm", p2);
		Open_Xbm_File();
		break;
	      }
	    case XPMFILE: 
	      {
		make_name(xpmfile, "xpm", p2);
		Open_Xpm_File("r");
		break;
	      }
	    case XBMDUMP:  demode=XBMDUMP;  i--; break;
	    case XPMDUMP:  demode=XPMDUMP;  i--; break;
	    case ECHO:     eflag=TRUE;      i--; break;
	    case FONT0816: glcdfont=HC8X16; i--; break;
	    case FONT1616: glcdfont=C16X16; i--; break;
	    case GFNT1616: glcdfont=G16X16; i--; break;
	    case GLCDNIDX: glcdhash=atoi(p2); break;
	    case IFNT1616: glcdfont=I16X16; i--; break;
	    case UCGRAM0: udgcmode=UCGRAM0; i--; break;
	    case UCGRAM2: udgcmode=UCGRAM2; i--; break;
	    case UCGRAM4: udgcmode=UCGRAM4; i--; break;
	    case UCGRAM6: udgcmode=UCGRAM6; i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
	    case CLRHOME:  demode=CLRHOME;  spimode=WPIMODE; i--; break;
	    case FSI8XTST: demode=FSI8XTST; spimode=WPIMODE; i--; break;
	    case GDSWTST:  demode=GDSWTST;  spimode=WPIMODE; i--; break;
#ifdef ICONRAMTEST
	    case IRSWTST:  demode=IRSWTST;  spimode=WPIMODE; i--; break;
#endif /* ICONRAMTEST */
	    case REVLINE1: demode=REVLINE1; spimode=WPIMODE; i--; break;
	    case REVLINE2: demode=REVLINE2; spimode=WPIMODE; i--; break;
	    case WLINTST:
	      {
		glcdfont=HC8X16;
		demode=WLINTST;
		spimode=WPIMODE;
		i--;
		break;
	      }
	    case WMSGTST:
	      {
		glcdfont=HC8X16;
		demode=WMSGTST;
		spimode=WPIMODE;
		i--;
		break;
	      }
#endif /* HAVE_WIRINGPISPI_H */
	    case FSI8BTST: demode=FSI8BTST; i--; break;
	    case INITGPIO: igflag=TRUE;     i--; break;
	    case STRBTST:  demode=STRBTST;  i--; break;
	    case WDTATST:  demode=WDTATST;  i--; break;
	    case WRGRTST:  demode=WRGRTST;  i--; break;
#ifdef HAVE_WIRINGPISPI_H
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;      i--; break;
	    case SYSMODE: spimode=SYSMODE;  i--; break;
	    case TSTMODE: tflag=TRUE; i--;  break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case WPIMODE: spimode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPI_H */
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

  /* Load an XBM file if declared and opened ... */
  if (xbmfopen)
    {
      if (vflag) printf("Loading XBM file ...\n");
      Read_Xbm_Header(vflag);
      npix8 = Read_Xbm_Pixel8_Data(&current_qc12864b_buffer, vflag);
      if (vflag) printf("%d Pixel8_Data bytes read ...\n", npix8);
      if (vflag) printf("XBM file read ...\n");
      Close_Xbm_File();
      if (vflag) printf("XBM file closed ...\n");
    }

  /* Load an XPM file if declared and opened ... */
  if (xpmfopen)
    {
      Read_Xpm_Header(vflag);
      Read_Xpm_Header_Data(vflag);
      xpmtype = Check_Xpm_Header();
      printf("XPM file type = %s\n", xbmkeylist[xpmtype]);
      switch(udgcmode)
	{
	case UCGRAM2:
	  {
	    npix = Read_Xpm_Udgc_Data(&cgram0002,
				      GLCD_SET_CGRAM_0002,
				      vflag);
	    break;
	  }
	case UCGRAM4:
	  {
	    npix = Read_Xpm_Udgc_Data(&cgram0004,
				      GLCD_SET_CGRAM_0004,
				      vflag);
	    break;
	  }
	case UCGRAM6:
	  {
	    npix = Read_Xpm_Udgc_Data(&cgram0006,
				      GLCD_SET_CGRAM_0006,
				      vflag);
	    break;
	  }
	default:
	case UCGRAM0:
	  {
	    npix = Read_Xpm_Udgc_Data(&cgram0000,
				      GLCD_SET_CGRAM_0000,
				      vflag);
	    break;
	  }
	}
      if (npix > 0) xpmlflag = TRUE;
      Close_Xpm_File();
    }

  /* +XBM or +XPM option leads to an even earlier exit, after the dump! */
  if (demode == XBMDUMP)
    {
      if (xbmfopen) fclose(xbmfn);
      xbm_dump_pixel_store(&current_qc12864b_buffer);
      exit(EXIT_SUCCESS);
    }

  if (demode == XPMDUMP)
    {
      if (xbmfopen) fclose(xbmfn);
      xpm_dump_pixel_store(&current_qc12864b_buffer);
      exit(EXIT_SUCCESS);
    }

  /* -IG option leads to an early exit after the system call ... */
  if (igflag)
    {
      init_glcdnamelist(vflag);
      if ((glcdhash >= 0) && (glcdhash < ST7920_GLCD_MODULE_NC)) current_glcd_nli = glcdhash;
      init_current_spi2p_8bit(&current_spi2p_8bit,
			      22,
			      27,
			      GPIO_UNASSIGNED,
			      17,
			      18,
			      spidev,
			      glcdnamelist[current_glcd_nli]);
      echo_any_spi2p_8bit_status(&current_spi2p_8bit);
      echo_pixel_store_status(&current_qc12864b_buffer);
#ifdef HAVE_WIRINGPI_H
      C2S_gpio_g_mode(current_spi2p_8bit.glcd_data.goe_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_spi2p_8bit.glcd_data.scl_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_spi2p_8bit.glcd_rs_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_spi2p_8bit.glcd_e_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* Tri-state the 74HC595 outputs ... */
      C2S_gpio_g_write(current_spi2p_8bit.glcd_data.goe_gpio,
		       GPIO_HIGH,
		       xflag);
      /* 74HC595 Shift register CL inactive ... */
      C2S_gpio_g_write(current_spi2p_8bit.glcd_data.scl_gpio,
		       GPIO_HIGH,
		       xflag);
      /* Register Select = instruction register */
      C2S_gpio_g_write(current_spi2p_8bit.glcd_rs_gpio,
		       GPIO_LOW,
		       xflag);
      /* Enable disabled */
      C2S_gpio_g_write(current_spi2p_8bit.glcd_e_gpio,
		       GPIO_LOW,
		       xflag);
      C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
      if (xbmfopen) fclose(xbmfn);
      exit(EXIT_SUCCESS);
    }
  
  /* Execute other options */
  /* Initialisation ... */
  init_glcdnamelist(vflag);
  if ((glcdhash >= 0) && (glcdhash < ST7920_GLCD_MODULE_NC)) current_glcd_nli = glcdhash;
  init_current_spi2p_8bit(&current_spi2p_8bit,
			  22,
			  27,
			  GPIO_UNASSIGNED,
			  17,
			  18,
			  spidev,
			  glcdnamelist[current_glcd_nli]);
  echo_any_spi2p_8bit_status(&current_spi2p_8bit);
  echo_pixel_store_status(&current_qc12864b_buffer);
  if (glcdfont == G16X16)
    {
      if (!xpmlflag) 
	{
	  preset_cgram_store(&cgram0000,
			     GLCD_SET_CGRAM_0000,
			     0x01,
			     0x01);
	  preset_cgram_store(&cgram0002,
			     GLCD_SET_CGRAM_0002,
			     0x07,
			     0x07);
	  preset_cgram_store(&cgram0004,
			     GLCD_SET_CGRAM_0004,
			     0x1f,
			     0x1f);
	  preset_cgram_store(&cgram0006,
			     GLCD_SET_CGRAM_0006,
			     0x7f,
			     0x7f);
	}
    }

  if (spimode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
#ifdef HAVE_WIRINGPI_H
      switch(demode)
	{
	case FSI8BTST:
	case STRBTST:
	case WDTATST:
	case WRGRTST:
	  {
	    C2S_gpio_g_mode(current_spi2p_8bit.glcd_data.goe_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_spi2p_8bit.glcd_data.scl_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_spi2p_8bit.glcd_rs_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_spi2p_8bit.glcd_e_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    /* Enable the 74HC595 outputs ... */
	    C2S_gpio_g_write(current_spi2p_8bit.glcd_data.goe_gpio,
			     GPIO_LOW,
			     xflag);
	    /* 74HC595 Shift register CL inactive ... */
	    C2S_gpio_g_write(current_spi2p_8bit.glcd_data.scl_gpio,
			     GPIO_HIGH,
			     xflag);
	    /* Register Select = instruction register */
	    C2S_gpio_g_write(current_spi2p_8bit.glcd_rs_gpio,
			     GPIO_LOW,
			     xflag);
	    /* Enable disabled */
	    C2S_gpio_g_write(current_spi2p_8bit.glcd_e_gpio,
			     GPIO_LOW,
			     xflag);
	    break;
	  }
	default: break;
	}
#endif /* HAVE_WIRINGPI_H */
    }

#ifdef HAVE_WIRINGPI_H
  if (spimode == WPIMODE)
    {  
      switch(demode)
	{
	case FSI8BTST:
	case STRBTST:
	case WDTATST:
	case WRGRTST:
#ifdef HAVE_WIRINGPISPI_H
	case CLRHOME:
	case FSI8XTST:
	case GDSWTST:
#ifdef ICONRAMTEST
	case IRSWTST:
#endif /* ICONRAMTEST */
	case REVLINE1:
	case REVLINE2:
	case WLINTST:
	case WMSGTST:
#endif /* HAVE_WIRINGPISPI_H */
	  {
	    printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
	    C2S_gpio_export(current_spi2p_8bit.glcd_rs_gpio,
			    GPIO_XOUT,
			    xflag);
	    C2S_gpio_export(current_spi2p_8bit.glcd_e_gpio,
			    GPIO_XOUT,
			    xflag);
	    if (current_spi2p_8bit.glcd_data.goe_gpio != GPIO_UNASSIGNED)
	      C2S_gpio_export(current_spi2p_8bit.glcd_data.goe_gpio,
			      GPIO_XOUT,
			      xflag);
	    if (current_spi2p_8bit.glcd_data.scl_gpio != GPIO_UNASSIGNED)
	      C2S_gpio_export(current_spi2p_8bit.glcd_data.scl_gpio,
			      GPIO_XOUT,
			      xflag);
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
	    C2S_gpio("exports", xflag);
	    if (eflag || vflag) 
	      {
		printf("ls -laF /sys/class/gpio/\n");
		if (xflag) system("ls -laF /sys/class/gpio/");
	      }
	    /* Enable the 74HC595 outputs ... */
	    WP_digitalWrite(current_spi2p_8bit.glcd_data.goe_gpio,
			    GPIO_LOW,
			    xflag);
	    /* 74HC595 Shift register CL inactive ... */
	    WP_digitalWrite(current_spi2p_8bit.glcd_data.scl_gpio,
			    GPIO_HIGH,
			    xflag);
	    /* Register Select = instruction register */
	    WP_digitalWrite(current_spi2p_8bit.glcd_rs_gpio,
			    GPIO_LOW,
			    xflag);
	    /* Enable disabled */
	    WP_digitalWrite(current_spi2p_8bit.glcd_e_gpio,
			    GPIO_LOW,
			    xflag);
	    break;
	  }
	default: break;
	}
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
	  current_spi2p_8bit.glcd_data.spifd = myspifd;
	  printf("*current_spi2p_8bit.glcd_data.spifd = %d\n",
		 *current_spi2p_8bit.glcd_data.spifd);
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  
  /* Demonstration ... */
  if (spimode == SYSMODE)
    {
      switch(demode)
	{
#ifdef HAVE_WIRINGPI_H
	case FSI8BTST:
	  {
	    C2S_Spi2pGlcdWriteRegister_Test(current_spi2p_8bit,
					    GLCD_FUNC_SET_8BIT_BASIC,
					    xflag);
	    delay(100);
	    C2S_Spi2pGlcdWriteRegister_Test(current_spi2p_8bit,
					    GLCD_FUNC_SET_8BIT_BASIC,
					    xflag);
	    delay(37);
	    C2S_Spi2pGlcdWriteRegister_Test(current_spi2p_8bit,
					    GLCD_DISPLAY_ON_CURSOR_BLINK,
					    xflag);
	    delay(100);
	    C2S_Spi2pGlcdWriteRegister_Test(current_spi2p_8bit,
					    GLCD_CLEAR,
					    xflag);
	    sleep(1);
	    C2S_Spi2pGlcdWriteRegister_Test(current_spi2p_8bit,
					    GLCD_CURSOR_UP_NO_SHIFT,
					    xflag);
	    sleep(2);
	    break;
	  }
	case STRBTST:
	  {
	    C2S_Spi2pGlcdStrobe_Test(current_spi2p_8bit, xflag);
	    break;
	  }
	case WDTATST:
	  {
	    for (i=0; i<16; i=i+2)
	      {
		j = i +0x30;
		k = j + 1;
		C2S_Spi2pGlcdWriteData_Test(current_spi2p_8bit, j, k, xflag);
	      }
	    break;
	  }
	case WRGRTST:
	  {
	    C2S_Spi2pGlcdWriteRegister_Test(current_spi2p_8bit,
					    GLCD_DISPLAY_OFF,
					    xflag);
	    C2S_Spi2pGlcdWriteRegister_Test(current_spi2p_8bit,
					    GLCD_DISPLAY_ON_CURSOR_BLINK,
					    xflag);
	    C2S_Spi2pGlcdWriteRegister_Test(current_spi2p_8bit,
					    GLCD_CLEAR,
					    xflag);
	    break;
	  }
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
      if (tflag)
	{
	  current_spi2p_8bit.glcd_wait = 500000; /* Wait for 500000 micro-seconds */
	  echo_any_spi2p_8bit_status(&current_spi2p_8bit);
	}
      switch(demode)
	{
	case CLRHOME:
	  {
	    WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
	    break;
	  }
	case FSI8BTST:
	  {
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_FUNC_SET_8BIT_BASIC,
				      xflag);
	    delayMicroseconds(100);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_FUNC_SET_8BIT_BASIC,
				      xflag);
	    delayMicroseconds(37);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_DISPLAY_ON_CURSOR_BLINK,
				      xflag);
	    delayMicroseconds(100);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_CLEAR,
				      xflag);
	    delay(11); /* ie. > 10ms */
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_CURSOR_UP_NO_SHIFT,
				      xflag);
	    sleep(5);
	    break;
	  }
	case FSI8XTST:
	  {
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_FUNC_SET_8BIT_EXTENDED,
				      xflag);
	    xisflag = TRUE;
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_STAND_BY,
				      xflag);
	    sleep(4);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_REVERSE_L1,
				      xflag);
	    sleep(2);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_REVERSE_L1,
				      xflag);
	    sleep(4);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_REVERSE_L2,
				      xflag);
	    sleep(2);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_REVERSE_L2,
				      xflag);
	    /* Lines 3 & 4 are not available! */
	    /*
	    sleep(2);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_REVERSE_L3,
				      xflag);
	    sleep(1);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_REVERSE_L3,
				      xflag);
	    sleep(2);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_REVERSE_L4,
				      xflag);
	    sleep(1);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_REVERSE_L4,
				      xflag);
	    */
	    sleep(5);
	    break;
	  }
	case GDSWTST:
	  {
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_DISPLAY_ON_NO_CURSOR,
				      xflag);
	    /* Extended mode ... */
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_FUNC_SET_8BIT_EXTENDED,
				      xflag);
	    xisflag = TRUE;
	    /* Allow IRAM or CGRAM (VSCROLL disabled) */
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_IRAM_ADDR_SET,
				      xflag);
	    sleep(1);
	    echo_pixel_store_status(&current_qc12864b_buffer);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_FUNC_SET_8BIT_GD_ON,
				      xflag);
	    sleep(1);
	    WP_Spi2pGlcdWriteGraphicsData(current_spi2p_8bit,
					  current_qc12864b_buffer,
					  xflag);
	    /* Dummy write to clear the QH* LED ...
	    WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
	    sleep(20);
	    fill_pixel_store(&current_qc12864b_buffer);
	    echo_pixel_store_status(&current_qc12864b_buffer);
	    WP_Spi2pGlcdWriteGraphicsData(current_spi2p_8bit,
					  current_qc12864b_buffer,
					  xflag);
	    sleep(5);
	    /* Dummy write to clear the QH* LED ...
	    WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
	    clear_pixel_store(&current_qc12864b_buffer);
	    echo_pixel_store_status(&current_qc12864b_buffer);
	    WP_Spi2pGlcdWriteGraphicsData(current_spi2p_8bit,
					  current_qc12864b_buffer,
					  xflag);
	    sleep(1);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_FUNC_SET_8BIT_GD_OFF,
				      xflag);
	    sleep(1);
	    break;
	  }
#ifdef ICONRAMTEST
	case IRSWTST:
	  {
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_DISPLAY_ON_NO_CURSOR,
				      xflag);
	    sleep(1);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_FUNC_SET_8BIT_EXTENDED,
				      xflag);
	    xisflag = TRUE;
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCDX_IRAM_ADDR_SET,
				      xflag);
	    sleep(1);
	    spiregr = GLCDX_SET_IRAM_VSCROLL + (0x04 & GLCDX_IRAM_ADDR_MASK);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      spiregr,
				      xflag);
	    WP_Spi2pGlcdWriteData(&current_spi2p_8bit, 0x00, 0xff, xflag);
	    WP_Spi2pGlcdWriteData(&current_spi2p_8bit, 0x00, 0xff, xflag);
	    WP_Spi2pGlcdWriteData(&current_spi2p_8bit, 0x00, 0xff, xflag);
	    WP_Spi2pGlcdWriteData(&current_spi2p_8bit, 0x00, 0xff, xflag);
	    WP_Spi2pGlcdWriteData(&current_spi2p_8bit, 0x00, 0xff, xflag);
	    WP_Spi2pGlcdWriteData(&current_spi2p_8bit, 0x00, 0xff, xflag);
	    WP_Spi2pGlcdWriteData(&current_spi2p_8bit, 0x00, 0xff, xflag);
	    sleep(1);
	    WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag);
	    break;
	  }
#endif /* ICONRAMTEST */	  
	case REVLINE1:
	case REVLINE2:
	  {
	    /* Extended mode ... */
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_FUNC_SET_8BIT_EXTENDED,
				      xflag);
	    xisflag = TRUE;
	    if (demode == REVLINE1) spiregr = GLCDX_REVERSE_L1;
	    else spiregr = GLCDX_REVERSE_L2;
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit, spiregr, xflag);
	    break;
	  }
	case STRBTST:
	  {
	    WP_Spi2pGlcdStrobe(&current_spi2p_8bit, xflag);
	    break;
	  }
	case WDTATST:
	  {
	    WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
	    switch(glcdfont)
	      {
	      case HC8X16:
		{
		  /* Revised, 128 x half-character test ... */
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<16; i=i+2)
			{
			  j = i + (l * 16);
			  k = j + 1;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<16; i=i+2)
			{
			  j = i + (l * 16) + 64;
			  k = j + 1;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  break;
		}
	      case C16X16:
		/* Basic, four line, full-character test */
		{
		  j = 0xa1;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x20;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x40;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x60;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  break;
		}
	      case G16X16:
		{
		  /* Load the characters ... */
		  WP_Spi2pGlcdLoadCgramData(current_spi2p_8bit,
					    cgram0000,
					    xflag);
		  WP_Spi2pGlcdLoadCgramData(current_spi2p_8bit,
					    cgram0002,
					    xflag);
		  WP_Spi2pGlcdLoadCgramData(current_spi2p_8bit,
					    cgram0004,
					    xflag);
		  WP_Spi2pGlcdLoadCgramData(current_spi2p_8bit,
					    cgram0006,
					    xflag);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  /* Basic, four line, four chars. (repeated) test */
		  j = 0;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = (i *2) % 8;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  break;
		}
	      case I16X16:
		/* Basic, four line, chinese Icon full-character test */
		{
		  j = 0xa1;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8);
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  j = 0xd7;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x20;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  j = 0xd7;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x40;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  j = 0xd7;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x60;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  j = 0xf7;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x20;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  j = 0xf7;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x40;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  sleep(10);
		  WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
		  j = 0xf7;
		  for (l=0; l<4; l++)
		    {
		      WP_Spi2pGlcdMoveToLine(current_spi2p_8bit, l, xflag);
		      sleep(1);
		      for (i=0; i<8; i++)
			{
			  k = i + (l * 8) + 0x60;
			  WP_Spi2pGlcdWriteData(&current_spi2p_8bit, j, k, xflag);
			}
		    }
		  /* Dummy write to clear the QH* LED ...
		  WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
		  break;
		}
	      default: break;
	      }
	    break;
	  }
	case WLINTST:
	  {
	    WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
	    sleep(1);
	    sprintf(message_buffer, 
		    "0x%02x SD HC0816#1", 
		    current_spi2p_8bit.glcd_ddram_idx[0]);
	    WP_Spi2pGlcdWriteMessage(current_spi2p_8bit,
				     xflag,
				     message_buffer);
	    spiregr = GLCD_SET_DDRAM + 
	      (current_spi2p_8bit.glcd_ddram_idx[1] & GLCD_DDRAM_ADDR_MASK);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit, spiregr, xflag);
	    sprintf(message_buffer, 
		    "0x%02x SD HC0816#2", 
		    current_spi2p_8bit.glcd_ddram_idx[1]);
	    WP_Spi2pGlcdWriteMessage(current_spi2p_8bit,
				     xflag,
				     message_buffer);
	    spiregr = GLCD_SET_DDRAM + 
	      (current_spi2p_8bit.glcd_ddram_idx[2] & GLCD_DDRAM_ADDR_MASK);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit, spiregr, xflag);
	    sprintf(message_buffer, 
		    "0x%02x SD+CSRx8 #3", 
		    current_spi2p_8bit.glcd_ddram_idx[2]);
	    WP_Spi2pGlcdCursorShiftRight(current_spi2p_8bit, 8, xflag);
	    WP_Spi2pGlcdWriteMessage(current_spi2p_8bit,
				     xflag,
				     message_buffer);
	    spiregr = GLCD_SET_DDRAM + 
	      (current_spi2p_8bit.glcd_ddram_idx[3] & GLCD_DDRAM_ADDR_MASK);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit, spiregr, xflag);
	    WP_Spi2pGlcdCursorShiftRight(current_spi2p_8bit, 8, xflag);
	    sprintf(message_buffer, 
		    "0x%02x SD+CSRx8 #4", 
		    current_spi2p_8bit.glcd_ddram_idx[3]);
	    WP_Spi2pGlcdWriteMessage(current_spi2p_8bit,
				     xflag,
				     message_buffer);
	    spiregr = GLCD_HOME;
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit, spiregr, xflag);
	    break;
	  }
	case WMSGTST:
	  {
	    WP_Spi2pGlcdHomeClear(current_spi2p_8bit, xflag);
	    sleep(1);
	    message_buffer[32] = NUL;
	    strcpy(buffer4, current_spi2p_8bit.glcd_data.name);
	    l = strlen(buffer4);
	    do
	      {
		if (l < 16) strcat(buffer4, " ");
		l = strlen(buffer4);
	      }
	    while (l<16);
	    /* strcat(buffer4, "HC0816"); */
	    strcat(buffer4, getRCSFile(TRUE));
	    l = strlen(buffer4);
	    do
	      {
		if (l < 32) strcat(buffer4, " ");
		l = strlen(buffer4);
	      }
	    while (l<32);
	    /* strcat(buffer4, "Character set.  Running on:     "); */
	    strcat(buffer4, getRCSDate());
	    strcat(buffer4, " ");
	    strcat(buffer4, message_buffer);
	    WP_Spi2pGlcdWriteMessage(current_spi2p_8bit,
				     xflag,
				     buffer4);
	    break;
	  }
	case WRGRTST:
	  {
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_DISPLAY_OFF,
				      xflag);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_DISPLAY_ON_CURSOR_BLINK,
				      xflag);
	    WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				      GLCD_CLEAR,
				      xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  
  /* Tidy up ... */

#ifdef HAVE_WIRINGPI_H
  if (spimode == SYSMODE)
    {
      /* Tri-state the 74HC595 outputs ... */
      C2S_gpio_g_write(current_spi2p_8bit.glcd_data.goe_gpio,
		       GPIO_HIGH,
		       xflag);
      /* 74HC595 Shift register CL active ... */
      C2S_gpio_g_write(current_spi2p_8bit.glcd_data.scl_gpio,
		       GPIO_LOW,
		       xflag);
      /* Register Select = instruction register */
      C2S_gpio_g_write(current_spi2p_8bit.glcd_rs_gpio,
		       GPIO_LOW,
		       xflag);
      /* Enable disabled */
      C2S_gpio_g_write(current_spi2p_8bit.glcd_e_gpio,
		       GPIO_LOW,
		       xflag);
    }
  
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
      /* Are we still using the eXtended Instruction Set? */
      if (xisflag)
	{
	  WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				    GLCD_FUNC_SET_8BIT_BASIC,
				    xflag);
	  delay(10);
	}
      WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				GLCD_HOME,
				xflag);
      WP_Spi2pGlcdWriteRegister(&current_spi2p_8bit,
				GLCD_DISPLAY_ON_CURSOR_BLINK,
				xflag);
      /* Enable disabled */
      WP_digitalWrite(current_spi2p_8bit.glcd_e_gpio,
		      GPIO_LOW,
		      xflag);
      /* Register Select = instruction register */
      WP_digitalWrite(current_spi2p_8bit.glcd_rs_gpio,
		      GPIO_LOW,
		      xflag);
      /* Tr-state the 74HC595 outputs ... */
      WP_digitalWrite(current_spi2p_8bit.glcd_data.goe_gpio,
		      GPIO_HIGH,
		      xflag);
      /* 74HC595 Shift register CL active ... */
      /*
      WP_digitalWrite(current_spi2p_8bit.glcd_data.scl_gpio,
		      GPIO_LOW,
		      xflag);
      */
      /* Dummy write to clear the QH* LED ...
      WP_write_byte(*current_spi2p_8bit.glcd_data.spifd, 0xff, xflag); */
      if (eflag || vflag) 
	{
	  printf("ls -laF /sys/class/gpio/\n");
	  if (xflag) system("ls -laF /sys/class/gpio/");
	}
      C2S_gpio_unexportall(xflag);
      if (wP_SPIfd[0] > 0)
	{
	  if (close(wP_SPIfd[0]) < 0) perror("close");
	}
      if (wP_SPIfd[1] > 0)
	{
	  if (close(wP_SPIfd[1]) < 0) perror("close");
	}
      if (eflag || vflag) 
	{
	  printf("ls -laF /sys/class/gpio/\n");
	  if (xflag) system("ls -laF /sys/class/gpio/");
	  C2S_gpio("readall", xflag);
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  if (xbmfopen) fclose(xbmfn);
  if (xpmfopen) fclose(xpmfn);
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: glcd_spi8bA_demo.c,v $
 * Revision 1.7  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.6  2017/09/11 20:28:18  pi
 * Interim Commit
 *
 * Revision 1.5  2017/09/11 19:54:49  pi
 * -glcd# n option added
 *
 * Revision 1.4  2017/09/11 19:06:04  pi
 * Interim Commit
 *
 * Revision 1.3  2017/09/09 19:36:35  pi
 * -WMT option tweaked
 *
 * Revision 1.2  2017/09/08 16:51:33  pi
 * Interim commit, partially working
 *
 * Revision 1.1  2017/09/08 13:16:28  pi
 * Initial revision
 *
 *
 *
 */

