/* Adafruit I2C (HT16K33) interfaced 8x8 LED matriX DEMOnstration code
 * $Id: i2c_ledx_demo.c,v 1.24 2018/03/31 21:32:45 pi Exp $
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
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_WIRINGPI_H
#include <wiringPi.h>
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPII2C_H
#include <wiringPiI2C.h>
#endif /* HAVE_WIRINGPII2C_H */
#ifdef HAVE_PIGPIOD_IF2_H
#include "pigpiod_wraps.h"
#endif /* HAVE_PIGPIOD_IF2_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "font.h"
#include "jace128.h"
#include "PCD8544_font.h"
#include "parsel.h"
#include "i2c_chips.h"
#include "i2c_wraps.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: i2c_ledx_demo.c,v $";
static char RCSId[]       = "$Id: i2c_ledx_demo.c,v 1.24 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.24 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.02 $";

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
  COMMENT,   /* * {in argfile only!} */
  ECHO,      /* -E                   */
  INQUIRE,   /* -I                   */
  BLNKTST,   /* -BIT                 */
  DIMMTST,   /* -DIT                 */
  FULLTST,   /* -FIT                 */
  FNT8X8,    /* -F8x8                */
  FNT8X5R,   /* -F8x5R               */
  FNT8X8JR,  /* -F8x8JR              */
  FNTTST1,   /* -FT1                 */
  FNTTST2,   /* -FT2                 */
  FNTTST3,   /* -FT3                 */
  MX08IN,    /* -MX08                */
  MX12IN,    /* -MX12                */
  MXKEYS,    /* -MXKY                */
  QUIKTST,   /* -QIT                 */
  QUICTST,   /* -QCT                 */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  SYSMODE,   /* -S                   */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIOD_IF2_H
  PIGDMODE,  /* -PGD                 */
#endif /* HAVE_PIGPIOD_IF2_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,fonttype,i2cmode,mxmodel;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[INQUIRE]  = "-I";
  arglist[BLNKTST]  = "-BIT";
  arglist[DIMMTST]  = "-DIT";
  arglist[FULLTST]  = "-FIT";
  arglist[FNT8X8]   = "-F8x8";
  arglist[FNT8X5R]  = "-F8x5R";
  arglist[FNT8X8JR] = "-F8x8JR";
  arglist[FNTTST1]  = "-FT1";
  arglist[FNTTST2]  = "-FT2";
  arglist[FNTTST3]  = "-FT3";
  arglist[MX08IN]   = "-MX08";
  arglist[MX12IN]   = "-MX12";
  arglist[MXKEYS]   = "-MXKY";
  arglist[QUIKTST]  = "-QIT";
  arglist[QUICTST]  = "-QCT";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  arglist[SYSMODE]  = "-S";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIOD_IF2_H
  arglist[PIGDMODE] = "-PGD";
#endif /* HAVE_PIGPIOD_IF2_H */
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
char eflag,iqflag,vflag,xflag;
unsigned int mxaddr;

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
	    case ARGFILE:  break;
	    case COMMENT:  printf("%s\n", buffer4); break;
	    case ECHO:     eflag=TRUE;        break;
	    case INQUIRE:  iqflag=TRUE;       break;
	    case BLNKTST:  demode=BLNKTST;    i2cmode=SYSMODE; break;
	    case DIMMTST:  demode=DIMMTST;    i2cmode=SYSMODE; break;
	    case FULLTST:  demode=FULLTST;    i2cmode=SYSMODE; break;
	    case FNT8X8:   fonttype=FNT8X8;   break;
	    case FNT8X5R:  fonttype=FNT8X5R;  break;
	    case FNT8X8JR: fonttype=FNT8X8JR; break;
	    case FNTTST1:  demode=FNTTST1;    break;
	    case FNTTST2:  demode=FNTTST2;    break;
	    case FNTTST3:  demode=FNTTST3;    break;
	    case MX08IN:   mxmodel=MX08IN;    mxaddr=0x70; break;
	    case MX12IN:   mxmodel=MX12IN;    mxaddr=0x72; break;
	    case MXKEYS:   mxmodel=MXKEYS;    mxaddr=0x77; break;
	    case QUIKTST:  demode=QUIKTST;    i2cmode=SYSMODE; break;
	    case QUICTST:  demode=QUICTST;    i2cmode=SYSMODE; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case SYSMODE:  i2cmode=SYSMODE; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE:  i2cmode=WPIMODE; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIOD_IF2_H
	    case PIGDMODE: i2cmode=PIGDMODE; break;
