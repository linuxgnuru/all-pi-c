/* I2C (2 x MCP23008) interfaced MultipleXed 8x8 LED matriX DEMOnstration code
 * (Adapted, in part, from code in demo.c)
 * $Id: i2c_mxledx_demo.c,v 1.10 2018/03/31 21:32:45 pi Exp $
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
static char RCSFile[]     = "$RCSfile: i2c_mxledx_demo.c,v $";
static char RCSId[]       = "$Id: i2c_mxledx_demo.c,v 1.10 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.10 $";
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
  ECHO,      /* -E                   */
  HUMPI1,    /* -HP1                 */
  INQUIRE,   /* -I                   */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  SYSMODE,   /* -S                   */
  SYSMXT,    /* -SMX                 */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  WPIMXT1,   /* -WMX1                */
  WPIMXT2,   /* -WMX2                */
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,fontis,i2cmode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[HUMPI1]   = "-HP1";
  arglist[INQUIRE]  = "-I";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  arglist[SYSMODE]  = "-S";
  arglist[SYSMXT]   = "-SMX";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  arglist[WPIMXT1]  = "-WMX1";
  arglist[WPIMXT2]  = "-WMX2";
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPII2C_H */
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
char eflag,iqflag,vflag,xflag;
unsigned int caaddr,rcaddr;

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
	    case HUMPI1:  demode=HUMPI1;  break;
	    case INQUIRE: iqflag=TRUE;    break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case SYSMODE: i2cmode=SYSMODE; break;
	    case SYSMXT:  demode=SYSMXT;   i2cmode=SYSMODE; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMXT1: demode=WPIMXT1;  i2cmode=WPIMODE; break;
	    case WPIMXT2: demode=WPIMXT2;  i2cmode=WPIMODE; break;
	    case WPIMODE: i2cmode=WPIMODE; break;
