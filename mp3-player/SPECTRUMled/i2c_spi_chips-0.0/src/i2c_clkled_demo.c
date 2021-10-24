/* I2C (HT16K33) interfaced 4-digit "Clock" LED + RGB LED DEMOnstration code
 * $Id: i2c_clkled_demo.c,v 1.21 2018/03/31 21:32:45 pi Exp $
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
#ifdef HAVE_PIGPIO_H
#include "pigpio_wraps.h"
#endif /* HAVE_PIGPIO_H */
#ifdef HAVE_PIGPIOD_IF2_H
#include "pigpiod_wraps.h"
#endif /* HAVE_PIGPIOD_IF2_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "parsel.h"
#include "i2c_chips.h"
#include "i2c_wraps.h"
#include "led_disptools.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: i2c_clkled_demo.c,v $";
static char RCSId[]       = "$Id: i2c_clkled_demo.c,v 1.21 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.21 $";
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
  CLKTST,    /* -CLKT                */
  CCLK,      /* -CCLK                */
  DIMMTST,   /* -DIT                 */
  DISINT,    /* -di n                */
  FULLTST,   /* -FIT                 */
  QUIKTST,   /* -QIT                 */
  SDGTTST,   /* -SDT                 */
  REDACT,    /* +R                   */
  GREENACT,  /* +G                   */
  BLUEACT,   /* +B                   */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  SYSMODE,   /* -S                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIO_H
  PIGMODE,   /* -PG                  */
#endif /* HAVE_PIGPIO_H */
#ifdef HAVE_PIGPIOD_IF2_H
  PIGDMODE,  /* -PGD                 */
#endif /* HAVE_PIGPIOD_IF2_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,i2cmode;
unsigned int rgbmask;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[INQUIRE]  = "-I";
  arglist[BLNKTST]  = "-BIT";
  arglist[CLKTST]   = "-CLKT";
  arglist[CCLK]     = "-CCLK";
  arglist[DIMMTST]  = "-DIT";
  arglist[DISINT]   = "-di";
  arglist[FULLTST]  = "-FIT";
  arglist[QUIKTST]  = "-QIT";
  arglist[SDGTTST]  = "-SDT";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  arglist[REDACT]   = "+R";
  arglist[GREENACT] = "+G";
  arglist[BLUEACT]  = "+B";
  arglist[SYSMODE]  = "-S";
  arglist[VERBOSE]  = "-V";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIO_H
  arglist[PIGMODE]  = "-PG";
#endif /* HAVE_PIGPIO_H */
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
int dival;
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
	    case ARGFILE: break;
	    case COMMENT: printf("%s\n", buffer4); break;
	    case ECHO:    eflag=TRUE; break;
	    case INQUIRE: iqflag=TRUE; break;
	    case BLNKTST: demode=BLNKTST; i2cmode=SYSMODE; break;
	    case CLKTST:  demode=CLKTST;  i2cmode=SYSMODE; break;
	    case CCLK:    demode=CCLK;    i2cmode=SYSMODE; break;
	    case DIMMTST: demode=DIMMTST; i2cmode=SYSMODE; break;
	    case DISINT:
	      {
		demode=DISINT;
		dival = atoi(buffer2);
		if (dival > 9999) dival = 9999;
		if (dival < 0) dival = 0;
		break;
	      }
	    case FULLTST: demode=FULLTST; i2cmode=SYSMODE; break;
	    case QUIKTST: demode=QUIKTST; i2cmode=SYSMODE; break;
	    case SDGTTST: demode=SDGTTST; i2cmode=SYSMODE; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case REDACT:   rgbmask=(rgbmask | REDON);   break;
	    case GREENACT: rgbmask=(rgbmask | GREENON); break;
	    case BLUEACT:  rgbmask=(rgbmask | BLUEON);  break;
	    case SYSMODE:  i2cmode=SYSMODE; break;
	    case VERBOSE:  eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE:  i2cmode=WPIMODE; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIO_H
	    case PIGMODE:  i2cmode=PIGMODE; break;
#endif /* HAVE_PIGPIO_H */
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
  strcat(buffer1, "         -E    {Echo command-line arguments},\n");
  strcat(buffer1, "         -I    {Inquire about i2c devices},\n");
  strcat(buffer1, "         -BIT  {Blink Initialisation Test},\n");
  strcat(buffer1, "         -CLKT {CLocK display Test},\n");
  strcat(buffer1, "         -CCLK {Constant CLocK display},\n");
  strcat(buffer1, "         -DIT  {Dimming Initialisation Test},\n");
  strcat(buffer1, "         -di n {Display Integer n},\n");
  strcat(buffer1, "         -FIT  {Full Initialisation Test},\n");
  strcat(buffer1, "         -QIT  {Quick Initialisation Test},\n");
  strcat(buffer1, "         -SDT  {Simple Digit Test},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         +R    {activate Red rgb led},\n");
  strcat(buffer1, "         +G    {activate Green rgb led},\n");
  strcat(buffer1, "         +B    {activate Blue rgb led},\n");
  strcat(buffer1, "         -S    {use System call methods - default},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -W    {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIO_H
  strcat(buffer1, "         -PG   {use PiGpio i2c methods**},\n");
  strcat(buffer1, "               {**NB: requires sudo \"prefix\"!},\n");
#endif /* HAVE_PIGPIO_H */
#ifdef HAVE_PIGPIOD_IF2_H
  strcat(buffer1, "         -PGD  {use PiGpioD i2c methods^^},\n");
  strcat(buffer1, "               {^^NB: requires a running pigpio daemon!},\n");
#endif /* HAVE_PIGPIOD_IF2_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

void C2S_Clear_4DRGB_Display(ht16k33 *any_4drgb,
			     char qx)
{
  (*any_4drgb).ssr = SSR_NORMAL;
  C2S_i2cout(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ssr,
	     qx);
  (*any_4drgb).dspr = (DSPR_ADDR & DISPOFFMSK);
  C2S_i2cout(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).dspr,
	     qx);
  (*any_4drgb).ddcb = ALLSEGOFF;
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  (*any_4drgb).ddcb = ALLRGBOFF;
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
}

void C2S_Fill_4DRGB_Display(ht16k33 *any_4drgb,
			    char qs,
			    char qx)
{
  (*any_4drgb).ssr = SSR_NORMAL;
  C2S_i2cout(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ssr,
	     qx);
  (*any_4drgb).dspr = (DSPR_ADDR | DISPONMSK);
  C2S_i2cout(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).dspr,
	     qx);
  (*any_4drgb).ddcb = ALLSEGON;
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  (*any_4drgb).ddcb = REDON;
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).ddcb | GREENON;
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).ddcb | BLUEON;
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
}

