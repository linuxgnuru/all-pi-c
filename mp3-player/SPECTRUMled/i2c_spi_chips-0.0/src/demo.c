/* DEMOnstration code
 * $Id: demo.c,v 1.3 2018/03/31 21:32:45 pi Exp $
 */

#define NO_ECHO  1

#define IS_MAIN          1
#define BLK_LEN          83  /* 80 + CR, LF & NUL */
#define BUF_LEN         256
#define CMD_LEN         138  /* 2 x FN_LEN + 10   */
#define FN_LEN           64

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_WIRINGPI_H
#include <wiringPi.h>
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPII2C_H
#include <wiringPiI2C.h>
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "parsel.h"
#include "i2c_chips.h"
#include "i2c_wraps.h"
#include "led_disptools.h"
#include "lcd_disptools.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: demo.c,v $";
static char RCSId[]       = "$Id: demo.c,v 1.3 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.3 $";
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
  COMMENT,   /* * {in argfile only!} */
  ADAPF1,    /* -AP1                 */
  ADAPF2,    /* -AP2                 */
  ECHO,      /* -E                   */
  HUMPI1,    /* -HP1                 */
  HUMPI2,    /* -HP2                 */
#ifdef HAVE_WIRINGPI_H
  HUMPI3,    /* -HP3                 */
#endif /* HAVE_WIRINGPI_H */
  INQUIRE,   /* -I                   */
  QUIET,     /* -Q                   */
  SILEM1,    /* -SE1                 */
  SILEM2,    /* -SE2                 */
  SILEM3,    /* -SE3                 */
  SILEM4,    /* -SE4                 */
  SOPIO1,    /* -SP1                 */
  SOPIO2,    /* -SP2                 */
  SYSMODE,   /* -S                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPII2C_H
  ADAPFQ,    /* -APQ                 */
  HUMPIQ,    /* -HPQ                 */
  SOPIOQ,    /* -SPQ                 */
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPII2C_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,i2cmode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ADAPF1]   = "-AP1";
  arglist[ADAPF2]   = "-AP2";
  arglist[ECHO]     = "-E";
  arglist[HUMPI1]   = "-HP1";
  arglist[HUMPI2]   = "-HP2";
#ifdef HAVE_WIRINGPI_H
  arglist[HUMPI3]   = "-HP3";
#endif /* HAVE_WIRINGPI_H */
  arglist[INQUIRE]  = "-I";
  arglist[QUIET]    = "-Q";
  arglist[SILEM1]   = "-SE1";
  arglist[SILEM2]   = "-SE2";
  arglist[SILEM3]   = "-SE3";
  arglist[SILEM4]   = "-SE4";
  arglist[SOPIO1]   = "-SP1";
  arglist[SOPIO2]   = "-SP2";
  arglist[SYSMODE]  = "-S";
  arglist[VERBOSE]  = "-V";
#ifdef HAVE_WIRINGPII2C_H
  arglist[ADAPFQ]   = "-APQ";
  arglist[HUMPIQ]   = "-HPQ";
  arglist[SOPIOQ]   = "-SPQ";
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPII2C_H */
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
char eflag,iflag,iqflag,oflag,vflag,xflag;

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
	    case ADAPF1:  demode=ADAPF1;   break;
	    case ADAPF2:  demode=ADAPF2;   break;
	    case ECHO:    eflag=TRUE;      break;
	    case HUMPI1:  demode=HUMPI1;   break;
	    case HUMPI2:  demode=HUMPI2;   break;
#ifdef HAVE_WIRINGPI_H
	    case HUMPI3:  demode=HUMPI3;   break;
#endif /* HAVE_WIRINGPI_H */
	    case INQUIRE: iqflag=TRUE;     break;
	    case QUIET:   vflag=FALSE;     break;
	    case SILEM1:  demode=SILEM1;   break;
	    case SILEM2:  demode=SILEM2;   break;
	    case SILEM3:  demode=SILEM3;   break;
	    case SILEM4:  demode=SILEM4;   break;
	    case SOPIO1:  demode=SOPIO1;   break;
	    case SOPIO2:  demode=SOPIO2;   break;
	    case SYSMODE: i2cmode=SYSMODE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPII2C_H
	    case ADAPFQ:  demode=ADAPFQ; i2cmode=WPIMODE; break;
	    case HUMPIQ:  demode=HUMPIQ; i2cmode=WPIMODE; break;
	    case SOPIOQ:  demode=SOPIOQ; i2cmode=WPIMODE; break;
	    case WPIMODE: i2cmode=WPIMODE; break;
