/* SPI compatible, serial 4Digit (Qifei) LED DEMOnstration code
 * $Id: spi_led_4dq_demo.c,v 1.4 2018/03/31 21:32:45 pi Exp $
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
static char RCSFile[]     = "$RCSfile: spi_led_4dq_demo.c,v $";
static char RCSId[]       = "$Id: spi_led_4dq_demo.c,v 1.4 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.4 $";
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
  CMUXTST,   /* -CMXT                */
  DALLON,    /* -DON                 */
  DALLOFF,   /* -DOFF                */
  MXDINT,    /* -MXDI d              */
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
  arglist[CMUXTST]   = "-CMXT";
  arglist[DALLON]    = "-DON";
  arglist[DALLOFF]   = "-DOFF";
  arglist[ECHO]      = "-E";
  arglist[MXDINT]    = "-MXDI";
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
int casclen,mxdival;

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
	    case CMUXTST:   demode=CMUXTST;   break;
	    case DALLON:    demode=DALLON;    break;
	    case DALLOFF:   demode=DALLOFF;   break;
	    case ECHO:      eflag=TRUE;       break;
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
  strcat(buffer1, "         -E      {Echo command-line arguments},\n");
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -CMXT   {Cascaded MultipleXed digits Test},\n");
  strcat(buffer1, "         -DON    {Display fully ON},\n");
  strcat(buffer1, "         -DOFF   {Display fully OFF},\n");
  strcat(buffer1, "         -MXDI d {MultipleXed Display of Integer d},\n");
  strcat(buffer1, "         -MXT    {MultipleXed digits Test},\n");
  strcat(buffer1, "         -Q      {Quiet - default},\n");
  strcat(buffer1, "         -SQT    {SeQuential digit Test},\n");
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
  char spidev;
  cmxudigit cfour7seg0a;
  int i,j,k,l;
  int mxc,ndigit;
  unsigned int anode_byte,cathode_byte;
  unsigned int mxdvd[4];
#ifdef HAVE_WIRINGPISPI_H
  int *myspifd;
  int wP_SPIfd[2];