#endif /* HAVE_PIGPIOD_IF2_H */
	    case XECUTE:   xflag=TRUE; break;
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
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -E      {Echo command-line arguments},\n");
  strcat(buffer1, "         -I      {Inquire about i2c devices},\n");
  strcat(buffer1, "         -MX08   {using 0.8in led MatriX module (default)},\n");
  strcat(buffer1, "         -MX12   {using 1.2in led MatriX module},\n");
  strcat(buffer1, "         -MXKY   {using KeYestudio 1.2in led MatriX module},\n");
  strcat(buffer1, "         -BIT    {Blink Initialisation Test},\n");
  strcat(buffer1, "         -DIT    {Dimming Initialisation Test},\n");
  strcat(buffer1, "         -FIT    {Full Initialisation Test},\n");
  strcat(buffer1, "         -F8x8   {use 8x8 Font - default},\n");
  strcat(buffer1, "         -F8x5R  {use Rotated 8x5 Font (PCD8544 font)},\n");
  strcat(buffer1, "         -F8x8JR {use Rotated 8x8 Font (Jupiter ACE font)},\n");
  strcat(buffer1, "         -FT1    {Font Test #1},\n");
  strcat(buffer1, "         -FT2    {Font Test #2},\n");
  strcat(buffer1, "         -FT3    {Font Test #3},\n");
  strcat(buffer1, "         -QIT    {Quick Initialisation Test},\n");
  strcat(buffer1, "         -QCT    {Quick font Conversion Test},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -S      {use System call methods - default},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -W      {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIOD_IF2_H
  strcat(buffer1, "         -PGD    {use PiGpioD i2c methods^^},\n");
  strcat(buffer1, "                 {^^NB: requires a running pigpio daemon!},\n");
#endif /* HAVE_PIGPIOD_IF2_H */
  strcat(buffer1, "         -X      {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

void C2S_Clear_8x8_Display(ht16k33 *any_8x8,
			   char qx)
{
  (*any_8x8).ssr = SSR_NORMAL;
  C2S_i2cout(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ssr,
	     qx);
  (*any_8x8).dspr = (DSPR_ADDR & DISPOFFMSK);
  C2S_i2cout(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).dspr,
	     qx);
  (*any_8x8).ddcb = ALLCOLOFF;
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
}

void C2S_Fill_8x8_Display(ht16k33 *any_8x8,
			  char qs,
			  char qx)
{
  (*any_8x8).ssr = SSR_NORMAL;
  C2S_i2cout(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ssr,
	     qx);
  (*any_8x8).dspr = (DSPR_ADDR | DISPONMSK);
  C2S_i2cout(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).dspr,
	     qx);
  (*any_8x8).ddcb = ALLCOLON;
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     qx);
}

void C2S_Load_8x8_Display(ht16k33 *any_8x8,
			  char qx)
{
  (*any_8x8).ddcb = (*any_8x8).ccfont[0];
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[1];
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[2];
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[3];
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[4];
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[5];
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[6];
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[7];
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
  C2S_i2cset(PI_B2,
	     (*any_8x8).addr,
	     (*any_8x8).ddap,
	     (*any_8x8).ddcb,
	     xflag);
}

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
void WP_Clear_8x8_Display(ht16k33 *any_8x8,
			  wpi2cvb reg,
			  char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  (*any_8x8).ssr = SSR_NORMAL;
  lreg.data = (*any_8x8).ssr;
  WP_i2cout(lreg, xflag);
  (*any_8x8).dspr = (DSPR_ADDR & DISPOFFMSK);
  lreg.data = (*any_8x8).dspr;
  WP_i2cout(lreg, xflag);
  (*any_8x8).ddcb = ALLCOLOFF;
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
}