#endif /* HAVE_WIRINGPII2C_H */
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
  strcat(buffer1, "         -AP1 {Adafruit Permaproto full demo #1},\n");
  strcat(buffer1, "         -AP2 {Adafruit Permaproto full demo #2},\n");
  strcat(buffer1, "         -E   {Echo command-line arguments},\n");
  strcat(buffer1, "         -HP1 {Humble Pi demo #1},\n");
  strcat(buffer1, "         -HP2 {Humble Pi demo #2},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -HP3 {Humble Pi demo #3},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -I   {Inquire about i2c devices},\n");
  strcat(buffer1, "         -Q   {Quiet - default},\n");
  strcat(buffer1, "         -SE1 {Sil Edge mount lcd demo #1},\n");
  strcat(buffer1, "         -SE2 {Sil Edge mount lcd demo #2},\n");
  strcat(buffer1, "         -SE3 {Sil Edge mount lcd demo #3},\n");
  strcat(buffer1, "         -SE4 {Sil Edge mount lcd demo #4},\n");
  strcat(buffer1, "         -SP1 {Slice of Pi/o demo #1},\n");
  strcat(buffer1, "         -SP2 {Slice of Pi/o demo #2},\n");
  strcat(buffer1, "         -S   {use System call methods - default},\n");
  strcat(buffer1, "         -V   {Verbose},\n");
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1,
	 "         -APQ {Adafruit Permaproto full device Query (implies -W)},\n");
  strcat(buffer1, "         -HPQ {Humble Pi device Query (implies -W)},\n");
  strcat(buffer1, "         -SPQ {Slice of Pi/o device Query (implies -W)},\n");
  strcat(buffer1, "         -W   {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
  strcat(buffer1, "         -X   {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

/* Humble Pi LED matrix #defines etc. ... */
/*
 * #define ALL_ROW_CATHODES_ON   0x00
 * #define ALL_ROW_CATHODES_OFF  0xff
 * #define ALL_COLUMN_ANODES_ON  0xff
 * #define ALL_COLUMN_ANODES_OFF 0x00
 *
 * Moved into led_disptools.h
 */

/*
 81 82 83 84 85 86 87 88
 71 72 73 74 75 76 77 78
 61 62 63 64 65 66 67 68
 51 52 53 54 55 56 57 58
 41 42 43 44 45 46 47 48
 31 32 33 34 35 36 37 38
 21 22 23 24 25 26 27 28
 11 12 13 14 15 16 17 18
*/

/*
  Row 8 7 6 5 4 3 2 1
  Bit 0 1 2 3 4 5 6 7

  Col 5 6 7 8 1 2 3 4
  Bit 0 1 2 3 4 5 6 7
*/

/*
 * #define RC1_ON 0x7f
 * #define RC2_ON 0xbf
 * #define RC3_ON 0xdf
 * #define RC4_ON 0xef
 * #define RC5_ON 0xf7
 * #define RC6_ON 0xfb
 * #define RC7_ON 0xfd
 * #define RC8_ON 0xfe
 * 
 * #define CA1_ON 0x10
 * #define CA2_ON 0x20
 * #define CA3_ON 0x40
 * #define CA4_ON 0x80
 * #define CA5_ON 0x01
 * #define CA6_ON 0x02
 * #define CA7_ON 0x04
 * #define CA8_ON 0x08
 *
 * Moved into led_disptools.h
 */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  int i,j,k,l,mca,mcb,mcc,mcd,mce,mcf,mxc;
  int data[16];
  mcp23008 adapio2,humpi22,humpi27;
  mcp23017 sopio;
  pcf8574 adapio1;
  lcdbpv2 silbhem;
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPII2C_H
  wpi2cvb i2c_chip[128];
#endif /* HAVE_WIRINGPII2C_H */

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
  demode  = LAST_ARG;
  i2cmode = SYSMODE;
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
	    case ADAPF1:  demode=ADAPF1; i--; break;
	    case ADAPF2:  demode=ADAPF2; i--; break;
	    case ECHO:    eflag=TRUE;    i--; break;
	    case HUMPI1:  demode=HUMPI1; i--; break;
	    case HUMPI2:  demode=HUMPI2; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case HUMPI3:  demode=HUMPI3; i--; break;
#endif /* HAVE_WIRINGPI_H */
	    case INQUIRE: iqflag=TRUE;   i--; break;
	    case QUIET:   vflag=FALSE;   i--; break;
	    case SILEM1:  demode=SILEM1; i--; break;
	    case SILEM2:  demode=SILEM2; i--; break;
	    case SILEM3:  demode=SILEM3; i--; break;
	    case SILEM4:  demode=SILEM4; i--; break;
	    case SOPIO1:  demode=SOPIO1; i--; break;
	    case SOPIO2:  demode=SOPIO2; i--; break;
	    case SYSMODE: i2cmode=SYSMODE; i--; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPII2C_H
	    case ADAPFQ:  demode=ADAPFQ; i2cmode=WPIMODE; i--; break;
	    case HUMPIQ:  demode=HUMPIQ; i2cmode=WPIMODE; i--; break;
	    case SOPIOQ:  demode=SOPIOQ; i2cmode=WPIMODE; i--; break;
	    case WPIMODE: i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
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

  /* -I option leads to an early exit after the system call ... */
  if (iqflag)
    {
      if (!xflag)
	{
	  mca = init_mcp23017(&sopio, 0x20, "sopio");
	  mcb = init_pcf8574(&adapio1, 0x21, "adapio1");
	  mcc = init_mcp23008(&adapio2, 0x24, "adapio2");
	  mcd = init_mcp23008(&humpi22, 0x22, "humpi22");
	  mce = init_mcp23008(&humpi27, 0x27, "humpi27");
	  mcf = init_lcdbpv2(&silbhem, 0x3a, "silbhem");
	  echo_mcp23017_status(sopio); 
	  echo_pcf8574_status(adapio1);
	  echo_mcp23008_status(adapio2);
	  echo_mcp23008_status(humpi22);
	  echo_mcp23008_status(humpi27);
	  echo_lcdbpv2_status(silbhem);
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  switch(demode)
    {
    case ADAPF1:
    case ADAPF2:
#ifdef HAVE_WIRINGPII2C_H
    case ADAPFQ:
#endif /* HAVE_WIRINGPII2C_H */
      {
	mcb = init_pcf8574(&adapio1, 0x21, "adapio1");
	mcc = init_mcp23008(&adapio2, 0x24, "adapio2");
	echo_pcf8574_status(adapio1);
	echo_mcp23008_status(adapio2);
	init_four7seg();
	break;
      }
    case HUMPI1:
    case HUMPI2:
#ifdef HAVE_WIRINGPII2C_H
    case HUMPIQ:
#endif /* HAVE_WIRINGPII2C_H */
      {
	/* HUMble PI */
	mcd = init_mcp23008(&humpi22, 0x22, "humpi22");
	mce = init_mcp23008(&humpi27, 0x27, "humpi27");
	echo_mcp23008_status(humpi22);
	echo_mcp23008_status(humpi27);
	break;
      }
    case SILEM1:
    case SILEM2:
    case SILEM3:
    case SILEM4:
      {
	/* SIL Edge Mounted lcd driver */
	mcf = init_lcdbpv2(&silbhem, 0x3a, "silbhem");
	echo_lcdbpv2_status(silbhem);
	break;
      }
    case SOPIO1:
    case SOPIO2:
#ifdef HAVE_WIRINGPII2C_H
    case SOPIOQ:
#endif /* HAVE_WIRINGPII2C_H */
      {
	/* Slice of Pi/o */
	mca = init_mcp23017(&sopio, 0x20, "sopio");
	echo_mcp23017_status(sopio); 
	init_sevenseg();
	break;
      }
    default: break;
    }

  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      switch(demode)
	{
	case ADAPF1:
	case ADAPF2:
	  {
	    /* ADAfruit Permaproto Full */
	    adapio1.qiop = ALL_GPIO_BITS_HIGH;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    C2S_i2cset(PI_B2,
		       adapio2.addr,
		       adapio2.iodir,
		       ALL_GPIO_BITS_OUT,
		       xflag);
	    C2S_i2cset(PI_B2,
		       adapio2.addr,
		       adapio2.iopol,
		       ALL_GPIO_BITS_NORMAL,
		       xflag);	    break;
	    break;
	  }
	case HUMPI1:
	case HUMPI2:
	  {
	    /* HUMble PI */
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.iodir,
		       ALL_GPIO_BITS_OUT,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.iopol,
		       ALL_GPIO_BITS_NORMAL,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.iodir,
		       ALL_GPIO_BITS_OUT,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.iopol,
		       ALL_GPIO_BITS_NORMAL,
		       xflag);
	    break;
	  }
#ifdef HAVE_WIRINGPI_H
	case HUMPI3:
	  {
	    C2S_gpio("mode 5 out", xflag);
	    C2S_gpio("write 5 0", xflag);
	    sleep(1);
	    C2S_gpio("write 5 1", xflag);
	    C2S_gpio("readall", xflag);
	    break;
	  }
#endif /* HAVE_WIRINGPI_H */
	case SILEM3:
	case SILEM4:
	  {
	    /* SIL Edge Mounted lcd driver */
	    /* Turn backlight half-on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       125,
		       xflag);	
	    /* Set Display Type (16 cols x 2 rows) */
	    data[0] = 2;
	    data[1] = 16;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.lcdtype,
			data,
			2,
			xflag);
	    /* Clear display */
	    C2S_i2cout(PI_B2,
		       silbhem.addr,
		       silbhem.cleardisp,
		       xflag);
	    /* Cursor on and blinking */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.hd44780,
		       LCD_DISPLAY_ON_CURSOR_BLINK,
		       xflag);
	    break;
	  }
	case SOPIO1:
	case SOPIO2:
	  {
	    /* Slice of Pi/o */
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.iodira,
		       ALL_GPIO_BITS_OUT,
		       xflag);
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.iodirb,
		       ALL_GPIO_BITS_OUT,
		       xflag);
	    break;
	  }
	default: break;
	}
    }

#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
#ifdef HAVE_WIRINGPI_H
      if (demode != HUMPI3) printf("Using wiringPiI2C methods: ...\n");
#else
      printf("Using wiringPiI2C methods: ...\n");
