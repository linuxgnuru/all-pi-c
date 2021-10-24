/* 128 x 64 OLED via 4-wire SPI DEMOnstration code
 * $Id: oled_spi_demo.c,v 1.6 2018/03/31 21:32:45 pi Exp $
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
#include "oled128.h"
#include "oled256.h"
#include "jace128.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: oled_spi_demo.c,v $";
static char RCSId[]       = "$Id: oled_spi_demo.c,v 1.6 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.6 $";
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
  INITGPIO,  /* -IG                  */
  INVERSE,   /* -inverse             */
  NORMAL,    /* -normal              */
  RSTCONT,   /* -RC                  */
  RSTINIT,   /* -RI                  */
  RSTTEST,   /* -RT                  */
  TSTDISP,   /* -TD                  */
#endif /* HAVE_WIRINGPI_H */
  OLED128,   /* -oled128             */
  OLED256,   /* -oled256             */
  JACE128,   /* -jace128             */
  QUIET,     /* -Q                   */
  SYSMODE,   /* -S                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,spimode,fontis;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
#ifdef HAVE_WIRINGPI_H
  arglist[INITGPIO] = "-IG";
  arglist[INVERSE]  = "-inverse";
  arglist[NORMAL]   = "-normal";
  arglist[RSTINIT]  = "-RI";
  arglist[RSTCONT]  = "-RC";
  arglist[RSTTEST]  = "-RT";
  arglist[TSTDISP]  = "-TD";
#endif /* HAVE_WIRINGPI_H */
  arglist[OLED128]  = "-oled128";
  arglist[OLED256]  = "-oled256";
  arglist[JACE128]  = "-jace128";
  arglist[QUIET]    = "-Q";
  arglist[SYSMODE]  = "-S";
  arglist[VERBOSE]  = "-V";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPISPI_H */
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
char eflag,igflag,vflag,xflag;
char message_buffer[BUF_LEN];
unsigned char *oledfont;
unsigned int oledcontrast;

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
	    case ECHO:    eflag=TRUE;     break;
#ifdef HAVE_WIRINGPI_H
	    case INITGPIO: igflag=TRUE;   break;
	    case INVERSE: demode=INVERSE; spimode=WPIMODE; break;
	    case NORMAL:  demode=NORMAL;  spimode=WPIMODE; break;
	    case RSTCONT: demode=RSTCONT; spimode=WPIMODE; oledcontrast=OLED_C4; break;
	    case RSTINIT: demode=RSTINIT; spimode=WPIMODE; break;
	    case RSTTEST: demode=RSTINIT; break;
	    case TSTDISP: demode=TSTDISP; break;
#endif /* HAVE_WIRINGPI_H */
	    case OLED128: fontis=OLED128; oledfont=oled128; break;
	    case OLED256: fontis=OLED256; oledfont=oled256; break;
	    case JACE128: fontis=JACE128; oledfont=jace128; break;
	    case QUIET:   vflag=FALSE;     break;
	    case SYSMODE: spimode=SYSMODE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
	    case WPIMODE: spimode=WPIMODE; break;