void C2S_Write_4DRGB_Digits(ht16k33 *any_4drgb,
			    char qs,
			    char qx)
{
  (*any_4drgb).ssr = SSR_NORMAL;
  C2S_i2cout(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ssr,
	     qx);
  (*any_4drgb).dspr = (DSPR_ADDR | DISPONMSK);
  C2S_i2cout(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).dspr,
	     qx);
  (*any_4drgb).ddcb = (*any_4drgb).digits[0];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[1];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[2];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[3];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
}

void C2S_Write_4DRGB_RGB(ht16k33 *any_4drgb,
			 unsigned int rgbmsk,
			 char qx)
{
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  load_ht16k33_rgb(any_4drgb, rgbmsk);
  (*any_4drgb).ddcb = (*any_4drgb).rgb;
  C2S_i2cset(PI_B2,
	     (*any_4drgb).addr,
	     (*any_4drgb).ddap,
	     (*any_4drgb).ddcb,
	     qx);
}

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
void WP_Clear_4DRGB_Display(ht16k33 *any_4drgb,
			    wpi2cvb reg,
			    char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  (*any_4drgb).ssr = SSR_NORMAL;
  lreg.data = (*any_4drgb).ssr;
  WP_i2cout(lreg, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR & DISPOFFMSK);
  lreg.data = (*any_4drgb).dspr;
  WP_i2cout(lreg, xflag);
  (*any_4drgb).ddcb = ALLSEGOFF;
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  lreg.data = (*any_4drgb).ddcb;
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
  (*any_4drgb).ddcb = ALLRGBOFF;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  lreg.data = (*any_4drgb).ddcb;
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
}

void WP_Fill_4DRGB_Display(ht16k33 *any_4drgb,
			   wpi2cvb reg,
			   char qs,
			   char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  (*any_4drgb).ssr = SSR_NORMAL;
  lreg.data = (*any_4drgb).ssr;
  WP_i2cout(lreg, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR | DISPONMSK);
  lreg.data = (*any_4drgb).dspr;
  WP_i2cout(lreg, xflag);
  (*any_4drgb).ddcb = ALLSEGON;
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  lreg.data = (*any_4drgb).ddcb;
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  (*any_4drgb).ddcb = REDON;
  lreg.reg  = (*any_4drgb).ddap;
  lreg.data = (*any_4drgb).ddcb;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).ddcb | GREENON;
  lreg.data = (*any_4drgb).ddcb;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).ddcb | BLUEON;
  lreg.data = (*any_4drgb).ddcb;
  WP_i2cset(lreg, xflag);
}

void WP_Write_4DRGB_Digits(ht16k33 *any_4drgb,
			   wpi2cvb reg,
			   char qs,
			   char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  (*any_4drgb).ssr = SSR_NORMAL;
  lreg.data = (*any_4drgb).ssr;
  WP_i2cout(lreg, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR | DISPONMSK);
  lreg.data = (*any_4drgb).dspr;
  WP_i2cout(lreg, xflag);
  (*any_4drgb).ddcb = (*any_4drgb).digits[0];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  lreg.data = (*any_4drgb).ddcb;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[1];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  lreg.data = (*any_4drgb).ddcb;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[2];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  lreg.data = (*any_4drgb).ddcb;
  WP_i2cset(lreg, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[3];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  lreg.reg  = (*any_4drgb).ddap;
  lreg.data = (*any_4drgb).ddcb;
  WP_i2cset(lreg, xflag);
}

void WP_Write_4DRGB_RGB(ht16k33 *any_4drgb,
			wpi2cvb reg,
			unsigned int rgbmsk,
			char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  load_ht16k33_rgb(any_4drgb, rgbmsk);
  (*any_4drgb).ddcb = (*any_4drgb).rgb;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  lreg.data = (*any_4drgb).ddcb;
  lreg.reg  = (*any_4drgb).ddap;
  WP_i2cset(lreg, xflag);
}

#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_PIGPIO_H
void PG_Clear_4DRGB_Display(ht16k33 *any_4drgb,
			    pgi2cvb pgi2c,
			    char qx)
{
  pgi2cvb lpgi2c;
  lpgi2c.handle = pgi2c.handle;
  (*any_4drgb).ssr = SSR_NORMAL;
  pgi2c.bVal = (*any_4drgb).ssr;
  PG_i2cout(pgi2c, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR & DISPOFFMSK);
  pgi2c.bVal = (*any_4drgb).dspr;
  PG_i2cout(pgi2c, xflag);
  (*any_4drgb).ddcb = ALLSEGOFF;
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  (*any_4drgb).ddcb = ALLRGBOFF;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
}

void PG_Fill_4DRGB_Display(ht16k33 *any_4drgb,
			   pgi2cvb pgi2c,
			   char qs,
			   char qx)
{
  pgi2cvb lpgi2c;
  lpgi2c.handle = pgi2c.handle;
  (*any_4drgb).ssr = SSR_NORMAL;
  pgi2c.bVal = (*any_4drgb).ssr;
  PG_i2cout(pgi2c, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR | DISPONMSK);
  pgi2c.bVal = (*any_4drgb).dspr;
  PG_i2cout(pgi2c, xflag);
  (*any_4drgb).ddcb = ALLSEGON;
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  (*any_4drgb).ddcb = REDON;
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).ddcb | GREENON;
  pgi2c.bVal = (*any_4drgb).ddcb;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).ddcb | BLUEON;
  pgi2c.bVal = (*any_4drgb).ddcb;
  PG_i2cset(pgi2c, xflag);
}

void PG_Write_4DRGB_Digits(ht16k33 *any_4drgb,
			   pgi2cvb pgi2c,
			   char qs,
			   char qx)
{
  pgi2cvb lpgi2c;
  lpgi2c.handle = pgi2c.handle;
  (*any_4drgb).ssr = SSR_NORMAL;
  pgi2c.bVal = (*any_4drgb).ssr;
  PG_i2cout(pgi2c, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR | DISPONMSK);
  pgi2c.bVal = (*any_4drgb).dspr;
  PG_i2cout(pgi2c, xflag);
  (*any_4drgb).ddcb = (*any_4drgb).digits[0];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[1];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[2];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[3];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
}

void PG_Write_4DRGB_RGB(ht16k33 *any_4drgb,
			pgi2cvb pgi2c,
			unsigned int rgbmsk,
			char qx)
{
  pgi2cvb lpgi2c;
  lpgi2c.handle = pgi2c.handle;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  load_ht16k33_rgb(any_4drgb, rgbmsk);
  (*any_4drgb).ddcb = (*any_4drgb).rgb;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  pgi2c.bVal = (*any_4drgb).ddcb;
  pgi2c.i2cReg = (*any_4drgb).ddap;
  PG_i2cset(pgi2c, xflag);
}
#endif /* HAVE_PIGPIO_H */

