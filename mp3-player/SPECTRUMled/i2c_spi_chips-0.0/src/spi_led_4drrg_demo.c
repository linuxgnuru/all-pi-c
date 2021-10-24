/* SPI compatible, serial 4Digit (Re-cycled^) Red|Green LED DEMOnstration code
 * ^http://www.cpmspectrepi.webspace.virginmedia.com/raspberry_pi/MoinMoinExport/Recycled4digitLedDisplay.html
 * $Id: spi_led_4drrg_demo.c,v 1.3 2018/03/31 21:32:45 pi Exp $
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
static char RCSFile[]     = "$RCSfile: spi_led_4drrg_demo.c,v $";
static char RCSId[]       = "$Id: spi_led_4drrg_demo.c,v 1.3 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.3 $";
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
  COMMENT,   /* * {in argfile only!} */
  ECHO,      /* -E                   */
  CLKION,    /* +CLKI                */
  DALLON,    /* -DON                 */
  DALLOFF,   /* -DOFF                */
  GXLON,     /* +GXL                 */
  L1TST,     /* -L1T                 */
  L2TST,     /* -L2T                 */
  L3TST,     /* -L3T                 */
  MXCLKT,    /* -MXCLKT              */
  MXDINT,    /* -MXDI d              */
  MUXTST,    /* -MXT                 */
  QUIET,     /* -Q                   */
  RXLON,     /* +RXL                 */
  SEQDIGTST, /* -SDT                 */
  YXLON,     /* +YXL                 */
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
  arglist[CLKION]    = "+CLKI";
  arglist[DALLON]    = "-DON";
  arglist[DALLOFF]   = "-DOFF";
  arglist[ECHO]      = "-E";
  arglist[GXLON]     = "+GXL";
  arglist[L1TST]     = "-L1T";
  arglist[L2TST]     = "-L2T";
  arglist[L3TST]     = "-L3T";
  arglist[MXCLKT]    = "-MXCLKT";
  arglist[MXDINT]    = "-MXDI";
  arglist[MUXTST]    = "-MXT";
  arglist[QUIET]     = "-Q";
  arglist[RXLON]     = "+RXL";
  arglist[SEQDIGTST] = "-SQT";
  arglist[YXLON]     = "+YXL";
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
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
	    case CLKION:  lgrmask=lgrmask | anode4dlgr[5]; break;
	    case DALLON:  demode=DALLON;    break;
	    case DALLOFF: demode=DALLOFF;   break;
	    case ECHO:    eflag=TRUE;       break;
	    case GXLON:   lgrmask=lgrmask | anode4dlgr[6]; break;
	    case L1TST:   demode=L1TST;     break;
	    case L2TST:   demode=L2TST;     break;
	    case L3TST:   demode=L3TST;     break;
	    case MXCLKT:  demode=MXCLKT;  spimode=WPIMODE; break;
	    case MXDINT:
	      {
		demode=MXDINT;
		mxdival = atoi(buffer2);
		if (mxdival > 9999) mxdival = 9999;
		if (mxdival < 0) mxdival = 0;
		break;
	      }
	    case MUXTST:    demode=MUXTST;    break;
	    case QUIET:     vflag=FALSE;      break;
	    case RXLON:     lgrmask=lgrmask | anode4dlgr[7]; break;
	    case SEQDIGTST: demode=SEQDIGTST; break;
	    case YXLON:     lgrmask=lgrmask | anode4dlgr[9]; break;
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
  strcat(buffer1, "         -E      {Echo command-line arguments},\n");
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         +CLKI   {CLocK Indicators enabled},\n");
  strcat(buffer1, "         -DON    {Display fully ON},\n");
  strcat(buffer1, "         -DOFF   {Display fully OFF},\n");
  strcat(buffer1, "         +GXL    {Green eXternal led on},\n");
  strcat(buffer1, "         -LnT    {cLock indicator n Test, n=1,2 or 3},\n");
  strcat(buffer1, "         -MXCLKT {MultipleXed CLocK display Test (implies -W)},\n");
  strcat(buffer1, "         -MXDI d {MultipleXed Display of Integer d},\n");
  strcat(buffer1, "         -MXT    {MultipleXed digits Test},\n");
  strcat(buffer1, "         -Q      {Quiet - default},\n");
  strcat(buffer1, "         +RXL    {Red eXternal led on},\n");
  strcat(buffer1, "         -SQT    {Sequential Digit Test},\n");
  strcat(buffer1, "         +YXL    {\"Yellow\" eXternal led on (when Multi-pleXed)},\n");
#ifndef NO_SUDO_ECHO_TO_SPI
  strcat(buffer1, "         -S      {use System call methods** - default},\n");
