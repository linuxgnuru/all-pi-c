/* I2C (2xMCP23008) interfaced RGB LED modules DEMOnstration code
 * $Id: i2c_rgb_led_demo.c,v 1.4 2018/03/31 21:32:45 pi Exp $
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

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: i2c_rgb_led_demo.c,v $";
static char RCSId[]       = "$Id: i2c_rgb_led_demo.c,v 1.4 2018/03/31 21:32:45 pi Exp $";
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
  C3TST,     /* -C3T                 */
  DNTST,     /* -DNT                 */
  ECHO,      /* -E                   */
  INQUIRE,   /* -I                   */
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
  arglist[C3TST]    = "-C3T";
  arglist[DNTST]    = "-DNT";
  arglist[ECHO]     = "-E";
  arglist[INQUIRE]  = "-I";
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
	    case C3TST:   demode=C3TST; break;
	    case DNTST:   demode=DNTST; break;
	    case ECHO:    eflag=TRUE;   break;
	    case INQUIRE: iqflag=TRUE;  break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case SYSMODE: i2cmode=SYSMODE; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
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
  strcat(buffer1, "         -C3T {3_Clr rgb diode Test},\n");
  strcat(buffer1, "         -DNT {Dn rgb diodes Test},\n");
  strcat(buffer1, "         -E   {Echo command-line arguments},\n");
  strcat(buffer1, "         -I   {Inquire about i2c devices},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -S   {use System call methods - default},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -W   {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X   {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

#define ALL_DN_ANODES_OFF   0xff
#define ALL_DN_ANODES_ON    0x00
#define DN_RGB_CATHODES_OFF 0xe0 /* | mask */
#define DN_RGB_CATHODES_ON  0x07 /* & mask */
#define CLR3_RGB_ANODES_OFF 0xe0 /* & mask */
#define CLR3_RGB_ANODES_ON  0x07 /* | mask */
#define DNR_CATHODE_OFF     0x80 /* | mask */
#define DNR_CATHODE_ON      0x67 /* & mask */
#define DNG_CATHODE_OFF     0x40 /* | mask */
#define DNG_CATHODE_ON      0xa7 /* & mask */
#define DNB_CATHODE_OFF     0x20 /* | mask */
#define DNB_CATHODE_ON      0xc7 /* & mask */
#define CLR3R_ANODE_OFF     0xea /* & mask */
#define CLR3R_ANODE_ON      0x04 /* | mask */
#define CLR3G_ANODE_OFF     0xed /* & mask */
#define CLR3G_ANODE_ON      0x02 /* | mask */
#define CLR3B_ANODE_OFF     0xee /* & mask */
#define CLR3B_ANODE_ON      0x01 /* | mask */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  int i,j,k,l,mca,mcb;
  int bite,data[16];
  mcp23008 keyesDn,keyesRGB;
  unsigned int dataDn,dataRGB;
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
	    case C3TST:   demode=C3TST; i--; break;
	    case DNTST:   demode=DNTST; i--; break;
	    case ECHO:    eflag=TRUE;   i--; break;
	    case INQUIRE: iqflag=TRUE;  i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE: i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case SYSMODE: i2cmode=SYSMODE; i--; break;

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
	  mca = init_mcp23008(&keyesDn, 0x20, "Led_RGB_V2");
	  mcb = init_mcp23008(&keyesRGB, 0x27, "3_Clr_RGB");
	  echo_mcp23008_status(keyesDn);
	  echo_mcp23008_status(keyesRGB);
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  mca = init_mcp23008(&keyesDn, 0x20, "Led_RGB_V2");
  mcb = init_mcp23008(&keyesRGB, 0x27, "3_Clr_RGB");
  dataDn  = ALL_DN_ANODES_OFF;
  dataRGB = DN_RGB_CATHODES_OFF & CLR3_RGB_ANODES_OFF;

  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      C2S_i2cset(PI_B2,
		 keyesDn.addr,
		 keyesDn.iodir,
		 ALL_GPIO_BITS_OUT,
		 xflag);
      C2S_i2cset(PI_B2,
		 keyesDn.addr,
		 keyesDn.iopol,
		 ALL_GPIO_BITS_NORMAL,
		 xflag);
      C2S_i2cset(PI_B2,
		 keyesDn.addr,
		 keyesDn.gpio,
		 dataDn,
		 xflag);
      C2S_i2cset(PI_B2,
		 keyesRGB.addr,
		 keyesRGB.iodir,
		 ALL_GPIO_BITS_OUT,
		 xflag);
      C2S_i2cset(PI_B2,
		 keyesRGB.addr,
		 keyesRGB.iopol,
		 ALL_GPIO_BITS_NORMAL,
		 xflag);
      C2S_i2cset(PI_B2,
		 keyesRGB.addr,
		 keyesRGB.gpio,
		 dataRGB,
		 xflag);
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      printf("Using wiringPiI2C methods: ...\n");
      i2c_chip[0].did = (int)(keyesDn.addr);
      if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	{
	  fprintf(stderr,
		  "Could not setup i2c_chip[0].did %d!",
		  i2c_chip[0].did);
	  perror("wiringPiI2CSetup");
	  exit(EXIT_FAILURE);
	}
      else printf("I2C Device 0x%02x has i2c_chip[0].fd of %d\n",
		  i2c_chip[0].did,
		  i2c_chip[0].fd);
      i2c_chip[1].did = (int)(keyesRGB.addr);
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
      i2c_chip[0].reg  = (int)(keyesDn.iodir);
      i2c_chip[0].data = ALL_GPIO_BITS_OUT;
      WP_i2cset(i2c_chip[0], xflag);
      i2c_chip[0].reg  = (int)(keyesDn.iopol);
      i2c_chip[0].data = ALL_GPIO_BITS_NORMAL;
      WP_i2cset(i2c_chip[0], xflag);
      i2c_chip[0].reg  = (int)(keyesDn.gpio);
      i2c_chip[0].data = dataDn;
      WP_i2cset(i2c_chip[0], xflag);
      i2c_chip[1].reg  = (int)(keyesRGB.iodir);
      i2c_chip[1].data = ALL_GPIO_BITS_OUT;
      WP_i2cset(i2c_chip[1], xflag);
      i2c_chip[1].reg  = (int)(keyesRGB.iopol);
      i2c_chip[1].data = ALL_GPIO_BITS_NORMAL;
      WP_i2cset(i2c_chip[1], xflag);
      i2c_chip[1].reg  = (int)(keyesRGB.gpio);
      i2c_chip[1].data = dataRGB;
      WP_i2cset(i2c_chip[1], xflag);
   }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  echo_mcp23008_status(keyesDn);
  echo_mcp23008_status(keyesRGB);

  /* Demonstration/Test ... */
  if (i2cmode == SYSMODE)
    {
      switch(demode)
	{
	case C3TST:
	  {
	    dataRGB = dataRGB | CLR3_RGB_ANODES_ON;
	    C2S_i2cset(PI_B2,
		       keyesRGB.addr,
		       keyesRGB.gpio,
		       dataRGB,
		       xflag);
	    sleep(3);
	    dataRGB = dataRGB & CLR3_RGB_ANODES_OFF;
	    C2S_i2cset(PI_B2,
		       keyesRGB.addr,
		       keyesRGB.gpio,
		       dataRGB,
		       xflag);
	    for (i=0; i<12; i++)
	      {
		j = i % 3;
		switch(j)
		  {
		  case 0: dataRGB = dataRGB | CLR3R_ANODE_ON; break;
		  case 1: dataRGB = dataRGB | CLR3G_ANODE_ON; break;
		  case 2: dataRGB = dataRGB | CLR3B_ANODE_ON; break;
		  }
		C2S_i2cset(PI_B2,
			   keyesRGB.addr,
			   keyesRGB.gpio,
			   dataRGB,
			   xflag);
		sleep(2);
		dataRGB = dataRGB & CLR3_RGB_ANODES_OFF;
		C2S_i2cset(PI_B2,
			   keyesRGB.addr,
			   keyesRGB.gpio,
			   dataRGB,
			   xflag);
		sleep(1);
	      }
	    break;
	  }
	case DNTST:
	  {
	    dataRGB = dataRGB & DN_RGB_CATHODES_ON;
	    C2S_i2cset(PI_B2,
		       keyesRGB.addr,
		       keyesRGB.gpio,
		       dataRGB,
		       xflag);
	    dataDn  = ALL_DN_ANODES_ON;
	    C2S_i2cset(PI_B2,
		       keyesDn.addr,
		       keyesDn.gpio,
		       dataDn,
		       xflag);
	    sleep(3);
	    dataDn  = ALL_DN_ANODES_OFF;
	    C2S_i2cset(PI_B2,
		       keyesDn.addr,
		       keyesDn.gpio,
		       dataDn,
		       xflag);
	    dataRGB = dataRGB | DN_RGB_CATHODES_OFF;
	    C2S_i2cset(PI_B2,
		       keyesRGB.addr,
		       keyesRGB.gpio,
		       dataRGB,
		       xflag);
	    dataDn  = ALL_DN_ANODES_ON;
	    C2S_i2cset(PI_B2,
		       keyesDn.addr,
		       keyesDn.gpio,
		       dataDn,
		       xflag);
	    for (i=0; i<12; i++)
	      {
		j = i % 3;
		switch(j)
		  {
		  case 0: dataRGB = dataRGB & DNR_CATHODE_ON; break;
		  case 1: dataRGB = dataRGB & DNG_CATHODE_ON; break;
		  case 2: dataRGB = dataRGB & DNB_CATHODE_ON; break;
		  }
		C2S_i2cset(PI_B2,
			   keyesRGB.addr,
			   keyesRGB.gpio,
			   dataRGB,
			   xflag);
		sleep(2);
		dataRGB = dataRGB | DN_RGB_CATHODES_OFF;
		C2S_i2cset(PI_B2,
			   keyesRGB.addr,
			   keyesRGB.gpio,
			   dataRGB,
			   xflag);
		sleep(1);
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
      /* printf("wiringPiI2C methods not yet implemented!\n"); */
      switch(demode)
	{
	case C3TST:
	  {
	    dataRGB = dataRGB | CLR3_RGB_ANODES_ON;
	    i2c_chip[1].reg  = (int)(keyesRGB.gpio);
	    i2c_chip[1].data = dataRGB;
	    WP_i2cset(i2c_chip[1], xflag);
	    sleep(3);
	    dataRGB = dataRGB & CLR3_RGB_ANODES_OFF;
	    i2c_chip[1].data = dataRGB;
	    WP_i2cset(i2c_chip[1], xflag);
	    break;
	  }
	case DNTST:
	  {
	    dataRGB = dataRGB & DN_RGB_CATHODES_ON;
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
 * $Log: i2c_rgb_led_demo.c,v $
 * Revision 1.4  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.3  2017/08/29 23:16:31  pi
 * Beginning to implement wiringPi methods
 *
 * Revision 1.2  2017/08/29 17:57:47  pi
 * Interim Commit
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.3  2015/07/04 22:32:04  pi
 * Preliminary tests working
 *
 * Revision 1.2  2015/07/04 16:12:04  pi
 * Interim commit
 *
 * Revision 1.1  2015/07/04 15:33:44  pi
 * Initial revision
 *
 *
 */