#endif /* HAVE_WIRINGPISPI_H */
  hw74hc595 qifei;
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
  casclen = 2;
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
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
	    case CMUXTST:   demode=CMUXTST;   i--; break;
	    case DALLON:    demode=DALLON;    i--; break;
	    case DALLOFF:   demode=DALLOFF;   i--; break;
	    case ECHO:      eflag=TRUE;       i--; break;
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
  /*
  init_anode4d();
  init_codes7seg0a();
  */
  init_four7seg0a();
  init_hw74hc595(&qifei, spidev, "QIFEI");
  echo_hw74hc595_status(qifei);

  switch(demode)
    {
    case CMUXTST:
      {
	ndigit = casclen * 4;
	cfour7seg0a.poll  = calloc(ndigit, (sizeof(unsigned int)));
	cfour7seg0a.data  = calloc(ndigit, (sizeof(int)));
	cfour7seg0a.dcode = calloc(ndigit, (sizeof(unsigned int)));
	if ((cfour7seg0a.poll != NULL) && (cfour7seg0a.data != NULL)
	    && (cfour7seg0a.dcode != NULL))
	  {
	    cfour7seg0a.ncascade = ndigit;
	    printf("Cascade cfour7seg0a has %d digits in %d sections\n",
		   cfour7seg0a.ncascade,
		   casclen);
	  }
	else
	  {
	    printf("Error: cmxudigit callocs failed\n");
	    exit(EXIT_FAILURE);
	  }	
	reset_cascade(led_4digit);
	anode_byte   = anode4d[0];      /* All disabled */
	cathode_byte = codes7seg0a[16]; /* All off */
	fill_cascade(led_4digit,
		     cathode_byte,
		     anode_byte,
		     casclen);
	if (spimode == SYSMODE) C2S_cascade2spidev(qifei.spidev,
						   led_4digit,
						   xflag);	
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
	case CMUXTST:
	  {
	    WP_write_cascade(*myspifd,
			     led_4digit,
			     xflag);
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
	case CMUXTST:
	  {
	    reset_cascade(led_4digit);
	    anode_byte   = anode4d[5];      /* All enabled */
	    cathode_byte = codes7seg0a[17]; /* All on */
	    fill_cascade(led_4digit,
			 cathode_byte,
			 anode_byte,
			 casclen);
	    C2S_cascade2spidev(qifei.spidev,
			       led_4digit,
			       xflag);
	    sleep(2);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		for (i=4; i>0; i--)
		  {
		    j = i + 4;		
		    reset_cascade(led_4digit);
		    anode_byte   = anode4d[i];    
		    cathode_byte = codes7seg0a[j];
		    k = add_to_cascade(led_4digit,
				       cathode_byte,
				       anode_byte,
				       casclen);
		    cathode_byte = codes7seg0a[i];
		    k = add_to_cascade(led_4digit,
				       cathode_byte,
				       anode_byte,
				       casclen);
		    C2S_cascade2spidev(qifei.spidev,
				       led_4digit,
				       xflag);
		    sleep(1);
		  }
	      }
	    sleep(1);
	    break;
	  }
	case DALLON:
	  {
	    anode_byte   = anode4d[5];      /* All enabled */
	    cathode_byte = codes7seg0a[17]; /* All on */
	    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
	    break;
	  }
	case DALLOFF:
	  {
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
	    break;
	  }
	case MUXTST:
	  {
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_four7seg0a_digit(i, FALSE, i);
		    anode_byte   = four7seg0a[k].poll;
		    cathode_byte = four7seg0a[k].dcode;
		    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
	    break;
	  }
	case MXDINT:
	  {
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
	    mxc = 5;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_four7seg0a_digit(i, FALSE, mxdvd[i-1]);
		    anode_byte   = four7seg0a[k].poll;
		    cathode_byte = four7seg0a[k].dcode;
		    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
	    break;
	  }
	case SEQDIGTST:
	  {
	    k = 0;
	    for (i=1; i<5; i++)
	      {
		anode_byte   = anode4d[i];
		cathode_byte = codes7seg0a[k];
		C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
		k++;
		sleep(2);
	      }
	    for (i=5; i<9; i++)
	      {
		anode_byte   = anode4d[i-4];
		cathode_byte = codes7seg0a[k];
		C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
		k++;
		sleep(2);
	      }
	    for (i=9; i<13; i++)
	      {
		anode_byte   = anode4d[i-8];
		cathode_byte = codes7seg0a[k];
		C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
		k++;
		sleep(2);
	      }
	    for (i=13; i<17; i++)
	      {
		anode_byte   = anode4d[i-12];
		cathode_byte = codes7seg0a[k];
		C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
		k++;
		sleep(2);
	      }
	    cathode_byte = codes7seg0a[18]; /* Decimal Point */
	    for (i=0; i<16; i++)
	      {
		anode_byte = anode4d[1+(i%4)];
		C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);
		sleep(1);
	      }
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    C2S_secho2spidev(qifei.spidev, cathode_byte, anode_byte, xflag);	    
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
	case CMUXTST:
	  {
	    reset_cascade(led_4digit);
	    anode_byte   = anode4d[5];      /* All enabled */
	    cathode_byte = codes7seg0a[17]; /* All on */
	    fill_cascade(led_4digit,
			 cathode_byte,
			 anode_byte,
			 casclen);
	    WP_write_cascade(*myspifd,
			     led_4digit,
			     xflag);
	    sleep(2);
	    mxc = 1600;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		for (i=4; i>0; i--)
		  {
		    j = i + 4;		
		    reset_cascade(led_4digit);
		    anode_byte   = anode4d[i];    
		    cathode_byte = codes7seg0a[j];
		    k = add_to_cascade(led_4digit,
				       cathode_byte,
				       anode_byte,
				       casclen);
		    cathode_byte = codes7seg0a[i];
		    k = add_to_cascade(led_4digit,
				       cathode_byte,
				       anode_byte,
				       casclen);
		    WP_write_cascade(*myspifd,
				     led_4digit,
				     xflag);
		    delay(2);
		  }
	      }
	    sleep(1);
	    break;
	  }
	case DALLON:
	  {
	    anode_byte   = anode4d[5];      /* All enabled */
	    cathode_byte = codes7seg0a[17]; /* All on */
	    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
	    break;
	  }
	case DALLOFF:
	  {
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
	    break;
	  }
	case MUXTST:
	  {
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
	    mxc = 600;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_four7seg0a_digit(i, FALSE, i);
		    anode_byte   = four7seg0a[k].poll;
		    cathode_byte = four7seg0a[k].dcode;
		    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
		    delay(5);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
	    break;
	  }
	case MXDINT:
	  {
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
	    mxc = 600;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=4;
		do
		  {
		    k = load_four7seg0a_digit(i, FALSE, mxdvd[i-1]);
		    anode_byte   = four7seg0a[k].poll;
		    cathode_byte = four7seg0a[k].dcode;
		    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
		    delay(5);
		    i--;
		  }
		while(k>0);
	      }	    
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
	    break;
	  }
	case SEQDIGTST:
	  {
	    k = 0;
	    for (i=1; i<5; i++)
	      {
		anode_byte   = anode4d[i];
		cathode_byte = codes7seg0a[k];
		WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
		k++;
		delay(500);
	      }
	    for (i=5; i<9; i++)
	      {
		anode_byte   = anode4d[i-4];
		cathode_byte = codes7seg0a[k];
		WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
		k++;
		delay(500);
	      }
	    for (i=9; i<13; i++)
	      {
		anode_byte   = anode4d[i-8];
		cathode_byte = codes7seg0a[k];
		WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
		k++;
		delay(500);
	      }
	    for (i=13; i<17; i++)
	      {
		anode_byte   = anode4d[i-12];
		cathode_byte = codes7seg0a[k];
		WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
		k++;
		delay(500);
	      }
	    cathode_byte = codes7seg0a[18] = 0x7f; /* Decimal Point */
	    for (i=0; i<64; i++)
	      {
		anode_byte = anode4d[1+(i%4)];
		WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
		delay(100);
	      }
	    anode_byte   = anode4d[0];      /* All disabled */
	    cathode_byte = codes7seg0a[16]; /* All off */
	    WP_write2bytes(*myspifd, cathode_byte, anode_byte, xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */
  if (demode == CMUXTST)
    {
      reset_cascade(led_4digit);
      anode_byte   = anode4d[0];      /* All disabled */
      cathode_byte = codes7seg0a[16]; /* All off */
      fill_cascade(led_4digit,
		   cathode_byte,
		   anode_byte,
		   casclen);
      if (spimode == SYSMODE) C2S_cascade2spidev(qifei.spidev,
						 led_4digit,
						 xflag);
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
      if (spimode == WPIMODE) WP_write_cascade(*myspifd,
					       led_4digit,
					       xflag);
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
      free(cfour7seg0a.dcode);
      free(cfour7seg0a.data);
      free(cfour7seg0a.poll);
    }
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
 * $Log: spi_led_4dq_demo.c,v $
 * Revision 1.4  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.3  2017/09/28 20:39:05  pi
 * Minor (message) bug corrected
 *
 * Revision 1.2  2017/08/14 19:18:58  pi
 * sudo echo --> printf "octal" workaround now in force (see ../common/spi_wraps.c)
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.12  2015/06/26 18:47:40  pi
 * -CMXT option implemented
 *
 * Revision 1.11  2015/06/25 22:55:41  pi
 * Interim commit
 *
 * Revision 1.10  2015/06/24 20:43:36  pi
 * Interim commit
 *
 * Revision 1.9  2015/06/22 21:21:29  pi
 * Minor tidy up
 *
 * Revision 1.8  2015/06/22 20:40:35  pi
 * -MXDI d option added
 *
 * Revision 1.7  2015/06/21 21:58:29  pi
 * Interim commit
 *
 * Revision 1.6  2015/06/13 21:46:01  pi
 * Basic (ie. non-multiplexed) options working
 *
 * Revision 1.5  2015/06/13 15:53:10  pi
 * Interim commit
 *
 * Revision 1.4  2015/06/10 22:14:29  pi
 * -MXT option working
 *
 * Revision 1.3  2015/06/09 10:02:41  pi
 * -W option now active
 *
 * Revision 1.2  2015/06/08 21:27:34  pi
 * -DON, -DOFF & -SQT options working in system mode
 *
 * Revision 1.1  2015/06/08 19:47:35  pi
 * Initial revision
 *
 *
 *
 */