#ifdef HAVE_PIGPIOD_IF2_H
void PGD_Clear_4DRGB_Display(ht16k33 *any_4drgb,
			    pgdi2cvb pgdi2c,
			    char qx)
{
  pgdi2cvb lpgdi2c;
  lpgdi2c.handle = pgdi2c.handle;
  (*any_4drgb).ssr = SSR_NORMAL;
  pgdi2c.bVal = (*any_4drgb).ssr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR & DISPOFFMSK);
  pgdi2c.bVal = (*any_4drgb).dspr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_4drgb).ddcb = ALLSEGOFF;
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  (*any_4drgb).ddcb = ALLRGBOFF;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
}

void PGD_Fill_4DRGB_Display(ht16k33 *any_4drgb,
			   pgdi2cvb pgdi2c,
			   char qs,
			   char qx)
{
  pgdi2cvb lpgdi2c;
  lpgdi2c.handle = pgdi2c.handle;
  (*any_4drgb).ssr = SSR_NORMAL;
  pgdi2c.bVal = (*any_4drgb).ssr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR | DISPONMSK);
  pgdi2c.bVal = (*any_4drgb).dspr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_4drgb).ddcb = ALLSEGON;
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  (*any_4drgb).ddcb = REDON;
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).ddcb | GREENON;
  pgdi2c.bVal = (*any_4drgb).ddcb;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).ddcb | BLUEON;
  pgdi2c.bVal = (*any_4drgb).ddcb;
  PGD_i2cset(pgdi2c, xflag);
}

void PGD_Write_4DRGB_Digits(ht16k33 *any_4drgb,
			    pgdi2cvb pgdi2c,
			    char qs,
			    char qx)
{
  pgdi2cvb lpgdi2c;
  lpgdi2c.handle = pgdi2c.handle;
  (*any_4drgb).ssr = SSR_NORMAL;
  pgdi2c.bVal = (*any_4drgb).ssr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_4drgb).dspr = (DSPR_ADDR | DISPONMSK);
  pgdi2c.bVal = (*any_4drgb).dspr;
  PGD_i2cout(pgdi2c, xflag);
  (*any_4drgb).ddcb = (*any_4drgb).digits[0];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT1ADDR);
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[1];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT2ADDR);
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[2];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT3ADDR);
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
  if (qs) sleep(1);
  (*any_4drgb).ddcb = (*any_4drgb).digits[3];
  (*any_4drgb).ddap = (DDAP_ADDR + DGT4ADDR);
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
}

void PGD_Write_4DRGB_RGB(ht16k33 *any_4drgb,
			 pgdi2cvb pgdi2c,
			 unsigned int rgbmsk,
			 char qx)
{
  pgdi2cvb lpgdi2c;
  lpgdi2c.handle = pgdi2c.handle;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  load_ht16k33_rgb(any_4drgb, rgbmsk);
  (*any_4drgb).ddcb = (*any_4drgb).rgb;
  (*any_4drgb).ddap = (DDAP_ADDR + RGBADDR);
  pgdi2c.bVal = (*any_4drgb).ddcb;
  pgdi2c.i2cReg = (*any_4drgb).ddap;
  PGD_i2cset(pgdi2c, xflag);
}
#endif /* HAVE_PIGPIOD_IF2_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char dateandtimeis[DATEANDTIME_BUF_LEN],timenowis[TIMENOW_BUF_LEN];
  char mxname[CHIP_NAME_LEN];
  int hours,mins,secs;
  int i,j,k,l,mca;
  int bite,data[16];
  int mrow;
  unsigned int mdigits[4],mdvd[4];
  ht16k33 green;
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#ifdef HAVE_WIRINGPII2C_H
  wpi2cvb i2c_chip;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIO_H
  int pigpiovsn;
  pgi2cvb pgi2c;
#endif /* HAVE_PIGPIO_H */
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
      /* Testing new functions ... */
      get_dateandtime(dateandtimeis);
      printf("\nCheck1: %s", dateandtimeis);
      sleep(2);
      get_timenow(timenowis);
      printf("Check2: %s\n", timenowis);
      /* ... */
      exit(EXIT_SUCCESS);
    }

  /* Set up defaults */
  eflag   = vflag = xflag = FALSE;
  demode  = LAST_ARG;
  i2cmode = SYSMODE;
  /*  mxaddr   = 0x70; */
  mxaddr  = 0x75; /* Module A0 & A2 I2C Addrlinks shorted! */
  rgbmask = ALLRGBOFF;
#ifdef HAVE_PIGPIO_H
  pigpiovsn    = -1; /* Not yet determined */
  pgi2c.handle = PG_UNDEF_HANDLE;
#endif /* HAVE_PIGPIO_H */
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
	    case ECHO:    eflag=TRUE;     i--; break;
	    case INQUIRE: iqflag=TRUE;    i--; break;
	    case BLNKTST: demode=BLNKTST; i--; break;
	    case CLKTST:  demode=CLKTST;  i--; break;
	    case CCLK:    demode=CCLK;    i--; break;
	    case DIMMTST: demode=DIMMTST; i--; break;
	    case DISINT:
	      {
		demode=DISINT;
		dival = atoi(p2);
		if (dival > 9999) dival = 9999;
		if (dival < 0) dival = 0;
		break;
	      }
	    case FULLTST: demode=FULLTST; i--; break;
	    case QUIKTST: demode=QUIKTST; i--; break;
	    case SDGTTST: demode=SDGTTST; i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE: i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case REDACT:   rgbmask=(rgbmask | REDON);   i--; break;
	    case GREENACT: rgbmask=(rgbmask | GREENON); i--; break;
	    case BLUEACT:  rgbmask=(rgbmask | BLUEON);  i--; break;
	    case SYSMODE:  i2cmode=SYSMODE; i--; break;
	    case VERBOSE:  eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_PIGPIO_H
	    case PIGMODE: i2cmode=PIGMODE; i--; break;
#endif /* HAVE_PIGPIO_H */
#ifdef HAVE_PIGPIOD_IF2_H
	    case PIGDMODE: i2cmode=PIGDMODE; i--; break;