#else /* NO_SUDO_ECHO_TO_SPI */
  strcat(buffer1, "         -S      {use System call methods - default},\n");
#endif /* NO_SUDO_ECHO_TO_SPI */
  strcat(buffer1, "         -V      {Verbose},\n");
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -W      {use Wiring pi spi methods},\n");
#endif /* HAVE_WIRINGPISPI_H */
  strcat(buffer1, "         -X      {eXecute demonstration}.\n");
#ifndef NO_SUDO_ECHO_TO_SPI
  strcat(buffer1, " ** NB: Uses \"sudo echo ...\" to the SPI device!");
#endif /* NO_SUDO_ECHO_TO_SPI */
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char dateandtimeis[DATEANDTIME_BUF_LEN],timenowis[TIMENOW_BUF_LEN];
  char spidev;
  int hours,mins,secs;
  int i,j,k,l,m;
  int mxc;
  unsigned int anode_byte,cathode_byte;
  unsigned int mxdvd[4];
#ifdef HAVE_WIRINGPISPI_H
  int *myspifd;
  int wP_SPIfd[2];
#endif /* HAVE_WIRINGPISPI_H */
  hw74hc595 xdtvbox;
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
  lgrmask = 0x00; /* cLock indicators & external Red|Green led disabled */
  spidev  = SPI_DEV00;
  spimode = SYSMODE;
#ifdef HAVE_WIRINGPISPI_H
  myspifd = &wP_SPIfd[0]; /* == SPI_DEV00 */
  wP_SPIfd[0] = wP_SPIfd[1] = -1; /* ie. as yet undefined */