void WP_Fill_8x8_Display(ht16k33 *any_8x8,
			 wpi2cvb reg,
			 char qs,
			 char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  (*any_8x8).ssr = SSR_NORMAL;
  lreg.data = (*any_8x8).ssr;
  WP_i2cout(lreg, xflag);
  (*any_8x8).dspr = (DSPR_ADDR | DISPONMSK);
  lreg.data = (*any_8x8).dspr;
  WP_i2cout(lreg, xflag);
  (*any_8x8).ddcb = ALLCOLON;
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
   lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
}

void WP_Load_8x8_Display(ht16k33 *any_8x8,
			 wpi2cvb reg,
			 char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  (*any_8x8).ddcb = (*any_8x8).ccfont[0];
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[1];
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[2];
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[3];
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[4];
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[5];
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[6];
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[7];
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
  lreg.data = (*any_8x8).ddcb;
  lreg.reg  = (*any_8x8).ddap;
  WP_i2cset(lreg, xflag);
}
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_PIGPIOD_IF2_H
void PGD_Clear_8x8_Display(ht16k33 *any_8x8,
			   pgdi2cvb pgdi2c,
			   char qx)
{
  pgdi2cvb lpgdi2c;
  lpgdi2c.handle = pgdi2c.handle;
  (*any_8x8).ssr = SSR_NORMAL;
  pgdi2c.bVal = (*any_8x8).ssr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_8x8).dspr = (DSPR_ADDR & DISPOFFMSK);
  pgdi2c.bVal = (*any_8x8).dspr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_8x8).ddcb = ALLCOLOFF;
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
}

void PGD_Fill_8x8_Display(ht16k33 *any_8x8,
			  pgdi2cvb pgdi2c,
			  char qs,
			  char qx)
{
  pgdi2cvb lpgdi2c;
  lpgdi2c.handle = pgdi2c.handle;
  (*any_8x8).ssr = SSR_NORMAL;
  pgdi2c.bVal = (*any_8x8).ssr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_8x8).dspr = (DSPR_ADDR | DISPONMSK);
  pgdi2c.bVal = (*any_8x8).dspr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_8x8).ddcb = ALLCOLON;
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
}