#endif /* HAVE_WIRINGPII2C_H */
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
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -E    {Echo command-line arguments},\n");
  strcat(buffer1, "         -HP1  {Simple,'Humble Pi' demo (un-multiplexed)},\n");
  strcat(buffer1, "         -I    {Inquire about i2c devices},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -S    {use System call methods - default},\n");
  strcat(buffer1, "         -SMX  {Slow MultipleX test- implies -S},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -WMX1 {medium MultipleX test - implies -W},\n");
  strcat(buffer1, "         -WMX2 {rapid MultipleX test - implies -W},\n");
  strcat(buffer1, "         -W    {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  int i,j,k,l,mca,mcb;
  int bite,data[16];
  unsigned int mark,space;
  mcp23008 humpi22,humpi27;
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#ifdef HAVE_WIRINGPII2C_H
  wpi2cvb i2c_chip[2];
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
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
  space   = MCP23008_I2C_DRT;
  mark    = space * 9;
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
	    case ECHO:    eflag=TRUE;     i--; break;
	    case HUMPI1:  demode=HUMPI1;  i--; break;
	    case INQUIRE: iqflag=TRUE;    i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMXT1: demode=WPIMXT1;  i--; i2cmode=WPIMODE; break;
	    case WPIMXT2: demode=WPIMXT2;  i--; i2cmode=WPIMODE; break;
	    case WPIMODE: i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case SYSMODE: i2cmode=SYSMODE; i--; break;
	    case SYSMXT:  demode=SYSMXT;   i2cmode=SYSMODE; i--; break;
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
	  mca = init_mcp23008(&humpi22, 0x22, "humpi22");
	  mcb = init_mcp23008(&humpi27, 0x27, "humpi27");
	  echo_mcp23008_status(humpi22);
	  echo_mcp23008_status(humpi27);
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */

  init_8x8ledx();
  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      mca = init_mcp23008(&humpi22, 0x22, "humpi22");
      mcb = init_mcp23008(&humpi27, 0x27, "humpi27");
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
      echo_mcp23008_status(humpi22);
      echo_mcp23008_status(humpi27);
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      printf("Using wiringPiI2C methods: ...\n");
      mca = init_mcp23008(&humpi22, 0x22, "humpi22");
      mcb = init_mcp23008(&humpi27, 0x27, "humpi27");
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
      echo_mcp23008_status(humpi22);
      echo_mcp23008_status(humpi27);
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

  /* Demonstration/Test ... */
  if (i2cmode == SYSMODE)
    {
      switch(demode)
	{
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
	case SYSMXT:
	  {
	    /* Enable all columns ... */
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_ON,
		       xflag);
	    /* Pre-enable all rows ... */
	    C2S_i2cset(PI_B2,
		       humpi27.addr,
		       humpi27.gpio,
		       ALL_ROW_CATHODES_ON,
		       xflag);
	    sleep(2);
	    /* Cycle through columns ... */
	    for (j=0; j<5; j++)
	      {
		for (i=1; i<9; i++)
		  {
		    C2S_i2cset(PI_B2,
			       humpi22.addr,
			       humpi22.gpio,
			       colanode[i],
			       xflag);
		    sleep(1);
		    C2S_i2cset(PI_B2,
			       humpi22.addr,
			       humpi22.gpio,
			       ALL_COLUMN_ANODES_OFF,
			       xflag);
		  }
	      }
	    sleep(2);
	    /* Pre-enable all columns ... */
	    C2S_i2cset(PI_B2,
		       humpi22.addr,
		       humpi22.gpio,
		       ALL_COLUMN_ANODES_ON,
		       xflag);
	    /* Cycle through rows ... */
	    for (j=0; j<5; j++)
	      {
		for (i=1; i<9; i++)
		  {
		    C2S_i2cset(PI_B2,
			       humpi27.addr,
			       humpi27.gpio,
			       rowcath[i],
			       xflag);
		    sleep(1);
		    C2S_i2cset(PI_B2,
			       humpi27.addr,
			       humpi27.gpio,
			       ALL_ROW_CATHODES_OFF,
			       xflag);
		  }
	      }
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
	case WPIMXT1:
	  {
	    /* NB: All calls will use these registers! */
	    i2c_chip[0].reg  = (int)(humpi22.gpio);
	    i2c_chip[1].reg  = (int)(humpi27.gpio);
	    /* Enable all columns ... */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_ON;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Pre-enable all rows ... */
	    i2c_chip[1].data = ALL_ROW_CATHODES_ON;
	    WP_i2cset(i2c_chip[1], xflag);
	    sleep(2);
	    /* Cycle through column display ... */
	    for (k=0; k<10; k++)
	      {
		/* Cycle through columns ... */
		for (j=0; j<32; j++)
		  {
		    for (i=1; i<9; i++)
		      {
			i2c_chip[0].data = colanode[i];
			WP_i2cset(i2c_chip[0], xflag);
			delay(8);
			i2c_chip[0].data = ALL_COLUMN_ANODES_OFF;
			WP_i2cset(i2c_chip[0], xflag);
			delay(1);
		      }
		  }
	      }
	    sleep(2);
	    /* Pre-enable all columns ... */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_ON;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Cycle through row display ... */
	    for (k=0; k<10; k++)
	      {
		/* Cycle through rows ... */
		for (j=0; j<32; j++)
		  {
		    for (i=1; i<9; i++)
		      {
			i2c_chip[1].data = rowcath[i];
			WP_i2cset(i2c_chip[1], xflag);
			delay(8);
			i2c_chip[1].data = ALL_ROW_CATHODES_OFF;
			WP_i2cset(i2c_chip[1], xflag);
			delay(1);
		      }
		  }
	      }
	    break;
	  }
	case WPIMXT2:
	  {
	    /* NB: All calls will use these registers! */
	    i2c_chip[0].reg  = (int)(humpi22.gpio);
	    i2c_chip[1].reg  = (int)(humpi27.gpio);
	    /* Enable all columns ... */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_ON;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Pre-enable all rows ... */
	    i2c_chip[1].data = ALL_ROW_CATHODES_ON;
	    WP_i2cset(i2c_chip[1], xflag);
	    sleep(2);
	    /* Cycle through column display ... */
	    for (k=0; k<25; k++)
	      {
		/* Cycle through columns ... */
		for (j=0; j<32; j++)
		  {
		    for (i=1; i<9; i++)
		      {
			i2c_chip[0].data = colanode[i];
			WP_i2cset(i2c_chip[0], xflag);
			delayMicroseconds(mark);
			i2c_chip[0].data = ALL_COLUMN_ANODES_OFF;
			WP_i2cset(i2c_chip[0], xflag);
			delayMicroseconds(space);
		      }
		  }
	      }
	    sleep(2);
	    /* Pre-enable all columns ... */
	    i2c_chip[0].data = ALL_COLUMN_ANODES_ON;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Cycle through row display ... */
	    for (k=0; k<25; k++)
	      {
		/* Cycle through rows ... */
		for (j=0; j<32; j++)
		  {
		    for (i=1; i<9; i++)
		      {
			i2c_chip[1].data = rowcath[i];
			WP_i2cset(i2c_chip[1], xflag);
			delayMicroseconds(mark);
			i2c_chip[1].data = ALL_ROW_CATHODES_OFF;
			WP_i2cset(i2c_chip[1], xflag);
			delayMicroseconds(space);
		      }
		  }
	      }
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: i2c_mxledx_demo.c,v $
 * Revision 1.10  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.9  2017/07/27 14:03:03  pi
 * Interim Commit
 *
 * Revision 1.8  2017/07/26 20:49:26  pi
 * SCMVersion 0.01, -HP1, -SMX, -WMX1 & -WMX2 options implemented
 *
 * Revision 1.7  2017/07/26 17:19:26  pi
 * Interim Commit
 *
 * Revision 1.6  2017/07/26 16:29:14  pi
 * Interim Commit
 *
 * Revision 1.5  2017/07/26 13:33:47  pi
 * Interim Commit
 *
 * Revision 1.4  2017/07/26 12:24:08  pi
 * Interim Commit
 *
 * Revision 1.3  2017/07/25 20:12:29  pi
 * Interim Commit
 *
 * Revision 1.2  2017/07/25 14:09:05  pi
 * Interim Commit
 *
 * Revision 1.1  2017/07/25 13:15:48  pi
 * Initial revision
 *
 *
 *
 *
 */

