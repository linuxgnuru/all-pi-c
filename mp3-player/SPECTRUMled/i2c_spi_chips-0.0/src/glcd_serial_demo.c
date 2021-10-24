/* GLCD (128x64 pixels - 12864ZW/ST7920), serial interface, DEMOnstration code
 * $Id: glcd_serial_demo.c,v 1.2 2018/03/31 21:32:45 pi Exp $
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

/* RCS info blocks */
static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: glcd_serial_demo.c,v $";
static char RCSId[]       = "$Id: glcd_serial_demo.c,v 1.2 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.2 $";
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
#ifdef HAVE_WIRINGPI_H
  BLTST,     /* -BLT                 */
  INITGPIO,  /* -IG                  */
#ifdef HAVE_WIRINGPISPI_H
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  QUIET,     /* -Q                   */
  SYSMODE,   /* -S                   */
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
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
#ifdef HAVE_WIRINGPI_H
  arglist[BLTST]    = "-BLT";
  arglist[INITGPIO] = "-IG";
#ifdef HAVE_WIRINGPISPI_H
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  arglist[QUIET]    = "-Q";
  arglist[SYSMODE]  = "-S";
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
char blflag,eflag,igflag,oflag,tflag,vflag,xflag,xisflag;
char message_buffer[BUF_LEN];

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
	    case ECHO:    eflag=TRUE;       break;
#ifdef HAVE_WIRINGPI_H
	    case BLTST:    demode=BLTST;    break;
	    case INITGPIO: igflag=TRUE;     break;
#ifdef HAVE_WIRINGPISPI_H
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;      break;
	    case SYSMODE: spimode=SYSMODE;  break;
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
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -BLT  {simple Back Light Test},\n");
  strcat(buffer1, "         -IG   {Initialise used gpio's & Gpio readall},\n");
#ifdef HAVE_WIRINGPISPI_H
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q    {Quiet - default},\n");
  strcat(buffer1, "         -S    {use System call methods** - default},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -W    {use Wiring pi spi methods},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.\n");
  strcat(buffer1, " ** NB: Uses \"sudo echo ...\" to the SPI device!");
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
  blflag   = TRUE;
  demode   = INITGPIO; /* Dummy mode */
  spidev   = SPI_DEV01;
  spimode  = SYSMODE;
  glcdfont = HC8X16;
  /* udgcmode = UCGRAM0; */
  init_pixel_store(&current_qc12864b_buffer);
#ifdef HAVE_WIRINGPISPI_H
  myspifd = &wP_SPIfd[1]; /* == SPI_DEV01 */
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
	    case ECHO:     eflag=TRUE;      i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
#endif /* HAVE_WIRINGPISPI_H */
#ifdef HAVE_WIRINGPISPI_H
#endif /* HAVE_WIRINGPISPI_H */
	    case BLTST:    demode=BLTST;    i--; break;
	    case INITGPIO: igflag=TRUE;     i--; break;
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;      i--; break;
	    case SYSMODE: spimode=SYSMODE;  i--; break;
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

  /* -IG option leads to an early exit after the system call ... */
  if (igflag)
    {
      init_current_spibl2(&current_spibl2,
			  4,
			  17,
			  SPI_DEV01,
			  "12864ZW");		      
      echo_any_spibl2_status(&current_spibl2);
      echo_pixel_store_status(&current_qc12864b_buffer);
#ifdef HAVE_WIRINGPI_H
      C2S_gpio_g_mode(current_spibl2.glcd_bl0_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_write(current_spibl2.glcd_bl0_gpio,
		       GPIO_HIGH,
		       xflag);
      C2S_gpio_g_mode(current_spibl2.glcd_bl1_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_write(current_spibl2.glcd_bl1_gpio,
		       GPIO_HIGH,
		       xflag);
      C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
      exit(EXIT_SUCCESS);
    }
  
  /* Execute other options */
  /* Initialisation ... */
  init_current_spibl2(&current_spibl2,
		      4,
		      17,
		      SPI_DEV01,
		      "12864ZW");		      
  echo_any_spibl2_status(&current_spibl2);

  if (spimode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
#ifdef HAVE_WIRINGPI_H
      switch(demode)
	{
	case BLTST:
	  {
	    C2S_gpio_g_mode(current_spibl2.glcd_bl0_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_write(current_spibl2.glcd_bl0_gpio,
			     GPIO_LOW,
			     xflag);
	    C2S_gpio_g_mode(current_spibl2.glcd_bl1_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_write(current_spibl2.glcd_bl1_gpio,
			     GPIO_LOW,
			     xflag);
	    break;
	  }
	default: break;
	}
#endif /* HAVE_WIRINGPI_H */
    }

  /* Demonstration ... */
  if (spimode == SYSMODE)
    {
      switch(demode)
	{
#ifdef HAVE_WIRINGPI_H
	case BLTST:
	  {
	    /* BackLight #0 on */
	    C2S_gpio_g_write(current_spibl2.glcd_bl0_gpio,
			     GPIO_HIGH,
			     xflag);
	    sleep(2);
	    /* BackLight #0 off, #1 on */
	    C2S_gpio_g_write(current_spibl2.glcd_bl0_gpio,
			     GPIO_LOW,
			     xflag);
	    C2S_gpio_g_write(current_spibl2.glcd_bl1_gpio,
			     GPIO_HIGH,
			     xflag);
	    sleep(2);
	    /* BackLight #0 & #1 on */
	    C2S_gpio_g_write(current_spibl2.glcd_bl0_gpio,
			     GPIO_HIGH,
			     xflag);
	    sleep(2);
	    /* BackLight #0 & #1 off */
	    C2S_gpio_g_write(current_spibl2.glcd_bl0_gpio,
			     GPIO_LOW,
			     xflag);
	    C2S_gpio_g_write(current_spibl2.glcd_bl1_gpio,
			     GPIO_LOW,
			     xflag);
	    break;
	  }
	default: break;
	}
#endif /* HAVE_WIRINGPI_H */
    }

  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: glcd_serial_demo.c,v $
 * Revision 1.2  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.3  2015/08/23 21:09:08  pi
 * Interim commit
 *
 * Revision 1.2  2015/08/15 16:42:24  pi
 * Interim commit
 *
 * Revision 1.1  2015/08/15 15:56:37  pi
 * Initial revision
 *
 *
 *
 */