#endif /* HAVE_WIRINGPI_H */
      switch(demode)
	{
	case ADAPF1:
	case ADAPF2:
	  {
	    /* ADAfruit Permaproto Full */
	    i2c_chip[0].did = (int)(adapio1.addr);
	    if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[0].did %d!",
			i2c_chip[0].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%02x has i2c_chip[0].fd of %d\n",
			i2c_chip[0].did, i2c_chip[0].fd);
	    i2c_chip[1].did = (int)(adapio2.addr);
	    if ((i2c_chip[1].fd=wiringPiI2CSetup(i2c_chip[1].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[1].did %d!",
			i2c_chip[1].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%02x has i2c_chip[1].fd of %d\n",
			i2c_chip[1].did,
			i2c_chip[1].fd);
	  i2c_chip[0].data = ALL_GPIO_BITS_HIGH;
	  WP_i2cout(i2c_chip[0], xflag);
	  i2c_chip[1].reg  = adapio2.iodir;
	  i2c_chip[1].data = ALL_GPIO_BITS_OUT;
	  WP_i2cset(i2c_chip[1], xflag);
	  i2c_chip[1].reg  = adapio2.iopol;
	  i2c_chip[1].data = ALL_GPIO_BITS_NORMAL;
	  WP_i2cset(i2c_chip[1], xflag);
	  break;
	  }
	case ADAPFQ:
	  {
	    /* ADAfruit Permaproto Full device Query*/
	    i2c_chip[0].did = (int)(adapio1.addr);
	    if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[0].did %d!",
			i2c_chip[0].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%02x has i2c_chip[0].fd of %d\n",
			i2c_chip[0].did, i2c_chip[0].fd);
	    i2c_chip[1].did = (int)(adapio2.addr);
	    if ((i2c_chip[1].fd=wiringPiI2CSetup(i2c_chip[1].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[1].did %d!",
			i2c_chip[1].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%02x has i2c_chip[1].fd of %d\n",
			i2c_chip[1].did,
			i2c_chip[1].fd);
	    break;
	  }
	case HUMPI1:
	case HUMPI2:
	  {
	    /* HUMble PI */
	    i2c_chip[0].did = (int)(humpi22.addr);
	    if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[0].did %d!",
			i2c_chip[0].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%02x has i2c_chip[0].fd of %d\n",
			i2c_chip[0].did, i2c_chip[0].fd);
	    i2c_chip[1].did = (int)(humpi27.addr);
	    if ((i2c_chip[1].fd=wiringPiI2CSetup(i2c_chip[1].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[1].did %d!",
			i2c_chip[1].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%02x has i2c_chip[1].fd of %d\n",
			i2c_chip[1].did,
			i2c_chip[1].fd);
	    i2c_chip[0].reg  = (int)(humpi22.iodir);
	    i2c_chip[0].data = ALL_GPIO_BITS_OUT;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(humpi22.iopol);
	    i2c_chip[0].data = ALL_GPIO_BITS_NORMAL;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[1].reg  = (int)(humpi27.iodir);
	    i2c_chip[1].data = ALL_GPIO_BITS_OUT;
	    WP_i2cset(i2c_chip[1], xflag);
	    i2c_chip[1].reg  = (int)(humpi27.iopol);
	    i2c_chip[1].data = ALL_GPIO_BITS_NORMAL;
	    WP_i2cset(i2c_chip[1], xflag);
	    break;
	  }
	case HUMPIQ:
	  {
	    /* HUMble Pi device Query*/
	    i2c_chip[0].did = (int)(humpi22.addr);
	    if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[0].did %d!",
			i2c_chip[0].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%02x has i2c_chip[0].fd of %d\n",
			i2c_chip[0].did, i2c_chip[0].fd);
	    i2c_chip[1].did = (int)(humpi27.addr);
	    if ((i2c_chip[1].fd=wiringPiI2CSetup(i2c_chip[1].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[1].did %d!",
			i2c_chip[1].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%02x has i2c_chip[1].fd of %d\n",
			i2c_chip[1].did,
			i2c_chip[1].fd);
	    break;
	  }
	case SILEM1:
	case SILEM2:
	  {
	    /* SIL Edge Mounted lcd driver */
	    i2c_chip[0].did = (int)(silbhem.addr);
	    if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[0].did %d!",
			i2c_chip[0].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%2x has i2c_chip[0].fd of %d\n",
			i2c_chip[0].did,
			i2c_chip[0].fd);
	    break;
	  }
	case SILEM3:
	case SILEM4:
	  {
	    /* SIL Edge Mounted lcd driver */
	    i2c_chip[0].did = (int)(silbhem.addr);
	    if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[0].did %d!",
			i2c_chip[0].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%2x has i2c_chip[0].fd of %d\n",
			i2c_chip[0].did,
			i2c_chip[0].fd);
	    /* Turn backlight half-on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 125;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Set Display Type (16 cols x 2 rows) */
	    i2c_chip[0].reg  = (int)(silbhem.lcdtype);
	    i2c_chip[0].data = 2;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + 16; 
	    WP_i2cset2(i2c_chip[0], xflag);
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    /* Cursor on and blinking */
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_DISPLAY_ON_CURSOR_BLINK;
	    WP_i2cset(i2c_chip[0], xflag);
	    break;
	  }
	case SOPIO1:
	case SOPIO2:
	  {
	    /* Slice of Pi/o */
	    i2c_chip[0].did = (int)(sopio.addr);
	    if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[0].did %d!",
			i2c_chip[0].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%2x has i2c_chip[0].fd of %d\n",
			i2c_chip[0].did,
			i2c_chip[0].fd);
	    i2c_chip[0].reg  = (int)(sopio.iodira);
	    i2c_chip[0].data = ALL_GPIO_BITS_OUT;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(sopio.iodirb);
	    i2c_chip[0].data = ALL_GPIO_BITS_OUT;
	    WP_i2cset(i2c_chip[0], xflag);
	    break;
	  }
	case SOPIOQ:
	  {
	    /* Slice of Pi/o device Query*/
	    i2c_chip[0].did = (int)(sopio.addr);
	    if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	      {
		fprintf(stderr,
			"Could not setup i2c_chip[0].did %d!",
			i2c_chip[0].did);
		perror("wiringPiI2CSetup");
		exit(EXIT_FAILURE);
	      }
	    else printf("I2C Device 0x%2x has i2c_chip[0].fd of %d\n",
			i2c_chip[0].did,
			i2c_chip[0].fd);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPII2C_H */

#ifdef HAVE_WIRINGPI_H
  if ((i2cmode == WPIMODE) && (demode == HUMPI3))
    {
      printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
      C2S_gpio_export(24, GPIO_XOUT, xflag);
      C2S_gpio("exports", xflag);
      if (eflag || vflag) 
	{
	  printf("ls -laF /sys/class/gpio/\n");
	  if (xflag) system("ls -laF /sys/class/gpio/");
	}
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
    }
#endif /* HAVE_WIRINGPI_H */
  
  /* Demonstration/Test ... */
  if (i2cmode == SYSMODE)
    {
      switch(demode)
	{
	case ADAPF1:
	  {
	    /* ADAfruit Permaproto Full #1 */
	    /* Check all digits first ... */
	    C2S_i2cset(PI_B2,
		       adapio2.addr,
		       adapio2.gpio,
		       ALL_GPIO_BITS_HIGH,
		       xflag);
	    adapio1.qiop = 0x0f;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    sleep(10);
	    adapio1.qiop = ALL_GPIO_BITS_HIGH;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    /* Cycle through the digits ... */
	    for (i=0; i<16; i++)
	      {		
		j = i % 4;
		switch(j)
		  {
		  case 0: adapio1.qiop=0xef; break;
		  case 1: adapio1.qiop=0xdf; break;
		  case 2: adapio1.qiop=0xbf; break;
		  case 3: adapio1.qiop=0x7f; break;
		  }
		C2S_i2cout(PI_B2,
			   adapio1.addr,
			   adapio1.qiop,
			   xflag);
		sleep(1);
	      }
	    adapio1.qiop = ALL_GPIO_BITS_HIGH;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    /* Incrementally display all four ... */
	    adapio1.qiop=0xef;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    sleep(2);
	    adapio1.qiop=0xcf;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    sleep(2);
	    adapio1.qiop=0x8f;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    sleep(2);
	    adapio1.qiop=0x0f;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    sleep(10);
	    adapio1.qiop = ALL_GPIO_BITS_HIGH;
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    break;
	  }
	case ADAPF2:
	  {
	    /* ADAfruit Permaproto Full #2 */
	    C2S_i2cset(PI_B2,
		       adapio2.addr,
		       adapio2.gpio,
		       codes7seg7[16],
		       xflag);
	    adapio1.qiop = cath4x7[0];
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    mxc = 16;
	    if (!xflag) mxc=1;
	    for (l=0; l<16; l=l+4)
	      {
		i = 4;
		do
		  {
		    k = load_four7seg_digit(i, FALSE, l+i-1);
		    C2S_i2cset(PI_B2,
			       adapio2.addr,
			       adapio2.gpio,
			       four7seg[k].dcode,
			       xflag);
		    adapio1.qiop = four7seg[k].poll;
		    C2S_i2cout(PI_B2,
			       adapio1.addr,
			       adapio1.qiop,
			       xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }
	    C2S_i2cset(PI_B2,
		       adapio2.addr,
		       adapio2.gpio,
		       codes7seg7[16],
		       xflag);
	    adapio1.qiop = cath4x7[0];
	    C2S_i2cout(PI_B2,
		       adapio1.addr,
		       adapio1.qiop,
		       xflag);
	    break;
	  }
	case HUMPI1:
	  {
	    /* HUMble PI #1*/
	    /* Check all dots first */
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_ON,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       ALL_ROW_CATHODES_ON,
		       xflag);
	    sleep(5);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_OFF,
		       xflag);
	    sleep(2);
	    /* Shrink & expand */
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_ON,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       0xe7,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       0x81,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       0xc3,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       0xc3,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       0x81,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       0xe7,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_OFF,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       ALL_ROW_CATHODES_OFF,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       0x81,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       0xe7,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       0xc3,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       0xc3,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       0xe7,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       0x81,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       0xff,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       0x00,
		       xflag);
	    sleep(5);
	    /* Disable display */
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_OFF,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       ALL_GPIO_BITS_LOW,
		       xflag);
	    break;
	  }
	case HUMPI2:
	  {
	    /* HUMble PI #2*/
	    /* Disable display */
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_OFF,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       ALL_GPIO_BITS_LOW,
		       xflag);
	    sleep(1);
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_ON,
		       xflag);
	    for (i=0; i<4; i++)
	      {
		C2S_i2cset(PI_B2,
			   humpi27.addr,
			   humpi27.gpio,
			   RC1_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi27.addr,
			   humpi27.gpio,
			   RC2_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi27.addr,
			   humpi27.gpio,
			   RC3_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi27.addr,
			   humpi27.gpio,
			   RC4_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi27.addr,
			   humpi27.gpio,
			   RC5_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi27.addr,
			   humpi27.gpio,
			   RC6_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi27.addr,
			   humpi27.gpio,
			   RC7_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi27.addr,
			   humpi27.gpio,
			   RC8_ON,
			   xflag);
		sleep(1);
	      }
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_OFF,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       ALL_ROW_CATHODES_ON,
		       xflag);
	    for (i=0; i<4; i++)
	      {
		C2S_i2cset(PI_B2,
			   humpi22.addr,
			   humpi22.gpio,
			   CA1_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi22.addr,
			   humpi22.gpio,
			   CA2_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi22.addr,
			   humpi22.gpio,
			   CA3_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi22.addr,
			   humpi22.gpio,
			   CA4_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi22.addr,
			   humpi22.gpio,
			   CA5_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi22.addr,
			   humpi22.gpio,
			   CA6_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi22.addr,
			   humpi22.gpio,
			   CA7_ON,
			   xflag);
		sleep(1);
		C2S_i2cset(PI_B2,
			   humpi22.addr,
			   humpi22.gpio,
			   CA8_ON,
			   xflag);
		sleep(1);
	      }
	    /* Disable display */
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_OFF,
		       xflag);
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       ALL_GPIO_BITS_LOW,
		       xflag);
	    break;
	  }
	case SILEM1:
	  {
	    /* SIL Edge Mounted lcd driver */
	    /* Turn backlight 10% on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       25,
		       xflag);
	    sleep(2);
	    /* Turn backlight half-on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       125,
		       xflag);	
	    sleep(2);
	    /* Turn backlight full-on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       250,
		       xflag);
	    sleep(2);
	    /* Turn backlight off */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       0,
		       xflag);
	    sleep(2);
	    /* Turn backlight half-on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       125,
		       xflag);
	    /* Set Display Type (16 cols x 2 rows) */
	    data[0] = 2;
	    data[1] = 16;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.lcdtype,
			data,
			2,
			xflag);
	    /* Clear display */
	    C2S_i2cout(PI_B2,
		       silbhem.addr,
		       silbhem.cleardisp,
		       xflag);
	    /* Set Cursor Position */
	    data[0] = 1;
	    data[1] = 1;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.cursorposn,
			data,
			2,
			xflag);
	    /* Write 16 single characters */
	    for (i=0; i<16; i++)
	      {
		sleep(1);
		j = i + 0x30;
		C2S_i2cset(PI_B2,
			    silbhem.addr,
			    silbhem.writechar,
			    j,
			    xflag);		
	      }
	    sleep(5);
	    /* Clear line */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.clearline,
		       2,
		       xflag);
	    /* Print String */
	    strcpy(buffer2, "16x2 LCD Display");
	    j = strlen(buffer2);
	    for (i=0; i<j; i++) data[i] = (int)(buffer2[i]);
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.printstr,
			data,
			j,
			xflag);
	    sleep(10);
	    /* Clear display */
	    C2S_i2cout(PI_B2,
		       silbhem.addr,
		       silbhem.cleardisp,
		       xflag);
	    sleep(1);
	    /* Turn backlight off */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       0,
		       xflag);
	    break;
	  }
	case SILEM2:
	  {
	    /* SIL Edge Mounted lcd driver */
	    /* Turn backlight half-on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       125,
		       xflag);	
	    /* Set Display Type (16 cols x 2 rows) */
	    data[0] = 2;
	    data[1] = 16;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.lcdtype,
			data,
			2,
			xflag);
	    /* Clear display */
	    C2S_i2cout(PI_B2,
		       silbhem.addr,
		       silbhem.cleardisp,
		       xflag);
	    /* Write out character set */
	    for (i=0; i<128; i++)
	      {
		if (i == 16) i = 32; /* No characters for 16 - 31 */
		j = (i / 16) % 2;
		k = i % 16;
		if (k == 0)
		  {
		    sleep(4);
		    if (j == 0) C2S_i2cset(PI_B2,
					 silbhem.addr,
					 silbhem.clearline,
					 1,
					 xflag);
		    else C2S_i2cset(PI_B2,
				    silbhem.addr,
				    silbhem.clearline,
				    2,
				    xflag);
		  }
		C2S_i2cset(PI_B2,
			   silbhem.addr,
			   silbhem.writechar,
			   i,
			   xflag);		
	      }
	    sleep(16);
	    /* Turn backlight almost off */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       2,
		       xflag);
	    break;
	  }
	case SILEM3:
	  {
	    /* Set Cursor Position */
	    data[0] = 1;
	    data[1] = 1;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.cursorposn,
			data,
			2,
			xflag);
	    /* Write 40 "upper-case" characters */
	    for (i=0; i<40; i++)
	      {
		sleep(1);
		if (i > 15)
		  {
		    /* Shift Display Left */
		    C2S_i2cset(PI_B2,
			       silbhem.addr,
			       silbhem.hd44780,
			       LCD_DISPLAY_SHIFT_LEFT,
			       xflag);	    
		  }
		j = i + 0x30;
		C2S_i2cset(PI_B2,
			    silbhem.addr,
			    silbhem.writechar,
			    j,
			    xflag);		
	      }
	    sleep(1);
	    /* Return Home */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.hd44780,
		       LCD_RETURN_HOME,
		       xflag);	    
	    
	    /* Set Cursor Position */
	    data[0] = 2;
	    data[1] = 1;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.cursorposn,
			data,
			2,
			xflag);
	    /* Write 40 "lower-case" characters */
	    for (i=40; i<80; i++)
	      {
		sleep(1);
		if (i > 55)
		  {
		    /* Shift Display Left */
		    C2S_i2cset(PI_B2,
			       silbhem.addr,
			       silbhem.hd44780,
			       LCD_DISPLAY_SHIFT_LEFT,
			       xflag);	    
		  }
		j = i + 0x30;
		C2S_i2cset(PI_B2,
			    silbhem.addr,
			    silbhem.writechar,
			    j,
			    xflag);		
	      }
	    sleep(10);
	    /* Clear display */
	    C2S_i2cout(PI_B2,
		       silbhem.addr,
		       silbhem.cleardisp,
		       xflag);
	    sleep(1);
	    /* Cursor Off */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.hd44780,
		       LCD_DISPLAY_ON_NO_CURSOR,
		       xflag);	    
	    break;
	  }
	case SOPIO1:
	  {
	    /* Slice of Pi/o #1 */
	    /* Check all digits first ... */
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpioa,
		       codes7seg0[17],
		       xflag);
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpiob,
		       cathode[9],
		       xflag);
	    sleep(5);
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpioa,
		       codes7seg0[16],
		       xflag);
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpiob,
		       cathode[0],
		       xflag);
	    /* Cycle through the digits ... */
	    for (i=0; i<16; i++)
	      {
		C2S_i2cset(PI_B2,
			   sopio.addr,
			   sopio.gpioa,
			   codes7seg0[i],
			   xflag);
		C2S_i2cset(PI_B2,
			   sopio.addr,
			   sopio.gpiob,
			   cathode[(i % 8) + 1],
			   xflag);
		sleep(1);
	      }
	    sleep(5);
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpioa,
		       codes7seg0[16],
		       xflag);
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpiob,
		       cathode[0],
		       xflag);
	    break;
	  }
	case SILEM4:
	  {
	    /* Set Cursor Position */
	    data[0] = 1;
	    data[1] = 1;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.cursorposn,
			data,
			2,
			xflag);
	    /* Write 0 - 7 */
	    for (i=0; i<8; i++)
	      {
		sleep(1);
		j = i + 0x30;
		C2S_i2cset(PI_B2,
			   silbhem.addr,
			   silbhem.writechar,
			   j,
			   xflag);
	      }
	    /* Create New User Char. 0 - 7 */
	    init_hd44780u(); /* Default "Chessboard" Char. */
	    for (i=0; i<8; i++)
	      {
		lcd_set_createchar_data(i, data);
		C2S_i2cseti(PI_B2,
			    silbhem.addr,
			    silbhem.createchar,
			    data,
			    9,
			    xflag);
	      }
	    /* Set Cursor Position */
	    data[0] = 2;
	    data[1] = 1;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.cursorposn,
			data,
			2,
			xflag);
	    /* Write User Chars. 0 - 7 */
	    for (i=0; i<8; i++)
	      {
		sleep(1);
		C2S_i2cset(PI_B2,
			   silbhem.addr,
			   silbhem.writechar,
			   i,
			   xflag);
	      }
	    sleep(10);
	    /* Clear display */
	    C2S_i2cout(PI_B2,
		       silbhem.addr,
		       silbhem.cleardisp,
		       xflag);
	    sleep(1);
	    /* Cursor Off */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.hd44780,
		       LCD_DISPLAY_ON_NO_CURSOR,
		       xflag);
	    sleep(1);
	    /* Turn backlight almost off */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       2,
		       xflag);	    
	    break;
	  }
	case SOPIO2:
	  {
	    /* Slice of Pi/o #2 */
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpioa,
		       codes7seg0[16],
		       xflag);
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpiob,
		       cathode[0],
		       xflag);
	    mxc = 10;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=8;
		do
		  {
		    k = load_sevenseg_digit(i, FALSE, i);
		    C2S_i2cset(PI_B2,
			       sopio.addr,
			       sopio.gpioa,
			       sevenseg[k].dcode,
			       xflag);
		    C2S_i2cset(PI_B2,
			       sopio.addr,
			       sopio.gpiob,
			       sevenseg[k].poll,
			       xflag);
		    sleep(1);
		    i--;
		  }
		while(k>0);
	      }
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpioa,
		       codes7seg0[16],
		       xflag);
	    C2S_i2cset(PI_B2,
		       sopio.addr,
		       sopio.gpiob,
		       cathode[0],
		       xflag);
	    break;
	  }
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
  if ((i2cmode == WPIMODE) && (demode == HUMPI3))
    {
      /*
      printf("Using 'C' system calls to 'gpio' and some wiringPi methods: ...\n");
      C2S_gpio("mode 5 out", xflag);
      mxc = 50;
      if (!xflag) mxc=5;     
      for (i=0; i<mxc; i++)
	{
	  C2S_gpio("write 5 0", xflag);
	  delay(50);
	  C2S_gpio("write 5 1", xflag);
	  delay(50);
	}
      C2S_gpio("readall", xflag);
      */
      mxc = 50;
      if (!xflag) mxc=5;     
      for (i=0; i<mxc; i++)
	{
	  WP_digitalWrite(24, GPIO_LOW, xflag);
	  delay(50);
	  WP_digitalWrite(24, GPIO_HIGH, xflag);
	  delay(50);
	}
    }
