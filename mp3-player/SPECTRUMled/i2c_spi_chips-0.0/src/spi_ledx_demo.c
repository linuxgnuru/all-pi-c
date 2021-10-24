/* Partially SPI compatible, serial LED matriX module DEMOnstration code
 * $Id: spi_ledx_demo.c,v 1.6 2018/03/31 21:32:45 pi Exp $
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
#ifdef HAVE_WIRINGPISPI_H
#include <wiringPiSPI.h>
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "font.h"
#include "PCD8544_font.h"
#include "parsel.h"
#include "spi_chips.h"
#include "spi_wraps.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: spi_ledx_demo.c,v $";
static char RCSId[]       = "$Id: spi_ledx_demo.c,v 1.6 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.6 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.03 $";

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
  QUIET,     /* -Q                   */
  BRITETST,  /* -BT                  */
  CASCLEN,   /* -cascade n           */
  CASCDBT,   /* -CBT                 */
  CASCDST,   /* -CST                 */
  CASCFT1,   /* -CFT1                */
  CASCFT2,   /* -CFT2                */
  CASCFT3,   /* -CFT3                */
  DPNTTST,   /* -DPT                 */
  DGTTST1,   /* -DT1                 */
  DGTTST2,   /* -DT2                 */
  DGTTST3,   /* -DT3                 */
  FNTTST1,   /* -FT1                 */
  FNTTST2,   /* -FT2                 */
  FNTTST3,   /* -FT3                 */
  FNT8X8,    /* -F8x8                */
  FNT8X5R,   /* -F8x5R               */
  LED8DGT,   /* -8DGT                */
  LEDMTX,    /* -MTX                 */
  LEDMTX2,   /* -MTX2                */
  LEDMTX4,   /* -MTX4                */
  SELFTST,   /* -ST                  */
  SYSMODE,   /* -S                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPISPI_H
  WPICFFT,   /* -WCFFT               */
  WPIFFT,    /* -WFFT                */
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPISPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,disptype,fonttype,spimode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[QUIET]    = "-Q";
  arglist[BRITETST] = "-BT";
  arglist[CASCLEN]  = "-cascade";
  arglist[CASCDBT]  = "-CBT";
  arglist[CASCDST]  = "-CST";
  arglist[CASCFT1]  = "-CFT1";
  arglist[CASCFT2]  = "-CFT2";
  arglist[CASCFT3]  = "-CFT3";
  arglist[DPNTTST]  = "-DPT";
  arglist[DGTTST1]  = "-DT1";
  arglist[DGTTST2]  = "-DT2";
  arglist[DGTTST3]  = "-DT3";
  arglist[FNTTST1]  = "-FT1";
  arglist[FNTTST2]  = "-FT2";
  arglist[FNTTST3]  = "-FT3";
  arglist[FNT8X8]   = "-F8x8";
  arglist[FNT8X5R]  = "-F8x5R";
  arglist[LED8DGT]  = "-8DGT";
  arglist[LEDMTX]   = "-MTX";
  arglist[LEDMTX2]  = "-MTX2";
  arglist[LEDMTX4]  = "-MTX4";
  arglist[SELFTST]  = "-ST";
  arglist[SYSMODE]  = "-S";
  arglist[VERBOSE]  = "-V";
#ifdef HAVE_WIRINGPISPI_H
  arglist[WPICFFT]  = "-WCFFT";
  arglist[WPIFFT]   = "-WFFT";
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPISPI_H */
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
char eflag,oflag,vflag,xflag;
int casclen;

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
	    case ECHO:     eflag=TRUE;       break;
	    case QUIET:    vflag=FALSE;      break;
	    case BRITETST: demode=BRITETST;  break;
	    case CASCDBT:  demode=CASCDBT;   break;
	    case CASCDST:  demode=CASCDST;   break;
	    case CASCLEN:  
	      {
		casclen=atoi(buffer2);
		if (casclen < 1) casclen = 1;
		if (casclen > MAX_CASCADE_LEN) casclen = MAX_CASCADE_LEN;  
		break;
	      }
	    case CASCFT1:  demode=CASCFT1;   break;
	    case CASCFT2:  demode=CASCFT2;   break;
	    case CASCFT3:  demode=CASCFT3;   break;
	    case DPNTTST:  demode=DPNTTST;   break;
	    case DGTTST1:  demode=DGTTST1;   break;
	    case DGTTST2:  demode=DGTTST2;   break;
	    case DGTTST3:  demode=DGTTST3;   break;
	    case FNTTST1:  demode=FNTTST1;   break;
	    case FNTTST2:  demode=FNTTST2;   break;
	    case FNTTST3:  demode=FNTTST3;   break;
	    case FNT8X8:   fonttype=FNT8X8;  break;
	    case FNT8X5R:  fonttype=FNT8X5R; break;
	    case LED8DGT:  disptype=LED8DGT; break;
	    case LEDMTX:   disptype=LEDMTX;  break;
	    case LEDMTX2:  disptype=LEDMTX2; casclen=2; break;
	    case LEDMTX4:  disptype=LEDMTX4; casclen=4; break;
	    case SELFTST:  demode=SELFTST;   break;
	    case SYSMODE:  spimode=SYSMODE;  break;
	    case VERBOSE:  eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPISPI_H
	    case WPICFFT:  spimode=WPIMODE; demode=WPICFFT; break;
	    case WPIFFT:   spimode=WPIMODE; demode=WPIFFT;  break;
	    case WPIMODE:  spimode=WPIMODE; break;
