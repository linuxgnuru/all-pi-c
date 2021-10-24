/* Partially SPI compatible, serial 2x4 Digit (LC Technology) LED DEMOnstration code
 * $Id: spi_led_2x4d_demo.c,v 1.6 2018/03/31 21:32:45 pi Exp $
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
#include "parsel.h"
#include "spi_chips.h"
#include "spi_wraps.h"
#include "led_disptools.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: spi_led_2x4d_demo.c,v $";
static char RCSId[]       = "$Id: spi_led_2x4d_demo.c,v 1.6 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.6 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.01 $";

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
  DALLON,    /* -DON                 */
  DALLOFF,   /* -DOFF                */
  ECHO,      /* -E                   */
  MXUDINT,   /* -MXUDI d             */
  MXLDINT,   /* -MXLDI d             */
  MX8DINT,   /* -MX8DI d             */
  MUXTST,    /* -MXT                 */
  QUIET,     /* -Q                   */
  SEQDIGTST, /* -SQT                 */
  SYSMODE,   /* -S                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPISPI_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPISPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,spimode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]   = "-a";
  arglist[COMMENT]   = "*";
  arglist[DALLON]    = "-DON";
  arglist[DALLOFF]   = "-DOFF";
  arglist[ECHO]      = "-E";
  arglist[MXUDINT]   = "-MXUDI";
  arglist[MXLDINT]   = "-MXLDI";
  arglist[MX8DINT]   = "-MX8DI";
  arglist[MUXTST]    = "-MXT";
  arglist[QUIET]     = "-Q";
  arglist[SEQDIGTST] = "-SQT";
  arglist[SYSMODE]   = "-S";
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
char eflag,oflag,vflag,xflag;
int mxdival;

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
	    case DALLON:    demode=DALLON;    break;
	    case DALLOFF:   demode=DALLOFF;   break;
	    case ECHO:      eflag=TRUE;       break;
	    case MXUDINT:
	      {
		demode=MXUDINT;
		mxdival = atoi(buffer2);
		if (mxdival > 9999) mxdival = 9999;
		if (mxdival < 0) mxdival = 0;
		break;
	      }
	    case MXLDINT:
	      {
		demode=MXLDINT;
		mxdival = atoi(buffer2);
		if (mxdival > 9999) mxdival = 9999;
		if (mxdival < 0) mxdival = 0;
		break;
	      }
	    case MX8DINT:
	      {
		demode=MX8DINT;
		mxdival = atoi(buffer2);
		if (mxdival > 99999999) mxdival = 99999999;
		if (mxdival < 0) mxdival = 0;
		break;
	      }
	    case MUXTST:    demode=MUXTST;    break;
	    case QUIET:     vflag=FALSE;      break;
	    case SEQDIGTST: demode=SEQDIGTST; break;
	    case SYSMODE:   spimode=SYSMODE;  break;
	    case VERBOSE:   eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPISPI_H
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
  strcat(buffer1, "         -E       {Echo command-line arguments},\n");
  strcat(buffer1, "         -DON     {Display fully ON},\n");
  strcat(buffer1, "         -DOFF    {Display fully OFF},\n");
  strcat(buffer1, "         -MXUDI d {MultipleXed Upper Display of 4-digit Integer d},\n");
  strcat(buffer1, "         -MXLDI d {MultipleXed Lower Display of 4-digit Integer d},\n");
  strcat(buffer1, "         -MX8DI d {MultipleXed Display of 8-digit Integer d},\n");
  strcat(buffer1, "         -MXT     {MultipleXed digits Test},\n");
  strcat(buffer1, "         -Q       {Quiet - default},\n");
  strcat(buffer1, "         -SQT     {SeQuential digit block Test},\n");
  strcat(buffer1, "         -S       {use System call methods - default},\n");
  strcat(buffer1, "         -V       {Verbose},\n");
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -W       {use Wiring pi spi methods},\n");
#endif /* HAVE_WIRINGPISPI_H */
  strcat(buffer1, "         -X       {eXecute demonstration}.\n");
  print_us(getRCSFile(TRUE), buffer1);
}

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char spidev;
  int i,j,k,l;
  int mxc;
  unsigned int digit_byte,segment_byte;
  unsigned int mxdvd[8];
#ifdef HAVE_WIRINGPISPI_H
  int *myspifd;
  int wP_SPIfd[2];
#endif /* HAVE_WIRINGPISPI_H */
  hw74hc164 lctech;
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
  eflag   = vflag = xflag = FALSE;
  spidev  = SPI_DEV00;
  spimode = SYSMODE;