#endif /* HAVE_PIGPIOD_IF2_H */
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
  
  strcpy(mxname, "4D_Clock+RGB_LED");
  
  /* -I option leads to an early exit after the system call ... */
  if (iqflag)
    {  
      if (!xflag)
	{
	  mca = init_ht16k33(&green,
			     mxaddr,
			     C4DGTRGB,
			     mxname);
	  echo_ht16k33_status(green);
	  /*
	  convert_ht16k33_font(&green);
	  echo_ht16k33_status(green);
	  */
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  mca = init_ht16k33(&green,
		     mxaddr,
		     C4DGTRGB,
		     mxname);
  echo_ht16k33_status(green);
  init_codes7seg0();

  if (demode == DISINT)
    {
      mdvd[0] = dival/1000;
      mdvd[1] = (dival-(mdvd[0]*1000))/100;
      mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
      mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
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
      i2c_chip.did = (int)(green.addr);
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

#ifdef HAVE_PIGPIO_H
  if (i2cmode == PIGMODE)
    {
      printf("Using pigpio I2C methods:\n");
      if ((pigpiovsn = gpioInitialise()) < 0)
	{
	  fprintf(stderr, "Could not initialise pigpio! ...\n");
	  perror("gpioInitialise");
	  exit(EXIT_FAILURE);
	}
      else printf("pigpio Version %d initialised ...\n", pigpiovsn);
      pgi2c.i2cBus   = PI_B2;
      pgi2c.i2cAddr  = (int)(green.addr);
      pgi2c.i2cFlags = 0; /* Currently always set to zero - see pigpio.h */
      if ((pgi2c.handle = i2cOpen(pgi2c.i2cBus, pgi2c.i2cAddr, pgi2c.i2cFlags)) < 0)
	{
	  fprintf(stderr, "Could not open I2C Device %d:0x%2x!",
		  pgi2c.i2cBus,
		  pgi2c.i2cAddr);
	  perror("i2cOpen");
	  exit(EXIT_FAILURE);
	}
      else printf("I2C Device %d:0x%2x has pgi2c.handle of %d\n",
		  pgi2c.i2cBus,
		  pgi2c.i2cAddr,
		  pgi2c.handle);
    }
#endif /* HAVE_PIGPIO_H */
 
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
	  pgdi2c.i2caddr  = (int)(green.addr);
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
	    C2S_Clear_4DRGB_Display(&green, xflag);
	    C2S_Fill_4DRGB_Display(&green, FALSE, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK20HZ);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dspr,
		       xflag);
	    sleep(4);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dspr,
		       xflag);	    
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK10HZ);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dspr,
		       xflag);
	    sleep(8);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dspr,
		       xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK05HZ);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dspr,
		       xflag);
	    sleep(16);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dspr,
		       xflag);
	    break;
	  }
	case CLKTST:
	  {
	    C2S_Clear_4DRGB_Display(&green, xflag);
	    if (rgbmask != ALLRGBOFF) C2S_Write_4DRGB_RGB(&green, rgbmask, xflag);
	    get_timenow(timenowis);
	    printf("Check3: %s\n", timenowis);
	    make_col_white(timenowis);
	    printf("Check4: %s\n", timenowis);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) hours = atoi(buffer2);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) mins = atoi(buffer2);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) secs = atoi(buffer2);
	    printf("Check5: %02d-%02d-%02d\n", hours, mins, secs);
	    dival = (100 * hours) + mins;
	    printf("Check6: %04d\n", dival);
	    mdvd[0] = dival/1000;
	    mdvd[1] = (dival-(mdvd[0]*1000))/100;
	    mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
	    mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
	    mdigits[0] = codes7seg0[mdvd[0]];
	    mdigits[1] = codes7seg0[mdvd[1]] | codes7seg0[18];
	    mdigits[2] = codes7seg0[mdvd[2]] | codes7seg0[18];
	    mdigits[3] = codes7seg0[mdvd[3]];
	    load_ht16k33_digits(&green, mdigits);
	    C2S_Write_4DRGB_Digits(&green, FALSE, xflag);
	    break;
	  }
	case CCLK:
	  {
	    C2S_Clear_4DRGB_Display(&green, xflag);
	    if (rgbmask != ALLRGBOFF) C2S_Write_4DRGB_RGB(&green, rgbmask, xflag);
	    for (i=0; i<300; i++)
	      {
		get_timenow(timenowis);
		if (vflag) printf("Check3: %s\n", timenowis);
		make_col_white(timenowis);
		if (vflag) printf("Check4: %s\n", timenowis);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) hours = atoi(buffer2);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) mins = atoi(buffer2);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) secs = atoi(buffer2);
		if (vflag) printf("Check5: %02d-%02d-%02d\n", hours, mins, secs);
		dival = (100 * hours) + mins;
		if (vflag) printf("Check6: %04d\n", dival);
		mdvd[0] = dival/1000;
		mdvd[1] = (dival-(mdvd[0]*1000))/100;
		mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
		mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
		mdigits[0] = codes7seg0[mdvd[0]];
		j = i % 2;
		if (j == 0)
		  {
		    mdigits[1] = codes7seg0[mdvd[1]] | codes7seg0[18];
		    mdigits[2] = codes7seg0[mdvd[2]] | codes7seg0[18];
		  }
		else
		  {
		    mdigits[1] = codes7seg0[mdvd[1]];
		    mdigits[2] = codes7seg0[mdvd[2]];
		  }
		mdigits[3] = codes7seg0[mdvd[3]];
		load_ht16k33_digits(&green, mdigits);
		C2S_Write_4DRGB_Digits(&green, FALSE, xflag);
		sleep(1);
	      }
	    break;
	  }
	case DIMMTST:
	  {
	    C2S_Clear_4DRGB_Display(&green, xflag);
	    C2S_Fill_4DRGB_Display(&green, FALSE, xflag);
	    green.dmsr = (DIMSETMASK | DIM01_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dspr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM02_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM03_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM04_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM05_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM06_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM07_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM08_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM09_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM10_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM11_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM12_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM13_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM14_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM15_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM16_16);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dmsr,
		       xflag);
	    break;
	  }
	case DISINT:
	  {
	    C2S_Clear_4DRGB_Display(&green, xflag);
	    if (rgbmask != ALLRGBOFF) C2S_Write_4DRGB_RGB(&green, rgbmask, xflag);
	    mdigits[0] = codes7seg0[mdvd[0]];
	    mdigits[1] = codes7seg0[mdvd[1]];
	    mdigits[2] = codes7seg0[mdvd[2]];
	    mdigits[3] = codes7seg0[mdvd[3]];
	    load_ht16k33_digits(&green, mdigits);
	    C2S_Write_4DRGB_Digits(&green, FALSE, xflag);
	    break;
	  }
	case FULLTST:
	  {
	    C2S_Clear_4DRGB_Display(&green, xflag);
	    C2S_Fill_4DRGB_Display(&green, TRUE, xflag);	    
	    break;
	  }
	case QUIKTST:
	  {
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.ssr,
		       xflag);
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.dspr,
		       xflag);
	    sleep(10);
	    green.ssr = SSR_STANDBY;
	    C2S_i2cout(PI_B2,
		       green.addr,
		       green.ssr,
		       xflag);	
	    break;
	  }
	case SDGTTST:
	  {
	    C2S_Clear_4DRGB_Display(&green, xflag);
	    if (rgbmask != ALLRGBOFF) C2S_Write_4DRGB_RGB(&green, rgbmask, xflag);
	    mdigits[0] = codes7seg0[ 0];
	    mdigits[1] = codes7seg0[ 1] | codes7seg0[18];
	    mdigits[2] = codes7seg0[ 2] | codes7seg0[18];
	    mdigits[3] = codes7seg0[ 3];
	    load_ht16k33_digits(&green, mdigits);
	    C2S_Write_4DRGB_Digits(&green, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[ 4];
	    mdigits[1] = codes7seg0[ 5];
	    mdigits[2] = codes7seg0[ 6];
	    mdigits[3] = codes7seg0[ 7];
	    load_ht16k33_digits(&green, mdigits);
	    C2S_Write_4DRGB_Digits(&green, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[ 8];
	    mdigits[1] = codes7seg0[ 9] | codes7seg0[18];
	    mdigits[2] = codes7seg0[10] | codes7seg0[18];
	    mdigits[3] = codes7seg0[11];
	    load_ht16k33_digits(&green, mdigits);
	    C2S_Write_4DRGB_Digits(&green, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[12];
	    mdigits[1] = codes7seg0[13];
	    mdigits[2] = codes7seg0[14];
	    mdigits[3] = codes7seg0[15];
	    load_ht16k33_digits(&green, mdigits);
	    C2S_Write_4DRGB_Digits(&green, TRUE, xflag);
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
	    WP_Clear_4DRGB_Display(&green, i2c_chip, xflag);
	    WP_Fill_4DRGB_Display(&green, i2c_chip, FALSE, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK20HZ);
	    i2c_chip.data = green.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(4);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    i2c_chip.data = green.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK10HZ);
	    i2c_chip.data = green.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(8);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    i2c_chip.data = green.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK05HZ);
	    i2c_chip.data = green.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(16);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    i2c_chip.data = green.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    break;
	  }
	case CLKTST:
	  {
	    WP_Clear_4DRGB_Display(&green, i2c_chip, xflag);
	    if (rgbmask != ALLRGBOFF) WP_Write_4DRGB_RGB(&green, i2c_chip, rgbmask, xflag);
	    get_timenow(timenowis);
	    printf("Check3: %s\n", timenowis);
	    make_col_white(timenowis);
	    printf("Check4: %s\n", timenowis);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) hours = atoi(buffer2);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) mins = atoi(buffer2);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) secs = atoi(buffer2);
	    printf("Check5: %02d-%02d-%02d\n", hours, mins, secs);
	    dival = (100 * hours) + mins;
	    printf("Check6: %04d\n", dival);
	    mdvd[0] = dival/1000;
	    mdvd[1] = (dival-(mdvd[0]*1000))/100;
	    mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
	    mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
	    mdigits[0] = codes7seg0[mdvd[0]];
	    mdigits[1] = codes7seg0[mdvd[1]] | codes7seg0[18];
	    mdigits[2] = codes7seg0[mdvd[2]] | codes7seg0[18];
	    mdigits[3] = codes7seg0[mdvd[3]];
	    load_ht16k33_digits(&green, mdigits);
	    WP_Write_4DRGB_Digits(&green, i2c_chip, FALSE, xflag);
	    break;
	  }
	case CCLK:
	  {
	    WP_Clear_4DRGB_Display(&green, i2c_chip, xflag);
	    if (rgbmask != ALLRGBOFF) WP_Write_4DRGB_RGB(&green, i2c_chip, rgbmask, xflag);
	    for (i=0; i<300; i++)
	      {
		get_timenow(timenowis);
		if (vflag) printf("Check3: %s\n", timenowis);
		make_col_white(timenowis);
		if (vflag) printf("Check4: %s\n", timenowis);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) hours = atoi(buffer2);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) mins = atoi(buffer2);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) secs = atoi(buffer2);
		if (vflag) printf("Check5: %02d-%02d-%02d\n", hours, mins, secs);
		dival = (100 * hours) + mins;
		if (vflag) printf("Check6: %04d\n", dival);
		mdvd[0] = dival/1000;
		mdvd[1] = (dival-(mdvd[0]*1000))/100;
		mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
		mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
		mdigits[0] = codes7seg0[mdvd[0]];
		j = i % 2;
		if (j == 0)
		  {
		    mdigits[1] = codes7seg0[mdvd[1]] | codes7seg0[18];
		    mdigits[2] = codes7seg0[mdvd[2]] | codes7seg0[18];
		  }
		else
		  {
		    mdigits[1] = codes7seg0[mdvd[1]];
		    mdigits[2] = codes7seg0[mdvd[2]];
		  }
		mdigits[3] = codes7seg0[mdvd[3]];
		load_ht16k33_digits(&green, mdigits);
		WP_Write_4DRGB_Digits(&green, i2c_chip, FALSE, xflag);
		sleep(1);
	      }
	    break;
	  }
	case DIMMTST:
	  {
	    WP_Clear_4DRGB_Display(&green, i2c_chip, xflag);
	    WP_Fill_4DRGB_Display(&green, i2c_chip, FALSE, xflag);
	    green.dmsr = (DIMSETMASK | DIM01_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    i2c_chip.data = green.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM02_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM03_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM04_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM05_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM06_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM07_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM08_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM09_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM10_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM11_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM12_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM13_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM14_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM15_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM16_16);
	    i2c_chip.data = green.dmsr;
	    WP_i2cout(i2c_chip, xflag);
	    break;
	  }
	case DISINT:
	  {
	    WP_Clear_4DRGB_Display(&green, i2c_chip, xflag);
	    if (rgbmask != ALLRGBOFF) WP_Write_4DRGB_RGB(&green, i2c_chip, rgbmask, xflag);
	    mdigits[0] = codes7seg0[mdvd[0]];
	    mdigits[1] = codes7seg0[mdvd[1]];
	    mdigits[2] = codes7seg0[mdvd[2]];
	    mdigits[3] = codes7seg0[mdvd[3]];
	    load_ht16k33_digits(&green, mdigits);
	    WP_Write_4DRGB_Digits(&green, i2c_chip, FALSE, xflag);
	    break;
	  }
	case FULLTST:
	  {
	    WP_Clear_4DRGB_Display(&green, i2c_chip, xflag);
	    WP_Fill_4DRGB_Display(&green, i2c_chip, TRUE, xflag);
	    break;
	  }
	case QUIKTST:
	  {
	    i2c_chip.data = green.ssr;
	    WP_i2cout(i2c_chip, xflag);
	    i2c_chip.data = green.dspr;
	    WP_i2cout(i2c_chip, xflag);
	    sleep(10);
	    green.ssr = SSR_STANDBY;
	    i2c_chip.data = green.ssr;
	    WP_i2cout(i2c_chip, xflag);
	    break;
	  }
	case SDGTTST:
	  {
	    WP_Clear_4DRGB_Display(&green, i2c_chip, xflag);
	    if (rgbmask != ALLRGBOFF) WP_Write_4DRGB_RGB(&green, i2c_chip, rgbmask, xflag);
	    mdigits[0] = codes7seg0[ 0];
	    mdigits[1] = codes7seg0[ 1] | codes7seg0[18];
	    mdigits[2] = codes7seg0[ 2] | codes7seg0[18];
	    mdigits[3] = codes7seg0[ 3];
	    load_ht16k33_digits(&green, mdigits);
	    WP_Write_4DRGB_Digits(&green, i2c_chip, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[ 4];
	    mdigits[1] = codes7seg0[ 5];
	    mdigits[2] = codes7seg0[ 6];
	    mdigits[3] = codes7seg0[ 7];
	    load_ht16k33_digits(&green, mdigits);
	    WP_Write_4DRGB_Digits(&green, i2c_chip, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[ 8];
	    mdigits[1] = codes7seg0[ 9] | codes7seg0[18];
	    mdigits[2] = codes7seg0[10] | codes7seg0[18];
	    mdigits[3] = codes7seg0[11];
	    load_ht16k33_digits(&green, mdigits);
	    WP_Write_4DRGB_Digits(&green, i2c_chip, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[12];
	    mdigits[1] = codes7seg0[13];
	    mdigits[2] = codes7seg0[14];
	    mdigits[3] = codes7seg0[15];
	    load_ht16k33_digits(&green, mdigits);
	    WP_Write_4DRGB_Digits(&green, i2c_chip, TRUE, xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  
#ifdef HAVE_PIGPIO_H
  if (i2cmode == PIGMODE)
    {
      switch(demode)
	{
	case BLNKTST:
	  {
	    PG_Clear_4DRGB_Display(&green, pgi2c, xflag);
	    PG_Fill_4DRGB_Display(&green, pgi2c, FALSE, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK20HZ);
	    pgi2c.bVal = green.dspr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(4);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    pgi2c.bVal = green.dspr;
	    PG_i2cout(pgi2c, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK10HZ);
	    pgi2c.bVal = green.dspr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(8);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    pgi2c.bVal = green.dspr;
	    PG_i2cout(pgi2c, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK05HZ);
	    pgi2c.bVal = green.dspr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(16);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    pgi2c.bVal = green.dspr;
	    PG_i2cout(pgi2c, xflag);
	    break;
	  }
	case CLKTST:
	  {
	    PG_Clear_4DRGB_Display(&green, pgi2c, xflag);
	    if (rgbmask != ALLRGBOFF) PG_Write_4DRGB_RGB(&green, pgi2c, rgbmask, xflag);
	    get_timenow(timenowis);
	    printf("Check3: %s\n", timenowis);
	    make_col_white(timenowis);
	    printf("Check4: %s\n", timenowis);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) hours = atoi(buffer2);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) mins = atoi(buffer2);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) secs = atoi(buffer2);
	    printf("Check5: %02d-%02d-%02d\n", hours, mins, secs);
	    dival = (100 * hours) + mins;
	    printf("Check6: %04d\n", dival);
	    mdvd[0] = dival/1000;
	    mdvd[1] = (dival-(mdvd[0]*1000))/100;
	    mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
	    mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
	    mdigits[0] = codes7seg0[mdvd[0]];
	    mdigits[1] = codes7seg0[mdvd[1]] | codes7seg0[18];
	    mdigits[2] = codes7seg0[mdvd[2]] | codes7seg0[18];
	    mdigits[3] = codes7seg0[mdvd[3]];
	    load_ht16k33_digits(&green, mdigits);
	    PG_Write_4DRGB_Digits(&green, pgi2c, FALSE, xflag);
	    break;
	  }
	case CCLK:
	  {
	    PG_Clear_4DRGB_Display(&green, pgi2c, xflag);
	    if (rgbmask != ALLRGBOFF) PG_Write_4DRGB_RGB(&green, pgi2c, rgbmask, xflag);
	    for (i=0; i<300; i++)
	      {
		get_timenow(timenowis);
		if (vflag) printf("Check3: %s\n", timenowis);
		make_col_white(timenowis);
		if (vflag) printf("Check4: %s\n", timenowis);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) hours = atoi(buffer2);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) mins = atoi(buffer2);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) secs = atoi(buffer2);
		if (vflag) printf("Check5: %02d-%02d-%02d\n", hours, mins, secs);
		dival = (100 * hours) + mins;
		if (vflag) printf("Check6: %04d\n", dival);
		mdvd[0] = dival/1000;
		mdvd[1] = (dival-(mdvd[0]*1000))/100;
		mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
		mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
		mdigits[0] = codes7seg0[mdvd[0]];
		j = i % 2;
		if (j == 0)
		  {
		    mdigits[1] = codes7seg0[mdvd[1]] | codes7seg0[18];
		    mdigits[2] = codes7seg0[mdvd[2]] | codes7seg0[18];
		  }
		else
		  {
		    mdigits[1] = codes7seg0[mdvd[1]];
		    mdigits[2] = codes7seg0[mdvd[2]];
		  }
		mdigits[3] = codes7seg0[mdvd[3]];
		load_ht16k33_digits(&green, mdigits);
		PG_Write_4DRGB_Digits(&green, pgi2c, FALSE, xflag);
		sleep(1);
	      }
	    break;
	  }
	case DIMMTST:
	  {
	    PG_Clear_4DRGB_Display(&green, pgi2c, xflag);
	    PG_Fill_4DRGB_Display(&green, pgi2c, FALSE, xflag);
	    green.dmsr = (DIMSETMASK | DIM01_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    pgi2c.bVal = green.dspr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM02_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM03_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM04_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM05_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM06_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM07_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM08_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM09_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM10_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM11_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM12_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM13_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM14_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM15_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM16_16);
	    pgi2c.bVal = green.dmsr;
	    PG_i2cout(pgi2c, xflag);
	    break;
	  }
	case DISINT:
	  {
	    PG_Clear_4DRGB_Display(&green, pgi2c, xflag);
	    if (rgbmask != ALLRGBOFF) PG_Write_4DRGB_RGB(&green, pgi2c, rgbmask, xflag);
	    mdigits[0] = codes7seg0[mdvd[0]];
	    mdigits[1] = codes7seg0[mdvd[1]];
	    mdigits[2] = codes7seg0[mdvd[2]];
	    mdigits[3] = codes7seg0[mdvd[3]];
	    load_ht16k33_digits(&green, mdigits);
	    PG_Write_4DRGB_Digits(&green, pgi2c, FALSE, xflag);
	    break;
	  }
	case FULLTST:
	  {
	    PG_Clear_4DRGB_Display(&green, pgi2c, xflag);
	    PG_Fill_4DRGB_Display(&green, pgi2c, TRUE, xflag);
	    break;
	  }
	case QUIKTST:
	  {
	    pgi2c.bVal = green.ssr;
	    PG_i2cout(pgi2c, xflag);
	    pgi2c.bVal = green.dspr;
	    PG_i2cout(pgi2c, xflag);
	    sleep(10);
	    green.ssr = SSR_STANDBY;
	    pgi2c.bVal = green.ssr;
	    PG_i2cout(pgi2c, xflag);
	    break;
	  }
	case SDGTTST:
	  {
	    PG_Clear_4DRGB_Display(&green, pgi2c, xflag);
	    if (rgbmask != ALLRGBOFF) PG_Write_4DRGB_RGB(&green, pgi2c, rgbmask, xflag);
	    mdigits[0] = codes7seg0[ 0];
	    mdigits[1] = codes7seg0[ 1] | codes7seg0[18];
	    mdigits[2] = codes7seg0[ 2] | codes7seg0[18];
	    mdigits[3] = codes7seg0[ 3];
	    load_ht16k33_digits(&green, mdigits);
	    PG_Write_4DRGB_Digits(&green, pgi2c, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[ 4];
	    mdigits[1] = codes7seg0[ 5];
	    mdigits[2] = codes7seg0[ 6];
	    mdigits[3] = codes7seg0[ 7];
	    load_ht16k33_digits(&green, mdigits);
	    PG_Write_4DRGB_Digits(&green, pgi2c, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[ 8];
	    mdigits[1] = codes7seg0[ 9] | codes7seg0[18];
	    mdigits[2] = codes7seg0[10] | codes7seg0[18];
	    mdigits[3] = codes7seg0[11];
	    load_ht16k33_digits(&green, mdigits);
	    PG_Write_4DRGB_Digits(&green, pgi2c, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[12];
	    mdigits[1] = codes7seg0[13];
	    mdigits[2] = codes7seg0[14];
	    mdigits[3] = codes7seg0[15];
	    load_ht16k33_digits(&green, mdigits);
	    PG_Write_4DRGB_Digits(&green, pgi2c, TRUE, xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_PIGPIO_H */
  
#ifdef HAVE_PIGPIOD_IF2_H
  if (i2cmode == PIGDMODE)
    {
      switch(demode)
	{
	case BLNKTST:
	  {
	    PGD_Clear_4DRGB_Display(&green, pgdi2c, xflag);
	    PGD_Fill_4DRGB_Display(&green, pgdi2c, FALSE, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK20HZ);
	    pgdi2c.bVal = green.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(4);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    pgdi2c.bVal = green.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK10HZ);
	    pgdi2c.bVal = green.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(8);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    pgdi2c.bVal = green.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr | BLINK05HZ);
	    pgdi2c.bVal = green.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(16);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    green.dspr = (green.dspr & BLINKOFF);
	    pgdi2c.bVal = green.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    break;
	  }
	case CLKTST:
	  {
	    PGD_Clear_4DRGB_Display(&green, pgdi2c, xflag);
	    if (rgbmask != ALLRGBOFF) PGD_Write_4DRGB_RGB(&green, pgdi2c, rgbmask, xflag);
	    get_timenow(timenowis);
	    printf("Check3: %s\n", timenowis);
	    make_col_white(timenowis);
	    printf("Check4: %s\n", timenowis);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) hours = atoi(buffer2);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) mins = atoi(buffer2);
	    l = parse_spt(buffer2, timenowis);
	    if (l > 0) secs = atoi(buffer2);
	    printf("Check5: %02d-%02d-%02d\n", hours, mins, secs);
	    dival = (100 * hours) + mins;
	    printf("Check6: %04d\n", dival);
	    mdvd[0] = dival/1000;
	    mdvd[1] = (dival-(mdvd[0]*1000))/100;
	    mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
	    mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
	    mdigits[0] = codes7seg0[mdvd[0]];
	    mdigits[1] = codes7seg0[mdvd[1]] | codes7seg0[18];
	    mdigits[2] = codes7seg0[mdvd[2]] | codes7seg0[18];
	    mdigits[3] = codes7seg0[mdvd[3]];
	    load_ht16k33_digits(&green, mdigits);
	    PGD_Write_4DRGB_Digits(&green, pgdi2c, FALSE, xflag);
	    break;
	  }
	case CCLK:
	  {
	    PGD_Clear_4DRGB_Display(&green, pgdi2c, xflag);
	    if (rgbmask != ALLRGBOFF) PGD_Write_4DRGB_RGB(&green, pgdi2c, rgbmask, xflag);
	    for (i=0; i<300; i++)
	      {
		get_timenow(timenowis);
		if (vflag) printf("Check3: %s\n", timenowis);
		make_col_white(timenowis);
		if (vflag) printf("Check4: %s\n", timenowis);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) hours = atoi(buffer2);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) mins = atoi(buffer2);
		l = parse_spt(buffer2, timenowis);
		if (l > 0) secs = atoi(buffer2);
		if (vflag) printf("Check5: %02d-%02d-%02d\n", hours, mins, secs);
		dival = (100 * hours) + mins;
		if (vflag) printf("Check6: %04d\n", dival);
		mdvd[0] = dival/1000;
		mdvd[1] = (dival-(mdvd[0]*1000))/100;
		mdvd[2] = (dival-(mdvd[0]*1000)-(mdvd[1]*100))/10;
		mdvd[3] = (dival-(mdvd[0]*1000)-(mdvd[1]*100)-(mdvd[2]*10));
		mdigits[0] = codes7seg0[mdvd[0]];
		j = i % 2;
		if (j == 0)
		  {
		    mdigits[1] = codes7seg0[mdvd[1]] | codes7seg0[18];
		    mdigits[2] = codes7seg0[mdvd[2]] | codes7seg0[18];
		  }
		else
		  {
		    mdigits[1] = codes7seg0[mdvd[1]];
		    mdigits[2] = codes7seg0[mdvd[2]];
		  }
		mdigits[3] = codes7seg0[mdvd[3]];
		load_ht16k33_digits(&green, mdigits);
		PGD_Write_4DRGB_Digits(&green, pgdi2c, FALSE, xflag);
		sleep(1);
	      }
	    break;
	  }
	case DIMMTST:
	  {
	    PGD_Clear_4DRGB_Display(&green, pgdi2c, xflag);
	    PGD_Fill_4DRGB_Display(&green, pgdi2c, FALSE, xflag);
	    green.dmsr = (DIMSETMASK | DIM01_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    green.dspr = (DSPR_ADDR | DISPONMSK);
	    pgdi2c.bVal = green.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM02_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM03_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM04_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM05_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM06_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM07_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM08_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM09_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM10_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM11_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM12_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM13_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM14_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM15_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(1);
	    green.dmsr = (DIMSETMASK | DIM16_16);
	    pgdi2c.bVal = green.dmsr;
	    PGD_i2cout(pgdi2c, xflag);
	    break;
	  }
	case DISINT:
	  {
	    PGD_Clear_4DRGB_Display(&green, pgdi2c, xflag);
	    if (rgbmask != ALLRGBOFF) PGD_Write_4DRGB_RGB(&green, pgdi2c, rgbmask, xflag);
	    mdigits[0] = codes7seg0[mdvd[0]];
	    mdigits[1] = codes7seg0[mdvd[1]];
	    mdigits[2] = codes7seg0[mdvd[2]];
	    mdigits[3] = codes7seg0[mdvd[3]];
	    load_ht16k33_digits(&green, mdigits);
	    PGD_Write_4DRGB_Digits(&green, pgdi2c, FALSE, xflag);
	    break;
	  }
	case FULLTST:
	  {
	    PGD_Clear_4DRGB_Display(&green, pgdi2c, xflag);
	    PGD_Fill_4DRGB_Display(&green, pgdi2c, TRUE, xflag);
	    break;
	  }
	case QUIKTST:
	  {
	    pgdi2c.bVal = green.ssr;
	    PGD_i2cout(pgdi2c, xflag);
	    pgdi2c.bVal = green.dspr;
	    PGD_i2cout(pgdi2c, xflag);
	    sleep(10);
	    green.ssr = SSR_STANDBY;
	    pgdi2c.bVal = green.ssr;
	    PGD_i2cout(pgdi2c, xflag);
	    break;
	  }
	case SDGTTST:
	  {
	    PGD_Clear_4DRGB_Display(&green, pgdi2c, xflag);
	    if (rgbmask != ALLRGBOFF) PGD_Write_4DRGB_RGB(&green, pgdi2c, rgbmask, xflag);
	    mdigits[0] = codes7seg0[ 0];
	    mdigits[1] = codes7seg0[ 1] | codes7seg0[18];
	    mdigits[2] = codes7seg0[ 2] | codes7seg0[18];
	    mdigits[3] = codes7seg0[ 3];
	    load_ht16k33_digits(&green, mdigits);
	    PGD_Write_4DRGB_Digits(&green, pgdi2c, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[ 4];
	    mdigits[1] = codes7seg0[ 5];
	    mdigits[2] = codes7seg0[ 6];
	    mdigits[3] = codes7seg0[ 7];
	    load_ht16k33_digits(&green, mdigits);
	    PGD_Write_4DRGB_Digits(&green, pgdi2c, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[ 8];
	    mdigits[1] = codes7seg0[ 9] | codes7seg0[18];
	    mdigits[2] = codes7seg0[10] | codes7seg0[18];
	    mdigits[3] = codes7seg0[11];
	    load_ht16k33_digits(&green, mdigits);
	    PGD_Write_4DRGB_Digits(&green, pgdi2c, TRUE, xflag);
	    sleep(5);
	    mdigits[0] = codes7seg0[12];
	    mdigits[1] = codes7seg0[13];
	    mdigits[2] = codes7seg0[14];
	    mdigits[3] = codes7seg0[15];
	    load_ht16k33_digits(&green, mdigits);
	    PGD_Write_4DRGB_Digits(&green, pgdi2c, TRUE, xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_PIGPIOD_IF2_H */
  
  /* Tidy up ... */
#ifdef HAVE_PIGPIO_H
  if (i2cmode == PIGMODE)
    {
      if (pgi2c.handle != PG_UNDEF_HANDLE) 
	{
	  printf("Closing I2C Device %d:0x%2x\n",
		 pgi2c.i2cBus,
		 pgi2c.i2cAddr);
	  k = i2cClose(pgi2c.handle);
	}
      if (pigpiovsn > 0) gpioTerminate();
      printf("pigpio Version %d terminated.\n", pigpiovsn);
    }
#endif /* HAVE_PIGPIO_H */

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
  
  echo_ht16k33_status(green);
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: i2c_clkled_demo.c,v $
 * Revision 1.21  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.20  2018/01/07 21:40:16  pi
 * -QIT with -PGD option bug-fix
 *
 * Revision 1.19  2018/01/01 22:38:18  pi
 * SCMVersion 0.02
 *
 * Revision 1.18  2018/01/01 22:35:43  pi
 * -PG & -PGD options fully supported
 *
 * Revision 1.17  2017/12/26 23:19:45  pi
 * Partial implementation of -PG & -PGD (pigpio/pigpiod) methods
 *
 * Revision 1.16  2017/12/26 17:30:59  pi
 * Interim commit - WIP!!! TBC!!!
 *
 * Revision 1.15  2017/12/23 19:01:25  pi
 * Interim commit
 *
 * Revision 1.14  2017/12/22 23:33:20  pi
 * Beginning to implement -PGD methods
 *
 * Revision 1.13  2017/12/21 22:55:08  pi
 * Beginning to use more pigpio methods etc.
 *
 * Revision 1.12  2017/12/18 21:56:30  pi
 * SCMVersion 0.01
 *
 * Revision 1.11  2017/12/18 21:55:16  pi
 * +R, +G & +B options added.
 *
 * Revision 1.10  2017/12/18 18:40:29  pi
 * -W mode implemented
 *
 * Revision 1.9  2017/12/18 15:15:44  pi
 * Default I2C Addr now 0x75 (Module A0 & A2 links shorted)
 *
 * Revision 1.8  2017/12/16 23:49:32  pi
 * Beginning to implement -W mode
 *
 * Revision 1.7  2017/12/15 22:26:27  pi
 * -CLKT, -CCLK, -di n & (missing) -V options added
 *
 * Revision 1.6  2017/12/14 21:45:48  pi
 * -SDT option implemented
 *
 * Revision 1.5  2017/12/13 22:47:14  pi
 * Interim commit
 *
 * Revision 1.4  2017/12/11 22:13:35  pi
 * Interim commit
 *
 * Revision 1.3  2017/12/11 17:22:38  pi
 * Interim commit
 *
 * Revision 1.2  2017/12/11 16:27:22  pi
 * Some matrix code removed
 *
 * Revision 1.1  2017/12/11 16:13:05  pi
 * Initial revision
 *
 *
 *
 */