#endif /* HAVE_WIRINGPISPI_H */
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
  strcat(buffer1, "         -cascade d\n");
  strcat(buffer1, "                {CASCADE of d matrices (1 < d <= 128)},\n");
  strcat(buffer1, "         -8DGT  {8-DiGiT led display},\n");
  strcat(buffer1, "         -MTX   {8x8 led MaTriX display - default},\n");
  strcat(buffer1, "         -MTX2  {16x8 dual led MaTriX display#},\n");
  strcat(buffer1, "         -MTX4  {32x8 quad led MaTriX display},\n");
  strcat(buffer1, "                {#on spidev0.0, others on spidev0.1},\n");
  strcat(buffer1, "         -E     {Echo command-line arguments},\n");
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q     {Quiet - default},\n");
  strcat(buffer1, "         -BT    {Brightness Test},\n");
  strcat(buffer1, "         -CBT   {Cascade Brightness Test},\n");
  strcat(buffer1, "         -CST   {Cascade Self Test},\n");
  strcat(buffer1, "         -CFT1  {Cascade Font Test #1},\n");
  strcat(buffer1, "         -CFT2  {Cascade Font Test #2},\n");
  strcat(buffer1, "         -CFT3  {Cascade Font Test #3},\n");
  /*
  strcat(buffer1, "         -CBT   {Cascade Brightness Test^^},\n");
  strcat(buffer1, "         -CST   {Cascade Self Test^^},\n");
  strcat(buffer1, "         -CFT1  {Cascade Font Test #1^^},\n");
  strcat(buffer1, "         -CFT2  {Cascade Font Test #2^^},\n");
  strcat(buffer1, "         -CFT3  {Cascade Font Test #3^^},\n");
  strcat(buffer1, "                {^^ default is for two matrices},\n");
 */
  strcat(buffer1, "         -DPT   {Decimal Point Test},\n");
  strcat(buffer1, "         -DT1   {Digit Test #1},\n");
  strcat(buffer1, "         -DT2   {Digit Test #2},\n");
  strcat(buffer1, "         -DT3   {Digit Test #3},\n");
  strcat(buffer1, "         -FT1   {Font Test #1},\n");
  strcat(buffer1, "         -FT2   {Font Test #2},\n");
  strcat(buffer1, "         -FT3   {Font Test #3},\n");
  strcat(buffer1, "         -F8x8  {use 8x8 Font - default},\n");
  strcat(buffer1, "         -F8x5R {use Rotated 8x5 Font (PCD8544 font)},\n");
  strcat(buffer1, "         -ST    {Self Test the LED matrix},\n");
#ifndef NO_SUDO_ECHO_TO_SPI
  strcat(buffer1, "         -S     {use System call methods** - default},\n");
#else /* NO_SUDO_ECHO_TO_SPI */
  strcat(buffer1, "         -S     {use System call methods - default},\n");
#endif /* NO_SUDO_ECHO_TO_SPI */
  strcat(buffer1, "         -V     {Verbose},\n");
#ifdef HAVE_WIRINGPISPI_H
  /* strcat(buffer1, "         -WCFFT {Wiring pi Cascaded Full Font Test^^},\n"); */
  strcat(buffer1, "         -WCFFT {Wiring pi Cascaded Full Font Test},\n");
  strcat(buffer1, "         -WFFT  {Wiring pi Full Font Test},\n");
  strcat(buffer1, "         -W     {use Wiring pi spi methods},\n");
#endif /* HAVE_WIRINGPISPI_H */
  strcat(buffer1, "         -X     {eXecute demonstration}.\n");
#ifndef NO_SUDO_ECHO_TO_SPI
  strcat(buffer1, " ** NB: Uses \"sudo echo ...\" to the SPI device!");
#endif /* NO_SUDO_ECHO_TO_SPI */
  print_us(getRCSFile(TRUE), buffer1);
}