#ifdef HAVE_WIRINGPISPI_H
  myspifd = &wP_SPIfd[0]; /* == SPI_DEV00 */
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
	    case DALLON:    demode=DALLON;    i--; break;
	    case DALLOFF:   demode=DALLOFF;   i--; break;
	    case ECHO:      eflag=TRUE;       i--; break;
	    case MXUDINT:
	      {
		demode=MXUDINT;
		mxdival = atoi(p2);
		if (mxdival > 9999) mxdival = 9999;
		if (mxdival < 0) mxdival = 0;
		break;
	      }
	    case MXLDINT:
	      {
		demode=MXLDINT;
		mxdival = atoi(p2);
		if (mxdival > 9999) mxdival = 9999;
		if (mxdival < 0) mxdival = 0;
		break;
	      }
	    case MX8DINT:
	      {
		demode=MX8DINT;
		mxdival = atoi(p2);
		if (mxdival > 99999999) mxdival = 99999999;
		if (mxdival < 0) mxdival = 0;
		break;
	      }
	    case MUXTST:    demode=MUXTST;    i--; break;
	    case QUIET:     vflag=FALSE;      i--; break;
	    case SEQDIGTST: demode=SEQDIGTST; i--; break;
	    case SYSMODE:   spimode=SYSMODE;  i--; break;
	    case VERBOSE:   eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPISPI_H
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
  init_ulfour7seg0a();
  init_hw74hc164(&lctech, 2, spidev, "LCTech2x4");
  echo_hw74hc164_status(lctech);
  /* init_codes7seg0a(); */

  if (demode == MXUDINT)
    {
	mxdvd[0] = mxdival/1000;
	mxdvd[1] = (mxdival-(mxdvd[0]*1000))/100;
	mxdvd[2] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100))/10;
	mxdvd[3] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100)-(mxdvd[2]*10));
    }

  if (demode == MXLDINT)
    {
	mxdvd[4] = mxdival/1000;
	mxdvd[5] = (mxdival-(mxdvd[4]*1000))/100;
	mxdvd[6] = (mxdival-(mxdvd[4]*1000)-(mxdvd[5]*100))/10;
	mxdvd[7] = (mxdival-(mxdvd[4]*1000)-(mxdvd[5]*100)-(mxdvd[6]*10));
    }
  
  if (demode == MX8DINT)
    {
	mxdvd[0] = mxdival/10000000;
	mxdvd[1] = (mxdival-(mxdvd[0]*10000000))/1000000;
	mxdvd[2] = (mxdival-(mxdvd[0]*10000000)-(mxdvd[1]*1000000))/100000;
	mxdvd[3] = (mxdival-(mxdvd[0]*10000000)-(mxdvd[1]*1000000)-(mxdvd[2]*100000))/10000;
	mxdvd[4] = (mxdival-(mxdvd[0]*10000000)-(mxdvd[1]*1000000)-(mxdvd[2]*100000)
		    -(mxdvd[3]*10000))/1000;
	mxdvd[5] = (mxdival-(mxdvd[0]*10000000)-(mxdvd[1]*1000000)-(mxdvd[2]*100000)
		    -(mxdvd[3]*10000)-(mxdvd[4]*1000))/100;
	mxdvd[6] = (mxdival-(mxdvd[0]*10000000)-(mxdvd[1]*1000000)-(mxdvd[2]*100000)
		    -(mxdvd[3]*10000)-(mxdvd[4]*1000)-(mxdvd[5]*100))/10;
	mxdvd[7] = (mxdival-(mxdvd[0]*10000000)-(mxdvd[1]*1000000)-(mxdvd[2]*100000)
		    -(mxdvd[3]*10000)-(mxdvd[4]*1000)-(mxdvd[5]*100)-(mxdvd[6]*10));
    }

  if (vflag) printf("mxdvd digits: %1d-%1d-%1d-%1d-%1d-%1d-%1d-%1d\n",
		    mxdvd[0],
		    mxdvd[1],
		    mxdvd[2],
		    mxdvd[3],
		    mxdvd[4],
		    mxdvd[5],
		    mxdvd[6],
		    mxdvd[07]);
  
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
      if (*myspifd > 0)
	{
	  printf("*myspyfd = %d\n", *myspifd);
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Demonstration ... */
  if (spimode == SYSMODE)
    {
      switch(demode)
	{
	case DALLON:
	  {
	    digit_byte   = ALL_ADIGITS_ON;
	    segment_byte = ALL_CSEGSDP_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    break;
	  }
	case DALLOFF:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    break;
	  }
	case MUXTST:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=8;
		do
		  {
		    k = load_ulfour7seg0a_digit(i, FALSE, i);
		    digit_byte   = ulfour7seg0a[k].poll;
		    segment_byte = ulfour7seg0a[k].dcode;
		    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }	    
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    break;
	  }
	case MXUDINT:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_ulfour7seg0a_digit(i, FALSE, mxdvd[i-1]);
		    digit_byte   = ulfour7seg0a[k].poll;
		    segment_byte = ulfour7seg0a[k].dcode;
		    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    break;
	  }
	case MXLDINT:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_ulfour7seg0a_digit(i+4, FALSE, mxdvd[i+3]);
		    digit_byte   = ulfour7seg0a[k].poll;
		    segment_byte = ulfour7seg0a[k].dcode;
		    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
		    sleep(1);
		    i--;
		  }
		while(k>4);
	      }
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    break;
	  }
	case MX8DINT:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=8;
		do
		  {
		    k = load_ulfour7seg0a_digit(i, FALSE, mxdvd[i-1]);
		    digit_byte   = ulfour7seg0a[k].poll;
		    segment_byte = ulfour7seg0a[k].dcode;
		    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    break;
	  }
	case SEQDIGTST:
	  {
	    segment_byte = codes7seg0a[0];
	    digit_byte   = ADIGIT_U1_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[1];
	    digit_byte   = ADIGIT_U2_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[2];
	    digit_byte   = ADIGIT_U3_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[3];
	    digit_byte   = ADIGIT_U4_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[4];
	    digit_byte   = ADIGIT_L1_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[5];
	    digit_byte   = ADIGIT_L2_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[6];
	    digit_byte   = ADIGIT_L3_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[7];
	    digit_byte   = ADIGIT_L4_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = ALL_CSEGSDP_ON;
	    digit_byte   = ALL_ADIGITS_ON;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
	    sleep(2);
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    C2S_secho2spidev(lctech.spidev, segment_byte, digit_byte, xflag);
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
	case DALLON:
	  {
	    digit_byte   = ALL_ADIGITS_ON;
	    segment_byte = ALL_CSEGSDP_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    break;
	  }
	case DALLOFF:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    break;
	  }
	case MUXTST:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    mxc = 600;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=8;
		do
		  {
		    k = load_ulfour7seg0a_digit(i, FALSE, i);
		    digit_byte   = ulfour7seg0a[k].poll;
		    segment_byte = ulfour7seg0a[k].dcode;
		    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
		    delay(5);
		    i--;
		  }
		while(k>0);
	      }	    
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    break;
	  }
	case MXUDINT:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    mxc = 600;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_ulfour7seg0a_digit(i, FALSE, mxdvd[i-1]);
		    digit_byte   = ulfour7seg0a[k].poll;
		    segment_byte = ulfour7seg0a[k].dcode;
		    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
		    delay(5);
		    i--;
		  }
		while(k>0);
	      }	    
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    break;
	  }
	case MXLDINT:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    mxc = 600;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_ulfour7seg0a_digit(i+4, FALSE, mxdvd[i+3]);
		    digit_byte   = ulfour7seg0a[k].poll;
		    segment_byte = ulfour7seg0a[k].dcode;
		    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
		    delay(5);
		    i--;
		  }
		while(k>4);
	      }	    
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    break;
	  }
	case MX8DINT:
	  {
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    mxc = 600;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=8;
		do
		  {
		    k = load_ulfour7seg0a_digit(i, FALSE, mxdvd[i-1]);
		    digit_byte   = ulfour7seg0a[k].poll;
		    segment_byte = ulfour7seg0a[k].dcode;
		    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
		    delay(5);
		    i--;
		  }
		while(k>0);
	      }	    
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    break;
	  }
	case SEQDIGTST:
	  {
	    segment_byte = codes7seg0a[0];
	    digit_byte   = ADIGIT_U1_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[1];
	    digit_byte   = ADIGIT_U2_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[2];
	    digit_byte   = ADIGIT_U3_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[3];
	    digit_byte   = ADIGIT_U4_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[4];
	    digit_byte   = ADIGIT_L1_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[5];
	    digit_byte   = ADIGIT_L2_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[6];
	    digit_byte   = ADIGIT_L3_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = codes7seg0a[7];
	    digit_byte   = ADIGIT_L4_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(1);
	    segment_byte = ALL_CSEGSDP_ON;
	    digit_byte   = ALL_ADIGITS_ON;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    sleep(2);
	    digit_byte   = ALL_ADIGITS_OFF;
	    segment_byte = ALL_CSEGSDP_OFF;
	    WP_write2bytes(*myspifd, segment_byte, digit_byte, xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  if (wP_SPIfd[0] > 0) close(wP_SPIfd[0]);
  if (wP_SPIfd[1] > 0) close(wP_SPIfd[1]);
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: spi_led_2x4d_demo.c,v $
 * Revision 1.6  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.5  2017/09/30 18:23:02  pi
 * SCMVersion 0.01
 *
 * Revision 1.4  2017/09/28 22:22:47  pi
 * -SQT now using codes7seg0a[]
 *
 * Revision 1.3  2017/09/28 20:39:05  pi
 * -SQT option implemented
 *
 * Revision 1.2  2017/09/28 20:00:36  pi
 * Simple, ON/OFF tests implemented
 *
 * Revision 1.1  2017/09/28 19:25:05  pi
 * Initial revision
 *
 *
 *
 */