#endif /* HAVE_WIRINGPI_H */
  
#ifdef HAVE_WIRINGPII2C_H
  if ((i2cmode == WPIMODE) && (demode != HUMPI3))
    {
      switch(demode)
	{
	case ADAPF1:
	  {
	    /* ADAfruit Permaproto Full #1 */
	    /* Check all digits first ... */
	    i2c_chip[1].reg  = (int)(adapio2.gpio);
	    i2c_chip[1].data = ALL_GPIO_BITS_HIGH;
	    WP_i2cset(i2c_chip[1], xflag);
	    i2c_chip[0].data = 0x0f;
	    WP_i2cout(i2c_chip[0], xflag);
	    sleep(10);
	    i2c_chip[0].data = ALL_GPIO_BITS_HIGH;
	    WP_i2cout(i2c_chip[0], xflag);
	    /* Cycle through the digits ... */
	    for (i=0; i<16; i++)
	      {		
		j = i % 4;
		switch(j)
		  {
		  case 0: adapio1.qiop=0xef; break;
		  case 1: adapio1.qiop=0xdf; break;
		  case 2: adapio1.qiop=0xbf; break;
		  case 3: adapio1.qiop=0x7f; break;
		  }
		i2c_chip[0].data = (int)(adapio1.qiop);
		WP_i2cout(i2c_chip[0], xflag);
		sleep(1);
	      }
	    i2c_chip[0].data = ALL_GPIO_BITS_HIGH;
	    WP_i2cout(i2c_chip[0], xflag);
	    /* Incrementally display all four ... */
	    adapio1.qiop=0xef;
	    i2c_chip[0].data = (int)(adapio1.qiop);
	    WP_i2cout(i2c_chip[0], xflag);
	    sleep(2);
	    adapio1.qiop=0xcf;
	    i2c_chip[0].data = (int)(adapio1.qiop);
	    WP_i2cout(i2c_chip[0], xflag);
	    sleep(2);
	    adapio1.qiop=0x8f;
	    i2c_chip[0].data = (int)(adapio1.qiop);
	    WP_i2cout(i2c_chip[0], xflag);
	    sleep(2);
	    adapio1.qiop=0x0f;
	    i2c_chip[0].data = (int)(adapio1.qiop);
	    WP_i2cout(i2c_chip[0], xflag);
	    sleep(10);
	    i2c_chip[0].data = ALL_GPIO_BITS_HIGH;
	    WP_i2cout(i2c_chip[0], xflag);
	    break;
	  }
	case ADAPF2:
	  {
	    /* ADAfruit Permaproto Full #2 */
	    i2c_chip[1].reg  = (int)(adapio2.gpio); /* Used for all [1] calls */
	    i2c_chip[1].data = codes7seg7[16];
	    WP_i2cset(i2c_chip[1], xflag);
	    mxc = 250;
	    if (!xflag) mxc=1;
	    for (l=0; l<16; l=l+4)
	      {
		for (j=0; j<mxc; j++)
		  {
		    i = 4;
		    do
		      {
			i2c_chip[0].data = cath4x7[0];
			WP_i2cout(i2c_chip[0], xflag);
			k = load_four7seg_digit(i, FALSE, l+i-1);
			i2c_chip[1].data = four7seg[k].dcode;
			WP_i2cset(i2c_chip[1], xflag);
			i2c_chip[0].data = four7seg[k].poll;
			WP_i2cout(i2c_chip[0], xflag);
			delay(6);
			i--;
		      }
		    while(k>0);
		  }
	      }
	    i2c_chip[1].data = codes7seg7[16];
	    WP_i2cset(i2c_chip[1], xflag);
	    i2c_chip[0].data = cath4x7[0];
	    WP_i2cout(i2c_chip[0], xflag);
	    sleep(2);
	    mxc = 500;
	    if (!xflag) mxc=1;
	    for (j=0; j<mxc; j++)
	      {
		i = 4;
		do
		  {
		    i2c_chip[0].data = cath4x7[0];
		    WP_i2cout(i2c_chip[0], xflag);
		    switch(i)
		      {
		      case 1: l=3; k=load_four7seg_digit(i, TRUE, l); break;
		      case 2: l=1; k=load_four7seg_digit(i, FALSE, l); break;
		      case 3: l=4; k=load_four7seg_digit(i, FALSE, l); break;
		      case 4: l=2; k=load_four7seg_digit(i, FALSE, l); break;
		      }
		    i2c_chip[1].data = four7seg[k].dcode;
		    WP_i2cset(i2c_chip[1], xflag);
		    i2c_chip[0].data = four7seg[k].poll;
		    WP_i2cout(i2c_chip[0], xflag);
		    delay(6);
		    i--;
		  }
		while(k>0);
	      }	  
	    i2c_chip[1].data = codes7seg7[16];
	    WP_i2cset(i2c_chip[1], xflag);
	    i2c_chip[0].data = cath4x7[0];
	    WP_i2cout(i2c_chip[0], xflag);
	    break;
	  }
	case ADAPFQ:
	  {
	    /* ADAfruit Permaproto Full Query */
	    i2c_chip[1].reg = (int)(adapio2.iocon);
	    i2c_chip[1].data = WP_i2cget(i2c_chip[1], xflag);
	    if (xflag && (i2c_chip[1].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[1].did,
		       i2c_chip[1].reg,
		       i2c_chip[1].data);
	      }
	    i2c_chip[1].reg = (int)(adapio2.iodir);
	    i2c_chip[1].data = WP_i2cget(i2c_chip[1], xflag);
	    if (xflag && (i2c_chip[1].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[1].did,
		       i2c_chip[1].reg,
		       i2c_chip[1].data);
	      }
	    i2c_chip[1].reg = (int)(adapio2.iopol);
	    i2c_chip[1].data = WP_i2cget(i2c_chip[1], xflag);
	    if (xflag && (i2c_chip[1].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[1].did,
		       i2c_chip[1].reg,
		       i2c_chip[1].data);
	      }
	    i2c_chip[1].reg = (int)(adapio2.gpio);
	    i2c_chip[1].data = WP_i2cget(i2c_chip[1], xflag);
	    if (xflag && (i2c_chip[1].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[1].did,
		       i2c_chip[1].reg,
		       i2c_chip[1].data);
	      }
	    break;
	  }
	case HUMPI1:
	  {
	    /* HUMble PI #1*/
	    /* NB: All calls will use these registers! */
	    i2c_chip[0].reg  = (int)(humpi22.gpio);
	    i2c_chip[1].reg  = (int)(humpi27.gpio);
	    /* Check all dots first */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_ON;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[1].data = ALL_ROW_CATHODES_ON;
	    WP_i2cset(i2c_chip[1], xflag);
	    sleep(5);
	    i2c_chip[0].data = ALL_COLUMN_ANODES_OFF;
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(2);
	    /* Shrink & expand */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_ON;
	    WP_i2cset(i2c_chip[0], xflag);
	    for (i=0; i<5; i++)
	      {
		delay(400);
		i2c_chip[0].data = 0xe7;
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[1].data = 0x81;
		WP_i2cset(i2c_chip[1], xflag);
		delay(400);
		i2c_chip[0].data = 0xc3;
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[1].data = 0xc3;
		WP_i2cset(i2c_chip[1], xflag);
		delay(400);
		i2c_chip[0].data = 0x81;
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[1].data = 0xe7;
		WP_i2cset(i2c_chip[1], xflag);
		delay(400);
		i2c_chip[0].data = ALL_COLUMN_ANODES_OFF;
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[1].data = ALL_ROW_CATHODES_OFF;
		WP_i2cset(i2c_chip[1], xflag);
		delay(400);
		i2c_chip[0].data = 0x81;
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[1].data = 0xe7;
		WP_i2cset(i2c_chip[1], xflag);
		delay(400);
		i2c_chip[0].data = 0xc3;
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[1].data = 0xc3;
		WP_i2cset(i2c_chip[1], xflag);
		delay(400);
		i2c_chip[0].data = 0xe7;
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[1].data = 0x81;
		WP_i2cset(i2c_chip[1], xflag);
		delay(400);
		i2c_chip[0].data = ALL_COLUMN_ANODES_ON;
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[1].data = ALL_ROW_CATHODES_ON;
		WP_i2cset(i2c_chip[1], xflag);
	      }
	    sleep(2);
	    /* Disable display */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_OFF;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[1].data = ALL_GPIO_BITS_LOW;
	    WP_i2cset(i2c_chip[1], xflag);
	    break;
	  }
	case HUMPI2:
	  {
	    /* HUMble PI #2*/
	    /* NB: All calls will use these registers! */
	    i2c_chip[0].reg  = (int)(humpi22.gpio);
	    i2c_chip[1].reg  = (int)(humpi27.gpio);
	    /* Disable display */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_OFF;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[1].data = ALL_GPIO_BITS_LOW;
	    WP_i2cset(i2c_chip[1], xflag);
	    sleep(1);
	    i2c_chip[0].data = ALL_COLUMN_ANODES_ON;
	    WP_i2cset(i2c_chip[0], xflag);
	    for (i=0; i<10; i++)
	      {
		i2c_chip[1].data = RC1_ON;
		WP_i2cset(i2c_chip[1], xflag);
		delay(100);
		i2c_chip[1].data = RC2_ON;
		WP_i2cset(i2c_chip[1], xflag);
		delay(100);
		i2c_chip[1].data = RC3_ON;
		WP_i2cset(i2c_chip[1], xflag);
		delay(100);
		i2c_chip[1].data = RC4_ON;
		WP_i2cset(i2c_chip[1], xflag);
		delay(100);
		i2c_chip[1].data = RC5_ON;
		WP_i2cset(i2c_chip[1], xflag);
		delay(100);
		i2c_chip[1].data = RC6_ON;
		WP_i2cset(i2c_chip[1], xflag);
		delay(100);
		i2c_chip[1].data = RC7_ON;
		WP_i2cset(i2c_chip[1], xflag);
		delay(100);
		i2c_chip[1].data = RC8_ON;
		WP_i2cset(i2c_chip[1], xflag);
		delay(100);
	      }
	    i2c_chip[0].data = ALL_COLUMN_ANODES_OFF;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[1].data = ALL_ROW_CATHODES_ON;
	    WP_i2cset(i2c_chip[1], xflag);
	    for (i=0; i<10; i++)
	      {
		i2c_chip[0].data = CA1_ON;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
		i2c_chip[0].data = CA2_ON;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
		i2c_chip[0].data = CA3_ON;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
		i2c_chip[0].data = CA4_ON;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
		i2c_chip[0].data = CA5_ON;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
		i2c_chip[0].data = CA6_ON;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
		i2c_chip[0].data = CA7_ON;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
		i2c_chip[0].data = CA8_ON;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
	      }
	    /* Disable display */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_OFF;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[1].data = ALL_GPIO_BITS_LOW;
	    WP_i2cset(i2c_chip[1], xflag);
	    break;
	  }
	case HUMPIQ:
	  {
	    /* HUMble Pi device Query*/
	    i2c_chip[0].reg = (int)(humpi22.iocon);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(humpi22.iodir);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(humpi22.iopol);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(humpi22.gpio);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[1].reg = (int)(humpi27.iocon);
	    i2c_chip[1].data = WP_i2cget(i2c_chip[1], xflag);
	    if (xflag && (i2c_chip[1].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[1].did,
		       i2c_chip[1].reg,
		       i2c_chip[1].data);
	      }
	    i2c_chip[1].reg = (int)(humpi27.iodir);
	    i2c_chip[1].data = WP_i2cget(i2c_chip[1], xflag);
	    if (xflag && (i2c_chip[1].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[1].did,
		       i2c_chip[1].reg,
		       i2c_chip[1].data);
	      }
	    i2c_chip[1].reg = (int)(humpi27.iopol);
	    i2c_chip[1].data = WP_i2cget(i2c_chip[1], xflag);
	    if (xflag && (i2c_chip[1].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[1].did,
		       i2c_chip[1].reg,
		       i2c_chip[1].data);
	      }
	    i2c_chip[1].reg = (int)(humpi27.gpio);
	    i2c_chip[1].data = WP_i2cget(i2c_chip[1], xflag);
	    if (xflag && (i2c_chip[1].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[1].did,
		       i2c_chip[1].reg,
		       i2c_chip[1].data);
	      }
	    break;
	  }
	case SILEM1:
	  {
	    /* SIL Edge Mounted lcd driver */
	    /* Turn backlight 10% on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 25;
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(2);
	    /* Turn backlight half-on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 125;
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(2);
	    /* Turn backlight full-on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 250;
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(2);
	    /* Turn backlight off */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 0;
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(2);
	    /* Turn backlight half-on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 125;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Set Display Type (16 cols x 2 rows) */
	    i2c_chip[0].reg  = (int)(silbhem.lcdtype);
	    i2c_chip[0].data = 2;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + 16; 
	    WP_i2cset2(i2c_chip[0], xflag);
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    /* Set Cursor Position */
	    i2c_chip[0].reg  = (int)(silbhem.cursorposn);
	    i2c_chip[0].data = 1;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + 1; 
	    WP_i2cset2(i2c_chip[0], xflag);
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    /* Write 16 single characters */
	    for (i=0; i<16; i++)
	      {
		delay(500);
		j = i + 0x30;
		i2c_chip[0].reg  = (int)(silbhem.writechar);
		i2c_chip[0].data = j;
		WP_i2cset(i2c_chip[0], xflag);
	      }
	    sleep(5);
	    /* Clear line */
	    i2c_chip[0].reg  = (int)(silbhem.clearline);
	    i2c_chip[0].data = 2;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Emulate print string */
	    i2c_chip[0].reg  = (int)(silbhem.writechar);
	    strncpy(i2c_chip[0].ibytes,
		    "raspigrey SILEM1",
		    (WP_MAX_IBYTES-1));
	    WP_i2cseti(i2c_chip[0],
		       strlen(i2c_chip[0].ibytes),
		       xflag);
	    sleep(10);
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    sleep(1);
	    /* Turn backlight off */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 0;
	    WP_i2cset(i2c_chip[0], xflag);
	    break;
	  }
	case SILEM2:
	  {
	    /* Turn backlight half-on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 125;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Set Display Type (16 cols x 2 rows) */
	    i2c_chip[0].reg  = (int)(silbhem.lcdtype);
	    i2c_chip[0].data = 2;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + 16; 
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    /* Write out character set */
	    for (i=0; i<128; i++)
	      {
		if (i == 16) i = 32; /* No characters for 16 - 31 */
		j = (i / 16) % 2;
		k = i % 16;
		if (k == 0)
		  {
		    delay(3600);
		    if (j == 0)
		      {
			i2c_chip[0].reg  = (int)(silbhem.clearline);
			i2c_chip[0].data = 1;
			WP_i2cset(i2c_chip[0], xflag);
		      }
		    else
		      {
			i2c_chip[0].reg  = (int)(silbhem.clearline);
			i2c_chip[0].data = 2;
			WP_i2cset(i2c_chip[0], xflag);
		      }
		  }
		delay(400);
		i2c_chip[0].reg  = (int)(silbhem.writechar);
		i2c_chip[0].data = i;
		WP_i2cset(i2c_chip[0], xflag);
	      }
	    sleep(16);
	    /* Turn backlight almost off */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 2;
	    WP_i2cset(i2c_chip[0], xflag);
	    break;
	  }
	case SILEM3:
	  {
	    /* Set Cursor Position */
	    i2c_chip[0].reg  = (int)(silbhem.cursorposn);
	    i2c_chip[0].data = 1;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + 1; /* Byte order Low - High */
	    WP_i2cset2(i2c_chip[0], xflag);
	    /* Write 40 "upper-case" characters */
	    for (i=0; i<40; i++)
	      {
		delay(800);
		if (i > 15)
		  {
		    /* Shift Display Left */
		    i2c_chip[0].reg  = (int)(silbhem.hd44780);
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT;
		    WP_i2cset(i2c_chip[0], xflag);
		  }
		j = i + 0x30;
		i2c_chip[0].reg  = (int)(silbhem.writechar);
		i2c_chip[0].data = j;
		WP_i2cset(i2c_chip[0], xflag);
	      }
	    sleep(1);
	    /* Return Home */
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_RETURN_HOME;
	    WP_i2cset(i2c_chip[0], xflag);
	    delay(10);
	    /* Set Cursor Position */
	    i2c_chip[0].reg  = (int)(silbhem.cursorposn);
	    i2c_chip[0].data = 1;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + 2; /* Byte order Low - High */
	    WP_i2cset2(i2c_chip[0], xflag);
	    /* Write 40 "lower-case" characters */
	    for (i=40; i<80; i++)
	      {
		delay(800);
		if (i > 55)
		  {
		    /* Shift Display Left */
		    i2c_chip[0].reg  = (int)(silbhem.hd44780);
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT;
		    WP_i2cset(i2c_chip[0], xflag);
		  }
		j = i + 0x30;
		i2c_chip[0].reg  = (int)(silbhem.writechar);
		i2c_chip[0].data = j;
		WP_i2cset(i2c_chip[0], xflag);
	      }
	    sleep(8);
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Cursor Off */
	    break;
	  }
	case SOPIO1:
	  {
	    /* Slice of Pi/o #1 */
	    /* Check all digits first ... */
	    i2c_chip[0].reg  = (int)(sopio.gpioa);
	    i2c_chip[0].data = codes7seg0[17];
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(sopio.gpiob);
	    i2c_chip[0].data = cathode[9];
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(5);
	    i2c_chip[0].reg  = (int)(sopio.gpioa);
	    i2c_chip[0].data = codes7seg0[16];
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(sopio.gpiob);
	    i2c_chip[0].data = cathode[0];
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Cycle through the digits ... */
	    for (i=0; i<16; i++)
	      {
		i2c_chip[0].reg  = (int)(sopio.gpioa);
		i2c_chip[0].data = codes7seg0[i];
		WP_i2cset(i2c_chip[0], xflag);
		i2c_chip[0].reg  = (int)(sopio.gpiob);
		i2c_chip[0].data = cathode[(i % 8) + 1];
		WP_i2cset(i2c_chip[0], xflag);
		sleep(1);
	      }
	    sleep(5);
	    i2c_chip[0].reg  = (int)(sopio.gpioa);
	    i2c_chip[0].data = codes7seg0[16];
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(sopio.gpiob);
	    i2c_chip[0].data = cathode[0];
	    WP_i2cset(i2c_chip[0], xflag);
	    break;
	  }
	case SOPIO2:
	  {
	    /* Slice of Pi/o #2 */
	    i2c_chip[0].reg  = (int)(sopio.gpioa);
	    i2c_chip[0].data = codes7seg0[16];
	    WP_i2cset(i2c_chip[0], xflag);
	    mxc = 500;
	    if (!xflag) mxc=1;
	    for (l=mxc; l>0; l--)
	      {
		i=8;
		do
		  {
		    i2c_chip[0].reg  = (int)(sopio.gpiob);
		    i2c_chip[0].data = cathode[0];
		    WP_i2cset(i2c_chip[0], xflag);
		    k = load_sevenseg_digit(i, FALSE, i);
		    i2c_chip[0].reg  = (int)(sopio.gpioa);
		    i2c_chip[0].data = sevenseg[k].dcode;
		    WP_i2cset(i2c_chip[0], xflag);
		    i2c_chip[0].reg  = (int)(sopio.gpiob);
		    i2c_chip[0].data = sevenseg[k].poll;
		    WP_i2cset(i2c_chip[0], xflag);
		    delay(6);
		    i--;
		  }
		while(k>0);
	      }
	    i2c_chip[0].reg  = (int)(sopio.gpioa);
	    i2c_chip[0].data = codes7seg0[16];
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(sopio.gpiob);
	    i2c_chip[0].data = cathode[0];
	    WP_i2cset(i2c_chip[0], xflag);
	    break;
	  }
	case SOPIOQ:
	  {
	    /* Slice of Pi/o device Query */
	    i2c_chip[0].reg = (int)(sopio.iocon);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(sopio.iodira);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(sopio.iodirb);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(sopio.ipola);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(sopio.ipolb);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(sopio.gpioa);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    i2c_chip[0].reg = (int)(sopio.gpiob);
	    i2c_chip[0].data = WP_i2cget(i2c_chip[0], xflag);
	    if (xflag && (i2c_chip[0].data != -1))
	      {
		printf("I2C Device 0x%02x Register 0x%02x = 0x%02x\n",
		       i2c_chip[0].did,
		       i2c_chip[0].reg,
		       i2c_chip[0].data);
	      }
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPII2C_H */

#ifdef HAVE_WIRINGPI_H
  /* Tidy up ... */
  if ((i2cmode == WPIMODE) && (demode == HUMPI3))
    {
      C2S_gpio_unexportall(xflag);
    }
#endif /* HAVE_WIRINGPI_H */
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: demo.c,v $
 * Revision 1.3  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.2  2017/07/25 14:09:05  pi
 * Interim Commit
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.1  2014/07/18 19:13:46  pi
 * Initial revision
 *
 * Revision 1.1  2014/05/26 21:45:35  pi
 * Initial revision
 *
 * Revision 1.16  2014/03/21 18:49:09  pi
 * Interim commit
 *
 * Revision 1.15  2014/03/09 21:53:08  pi
 * Interim commit
 *
 * Revision 1.14  2014/03/09 19:43:25  pi
 * Interim commit
 *
 * Revision 1.13  2014/03/08 22:41:44  pi
 * -SILEM3 demo
 *
 * Revision 1.12  2014/03/08 22:13:53  pi
 * Interim commit
 *
 * Revision 1.11  2014/03/04 22:02:12  pi
 * -SE1 and preliminary -SE2 options added
 *
 * Revision 1.10  2014/03/02 22:47:24  pi
 * Interim commit
 *
 * Revision 1.9  2014/03/02 15:13:21  pi
 * Interim commit
 *
 * Revision 1.8  2014/03/02 14:30:55  pi
 * Interim commit
 *
 * Revision 1.7  2014/03/02 14:18:50  pi
 * Interim commit
 *
 * Revision 1.6  2014/03/01 18:22:15  pi
 * LCD Backpack driven display tests using 'C' system calls added
 *
 * Revision 1.5  2014/03/01 13:58:17  pi
 * Interim commit, before LCD code generation
 *
 * Revision 1.4  2013/06/11 20:51:49  pi
 * Interim commit
 *
 * Revision 1.3  2013/06/07 21:29:22  pi
 * Interim commit
 *
 * Revision 1.2  2013/06/03 21:05:20  pi
 * Bug-fix to enable compilation w/o wiringPi tools
 *
 * Revision 1.1  2013/06/03 20:59:20  pi
 * Initial revision
 *
 * Revision 1.17  2013/05/29 22:54:55  pi
 * -SPQ option working
 * SCMVersion 0.03
 *
 * Revision 1.16  2013/05/29 22:32:35  pi
 * -SP1 & -SP2 options added
 *
 * Revision 1.15  2013/05/27 21:46:38  pi
 * Interim commit (-HPQ option added)
 *
 * Revision 1.14  2013/05/26 23:33:57  pi
 * -APQ option beginning to test WP_i2cget() function
 *
 * Revision 1.13  2013/05/26 21:59:06  pi
 * Now using 
 * C2S_i2cout()
 *
 * Revision 1.12  2013/05/26 21:26:32  pi
 * Now using WP_i2cout()
 *
 * Revision 1.11  2013/05/24 21:56:15  pi
 * Interim commit
 * -AP2 option working. Using libled_disptools.a 
 * SCMVersion 0.02
 *
 * Revision 1.10  2013/05/24 21:07:00  pi
 * Interim commit
 * -AP1 option working
 *
 * Revision 1.9  2013/05/19 21:30:01  pi
 * Interim commit
 * WiringPi version of -HP2 working
 *
 * Revision 1.8  2013/05/19 21:11:52  pi
 * Interim commit
 * WiringPi form of -HP1 working
 *
 * Revision 1.7  2013/05/19 19:54:05  pi
 * Interim commit
 * -HP1 & -HP2 options working via 'C' system methods
 *
 * Revision 1.6  2013/05/16 21:42:16  pi
 * -HP1 system call working
 *
 * Revision 1.5  2013/05/15 23:07:59  pi
 * Interim commit
 *
 * Revision 1.4  2013/05/15 22:31:18  pi
 * Interim commit
 *
 * Revision 1.3  2013/05/15 16:51:31  pi
 * Interim commit
 *
 * Revision 1.2  2013/05/15 15:35:16  pi
 * Begining to use libi2c_chips
 *
 * Revision 1.1  2013/05/15 08:10:38  pi
 * Initial revision
 *
 *
 *
 */