void C2S_secho_font2Digits(max7219 *chip,
			   char *thisfont,
			   unsigned int fidx,
			   int nrow,
			   char qx)
{
  unsigned int lfidx;
  int lrow;
  lfidx = fidx * nrow;
  lrow = 0;
  C2S_secho2spidev((*chip).spidev,
		   (*chip).Digit0,
		   (unsigned int)(thisfont[lfidx]),
		   qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  C2S_secho2spidev((*chip).spidev,
		   (*chip).Digit1,
		   (unsigned int)(thisfont[lfidx]),
		   qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  C2S_secho2spidev((*chip).spidev,
		   (*chip).Digit2,
		   (unsigned int)(thisfont[lfidx]),
		   qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  C2S_secho2spidev((*chip).spidev,
		   (*chip).Digit3,
		   (unsigned int)(thisfont[lfidx]),
		   qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  C2S_secho2spidev((*chip).spidev,
		   (*chip).Digit4,
		   (unsigned int)(thisfont[lfidx]),
		   qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  C2S_secho2spidev((*chip).spidev,
		   (*chip).Digit5,
		   (unsigned int)(thisfont[lfidx]),
		   qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  C2S_secho2spidev((*chip).spidev,
		   (*chip).Digit6,
		   (unsigned int)(thisfont[lfidx]),
		   qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  C2S_secho2spidev((*chip).spidev,
		   (*chip).Digit7,
		   (unsigned int)(thisfont[lfidx]),
		   qx);
}

void C2S_secho_font2cascadeDigits(max7219 *chip,
				  char *thisfont,
				  unsigned int fidx,
				  int nrow,
				  cascade thiscascade[],
				  int casposn,
				  int caslen,
				  char qx)
{
  unsigned int lfidx;
  int k,lrow;
  lfidx = fidx * nrow;
  lrow = 0;
  reset_cascade(thiscascade);
  fill_cascade(thiscascade,
	       (*chip).NoOp,
	       NOOP_DATA,
	       caslen);
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit0,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  C2S_cascade2spidev((*chip).spidev,
		     thiscascade,
		     xflag);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit1,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  C2S_cascade2spidev((*chip).spidev,
		     thiscascade,
		     xflag);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit2,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  C2S_cascade2spidev((*chip).spidev,
		     thiscascade,
		     xflag);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit3,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  C2S_cascade2spidev((*chip).spidev,
		     thiscascade,
		     xflag);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit4,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  C2S_cascade2spidev((*chip).spidev,
		     thiscascade,
		     xflag);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit5,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  C2S_cascade2spidev((*chip).spidev,
		     thiscascade,
		     xflag);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit6,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  C2S_cascade2spidev((*chip).spidev,
		     thiscascade,
		     xflag);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit7,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  C2S_cascade2spidev((*chip).spidev,
		     thiscascade,
		     xflag);
}
				  
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
/* Temporary Kernel BUG FIX for 'C' system calls ... */
#define TKBUGFIX 1
/* Comment out above when no longer needed */
void WP_write_font2Digits(int spifd,
			  max7219 *chip,
			  char *thisfont,
			  unsigned int fidx,
			  int nrow,
			  char qx)
{
  unsigned int lfidx;
  int lrow;
  lfidx = fidx * nrow;
  lrow = 0;
  WP_write2bytes(spifd,
		 (*chip).Digit0,
		 (unsigned int)(thisfont[lfidx]),
		 qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  WP_write2bytes(spifd,
		 (*chip).Digit1,
		 (unsigned int)(thisfont[lfidx]),
		 qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  WP_write2bytes(spifd,
		 (*chip).Digit2,
		 (unsigned int)(thisfont[lfidx]),
		 qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  WP_write2bytes(spifd,
		 (*chip).Digit3,
		 (unsigned int)(thisfont[lfidx]),
		 qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  WP_write2bytes(spifd,
		 (*chip).Digit4,
		 (unsigned int)(thisfont[lfidx]),
		 qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  WP_write2bytes(spifd,
		 (*chip).Digit5,
		 (unsigned int)(thisfont[lfidx]),
		 qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  WP_write2bytes(spifd,
		 (*chip).Digit6,
		 (unsigned int)(thisfont[lfidx]),
		 qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  WP_write2bytes(spifd,
		 (*chip).Digit7,
		 (unsigned int)(thisfont[lfidx]),
		 qx);
}

void WP_write_font2cascadeDigits(int spifd,
				 max7219 *chip,
				 char *thisfont,
				 unsigned int fidx,
				 int nrow,
				 cascade thiscascade[],
				 int casposn,
				 int caslen,
				 char qx)
{
  unsigned int lfidx;
  int k,lrow;
  lfidx = fidx * nrow;
  lrow = 0;
  reset_cascade(thiscascade);
  fill_cascade(thiscascade,
	       (*chip).NoOp,
	       NOOP_DATA,
	       caslen);
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit0,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  WP_write_cascade(spifd, thiscascade, qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit1,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  WP_write_cascade(spifd, thiscascade, qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit2,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  WP_write_cascade(spifd, thiscascade, qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit3,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  WP_write_cascade(spifd, thiscascade, qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit4,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  WP_write_cascade(spifd, thiscascade, qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit5,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  WP_write_cascade(spifd, thiscascade, qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit6,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  WP_write_cascade(spifd, thiscascade, qx);
  lfidx++;
  lrow++;
  if (lrow == nrow) return;
  k = insert_into_cascade(thiscascade,
			  (*chip).Digit7,
			  (unsigned int)(thisfont[lfidx]),
			  casposn,
			  caslen);
  WP_write_cascade(spifd, thiscascade, qx);
}

#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char *myfont;
  char spidev;
  int i,j,k,l;
  int mrow,nbytes;
  unsigned int all_led_off,all_led_on;
#ifdef HAVE_WIRINGPISPI_H
  int wP_SPIfd[2],wP_SPIfdx;
#endif /* HAVE_WIRINGPISPI_H */
  mcp23S08 dummy;
  max7219 ledx8x8;
  max7221 dummy2;

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
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
#ifdef TKBUGFIX
      printf("Testing Kernel Bug Fix (wiringPi workaround) for -S methods...\n");
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
#endif /* TKBUGFIX */      
      exit(EXIT_SUCCESS);
    }

  /* Set up defaults */
  eflag    = vflag = xflag = FALSE;
  casclen  = 1;
  spidev   = SPI_DEV01;
  spimode  = SYSMODE;
  disptype = LEDMTX;
  fonttype = FNT8X8;
#ifdef HAVE_WIRINGPISPI_H
  wP_SPIfd[0] = wP_SPIfd[1] = -1; /* ie. as yet undefined */
#endif /* HAVE_WIRINGPISPI_H */
 
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
	    case ECHO:     eflag=TRUE;       i--; break;
	    case QUIET:    vflag=FALSE;      i--; break;
	    case BRITETST: demode=BRITETST;  i--; break;
	    case CASCLEN:  
	      {
		casclen=atoi(p2);
		if (casclen < 1) casclen = 1;
		if (casclen > MAX_CASCADE_LEN) casclen = MAX_CASCADE_LEN;  
		break;
	      }
	    case CASCDBT:  demode=CASCDBT;   i--; break;
	    case CASCDST:  demode=CASCDST;   i--; break;
	    case CASCFT1:  demode=CASCFT1;   i--; break;
	    case CASCFT2:  demode=CASCFT2;   i--; break;
	    case CASCFT3:  demode=CASCFT3;   i--; break;
	    case DPNTTST:  demode=DPNTTST;   i--; break;
	    case DGTTST1:  demode=DGTTST1;   i--; break;
	    case DGTTST2:  demode=DGTTST2;   i--; break;
	    case DGTTST3:  demode=DGTTST3;   i--; break;
	    case FNTTST1:  demode=FNTTST1;   i--; break;
	    case FNTTST2:  demode=FNTTST2;   i--; break;
	    case FNTTST3:  demode=FNTTST3;   i--; break;
	    case FNT8X8:   fonttype=FNT8X8;  i--; break;
	    case FNT8X5R:  fonttype=FNT8X5R; i--; break;
	    case LED8DGT:  disptype=LED8DGT; i--; break;
	    case LEDMTX:   disptype=LEDMTX;  i--; break;
	    case LEDMTX2:
	      {
		disptype=LEDMTX2;
		casclen=2;
		i--;
		break;
	      }
	    case LEDMTX4:
	      {
		disptype=LEDMTX4;
		casclen=4;
		i--;
		break;
	      }
	    case SELFTST:  demode=SELFTST;   i--; break;
	    case SYSMODE:  spimode=SYSMODE;  i--; break;
	    case VERBOSE:  eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPISPI_H
	    case WPICFFT:  spimode=WPIMODE; demode=WPICFFT; i--; break;
	    case WPIFFT:   spimode=WPIMODE; demode=WPIFFT;  i--; break;
	    case WPIMODE:  spimode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPISPI_H */
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
  if (disptype == LEDMTX2) spidev = SPI_DEV00;

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

  /* Initialisation ... */
  
  switch(fonttype)
    {
    case FNT8X5R:
      {
	myfont = font8x5;
	mrow   = font8x5Width;
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
  /*
  if ((disptype == LEDMTX) || (disptype == LEDMTX2)) 
    {
      init_max7219(&ledx8x8, spidev, "ledx8x8");
      all_led_off = ALL_DOTS_OFF;
      all_led_on  = ALL_DOTS_ON;
    }
  if (disptype == LED8DGT)
    {
      init_max7219(&ledx8x8, spidev, "led8digit");
      all_led_off = ALL_SEG_DP_OFF;
      all_led_on  = ALL_SEG_DP_ON;
    }
  */
  switch(disptype)
    {
    case LEDMTX2:
      {
	init_max7219(&ledx8x8, spidev, "ledx8x8x2");
	all_led_off = ALL_DOTS_OFF;
	all_led_on  = ALL_DOTS_ON;
	break;
      }
    case LEDMTX4:
      {
	init_max7219(&ledx8x8, spidev, "ledx8x8x4");
	all_led_off = ALL_DOTS_OFF;
	all_led_on  = ALL_DOTS_ON;
	break;
      }
    case LED8DGT:
      {
	init_max7219(&ledx8x8, spidev, "led8digit");
	all_led_off = ALL_SEG_DP_OFF;
	all_led_on  = ALL_SEG_DP_ON;
	break;
      }
    case LEDMTX:
    default:
      {
	init_max7219(&ledx8x8, spidev, "ledx8x8");
	all_led_off = ALL_DOTS_OFF;
	all_led_on  = ALL_DOTS_ON;	
	break;
      }
    }
  echo_max7219_status(ledx8x8);
  if (casclen > 1) printf("Cascade of %d Matrices\n", casclen);

  
  if (spimode == SYSMODE)
    {
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
#ifdef TKBUGFIX
      printf("Testing Kernel Bug Fix (wiringPi workaround) for -S methods...\n");
      /* Dummy wiringPi SPI init to (attempt to) set SPI clocks ...*/
      if ((wP_SPIfd[0]=wiringPiSPISetup(0, 500000)) < 0)
	{
	  fprintf(stderr, "Could not setup wP_SPIfd[0]!\n");
	  perror("wiringPiSPISetup");
	  exit(EXIT_FAILURE);
	}
      if ((wP_SPIfd[1]=wiringPiSPISetup(1, 500000)) < 0)
	{
	  fprintf(stderr, "Could not setup wP_SPIfd[1]!\n");
	  perror("wiringPiSPISetup");
	  exit(EXIT_FAILURE);
	}
#endif /* TKBUGFIX */
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
      
      switch(demode)
	{
	case BRITETST:
	case DPNTTST:
	case DGTTST1:
	case DGTTST2:
	case DGTTST3:
	case FNTTST1:
	case FNTTST2:
	case FNTTST3:
	  {
	    printf("Using 'C' system call-based methods: ...\n");
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.DisplayTest,
			     DISPLAY_NORMAL_MODE,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Shutdown,
			     NORMAL_OPERATION,
			     xflag);
	    /*
	    if (disptype == LEDMTX)
	      C2S_secho2spidev(ledx8x8.spidev,
			       ledx8x8.DecodeMode,
			       NO_DECODE_ALL_DIGITS,
			       xflag);
	    if (disptype == LED8DGT)
	      C2S_secho2spidev(ledx8x8.spidev,
			       ledx8x8.DecodeMode,
			       CODE_B_DECODE_ALL_DIGITS,
			       xflag);
	    */
	    switch(disptype)
	      {
	      case LED8DGT:
		{
		  C2S_secho2spidev(ledx8x8.spidev,
				   ledx8x8.DecodeMode,
				   CODE_B_DECODE_ALL_DIGITS,
				   xflag);
		  break;
		}
	      case LEDMTX:
	      case LEDMTX2:
	      case LEDMTX4:
	      default:
		{
		  C2S_secho2spidev(ledx8x8.spidev,
				   ledx8x8.DecodeMode,
				   NO_DECODE_ALL_DIGITS,
				   xflag);
		  break;
		}
	      }
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit0,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit1,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit2,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit3,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit4,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit5,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit6,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit7,
			     all_led_off,
			     xflag);
	    break;
	  }
	case CASCDBT:
	case CASCFT1:
	case CASCFT2:
	case CASCFT3:
	  {
	    printf("Using 'C' system call-based methods: ...\n");
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.DisplayTest,
			 DISPLAY_NORMAL_MODE,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Shutdown,
			 NORMAL_OPERATION,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.DecodeMode,
			 NO_DECODE_ALL_DIGITS,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit0,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit1,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit2,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit3,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit4,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit5,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit6,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit7,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
 	    break;
	  }
	case SELFTST:
	case CASCDST:
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
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
      if (disptype != LEDMTX2) wP_SPIfdx = wP_SPIfd[1];
      else wP_SPIfdx = wP_SPIfd[0];
      switch(demode)
	{
	case BRITETST:
	case DPNTTST:
	case DGTTST1:
	case DGTTST2:
	case DGTTST3:
	case FNTTST1:
	case FNTTST2:
	case FNTTST3:
	case WPIFFT:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.DisplayTest,
			   DISPLAY_NORMAL_MODE,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Shutdown,
			   NORMAL_OPERATION,
			   xflag);
	    if (disptype == LEDMTX)
	      WP_write2bytes(wP_SPIfdx,
			     ledx8x8.DecodeMode,
			     NO_DECODE_ALL_DIGITS,
			     xflag);
	    if (disptype == LEDMTX2)
	      WP_write2bytes(wP_SPIfdx,
			     ledx8x8.DecodeMode,
			     NO_DECODE_ALL_DIGITS,
			     xflag);
	    if (disptype == LEDMTX4)
	      WP_write2bytes(wP_SPIfdx,
			     ledx8x8.DecodeMode,
			     NO_DECODE_ALL_DIGITS,
			     xflag);
	    if (disptype == LED8DGT)
	      WP_write2bytes(wP_SPIfdx,
			     ledx8x8.DecodeMode,
			     CODE_B_DECODE_ALL_DIGITS,
			     xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit0,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit1,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit2,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit3,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit4,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit5,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit6,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit7,
			   all_led_off,
			   xflag);
	    break;
	  }
	case CASCDBT:
	case CASCFT1:
	case CASCFT2:
	case CASCFT3:
	case WPICFFT:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.DisplayTest,
			 DISPLAY_NORMAL_MODE,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Shutdown,
			 NORMAL_OPERATION,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.DecodeMode,
			 NO_DECODE_ALL_DIGITS,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit0,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit1,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit2,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit3,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit4,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit5,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit6,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit7,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    break;
	  }
	case SELFTST:
	case CASCDST:
	default: break;
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Demonstration ... */

  if (spimode == SYSMODE)
    {
      if ((demode == SELFTST) || (demode == CASCDST))
	printf("Using 'C' system call-based methods: ...\n");
      switch(demode)
	{
	case BRITETST:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MIN_INTENSITY,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit0,
			     all_led_on,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit1,
			     all_led_on,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit2,
			     all_led_on,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit3,
			     all_led_on,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit4,
			     all_led_on,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit5,
			     all_led_on,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit6,
			     all_led_on,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit7,
			     all_led_on,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    sleep(2);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MAX_INTENSITY,
			     xflag);
	    sleep(3);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    sleep(2);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MIN_INTENSITY,
			     xflag);
	    sleep(1);
	    break;
	  }
	case CASCDBT:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit0,
			 all_led_on,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit1,
			 all_led_on,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit2,
			 all_led_on,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit3,
			 all_led_on,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit4,
			 all_led_on,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit5,
			 all_led_on,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit6,
			 all_led_on,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit7,
			 all_led_on,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MED_INTENSITY,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(2);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MAX_INTENSITY,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(3);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MED_INTENSITY,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(2);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(1);
	    break;
	  }
	case CASCFT1:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    l = 7 + casclen;
	    for (i=0; i<l; i=i+casclen)
	      {
		j = i + 48;
		for (k=0; k<casclen; k++)
		  {
		    C2S_secho_font2cascadeDigits(&ledx8x8,
						 myfont,
						 j,
						 mrow,
						 led_matrix,
						 k,
						 casclen,
						 xflag);
		    j++;
		  }
		sleep(2);
	      }
	    break;
	  }
	case CASCFT2:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    l = 24 + casclen;
	    for (i=0; i<l; i=i+casclen)
	      {
		j = i + 65;
		for (k=0; k<casclen; k++)
		  {
		    C2S_secho_font2cascadeDigits(&ledx8x8,
						 myfont,
						 j,
						 mrow,
						 led_matrix,
						 k,
						 casclen,
						 xflag);
		    j++;
		  }
		sleep(2);
	      }
	    break;
	  }
	case CASCFT3:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    l = 24 + casclen;
	    for (i=0; i<l; i=i+casclen)
	      {
		j = i + 97;
		for (k=0; k<casclen; k++)
		  {
		    C2S_secho_font2cascadeDigits(&ledx8x8,
						 myfont,
						 j,
						 mrow,
						 led_matrix,
						 k,
						 casclen,
						 xflag);
		    j++;
		  }
		sleep(2);
	      }
	    break;
	  }
	case DPNTTST:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit0,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit1,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit2,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit3,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit4,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit5,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit6,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit7,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(2);
	    break;
	  }
	case DGTTST1:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit0,
			     0,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit1,
			     1,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit2,
			     2,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit3,
			     3,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit4,
			     4,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit5,
			     5,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit6,
			     6,
			     xflag);
	    sleep(1);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit7,
			     7,
			     xflag);
	    sleep(1);
	    break;
	  }
	case DGTTST2:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit7,
			     (3 + DP_OFFSET),
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit6,
			     1,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit5,
			     4,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit4,
			     1,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit3,
			     5,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit2,
			     9,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit1,
			     2,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit0,
			     7,
			     xflag);
	    sleep(8);
	    break;
	  }
	case DGTTST3:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit7,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit6,
			     CODE_B_HYPHEN,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit5,
			     CODE_B_H,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit4,
			     CODE_B_E,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit3,
			     CODE_B_L,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit2,
			     CODE_B_P,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit1,
			     CODE_B_HYPHEN,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit0,
			     ALL_SEG_OFF_DP_ON,
			     xflag);
	    sleep(8);
	    break;
	  }
	case FNTTST1:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    for (i=0; i<9; i++)
	      {
		j = i + 48;
		C2S_secho_font2Digits(&ledx8x8,
				      myfont,
				      j,
				      mrow,
				      xflag);
		sleep(1);
	      }
	    break;
	  }
	case FNTTST2:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		C2S_secho_font2Digits(&ledx8x8,
				      myfont,
				      j,
				      mrow,
				      xflag);
		sleep(1);
	      }
	    break;
	  }
	case FNTTST3:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Intensity,
			     MED_INTENSITY,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.ScanLimit,
			     SCAN_ALL_DIGITS,
			     xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		C2S_secho_font2Digits(&ledx8x8,
				      myfont,
				      j,
				      mrow,
				      xflag);
		sleep(1);
	      }
	    break;
	  }
	case SELFTST:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.DisplayTest,
			     DISPLAY_TEST_MODE,
			     xflag);
	    sleep(2);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.DisplayTest,
			     DISPLAY_NORMAL_MODE,
			     xflag);
	    break;
	  }
	case CASCDST:
	  {
	    reset_cascade(led_matrix);
	    for (i=0; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_TEST_MODE,
				 MAX_CASCADE_LEN);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    for (i=0; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_NORMAL_MODE,
				 MAX_CASCADE_LEN);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(1);
	    for (i=0; i<casclen; i++)
	      {
		reset_cascade(led_matrix);
		for (j=0; j<casclen; j++)
		  {
		    if (j == i)
		      {
			k = add_to_cascade(led_matrix,
					   ledx8x8.DisplayTest,
					   DISPLAY_TEST_MODE,
					   MAX_CASCADE_LEN);
		      }
		    else
		      {
			k = add_to_cascade(led_matrix,
					   ledx8x8.NoOp,
					   NOOP_DATA,
					   MAX_CASCADE_LEN);
		      }
		  }		
		C2S_cascade2spidev(ledx8x8.spidev,
				   led_matrix,
				   xflag);
		sleep(1);	
	      }
	    reset_cascade(led_matrix);
	    for (i=0; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_NORMAL_MODE,
				 MAX_CASCADE_LEN);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    /*
	    reset_cascade(led_matrix);
	    k = add_to_cascade(led_matrix,
			       ledx8x8.DisplayTest,
			       DISPLAY_TEST_MODE,
			       MAX_CASCADE_LEN);
	    for (i=1; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.NoOp,
				 NOOP_DATA,
				 MAX_CASCADE_LEN);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    k = add_to_cascade(led_matrix,
			       ledx8x8.DisplayTest,
			       DISPLAY_NORMAL_MODE,
			       MAX_CASCADE_LEN);
	    for (i=1; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.NoOp,
				 NOOP_DATA,
				 MAX_CASCADE_LEN);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    k = add_to_cascade(led_matrix,
			       ledx8x8.NoOp,
			       NOOP_DATA,
			       MAX_CASCADE_LEN);
	    for (i=1; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_TEST_MODE,
				 MAX_CASCADE_LEN);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    k = add_to_cascade(led_matrix,
			       ledx8x8.NoOp,
			       NOOP_DATA,
			       MAX_CASCADE_LEN);
	    for (i=1; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_NORMAL_MODE,
				 MAX_CASCADE_LEN);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    sleep(1);
	    */
	    break;
	  }
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
      switch(demode)
	{
	case BRITETST:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MIN_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit0,
			   all_led_on,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit1,
			   all_led_on,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit2,
			   all_led_on,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit3,
			   all_led_on,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit4,
			   all_led_on,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit5,
			   all_led_on,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit6,
			   all_led_on,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit7,
			   all_led_on,
			   xflag);
	    sleep(1);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    sleep(2);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MAX_INTENSITY,
			   xflag);
	    sleep(3);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    sleep(2);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MIN_INTENSITY,
			   xflag);
	    sleep(1);
	    break;
	  }
	case CASCDBT:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit0,
			 all_led_on,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit1,
			 all_led_on,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit2,
			 all_led_on,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit3,
			 all_led_on,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit4,
			 all_led_on,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit5,
			 all_led_on,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit6,
			 all_led_on,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit7,
			 all_led_on,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MED_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(2);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MAX_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(3);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MED_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(2);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(1);
	    break;
	  }
	case DPNTTST:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit0,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(1);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit1,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(1);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit2,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(1);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit3,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(1);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit4,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(1);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit5,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(1);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit6,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(1);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit7,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(2);
	    break;
	  }
	case DGTTST1:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit0,
			   0,
			   xflag);
	    delay(500);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit1,
			   1,
			   xflag);
	    delay(500);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit2,
			   2,
			   xflag);
	    delay(500);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit3,
			   3,
			   xflag);
	    delay(500);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit4,
			   4,
			   xflag);
	    delay(500);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit5,
			   5,
			   xflag);
	    delay(500);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit6,
			   6,
			   xflag);
	    delay(500);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit7,
			   7,
			   xflag);
	    delay(1000);
	    break;
	  }
	case DGTTST2:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit7,
			   (3 + DP_OFFSET),
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit6,
			   1,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit5,
			   4,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit4,
			   1,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit3,
			   5,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit2,
			   9,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit1,
			   2,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit0,
			   7,
			   xflag);
	    sleep(8);
	    break;
	  }
	case DGTTST3:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit7,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit6,
			   CODE_B_HYPHEN,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit5,
			   CODE_B_H,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit4,
			   CODE_B_E,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit3,
			   CODE_B_L,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit2,
			   CODE_B_P,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit1,
			   CODE_B_HYPHEN,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit0,
			   ALL_SEG_OFF_DP_ON,
			   xflag);
	    sleep(8);
	    break;
	  }
	case FNTTST1:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    for (i=0; i<9; i++)
	      {
		j = i + 48;
		WP_write_font2Digits(wP_SPIfdx,
				     &ledx8x8,
				     myfont,
				     j,
				     mrow,
				     xflag);
		sleep(1);
	      }
	    break;
	  }
	case FNTTST2:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		WP_write_font2Digits(wP_SPIfdx,
				     &ledx8x8,
				     myfont,
				     j,
				     mrow,
				     xflag);
		sleep(1);
	      }
	    break;
	  }
	case CASCFT1:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    l = 7 + casclen;
	    for (i=0; i<l; i=i+casclen)
	      {
		j = i + 48;
		for (k=0; k<casclen; k++)
		  {
		    WP_write_font2cascadeDigits(wP_SPIfdx,
						&ledx8x8,
						myfont,
						j,
						mrow,
						led_matrix,
						k,
						casclen,
						xflag);
		    j++;
		  }
		sleep(2);
	      }
	    break;
	  }
	case CASCFT2:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    l = 24 + casclen;
	    for (i=0; i<l; i=i+casclen)
	      {
		j = i + 65;
		for (k=0; k<casclen; k++)
		  {
		    WP_write_font2cascadeDigits(wP_SPIfdx,
						&ledx8x8,
						myfont,
						j,
						mrow,
						led_matrix,
						k,
						casclen,
						xflag);
		    j++;
		  }
		sleep(2);
	      }
	    break;
	  }
	case CASCFT3:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    l = 24 + casclen;
	    for (i=0; i<l; i=i+casclen)
	      {
		j = i + 97;
		for (k=0; k<casclen; k++)
		  {
		    WP_write_font2cascadeDigits(wP_SPIfdx,
						&ledx8x8,
						myfont,
						j,
						mrow,
						led_matrix,
						k,
						casclen,
						xflag);
		    j++;
		  }
		sleep(2);
	      }
	    break;
	  }
	case FNTTST3:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		WP_write_font2Digits(wP_SPIfdx,
				     &ledx8x8,
				     myfont,
				     j,
				     mrow,
				     xflag);
		sleep(1);
	      }
	    break;
	  }
	case SELFTST:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.DisplayTest,
			   DISPLAY_TEST_MODE,
			   xflag);
	    sleep(2);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.DisplayTest,
			   DISPLAY_NORMAL_MODE,
			   xflag);
	    break;
	  }
	case CASCDST:
	  {
	    reset_cascade(led_matrix);
	    for (i=0; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_TEST_MODE,
				 MAX_CASCADE_LEN);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    for (i=0; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_NORMAL_MODE,
				 MAX_CASCADE_LEN);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(1);
	    for (i=0; i<casclen; i++)
	      {
		reset_cascade(led_matrix);
		for (j=0; j<casclen; j++)
		  {
		    if (j == i)
		      {
			k = add_to_cascade(led_matrix,
					   ledx8x8.DisplayTest,
					   DISPLAY_TEST_MODE,
					   MAX_CASCADE_LEN);
		      }
		    else
		      {
			k = add_to_cascade(led_matrix,
					   ledx8x8.NoOp,
					   NOOP_DATA,
					   MAX_CASCADE_LEN);
		      }
		  }		
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(1);
	      }
	    reset_cascade(led_matrix);
	    for (i=0; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_NORMAL_MODE,
				 MAX_CASCADE_LEN);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    /*
	    reset_cascade(led_matrix);
	    k = add_to_cascade(led_matrix,
			       ledx8x8.DisplayTest,
			       DISPLAY_NORMAL_MODE,
			       MAX_CASCADE_LEN);
	    for (i=1; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.NoOp,
				 NOOP_DATA,
				 MAX_CASCADE_LEN);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    k = add_to_cascade(led_matrix,
			       ledx8x8.NoOp,
			       NOOP_DATA,
			       MAX_CASCADE_LEN);
	    for (i=1; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_TEST_MODE,
				 MAX_CASCADE_LEN);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(1);
	    reset_cascade(led_matrix);
	    k = add_to_cascade(led_matrix,
			       ledx8x8.NoOp,
			       NOOP_DATA,
			       MAX_CASCADE_LEN);
	    for (i=1; i<casclen; i++)
	      k = add_to_cascade(led_matrix,
				 ledx8x8.DisplayTest,
				 DISPLAY_NORMAL_MODE,
				 MAX_CASCADE_LEN);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    sleep(1);
	    */
	    break;
	  }
	case WPICFFT:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Intensity,
			 MIN_INTENSITY,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.ScanLimit,
			 SCAN_ALL_DIGITS,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    for (i=0; i<256; i=i+2)
	      {
		WP_write_font2cascadeDigits(wP_SPIfdx,
					    &ledx8x8,
					    myfont,
					    i,
					    mrow,
					    led_matrix,
					    0,
					    casclen,
					    xflag);
		j = i + 1;
		WP_write_font2cascadeDigits(wP_SPIfdx,
					    &ledx8x8,
					    myfont,
					    j,
					    mrow,
					    led_matrix,
					    1,
					    casclen,
					    xflag);
		delay(1000);
	      }
	    break;
	  }
	case WPIFFT:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Intensity,
			   MED_INTENSITY,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.ScanLimit,
			   SCAN_ALL_DIGITS,
			   xflag);
	    for (i=0; i<256; i++)
	      {
		WP_write_font2Digits(wP_SPIfdx,
				     &ledx8x8,
				     myfont,
				     i,
				     mrow,
				     xflag);
		delay(500);
	      }
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */
  if (spimode == SYSMODE)
    {
      /* Turn off all segments? */
      switch (demode)
	{
	case SELFTST:
	case CASCDST:
	  {
	    /* Do nothing ... */
	    break;
	  }
	case CASCDBT:
	case CASCFT1:
	case CASCFT2:
	case CASCFT3:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit0,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit1,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit2,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit3,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit4,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit5,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit6,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit7,
			 all_led_off,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    break;
	  }
	default:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit0,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit1,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit2,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit3,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit4,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit5,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit6,
			     all_led_off,
			     xflag);
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Digit7,
			     all_led_off,
			     xflag);
	    break;
	  }
	}
      /* Shutdown */
      switch (demode)
	{
	case CASCDBT:
	case CASCFT1:
	case CASCFT2:
	case CASCFT3:
	case CASCDST:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Shutdown,
			 SHUTDOWN_MODE,
			 casclen);
	    C2S_cascade2spidev(ledx8x8.spidev,
			       led_matrix,
			       xflag);
	    break;
	  }
	default:
	  {
	    C2S_secho2spidev(ledx8x8.spidev,
			     ledx8x8.Shutdown,
			     SHUTDOWN_MODE,
			     xflag);
	    break;
	  }
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
       /* Turn off all segments? */
      switch (demode)
	{
	case SELFTST:
	case CASCDST:
	  {
	    /* Do nothing ... */
	    break;
	  }
	case CASCDBT:
	case CASCFT1:
	case CASCFT2:
	case CASCFT3:
	case WPICFFT:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit0,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit1,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit2,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit3,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit4,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit5,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit6,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Digit7,
			 all_led_off,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    break;
	  }
	default:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit0,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit1,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit2,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit3,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit4,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit5,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit6,
			   all_led_off,
			   xflag);
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Digit7,
			   all_led_off,
			   xflag);
	    break;
	  }
	}
      /* Shutdown */
      switch (demode)
	{
	case CASCDBT:
	case CASCDST:
	case CASCFT1:
	case WPICFFT:
	  {
	    reset_cascade(led_matrix);
	    fill_cascade(led_matrix,
			 ledx8x8.Shutdown,
			 SHUTDOWN_MODE,
			 casclen);
	    WP_write_cascade(wP_SPIfdx,
			     led_matrix,
			     xflag);
	    break;
	  }
	default:
	  {
	    WP_write2bytes(wP_SPIfdx,
			   ledx8x8.Shutdown,
			   SHUTDOWN_MODE,
			   xflag);
	    break;
	  }
	}
      if (wP_SPIfd[0] > 0) close(wP_SPIfd[0]);
      if (wP_SPIfd[1] > 0) close(wP_SPIfd[1]);
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
#ifdef TKBUGFIX
  if (spimode == SYSMODE)
    {
      if (wP_SPIfd[0] > 0) close(wP_SPIfd[0]);
      if (wP_SPIfd[1] > 0) close(wP_SPIfd[1]);
    }