void PGD_Load_8x8_Display(ht16k33 *any_8x8,
			  pgdi2cvb pgdi2c,
			  char qx)
{
  pgdi2cvb lpgdi2c;
  lpgdi2c.handle = pgdi2c.handle;
  (*any_8x8).ddcb = (*any_8x8).ccfont[0];
  (*any_8x8).ddap = (DDAP_ADDR + ROW0ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[1];
  (*any_8x8).ddap = (DDAP_ADDR + ROW1ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[2];
  (*any_8x8).ddap = (DDAP_ADDR + ROW2ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[3];
  (*any_8x8).ddap = (DDAP_ADDR + ROW3ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[4];
  (*any_8x8).ddap = (DDAP_ADDR + ROW4ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[5];
  (*any_8x8).ddap = (DDAP_ADDR + ROW5ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[6];
  (*any_8x8).ddap = (DDAP_ADDR + ROW6ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_8x8).ddcb = (*any_8x8).ccfont[7];
  (*any_8x8).ddap = (DDAP_ADDR + ROW7ADDR);
  pgdi2c.bVal = (*any_8x8).ddcb;
  pgdi2c.i2cReg = (*any_8x8).ddap;
  PGD_i2cset(pgdi2c, xflag);
}
#endif /* HAVE_PIGPIOD_IF2_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char *myfont;
  char mxname[CHIP_NAME_LEN];
  int i,j,k,l,mca;
  int bite,data[16];
  int mrow;
  ht16k33 yellow;
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#ifdef HAVE_WIRINGPII2C_H
  wpi2cvb i2c_chip;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIOD_IF2_H
  pgdi2cvb pgdi2c;
#endif /* HAVE_PIGPIOD_IF2_H */
  
  setup_RCSlist();

  /* printf("getRCSVersion reports %s\n", getRCSVersion()); */

  /* Initialise argument list etc. */
  setupargs();

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
  eflag    = vflag = xflag = FALSE;
  demode   = LAST_ARG;
  i2cmode  = SYSMODE;
  fonttype = FNT8X8;
  mxaddr   = 0x70;
#ifdef HAVE_PIGPIOD_IF2_H
  pgdi2c.thispi = -1; /* Not yet determined */
  pgdi2c.handle = PGD_UNDEF_HANDLE;
#endif /* HAVE_PIGPIOD_IF2_H */
  make_name(argfile, "arg", getRCSFile(FALSE));

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
	    case ECHO:     eflag=TRUE;        i--; break;
	    case INQUIRE:  iqflag=TRUE;       i--; break;
	    case BLNKTST:  demode=BLNKTST;    i--; break;
	    case DIMMTST:  demode=DIMMTST;    i--; break;
	    case FULLTST:  demode=FULLTST;    i--; break;
	    case FNTTST1:  demode=FNTTST1;    i--; break;
	    case FNTTST2:  demode=FNTTST2;    i--; break;
	    case FNTTST3:  demode=FNTTST3;    i--; break;
	    case FNT8X8:   fonttype=FNT8X8;   i--; break;
	    case FNT8X5R:  fonttype=FNT8X5R;  i--; break;
	    case FNT8X8JR: fonttype=FNT8X8JR; i--; break;
	    case MX08IN:   mxmodel=MX08IN;    mxaddr=0x70; i--; break;
	    case MX12IN:   mxmodel=MX12IN;    mxaddr=0x72; i--; break;
	    case MXKEYS:   mxmodel=MXKEYS;    mxaddr=0x77; i--; break;
	    case QUIKTST:  demode=QUIKTST;    i--; break;
	    case QUICTST:  demode=QUICTST;    i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE:  i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case SYSMODE:  i2cmode=SYSMODE; i--; break;
#ifdef HAVE_PIGPIOD_IF2_H
	    case PIGDMODE: i2cmode=PIGDMODE; i--; break;
#endif /* HAVE_PIGPIOD_IF2_H */
	    case XECUTE:   xflag=TRUE; i--; break;
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

  switch(mxmodel)
    {
    case MX12IN:
      {
	strcpy(mxname, "ADA30mm8x8LEDMatrix");
	break;
      }
    case MXKEYS:
      {
	strcpy(mxname, "KEYS30mm8x8LEDMatrix");
	break;
      }
    case MX08IN:
    default:
      {
	strcpy(mxname, "ADA20mm8x8LEDMatrix");
	break;
      }
    }
  
  /* -I option leads to an early exit after the system call ... */
  if (iqflag)
    {  
      if (!xflag)
	{
	  mca = init_ht16k33(&yellow,
			     mxaddr,
			     MTX_8X8,
			     mxname);
	  echo_ht16k33_status(yellow);
	  /*
	  convert_ht16k33_font(&yellow);
	  echo_ht16k33_status(yellow);
	  */
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  mca = init_ht16k33(&yellow,
		     mxaddr,
		     MTX_8X8,
		     mxname);
  echo_ht16k33_status(yellow);
  switch(fonttype)
    {
    case FNT8X5R:
      {
	myfont = font8x5;
	mrow   = font8x5Width;
	break;
      }     
    case FNT8X8JR:
      {
	myfont = jace128;
	mrow   = jace128Width;
	break;
      }     
    case FNT8X8:
    default:
      {
	myfont = font;
	mrow   = fontHeight;
	break;
      }
    }

  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      printf("Using wiringPiI2C methods: ...\n");
      i2c_chip.did = (int)(yellow.addr);
      if ((i2c_chip.fd=wiringPiI2CSetup(i2c_chip.did)) < 0)
	{
	  fprintf(stderr,
		  "Could not setup i2c_chip.did %d!",
		  i2c_chip.did);
	  perror("wiringPiI2CSetup");
	  exit(EXIT_FAILURE);
	}
      else printf("I2C Device 0x%02x has i2c_chip.fd of %d\n",
		  i2c_chip.did, i2c_chip.fd);
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_PIGPIOD_IF2_H
  if (i2cmode == PIGDMODE)
    {
      printf("Using pigpiod I2C methods:\n");
      if ((pgdi2c.thispi = pigpio_start(NULL, NULL) < 0))
	{
	  fprintf(stderr, "Could not start pigpiod I/Face! ...\n");
	  perror("pigpiostart");
	  exit(EXIT_FAILURE);
	}
      else
	{
	  printf("pigpiod I/Face connected to pi#%d ...\n", pgdi2c.thispi);
	  pgdi2c.i2cbus   = PI_B2;
	  pgdi2c.i2caddr  = (int)(yellow.addr);
	  pgdi2c.i2cflags = 0; /* Currently always set to zero - see pigpio.h */
	  if ((pgdi2c.handle = i2c_open(pgdi2c.thispi,
					pgdi2c.i2cbus,
					pgdi2c.i2caddr,
					pgdi2c.i2cflags)) < 0)
	    {
	      fprintf(stderr, "Could not open I2C Device %d:0x%2x!",
		      pgdi2c.i2cbus,
		      pgdi2c.i2caddr);
	      perror("i2c_open");
	      exit(EXIT_FAILURE);
	    }
	  else printf("I2C Device %d:0x%2x has pgdi2c.handle of %d\n",
		      pgdi2c.i2cbus,
		      pgdi2c.i2caddr,
		      pgdi2c.handle);
	}
    }
#endif /* HAVE_PIGPIOD_IF2_H */
  
  /* Demonstration/Test ... */
  if (i2cmode == SYSMODE)
    {
      switch(demode)
	{
	case BLNKTST:
	  {
	    C2S_Clear_8x8_Display(&yellow, xflag);
	    C2S_Fill_8x8_Display(&yellow, FALSE, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK20HZ);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    sleep(4);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);	    
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK10HZ);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    sleep(8);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK05HZ);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    sleep(16);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    break;
	  }
	case DIMMTST:
	  {
	    C2S_Clear_8x8_Display(&yellow, xflag);
	    C2S_Fill_8x8_Display(&yellow, FALSE, xflag);
	    yellow.dmsr = (DIMSETMASK | DIM01_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM02_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM03_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM04_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM05_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM06_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM07_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM08_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM09_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM10_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM11_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM12_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM13_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM14_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM15_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM16_16);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dmsr,
		       xflag);
	    break;
	  }
	case FULLTST:
	  {
	    C2S_Clear_8x8_Display(&yellow, xflag);
	    C2S_Fill_8x8_Display(&yellow, TRUE, xflag);	    
	    break;
	  }
	case FNTTST1:
	  {
	    C2S_Clear_8x8_Display(&yellow, xflag);
	    yellow.ssr = SSR_NORMAL;
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.ssr,
		       xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    for (i=0; i<10; i++)
	      {
		j = i + 48;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		C2S_Load_8x8_Display(&yellow, xflag);
		sleep(1);
		echo_ht16k33_status(yellow);
	      }
	    break;
	  }
	case FNTTST2:
	  {
	    C2S_Clear_8x8_Display(&yellow, xflag);
	    yellow.ssr = SSR_NORMAL;
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.ssr,
		       xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		C2S_Load_8x8_Display(&yellow, xflag);
		sleep(1);
	      }
	    echo_ht16k33_status(yellow);
	    break;
	  }
	case FNTTST3:
	  {
	    C2S_Clear_8x8_Display(&yellow, xflag);
	    yellow.ssr = SSR_NORMAL;
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.ssr,
		       xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		C2S_Load_8x8_Display(&yellow, xflag);
		sleep(1);
	      }
	    echo_ht16k33_status(yellow);
	    break;
	  }
	case QUIKTST:
	  {
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.ssr,
		       xflag);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    sleep(10);
	    yellow.ssr = SSR_STANDBY;
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.ssr,
		       xflag);	    
	    break;
	  }
	case QUICTST:
	  {
	    C2S_Clear_8x8_Display(&yellow, xflag);
	    yellow.ssr = SSR_NORMAL;
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.ssr,
		       xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    C2S_i2cout(PI_B2,
		       yellow.addr,
		       yellow.dspr,
		       xflag);
	    sleep(1);
	    convert_ht16k33_font(&yellow);
	    C2S_Load_8x8_Display(&yellow, xflag);
	    sleep(10);
	    echo_ht16k33_status(yellow);
	    break;
	  }
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      switch(demode)
	{
	case BLNKTST:
	  {
	    WP_Clear_8x8_Display(&yellow, i2c_chip, xflag);
	    WP_Fill_8x8_Display(&yellow, i2c_chip, FALSE, xflag);	    
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK20HZ);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(4);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK10HZ);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(8);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK05HZ);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(16);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    break;
	  }
	case DIMMTST:
	  {
	    WP_Clear_8x8_Display(&yellow, i2c_chip, xflag);
	    WP_Fill_8x8_Display(&yellow, i2c_chip, FALSE, xflag);	    
	    yellow.dmsr = (DIMSETMASK | DIM01_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM02_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM03_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM04_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM05_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM06_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM07_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM08_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM09_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM10_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM11_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM12_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM13_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM14_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM15_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM16_16);
	    i2c_chip.data = yellow.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    break;
	  }
	case FULLTST:
	  {
	    WP_Clear_8x8_Display(&yellow, i2c_chip, xflag);
	    WP_Fill_8x8_Display(&yellow, i2c_chip, TRUE, xflag);	    
	    break;
	  }
	case FNTTST1:
	  {
	    WP_Clear_8x8_Display(&yellow, i2c_chip, xflag);
	    yellow.ssr = SSR_NORMAL;
	    i2c_chip.data = yellow.ssr;
	    WP_i2cout(i2c_chip, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    for (i=0; i<10; i++)
	      {
		j = i + 48;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		WP_Load_8x8_Display(&yellow, i2c_chip, xflag);
		sleep(1);
		echo_ht16k33_status(yellow);
	      }
	    break;
	  }
	case FNTTST2:
	  {
	    WP_Clear_8x8_Display(&yellow, i2c_chip, xflag);
	    yellow.ssr = SSR_NORMAL;
	    i2c_chip.data = yellow.ssr;
	    WP_i2cout(i2c_chip, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		WP_Load_8x8_Display(&yellow, i2c_chip, xflag);
		sleep(1);
	      }
	    echo_ht16k33_status(yellow);
	    break;
	  }
	case FNTTST3:
	  {
	    WP_Clear_8x8_Display(&yellow, i2c_chip, xflag);
	    yellow.ssr = SSR_NORMAL;
	    i2c_chip.data = yellow.ssr;
	    WP_i2cout(i2c_chip, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		WP_Load_8x8_Display(&yellow, i2c_chip, xflag);
		sleep(1);
	      }
	    echo_ht16k33_status(yellow);
	    break;
	  }
	case QUIKTST:
	  {
	    i2c_chip.data = yellow.ssr;
	    WP_i2cout(i2c_chip, xflag);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(10);
	    yellow.ssr = SSR_STANDBY;
	    i2c_chip.data = yellow.ssr;
	    WP_i2cout(i2c_chip, xflag);
	    break;
	  }
	case QUICTST:
	  {
	    WP_Clear_8x8_Display(&yellow, i2c_chip, xflag);
	    yellow.ssr = SSR_NORMAL;
	    i2c_chip.data = yellow.ssr;
	    WP_i2cout(i2c_chip, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    i2c_chip.data = yellow.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    convert_ht16k33_font(&yellow);
	    WP_Load_8x8_Display(&yellow, i2c_chip, xflag);
	    sleep(10);
	    echo_ht16k33_status(yellow);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_PIGPIOD_IF2_H
  if (i2cmode == PIGDMODE)
    {
      switch(demode)
	{
	case BLNKTST:
	  {
	    PGD_Clear_8x8_Display(&yellow, pgdi2c, xflag);
	    PGD_Fill_8x8_Display(&yellow, pgdi2c, FALSE, xflag);	    
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK20HZ);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(4);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK10HZ);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(8);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr | BLINK05HZ);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(16);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    yellow.dspr = (yellow.dspr & BLINKOFF);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    break;
	  }
	case DIMMTST:
	  {
	    PGD_Clear_8x8_Display(&yellow, pgdi2c, xflag);
	    PGD_Fill_8x8_Display(&yellow, pgdi2c, FALSE, xflag);	    
	    yellow.dmsr = (DIMSETMASK | DIM01_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM02_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM03_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM04_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM05_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM06_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM07_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM08_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM09_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM10_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM11_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM12_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM13_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM14_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM15_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    yellow.dmsr = (DIMSETMASK | DIM16_16);
	    pgdi2c.bVal = yellow.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    break;
	  }
	case FULLTST:
	  {
	    PGD_Clear_8x8_Display(&yellow, pgdi2c, xflag);
	    PGD_Fill_8x8_Display(&yellow, pgdi2c, TRUE, xflag);	    
	    break;
	  }
	case FNTTST1:
	  {
	    PGD_Clear_8x8_Display(&yellow, pgdi2c, xflag);
	    yellow.ssr = SSR_NORMAL;
	    pgdi2c.bVal = yellow.ssr;
	    PGD_i2cout(pgdi2c, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    for (i=0; i<10; i++)
	      {
		j = i + 48;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		PGD_Load_8x8_Display(&yellow, pgdi2c, xflag);
		sleep(1);
		echo_ht16k33_status(yellow);
	      }
	    break;
	  }
	case FNTTST2:
	  {
	    PGD_Clear_8x8_Display(&yellow, pgdi2c, xflag);
	    yellow.ssr = SSR_NORMAL;
	    pgdi2c.bVal = yellow.ssr;
	    PGD_i2cout(pgdi2c, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		PGD_Load_8x8_Display(&yellow, pgdi2c, xflag);
		sleep(1);
	      }
	    echo_ht16k33_status(yellow);
	    break;
	  }
	case FNTTST3:
	  {
	    PGD_Clear_8x8_Display(&yellow, pgdi2c, xflag);
	    yellow.ssr = SSR_NORMAL;
	    pgdi2c.bVal = yellow.ssr;
	    PGD_i2cout(pgdi2c, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		load_ht16k33_crfont(&yellow,
				    myfont,
				    j,
				    mrow);
		convert_ht16k33_font(&yellow);
		PGD_Load_8x8_Display(&yellow, pgdi2c, xflag);
		sleep(1);
	      }
	    echo_ht16k33_status(yellow);
	    break;
	  }
	case QUIKTST:
	  {
	    pgdi2c.bVal = yellow.ssr;
	    PGD_i2cout(pgdi2c, xflag);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(10);
	    yellow.ssr = SSR_STANDBY;
	    pgdi2c.bVal = yellow.ssr;
	    PGD_i2cout(pgdi2c, xflag);
	    break;
	  }
	case QUICTST:
	  {
	    PGD_Clear_8x8_Display(&yellow, pgdi2c, xflag);
	    yellow.ssr = SSR_NORMAL;
	    pgdi2c.bVal = yellow.ssr;
	    PGD_i2cout(pgdi2c, xflag);
	    yellow.dspr = (DSPR_ADDR | DISPONMSK);
	    pgdi2c.bVal = yellow.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    convert_ht16k33_font(&yellow);
	    PGD_Load_8x8_Display(&yellow, pgdi2c, xflag);
	    sleep(10);
	    echo_ht16k33_status(yellow);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_PIGPIOD_IF2_H */
	  
  /* Tidy up ... */
  
#ifdef HAVE_PIGPIOD_IF2_H
  if (i2cmode == PIGDMODE)
    {
      if (pgdi2c.thispi >= 0)
	{
	  if (pgdi2c.handle != PGD_UNDEF_HANDLE) 
	    {
	      printf("Closing I2C Device %d:0x%2x\n",
		     pgdi2c.i2cbus,
		     pgdi2c.i2caddr);
	      k = i2c_close(pgdi2c.thispi, pgdi2c.handle);
	    }
	  pigpio_stop(pgdi2c.thispi);
	  printf("pigpiod I/Face disconnected from pi#%d ...\n", pgdi2c.thispi);
	}
    }
#endif /* HAVE_PIGPIOD_IF2_H */
  
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: i2c_ledx_demo.c,v $
 * Revision 1.24  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.23  2018/01/13 22:02:19  pi
 * All -PGD options implemented. SCMVersion 0.02
 *
 * Revision 1.22  2018/01/10 22:49:25  pi
 * Some options now have -PGD support
 *
 * Revision 1.21  2018/01/07 21:28:46  pi
 * Beginning to implement -PGD option
 *
 * Revision 1.20  2017/12/22 23:33:20  pi
 * Minor bug-fix
 *
 * Revision 1.19  2017/12/19 22:50:45  pi
 * -W option implemented
 *
 * Revision 1.18  2017/12/19 17:01:12  pi
 * KEYS matrix I2C addr now 0x77
 *
 * Revision 1.17  2017/12/16 23:49:32  pi
 * Interim Commit (Tidy up)
 *
 * Revision 1.16  2017/12/13 22:47:14  pi
 * Interim commit
 *
 * Revision 1.15  2017/08/19 11:35:32  pi
 * Keyestudio 8x8 matrix support added
 *
 * Revision 1.14  2017/08/11 14:07:09  pi
 * Interim Commit
 *
 * Revision 1.13  2017/08/08 19:42:00  pi
 * Two fonts now displayable
 *
 * Revision 1.12  2017/08/08 19:11:57  pi
 * Interim Commit
 *
 * Revision 1.11  2017/08/08 17:35:55  pi
 * Interim Commit
 *
 * Revision 1.10  2017/07/20 14:36:46  pi
 * Interim Commit
 *
 * Revision 1.9  2017/07/19 18:08:49  pi
 * Interim Commit
 *
 * Revision 1.8  2017/07/18 15:41:56  pi
 * Interim Commit
 *
 * Revision 1.7  2017/07/17 17:24:37  pi
 * Interim Commit. -BIT, -DIT, -FIT & -QIT initialisation tests implemented
 *
 * Revision 1.6  2017/07/17 16:38:42  pi
 * Interim Commit
 *
 * Revision 1.5  2017/07/17 16:32:25  pi
 * Interim Commit
 *
 * Revision 1.4  2017/07/17 16:16:02  pi
 * Interim Commit
 *
 * Revision 1.3  2017/07/17 13:03:33  pi
 * Interim Commit
 *
 * Revision 1.2  2017/07/16 17:33:17  pi
 * Interim commit
 *
 * Revision 1.1  2017/07/14 20:06:31  pi
 * Initial revision
 *
 *
 *
 *
 */