#endif /* HAVE_WIRINGPISPI_H */
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
  strcat(buffer1, "         -E   {Echo command-line arguments},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -IG  {Initialise used gpio's & Gpio readall},\n");
  strcat(buffer1, "         -inverse {INVERSE display**},\n");
  strcat(buffer1, "         -normal  {NORMAL display** - default},\n");
  strcat(buffer1, "         -RC  {Reset Contrast**},\n");
  strcat(buffer1, "         -RI  {Reset & Initialise Display**},\n");
  strcat(buffer1, "         -RT  {Reset Test},\n");
  strcat(buffer1, "         -TD  {Test Display},\n");
  strcat(buffer1, "              {** implies -W},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -oled128 {use oled128 font - default},\n");
  strcat(buffer1, "         -oled256 {use oled256 font},\n");
  strcat(buffer1, "         -jace128 {use jace128 font},\n");
  strcat(buffer1, "         -Q   {Quiet - default},\n");
  strcat(buffer1, "         -S   {use System call methods - default},\n");
  strcat(buffer1, "         -V   {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -W   {use Wiring pi methods},\n");
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X   {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
void C2S_SpiWriteSBCommand(spi_ssd1306_oled *chip,
			   char qx)
{
  char *lspidev;
  unsigned int lbyte;
  /* Data/¬Command = Command */
  C2S_gpio_g_write((*chip).dc_gpio,
		   GPIO_LOW,
		   xflag);
  lspidev = (*chip).spidev;
  lbyte   = (*chip).cmds;
  C2S_echob2spidev(lspidev, lbyte, qx);
}
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char spidev;
  int i,j,k,l,mca,mcb;
  int bite,data[16];
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#ifdef HAVE_WIRINGPISPI_H
  int *myspifd;
  int wP_SPIfd[2];
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  spi_ssd1306_oled current_oled;
  
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
  eflag        = igflag = vflag = xflag = FALSE;
  demode       = LAST_ARG;
  spidev       = SPI_DEV00;
  spimode      = SYSMODE;
  fontis       = OLED128; 
  oledfont     = oled128;
  oledcontrast = OLED_C4;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  myspifd = &wP_SPIfd[0]; /* == SPI_DEV00 */
  wP_SPIfd[0] = wP_SPIfd[1] = -1; /* ie. as yet undefined */
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  make_name(argfile, "arg", getRCSFile(FALSE));
  /* strcpy(message_buffer, "Nothing Yet!"); */
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
	    case ECHO: eflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case INITGPIO: igflag=TRUE;   i--; break;
	    case INVERSE: demode=INVERSE; spimode=WPIMODE;  i--; break;
	    case NORMAL:  demode=NORMAL;  spimode=WPIMODE;  i--; break;
	    case RSTCONT: demode=RSTCONT; oledcontrast=OLED_C4;  spimode=WPIMODE; i--; break;
	    case RSTINIT: demode=RSTINIT; spimode=WPIMODE;  i--; break;
	    case RSTTEST: demode=RSTTEST; i--; break;
	    case TSTDISP: demode=TSTDISP; i--; break;
#endif /* HAVE_WIRINGPI_H */
	    case OLED128: fontis=OLED128; oledfont=oled128; i--; break;
	    case OLED256: fontis=OLED256; oledfont=oled256; i--; break;
	    case JACE128: fontis=JACE128; oledfont=jace128; i--; break;
	    case QUIET:   vflag=FALSE;    i--; break;
	    case SYSMODE: spimode=SYSMODE; i--; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
	    case WPIMODE: spimode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPISPI_H */
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
      init_spi_ssd1306_oled(&current_oled,
			    spidev,
			    5,
			    6,
			    "KEYES");
      echo_spi_ssd1306_oled_status(current_oled);
#ifdef HAVE_WIRINGPI_H
      C2S_gpio_g_mode(current_oled.rst_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* ¬RESet in-active */
      C2S_gpio_g_write(current_oled.rst_gpio,
		      GPIO_HIGH,
		      xflag);
      C2S_gpio_g_mode(current_oled.dc_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* Data/¬Command = Command */
      C2S_gpio_g_write(current_oled.dc_gpio,
		      GPIO_LOW,
		      xflag);
      C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
      exit(EXIT_SUCCESS);
    }

  /* Initialisation ... */
  if (spimode == SYSMODE)
    {
      init_spi_ssd1306_oled(&current_oled,
			    spidev,
			    5,
			    6,
			    "KEYES");
      echo_spi_ssd1306_oled_status(current_oled);
      printf("Using 'C' system call-based methods: ...\n");
#ifdef HAVE_WIRINGPI_H
      C2S_gpio_g_mode(current_oled.rst_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* ¬RESet in-active */
      C2S_gpio_g_write(current_oled.rst_gpio,
		      GPIO_HIGH,
		      xflag);
      C2S_gpio_g_mode(current_oled.dc_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* Data/¬Command = Command mode */
      C2S_gpio_g_write(current_oled.dc_gpio,
		      GPIO_LOW,
		      xflag);
#endif /* HAVE_WIRINGPI_H */
    }
  
#ifdef HAVE_WIRINGPI_H
  if (spimode == WPIMODE)
    {
      init_spi_ssd1306_oled(&current_oled,
			    spidev,
			    5,
			    6,
			    "KEYES");
      echo_spi_ssd1306_oled_status(current_oled);
      printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
      /* Exporting the ReSeT gpio leads to spurious reset pulses so ... */
      /* ... use SYSTEM method instead! */
      C2S_gpio_g_mode(current_oled.rst_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* ¬RESet in-active */
      C2S_gpio_g_write(current_oled.rst_gpio,
		      GPIO_HIGH,
		      xflag);
      /* Data/¬Command GPIO can be exported ... */
      C2S_gpio_export(current_oled.dc_gpio, GPIO_XOUT, xflag);
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
      /* Data/¬Command = Command mode */
      WP_digitalWrite(current_oled.dc_gpio, GPIO_LOW, xflag);
      C2S_gpio("exports", xflag);
      if (eflag || vflag) 
	{
	  printf("ls -laF /sys/class/gpio/\n");
	  if (xflag) system("ls -laF /sys/class/gpio/");
	}
#ifdef HAVE_WIRINGPISPI_H
      printf("Using wiringPiSPI methods: ...\n");
      if ((wP_SPIfd[0]=wiringPiSPISetup(0, 500000)) < 0)
	{
	  fprintf(stderr, "Could not setup wP_SPIfd[0]!\n");
	  perror("wiringPiSPISetup");
	  exit(EXIT_FAILURE);
	}
      else printf("wP_SPIfd[0] = %d\n", wP_SPIfd[0]);
      if (*myspifd > 0)
	{
	  current_oled.spifd = myspifd;
	  printf("*current_oled.spifd = %d\n",
		 *current_oled.spifd);
	}
#endif /* HAVE_WIRINGPISPI_H */
   }
#endif /* HAVE_WIRINGPI_H */
  

  /* Demonstration/Test ... */
  if (spimode == SYSMODE)
    {
      switch(demode)
	{
	case RSTTEST:
	  {
	    C2S_gpio_g_write(current_oled.rst_gpio,
			     GPIO_LOW,
			     xflag);
	    sleep(1);
	    C2S_gpio_g_write(current_oled.rst_gpio,
			     GPIO_HIGH,
			     xflag);
	    break;
	  }
	case TSTDISP:
	  {
	    current_oled.cmds = OLED_DISPLAY_ON;
	    C2S_SpiWriteSBCommand(&current_oled, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_ON_NO_RAM;
	    C2S_SpiWriteSBCommand(&current_oled, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_ON_WITH_RAM;
	    C2S_SpiWriteSBCommand(&current_oled, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_OFF;
	    C2S_SpiWriteSBCommand(&current_oled, xflag);
	    break;
	  }
	default: break;
	}
    }
  
#ifdef HAVE_WIRINGPI_H
  if (spimode == WPIMODE)
    {
      switch(demode)
	{
	case RSTTEST:
	  {
	    C2S_gpio_g_write(current_oled.rst_gpio,
			     GPIO_LOW,
			     xflag);
	    delay(10);
	    C2S_gpio_g_write(current_oled.rst_gpio,
			     GPIO_HIGH,
			     xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPI_H */
  
  /* Tidy up ... */
  if (spimode == SYSMODE)
    {
      /* Data/¬Command = Command mode */
      C2S_gpio_g_write(current_oled.dc_gpio,
		      GPIO_LOW,
		      xflag);
    }
  
#ifdef HAVE_WIRINGPI_H
  if (spimode == WPIMODE)
    {
      /* Data/¬Command = Command mode */
      WP_digitalWrite(current_oled.dc_gpio, GPIO_LOW, xflag);
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
#endif /* HAVE_WIRINGPI_H */
  
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: oled_spi_demo.c,v $
 * Revision 1.6  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.5  2015/12/02 21:56:07  pi
 * Interim commit
 *
 * Revision 1.4  2015/11/20 22:41:12  pi
 * Interim commit
 *
 * Revision 1.3  2015/11/17 21:56:59  pi
 * Interim commit
 *
 * Revision 1.2  2015/11/11 23:22:02  pi
 * Interim commit
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 *
 *
 */