#endif /* TKBUGFIX */
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: spi_ledx_demo.c,v $
 * Revision 1.6  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.7  2018/03/12 18:50:04  pi
 * SCMVersion 0.03
 *
 * Revision 1.6  2018/03/12 18:35:34  pi
 * Improved -CST option
 *
 * Revision 1.5  2018/03/10 20:59:08  pi
 * Interim commit
 *
 * Revision 1.4  2018/03/09 21:43:31  pi
 * Interim Commit
 *
 * Revision 1.3  2018/03/09 17:30:25  pi
 * Kernel Bug Fix test for -S mode
 *
 * Revision 1.2  2018/03/08 22:01:18  pi
 * Interim commit
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.5  2017/08/31 16:19:22  pi
 * Added -MTX2 support to wiringPi methods too!
 *
 * Revision 1.4  2017/08/31 15:52:39  pi
 * -MTX2 (Dual 16x8 led matrix on SPI0) support added
 *
 * Revision 1.3  2017/08/14 19:18:58  pi
 * sudo echo --> printf "octal" workaround now in force (see ../common/spi_wraps.c)
 *
 * Revision 1.2  2017/08/08 19:11:57  pi
 * Minor tidy up
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.2  2014/07/28 22:14:16  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:13:46  pi
 * Initial revision
 *
 * Revision 1.20  2014/07/16 18:18:03  pi
 * Improved some cascade tests (for cascade of 3 matrices)
 *
 * Revision 1.19  2014/07/09 21:34:09  pi
 * Interim commit
 *
 * Revision 1.18  2014/07/02 21:07:32  pi
 * Beginning to test 8-digit led display
 * Interim commit
 *
 * Revision 1.17  2014/06/20 20:35:04  pi
 * Now closing wP_SPIfd[] channels
 *
 * Revision 1.16  2014/06/19 21:40:29  pi
 * Interim commit
 *
 * Revision 1.15  2014/06/17 20:34:39  pi
 * -WCFFT option added
 *
 * Revision 1.14  2014/06/16 21:46:28  pi
 * -CFTn options working
 *
 * Revision 1.13  2014/06/15 22:38:50  pi
 * Interim commit
 *
 * Revision 1.12  2014/06/14 21:55:39  pi
 * Interim commit
 *
 * Revision 1.11  2014/06/13 21:49:19  pi
 * Interim commit
 *
 * Revision 1.10  2014/06/13 20:51:09  pi
 * Interim commit
 *
 * Revision 1.9  2014/06/13 13:48:05  pi
 * Interim commit (-CST option)
 *
 * Revision 1.8  2014/06/12 23:28:54  pi
 * Interim commit
 *
 * Revision 1.7  2014/06/11 20:30:44  pi
 * -CST option added
 *
 * Revision 1.6  2014/06/07 17:25:10  pi
 * SCMVersion 0.01
 *
 * Revision 1.5  2014/06/04 22:04:57  pi
 * Interim commit
 *
 * Revision 1.4  2014/06/04 20:18:34  pi
 * Interim commit
 *
 * Revision 1.3  2014/06/03 21:03:14  pi
 * Interim commit
 *
 * Revision 1.2  2014/06/02 23:11:59  pi
 * Interim commit
 *
 * Revision 1.1  2014/06/02 22:09:21  pi
 * Initial revision
 *
 *
 *
 */