#endif /* HAVE_WIRINGPISPI_H */
 
  make_name(argfile, "arg", getRCSFile(FALSE));
  /*
  init_anode4dlgr();
  init_codes7seg7a();
  */
  init_four7seglgra();

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
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
	    case CLKION:  lgrmask=lgrmask | anode4dlgr[5]; i--; break;
	    case DALLON:  demode=DALLON;    i--; break;
	    case DALLOFF: demode=DALLOFF;   i--; break;
	    case ECHO:    eflag=TRUE;       i--; break;
	    case GXLON:   lgrmask=lgrmask | anode4dlgr[6]; i--; break;
	    case L1TST:   demode=L1TST;     i--; break;
	    case L2TST:   demode=L2TST;     i--; break;
	    case L3TST:   demode=L3TST;     i--; break;
	    case MXCLKT:  demode=MXCLKT;  spimode=WPIMODE; i--; break;
	    case MXDINT:
	      {
		demode=MXDINT;
		mxdival = atoi(p2);
		if (mxdival > 9999) mxdival = 9999;
		if (mxdival < 0) mxdival = 0;
		break;
	      }
	    case MUXTST:    demode=MUXTST;    i--; break;
	    case QUIET:     vflag=FALSE;      i--; break;
	    case RXLON:     lgrmask=lgrmask | anode4dlgr[7]; i--; break;
	    case SEQDIGTST: demode=SEQDIGTST; i--; break;
	    case YXLON:     lgrmask=lgrmask | anode4dlgr[9]; i--; break;
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
  init_hw74hc595(&xdtvbox, spidev, "XDTVBOX");
  echo_hw74hc595_status(xdtvbox);
  
  if (spimode == SYSMODE)
    {
      switch(demode)
	{
	case L1TST:
	case L2TST:
	case L3TST:
	  {
	    lgrmask=lgrmask | anode4dlgr[5]; 
	    break;
	  }
	case MXDINT:
	  {
	    mxdvd[0] = mxdival/1000;
	    mxdvd[1] = (mxdival-(mxdvd[0]*1000))/100;
	    mxdvd[2] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100))/10;
	    mxdvd[3] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100)-(mxdvd[2]*10));
	    break;
	  }
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
      if (*myspifd > 0)
	{
	  printf("*myspyfd = %d\n", *myspifd);
	}
      switch(demode)
	{
	case L1TST:
	case L2TST:
	case L3TST:
	  {
	    lgrmask=lgrmask | anode4dlgr[5]; 
	    break;
	  }
	case MXCLKT:
	  {
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
	    mxdival = (100 * hours) + mins;
	    printf("Check6: %04d\n", mxdival);
	    mxdvd[0] = mxdival/1000;
	    mxdvd[1] = (mxdival-(mxdvd[0]*1000))/100;
	    mxdvd[2] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100))/10;
	    mxdvd[3] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100)-(mxdvd[2]*10));
	    break;
	  }
	case MXDINT:
	  {
	    mxdvd[0] = mxdival/1000;
	    mxdvd[1] = (mxdival-(mxdvd[0]*1000))/100;
	    mxdvd[2] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100))/10;
	    mxdvd[3] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100)-(mxdvd[2]*10));
	    break;
	  }
	default: break;
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
	    anode_byte   = anode4dlgr[8];   /* All digit anodes enabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[17]; /* All on */
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case DALLOFF:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[16]; /* All off */
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case L1TST:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[19];
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case L2TST:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[20];
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case L3TST:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[21];
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case MUXTST:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=7;
		do
		  {
		    if (i < 5) k = load_four7seglgra_digit(i, FALSE, i, lgrmask);
		    else k = load_four7seglgra_digit(i, FALSE, (i-4), lgrmask);
		    anode_byte   = four7seglgra[k].poll;
		    cathode_byte = four7seglgra[k].dcode;
		    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case MXDINT:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_four7seglgra_digit(i, FALSE, mxdvd[i-1], lgrmask);
		    anode_byte   = four7seglgra[k].poll;
		    cathode_byte = four7seglgra[k].dcode;
		    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case SEQDIGTST:
	  {
	    k = 0;
	    for (i=1; i<5; i++)
	      {
		anode_byte   = anode4dlgr[i];
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[k];
		C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		k++;
		sleep(2);
	      }
	    for (i=5; i<9; i++)
	      {
		anode_byte   = anode4dlgr[i-4];
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[k];
		C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		k++;
		sleep(2);
	      }
	    for (i=9; i<13; i++)
	      {
		anode_byte   = anode4dlgr[i-8];
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[k];
		C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		k++;
		sleep(2);
	      }
	    for (i=13; i<17; i++)
	      {
		anode_byte   = anode4dlgr[i-12];
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[k];
		C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		k++;
		sleep(2);
	      }
	    cathode_byte = codes7seg7a[18]; /* Decimal Point */
	    for (i=0; i<16; i++)
	      {
		anode_byte = anode4dlgr[1+(i%4)];
		anode_byte = anode_byte | lgrmask;
		C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		sleep(1);
	      }
	    for (i=0; i<12; i++)
	      {
		j = i % 3;
		anode_byte   = anode4dlgr[0];   /* All disabled */
		lgrmask=lgrmask | anode4dlgr[5]; 
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[j+19];
		C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		sleep(1);
	      }
	    cathode_byte = codes7seg7a[16]; /* All off */
	    for (i=0; i<10; i++)
	      {
		anode_byte = anode4dlgr[0];   /* All disabled */
		j = i % 2;
		if (j == 0) lgrmask = anode4dlgr[6];
		else lgrmask = anode4dlgr[7];
		anode_byte = anode_byte | lgrmask;
		C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
		sleep(1);
	      }
	    anode_byte = anode4dlgr[0];   /* All disabled */
	    C2S_secho2spidev(xdtvbox.spidev, anode_byte, cathode_byte, xflag);
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
	    anode_byte   = anode4dlgr[8];   /* All digit anodes enabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[17]; /* All on */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case DALLOFF:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[16]; /* All off */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case L1TST:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[19];
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case L2TST:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[20];
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case L3TST:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    anode_byte   = anode_byte | lgrmask;
	    cathode_byte = codes7seg7a[21];
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case MUXTST:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    mxc = 1000;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=7;
		do
		  {
		    if (i < 5) k = load_four7seglgra_digit(i, FALSE, i, lgrmask);
		    else k = load_four7seglgra_digit(i, FALSE, (i-4), lgrmask);
		    anode_byte   = four7seglgra[k].poll;
		    cathode_byte = four7seglgra[k].dcode;
		    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		    delay(3);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case MXCLKT:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    mxc = 3000;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		get_timenow(timenowis);
		make_col_white(timenowis);
		m = parse_spt(buffer2, timenowis);
		if (m > 0) hours = atoi(buffer2);
		m = parse_spt(buffer2, timenowis);
		if (m > 0) mins = atoi(buffer2);
		m = parse_spt(buffer2, timenowis);
		if (m > 0) secs = atoi(buffer2);
		mxdival = (100 * hours) + mins;
		mxdvd[0] = mxdival/1000;
		mxdvd[1] = (mxdival-(mxdvd[0]*1000))/100;
		mxdvd[2] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100))/10;
		mxdvd[3] = (mxdival-(mxdvd[0]*1000)-(mxdvd[1]*100)-(mxdvd[2]*10));
		j = secs % 2;
		if (j == 0) lgrmask = lgrmask | anode4dlgr[5];
		else lgrmask = 0x00;
		i=6;
		do
		  {
		    if (i < 5)
		      {
			k = load_four7seglgra_digit(i, FALSE, mxdvd[i-1], 0x00);
			anode_byte   = four7seglgra[k].poll;
			cathode_byte = four7seglgra[k].dcode;
		      }
		    else
		      {
			k = load_four7seglgra_digit(i, FALSE, (i-4), lgrmask);
			anode_byte   = four7seglgra[k].poll;
			cathode_byte = four7seglgra[k].dcode;
		      }
		    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		    delay(4);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case MXDINT:
	  {
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    mxc = 600;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_four7seglgra_digit(i, FALSE, mxdvd[i-1], lgrmask);
		    anode_byte   = four7seglgra[k].poll;
		    cathode_byte = four7seglgra[k].dcode;
		    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		    delay(5);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4dlgr[0];   /* All disabled */
	    cathode_byte = codes7seg7a[16]; /* All off */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
	    break;
	  }
	case SEQDIGTST:
	  {
	    k = 0;
	    for (i=1; i<5; i++)
	      {
		anode_byte   = anode4dlgr[i];
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[k];
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		k++;
		delay(500);
	      }
	    for (i=5; i<9; i++)
	      {
		anode_byte   = anode4dlgr[i-4];
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[k];
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		k++;
		delay(500);
	      }
	    for (i=9; i<13; i++)
	      {
		anode_byte   = anode4dlgr[i-8];
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[k];
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		k++;
		delay(500);
	      }
	    for (i=13; i<17; i++)
	      {
		anode_byte   = anode4dlgr[i-12];
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[k];
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		k++;
		delay(500);
	      }
	    cathode_byte = codes7seg7a[18]; /* Decimal Point */
	    for (i=0; i<64; i++)
	      {
		anode_byte = anode4dlgr[1+(i%4)];
		anode_byte = anode_byte | lgrmask;
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		delay(100);
	      }
	    for (i=0; i<33; i++)
	      {
		j = i % 3;
		anode_byte   = anode4dlgr[0];   /* All disabled */
		lgrmask=lgrmask | anode4dlgr[5]; 
		anode_byte   = anode_byte | lgrmask;
		cathode_byte = codes7seg7a[j+19];
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		delay(200);
	      }
	    cathode_byte = codes7seg7a[16]; /* All off */
	    for (i=0; i<30; i++)
	      {
		anode_byte = anode4dlgr[0];   /* All disabled */
		j = i % 2;
		if (j == 0) lgrmask = anode4dlgr[6];
		else lgrmask = anode4dlgr[7];
		anode_byte = anode_byte | lgrmask;
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		delay(100);
	      }
	    for (i=0; i<60; i++)
	      {
		anode_byte = anode4dlgr[0];   /* All disabled */
		j = i % 2;
		if (j == 0) lgrmask = anode4dlgr[6];
		else lgrmask = anode4dlgr[7];
		anode_byte = anode_byte | lgrmask;
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		delay(50);
	      }
	    for (i=0; i<120; i++)
	      {
		anode_byte = anode4dlgr[0];   /* All disabled */
		j = i % 2;
		if (j == 0) lgrmask = anode4dlgr[6];
		else lgrmask = anode4dlgr[7];
		anode_byte = anode_byte | lgrmask;
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		delay(25);
	      }
	    for (i=0; i<300; i++)
	      {
		anode_byte = anode4dlgr[0];   /* All disabled */
		j = i % 2;
		if (j == 0) lgrmask = anode4dlgr[6];
		else lgrmask = anode4dlgr[7];
		anode_byte = anode_byte | lgrmask;
		WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
		delay(10);
	      }
	    anode_byte = anode4dlgr[0];   /* All disabled */
	    WP_write2bytes(*myspifd, anode_byte, cathode_byte, xflag);
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
 * $Log: spi_led_4drrg_demo.c,v $
 * Revision 1.3  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.2  2017/08/14 19:18:58  pi
 * sudo echo --> printf "octal" workaround now in force (see ../common/spi_wraps.c)
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.6  2015/06/30 22:52:36  pi
 * -MXCLKT option added
 *
 * Revision 1.5  2015/06/22 21:21:29  pi
 * -MXDI option added
 *
 * Revision 1.4  2015/06/16 14:42:54  pi
 * -MXT & +YXL options added
 *
 * Revision 1.3  2015/06/13 21:46:01  pi
 * Basic (ie. non-multiplexed) options working
 *
 * Revision 1.2  2015/06/13 17:01:44  pi
 * Interim commit
 *
 * Revision 1.1  2015/06/13 15:53:10  pi
 * Initial revision
 *
 *
 *
 *
 */

