/* LCD Hobbytronics I2C/serial backpack DEMOnstration code
 * $Id: lcd_hi2c_demo.c,v 1.7 2018/04/29 23:30:27 pi Exp $
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
#ifdef HAVE_WIRINGPII2C_H
#include <wiringPiI2C.h>
#endif /* HAVE_WIRINGPII2C_H */
#ifdef HAVE_PIGPIO_H
/* #include <pigpio.h> */
#include "pigpio_wraps.h"
#endif /* HAVE_PIGPIO_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "parsel.h"
#include "i2c_chips.h"
#include "i2c_wraps.h"
#include "lcd_disptools.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/04/29 23:30:27 $";
static char RCSFile[]     = "$RCSfile: lcd_hi2c_demo.c,v $";
static char RCSId[]       = "$Id: lcd_hi2c_demo.c,v 1.7 2018/04/29 23:30:27 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision $";
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
  INQUIRE,   /* -I                   */
  LCD0802,   /* -0802                */
  LCD1602,   /* -1602                */
  LCD1604,   /* -1604                */
  LCD2004,   /* -2004                */
  BKLTST,    /* -BLT                 */
  CLRHOME,   /* -CH                  */
  WDTATST,   /* -WDT                 */
  WLINTST,   /* -WLT                 */
  WMSGTST,   /* -WMT                 */
  QUIET,     /* -Q                   */
  SYSMODE,   /* -S                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPII2C_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPII2C_H */
#ifdef HAVE_PIGPIO_H
  PIGMODE,   /* -PG                  */
#endif /* HAVE_PIGPIO_H */
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
  arglist[ECHO]     = "-E";
  arglist[INQUIRE]  = "-I";
  arglist[LCD0802]  = "-0802";
  arglist[LCD1602]  = "-1602";
  arglist[LCD1604]  = "-1604";
  arglist[LCD2004]  = "-2004";
  arglist[BKLTST]   = "-BLT";
  arglist[CLRHOME]  = "-CH";
  arglist[WDTATST]  = "-WDT";
  arglist[WLINTST]  = "-WLT";
  arglist[WMSGTST]  = "-WMT";
  arglist[QUIET]    = "-Q";
  arglist[SYSMODE]  = "-S";
  arglist[VERBOSE]  = "-V";
#ifdef HAVE_WIRINGPII2C_H
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPII2C_H */
#ifdef HAVE_PIGPIO_H
  arglist[PIGMODE]  = "-PG";
#endif /* HAVE_PIGPIO_H */
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
char eflag,iqflag,oflag,vflag,xflag;
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
	    case INQUIRE: iqflag=TRUE;      break;
	    case LCD0802: lcdmodel=LCM0802; break;
	    case LCD1602: lcdmodel=LCM1602; break;
	    case LCD1604: lcdmodel=LCM1604; break;
	    case LCD2004: lcdmodel=LCM2004; break;
	    case CLRHOME: demode=CLRHOME;   break;
	    case BKLTST:  demode=BKLTST;    break;
	    case WDTATST: demode=WDTATST;   break;
	    case WLINTST: demode=WLINTST;   break;
	    case WMSGTST: demode=WMSGTST;   break;
	    case QUIET:   vflag=FALSE;      break;
	    case SYSMODE: i2cmode=SYSMODE;  break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE: i2cmode=WPIMODE; break;
#endif /* HAVE_WIRINGPII2C_H */
#ifdef HAVE_PIGPIO_H
	    case PIGMODE: i2cmode=PIGMODE; break;
#endif /* HAVE_PIGPIO_H */
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
  strcat(buffer1, "         -E    {Echo command-line arguments},\n");
  strcat(buffer1, "         -I    {Inquire about i2c devices},\n");
  strcat(buffer1, "         -0802 {LCD display type 0802},\n");
  strcat(buffer1, "         -1602 {LCD display type 1602 - default},\n");
  strcat(buffer1, "         -1604 {LCD display type 1604},\n");
  strcat(buffer1, "         -2004 {LCD display type 2004},\n");
  strcat(buffer1, "         -BLT  {BackLight Test},\n");
  strcat(buffer1, "         -CH   {Clear display & return Home},\n");
  strcat(buffer1, "         -WDT  {Write Data Test},\n");
  strcat(buffer1, "         -WLT  {Write Line info. Test},\n");
  strcat(buffer1, "         -WMT  {Write Message Test},\n");
  strcat(buffer1, "         -Q    {Quiet - default},\n");
  strcat(buffer1, "         -S    {use System call methods - default},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -W    {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
#ifdef HAVE_PIGPIO_H
  strcat(buffer1, "         -PG   {use PiGpio i2c methods**},\n");
  strcat(buffer1, "               {**NB: requires sudo \"prefix\"! },\n");
#endif /* HAVE_PIGPIO_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char dateandtimeis[DATEANDTIME_BUF_LEN],timenowis[TIMENOW_BUF_LEN];
  int i,j,k,l,mca;
  int ncol,nrow;
  int data[32];
  lcdbpv2 silbhem;
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPII2C_H
  wpi2cvb i2c_chip[128];
#endif /* HAVE_WIRINGPII2C_H */
#ifdef HAVE_PIGPIO_H
  int pigpiovsn;
  pgi2cvb pgi2c;
#endif /* HAVE_PIGPIO_H */

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
      exit(EXIT_SUCCESS);
    }

  /* Set up defaults */
  eflag     = vflag = xflag = FALSE;
  demode    = LAST_ARG;
  i2cmode   = SYSMODE;
  lcdmodel  = LCM1602;
#ifdef HAVE_PIGPIO_H
  pigpiovsn    = -1; /* Not yet determined */
  pgi2c.handle = PG_UNDEF_HANDLE;
#endif /* HAVE_PIGPIO_H */
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
	    case ECHO:    eflag=TRUE;       i--; break;
	    case INQUIRE: iqflag=TRUE;      i--; break;
	    case LCD0802: lcdmodel=LCM0802; i--; break;
	    case LCD1602: lcdmodel=LCM1602; i--; break;
	    case LCD1604: lcdmodel=LCM1604; i--; break;
	    case LCD2004: lcdmodel=LCM2004; i--; break;
	    case BKLTST:  demode=BKLTST;    i--; break;
	    case CLRHOME: demode=CLRHOME;   i--; break;
	    case WDTATST: demode=WDTATST;   i--; break;
	    case WLINTST: demode=WLINTST;   i--; break;
	    case WMSGTST: demode=WMSGTST;   i--; break;
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;     i--; break;
	    case SYSMODE: i2cmode=SYSMODE; i--; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE: i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
#ifdef HAVE_PIGPIO_H
	    case PIGMODE: i2cmode=PIGMODE; i--; break;
#endif /* HAVE_PIGPIO_H */
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
	  mca = init_lcdbpv2(&silbhem, 0x3a, "silbhem");
	  echo_lcdbpv2_status(silbhem);
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  mca = init_lcdbpv2(&silbhem, 0x3a, "silbhem");
  echo_lcdbpv2_status(silbhem);
  switch (lcdmodel)
    {
    case LCM0802:
      {
	/* Set Display Type (8 cols x 2 rows) */
	ncol = 8;
	nrow = 2;
	break;
      }
    case LCM1604:
      {
	/* Set Display Type (16 cols x 4 rows) */
	ncol = 16;
	nrow = 4;
	break;
      }
    case LCM2004:
      {
	/* Set Display Type (20 cols x 4 rows) */
	ncol = 20;
	nrow = 4;
	break;
      }
    case LCM1602:
    default:
      {
	/* Set Display Type (16 cols x 2 rows) */
	ncol = 16;
	nrow = 2;
	break;
      }
    }

  if (demode == WMSGTST)
    {
      switch (lcdmodel)
	{
	case LCM0802:
	  {
	    strcpy(buffer4, "LCM0802.");
	    message_buffer[8] = NUL;
	    strcat(buffer4, message_buffer);
	    break;
	  }
	case LCM1604:
	  {
	    strcpy(buffer4, "LCDBPv2 LCM1604.");
	    message_buffer[16] = NUL;
	    strcat(buffer4, message_buffer);
	    break;
	  }
	case LCM2004:
	  {
	    strcpy(buffer4, "LCDBPv2 LCM2004.....");
	    message_buffer[20] = NUL;
	    strcat(buffer4, message_buffer);
	    break;
	  }
	case LCM1602:
	default:
	  {
	    strcpy(buffer4, "LCDBPv2 LCM1602.");
	    message_buffer[16] = NUL;
	    strcat(buffer4, message_buffer);
	  }
	}
      if ((lcdmodel == LCM2004) || (lcdmodel == LCM1604))
	{
	  k = ncol * 2;
	  while (strlen(buffer4) < k) strcat(buffer4, " ");
	  get_date(message_buffer);
	  if (lcdmodel == LCM1604) message_buffer[16] = NUL;
	  else message_buffer[20] = NUL;
	  strcat(buffer4, message_buffer);
	  k = ncol * 3;
	  while (strlen(buffer4) < k) strcat(buffer4, " ");
	  get_timenow(timenowis);
	  strcpy(message_buffer, timenowis);
	  if (lcdmodel == LCM1604) message_buffer[16] = NUL;
	  else message_buffer[20] = NUL;
	  strcat(buffer4, message_buffer);
	}
      printf("%s\n", buffer4); 
    }
		
  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      switch(demode)
	{
	case BKLTST:
	case WDTATST:
	case WLINTST:
	case WMSGTST:
	  {
	    /* Turn backlight 30% on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       75,
		       xflag);
	    /* Set Display Type */
	    data[0] = nrow;
	    data[1] = ncol;
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
	    sleep(1);
	    /* Turn backlight 60% on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       150,
		       xflag);
	    break;
	  }
	default: 
	  {
	    /* Turn backlight Full-on */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.backlight,
		       250,
		       xflag);	
	    data[0] = nrow;
	    data[1] = ncol;
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
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      printf("Using wiringPiI2C methods: ...\n");
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
      switch(demode)
	{
	case BKLTST:
	case WDTATST:
	case WLINTST:
	case WMSGTST:
	  {
	    /* Turn backlight 30% on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 75;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Set Display Type */
	    i2c_chip[0].reg  = (int)(silbhem.lcdtype);
	    i2c_chip[0].data = ncol;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + nrow; 
	    WP_i2cset2(i2c_chip[0], xflag);
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    delay(500);
	    /* Turn backlight 60% on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 150;
	    WP_i2cset(i2c_chip[0], xflag);
	  }
	default:
	  {
	    /* Turn backlight Full-on */
	    i2c_chip[0].reg  = (int)(silbhem.backlight);
	    i2c_chip[0].data = 250;
	    WP_i2cset(i2c_chip[0], xflag);
	    /* Set Display Type */
	    i2c_chip[0].reg  = (int)(silbhem.lcdtype);
	    i2c_chip[0].data = ncol;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + nrow; 
	    WP_i2cset2(i2c_chip[0], xflag);
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    delay(500);
	    /* Cursor on and blinking */
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_DISPLAY_ON_CURSOR_BLINK;
	    WP_i2cset(i2c_chip[0], xflag);
	    break;
	  }
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_PIGPIO_H
  if (i2cmode == PIGMODE)
    {
      printf("Using pigpio I2C methods:\n");
      if ((pigpiovsn = gpioInitialise()) < 0)
	{
	  fprintf(stderr, "Could not initialise pigpio!");
	  perror("gpioInitialise");
	  exit(EXIT_FAILURE);
	}
      else printf("pigpio Version %d initialised ...\n", pigpiovsn);
      pgi2c.i2cBus   = PI_B2;
      pgi2c.i2cAddr  = silbhem.addr;
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

      switch(demode)
	{
	case BKLTST:
	case WDTATST:
	case WLINTST:
	case WMSGTST:
	  {
	    /* Turn backlight 30% on */
	    pgi2c.i2cReg = silbhem.backlight;
	    pgi2c.bVal   = 75;
	    PG_i2cset(pgi2c, xflag);
	    delay(500);
	    /* Set Display Type */
	    pgi2c.i2cReg = silbhem.lcdtype;
	    pgi2c.wVal   = ncol;
	    pgi2c.wVal   = (pgi2c.wVal * 0x100) + nrow; 
	    PG_i2cset2(pgi2c, xflag);
	    /* Clear display */
	    pgi2c.bVal = silbhem.cleardisp;
	    PG_i2cout(pgi2c, xflag);
	    delay(500);
	    /* Turn backlight 60% on */
	    pgi2c.i2cReg = silbhem.backlight;
	    pgi2c.bVal   = 150;
	    PG_i2cset(pgi2c, xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_PIGPIO_H */

  /* Demonstration/Test */

  if (i2cmode == SYSMODE)
    {
       switch(demode)
	{
	case BKLTST:
	  {
	    for (i=0; i<300; i=i+50)
	      {
		sleep(1);
		C2S_i2cset(PI_B2,
			   silbhem.addr,
			   silbhem.backlight,
			   i,
			   xflag);
	      }
	    sleep(2);
	    for (i=200; i>=0; i=i-50)
	      {
		C2S_i2cset(PI_B2,
			   silbhem.addr,
			   silbhem.backlight,
			   i,
			   xflag);
		sleep(1);
	      }
	    break;
	  }
	case WDTATST:
	  {
	    C2S_i2cout(PI_B2,
		       silbhem.addr,
		       silbhem.cleardisp,
		       xflag);
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.hd44780,
		       LCD_RETURN_HOME,
		       xflag);
	    sleep(2);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		data[0] = toascii(j);
		C2S_i2cset(PI_B2,
			   silbhem.addr,
			   silbhem.writechar,
			   data[0],
			   xflag);
		sleep(1);
		if (i == ncol) C2S_i2cset(PI_B2,
					  silbhem.addr,
					  silbhem.hd44780,
					  LCD_CURSOR_UP_SHIFTED,
					  xflag);
		if (i == 25) C2S_i2cset(PI_B2,
					  silbhem.addr,
					  silbhem.hd44780,
					  LCD_DISPLAY_SHIFT_LEFT,
					  xflag);
	      }
	    sleep(1);
	    k = (26 - ncol);
	    for (i=0; i<k; i++)C2S_i2cset(PI_B2,
					  silbhem.addr,
					  silbhem.hd44780,
					  LCD_DISPLAY_SHIFT_RIGHT,
					  xflag);
	    data[0] = 2;
	    data[1] = 1;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.cursorposn,
			data,
			2,
			xflag);
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.hd44780,
		       LCD_CURSOR_UP_NO_SHIFT,
		       xflag);
	    sleep(1);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		data[0] = toascii(j);
		C2S_i2cset(PI_B2,
			   silbhem.addr,
			   silbhem.writechar,
			   data[0],
			   xflag);
		sleep(1);
		if (i == ncol) C2S_i2cset(PI_B2,
					  silbhem.addr,
					  silbhem.hd44780,
					  LCD_CURSOR_UP_SHIFTED,
					  xflag);
		if (i == 25) C2S_i2cset(PI_B2,
					  silbhem.addr,
					  silbhem.hd44780,
					  LCD_DISPLAY_SHIFT_LEFT,
					  xflag);
	      }
	    sleep(1);
	    k = (26 - ncol);
	    for (i=0; i<k; i++)C2S_i2cset(PI_B2,
					  silbhem.addr,
					  silbhem.hd44780,
					  LCD_DISPLAY_SHIFT_RIGHT,
					  xflag);
	    sleep(1);
	    data[0] = 1;
	    data[1] = 1;
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.cursorposn,
			data,
			2,
			xflag);
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.hd44780,
		       LCD_CURSOR_UP_NO_SHIFT,
		       xflag);
	    sleep(1);
	    break;
	  }
	case WLINTST:
	  {
	    switch(lcdmodel)
	      {
	      case LCM0802:
		{
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     1,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL1 #1");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     2,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL2 #2");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  break;
		}
	      case LCM1604:
		{
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     1,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL1 LCM1604 #1");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     2,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL2 LCM1604 #2");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     3,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL3 LCM1604 #3");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     4,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL4 LCM1604 #4");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  break;
		}
	      case LCM2004:
		{
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     1,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL1 LCM2004 #1");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     2,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL2 LCM2004 #2");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     3,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL3 LCM2004 #3");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     4,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL4 LCM2004 #4");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     1,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL1 LCM1602 #1");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  C2S_i2cset(PI_B2,
			     silbhem.addr,
			     silbhem.clearline,
			     2,
			     xflag);
		  sleep(1);
		  strcpy(buffer4, "CL2 LCM1602 #2");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  C2S_i2cseti(PI_B2,
			      silbhem.addr,
			      silbhem.printstr,
			      data,
			      j,
			      xflag);
		  break;
		}
	      }
	    break;
	  }
	case WMSGTST:
	  {
	    /* Line 1 */
	    j = strlen(buffer4);
	    if (j > ncol) k = ncol;
	    else k = j;
	    for (i=0; i<k; i++) data[i] = toascii((int)(buffer4[i]));
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.printstr,
			data,
			j,
			xflag);
	    if (j <= ncol) break;
	    if (nrow < 2) break;
	    /* Line 2 */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.clearline,
		       2,
		       xflag);
	    j = strlen(buffer4) - ncol;
	    if (j > ncol) k = ncol;
	    else k = j + ncol;
	    for (i=0; i<k; i++) data[i] = toascii((int)(buffer4[i+ncol]));
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.printstr,
			data,
			j,
			xflag);
	    if (j <= ncol) break;
	    if (nrow < 4) break;
	    /* Line 3 */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.clearline,
		       3,
		       xflag);
	    l = 2 * ncol;
	    j = strlen(buffer4) - l;
	    if (j > ncol) k = l;
	    else k = j + l;
	    for (i=0; i<k; i++) data[i] = toascii((int)(buffer4[i+l]));
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.printstr,
			data,
			j,
			xflag);
	    /* Line 4 */
	    C2S_i2cset(PI_B2,
		       silbhem.addr,
		       silbhem.clearline,
		       4,
		       xflag);
	    l = 3 * ncol;
	    j = strlen(buffer4) - l;
	    if (j > ncol) k = l;
	    else k = j + l;
	    for (i=0; i<k; i++) data[i] = toascii((int)(buffer4[i+l]));
	    C2S_i2cseti(PI_B2,
			silbhem.addr,
			silbhem.printstr,
			data,
			j,
			xflag);
	    break;
	  }
	case CLRHOME: /* Do nothing as this is done earlier! */
	default: break;
	}
   }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
       switch(demode)
	{
	case BKLTST:
	  {
	    for (i=0; i<255; i=i+5)
	      {
		delay(100);
		i2c_chip[0].reg  = (int)(silbhem.backlight);
		i2c_chip[0].data = i;
		WP_i2cset(i2c_chip[0], xflag);
	      }
	    delay(1100);
	    for (i=250; i>=0; i=i-5)
	      {
		i2c_chip[0].reg  = (int)(silbhem.backlight);
		i2c_chip[0].data = i;
		WP_i2cset(i2c_chip[0], xflag);
		delay(100);
	      }
	    break;
	  }
	case WDTATST:
	  {
	    /* Clear display */
	    i2c_chip[0].data = silbhem.cleardisp;
	    WP_i2cout(i2c_chip[0], xflag);
	    /* Return Home */
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_RETURN_HOME;
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(1);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		i2c_chip[0].reg  = (int)(silbhem.writechar);
		i2c_chip[0].data = toascii(j);
		WP_i2cset(i2c_chip[0], xflag);
		delay(500);
		if (i == ncol)
		  {
		    i2c_chip[0].reg  = (int)(silbhem.hd44780);
		    i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED;
		    WP_i2cset(i2c_chip[0], xflag);
		  }
		if (i == 25)
		  {
		    i2c_chip[0].reg  = (int)(silbhem.hd44780);
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT;
		    WP_i2cset(i2c_chip[0], xflag);
		  }
	      }
	    sleep(1);
	    k = (26 - ncol);
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_DISPLAY_SHIFT_RIGHT;
	    for (i=0; i<k; i++)
	      {
		WP_i2cset(i2c_chip[0], xflag);
		delay(1);
	      }
	    i2c_chip[0].reg  = (int)(silbhem.cursorposn);
	    i2c_chip[0].data = 1;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + 2; 
	    WP_i2cset2(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT;
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(2);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		i2c_chip[0].reg  = (int)(silbhem.writechar);
		i2c_chip[0].data = toascii(j);
		WP_i2cset(i2c_chip[0], xflag);
		delay(500);
		if (i == ncol)
		  {
		    i2c_chip[0].reg  = (int)(silbhem.hd44780);
		    i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED;
		    WP_i2cset(i2c_chip[0], xflag);
		  }
		if (i == 25)
		  {
		    i2c_chip[0].reg  = (int)(silbhem.hd44780);
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT;
		    WP_i2cset(i2c_chip[0], xflag);
		  }
	      }
	    sleep(1);
	    k = (26 - ncol);
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_DISPLAY_SHIFT_RIGHT;
	    for (i=0; i<k; i++) 
	      {
		WP_i2cset(i2c_chip[0], xflag);
		delay(1);
	      }
	    i2c_chip[0].reg  = (int)(silbhem.cursorposn);
	    i2c_chip[0].data = 1;
	    i2c_chip[0].data = (i2c_chip[0].data * 0x100) + 1; 
	    WP_i2cset2(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(silbhem.hd44780);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT;
	    WP_i2cset(i2c_chip[0], xflag);
	    sleep(1);
	    break;
	  }
	case WLINTST:
	  {
	    switch(lcdmodel)
	      {
	      case LCM0802:
		{
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 1;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL1 #1");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++) 
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 2;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL2 #2");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  break;
		}
	      case LCM1604:
		{
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 1;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL1 LCM1604 #1");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 2;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL2 LCM1604 #2");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 3;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL3 LCM1604 #3");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 4;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL4 LCM1604 #4");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  break;
		}
	      case LCM2004:
		{
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 1;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL1 LCM2004 #1");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 2;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL2 LCM2004 #2");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 3;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL3 LCM2004 #3");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 4;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL4 LCM2004 #4");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  break;
		}
	      case LCM1602:
	      default:
		{
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 1;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL1 LCM1602 #1");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  i2c_chip[0].reg  = (int)(silbhem.clearline);
		  i2c_chip[0].data = 2;
		  WP_i2cset(i2c_chip[0], xflag);
		  delay(500);
		  strcpy(buffer4, "CL2 LCM1602 #2");
		  j = strlen(buffer4);
		  for (i=0; i<j; i++) data[i] = toascii((int)(buffer4[i]));
		  /* Emulate print string */
		  i2c_chip[0].reg  = (int)(silbhem.writechar);
		  for (i=0; i<j; i++)
		    {
		      i2c_chip[0].data = data[i];
		      WP_i2cset(i2c_chip[0], xflag);
		      delay(j);
		    }
		  break;
		}
	      }
	    break;
	  }
	case WMSGTST:
	  {
	    /* Line 1 */
	    j = strlen(buffer4);
	    if (j > ncol) k = ncol;
	    else k = j;
	    for (i=0; i<k; i++) data[i] = toascii((int)(buffer4[i]));
	    /* Emulate print string */
	    i2c_chip[0].reg  = (int)(silbhem.writechar);
	    for (i=0; i<k; i++)
	      {
		i2c_chip[0].data = data[i];
		WP_i2cset(i2c_chip[0], xflag);
	      }
	    break;
	  }
	case CLRHOME: /* Do nothing as this is done earlier! */
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
	case BKLTST:
	  {
	    for (i=0; i<255; i=i+5)
	      {
		delay(100);
		pgi2c.i2cReg = silbhem.backlight;
		pgi2c.bVal   = i;
		PG_i2cset(pgi2c, xflag);
	      }
	    delay(1100);
	    for (i=250; i>=0; i=i-5)
	      {
		pgi2c.i2cReg = silbhem.backlight;
		pgi2c.bVal   = i;
		PG_i2cset(pgi2c, xflag);
		delay(100);
	      }
	    break;
	  }
	case WDTATST:
	  {
	    /* Clear display */
	    pgi2c.bVal = silbhem.cleardisp;
	    PG_i2cout(pgi2c, xflag);
	    /* Return Home */
	    pgi2c.i2cReg = silbhem.hd44780;
	    pgi2c.bVal   = LCD_RETURN_HOME;
	    PG_i2cset(pgi2c, xflag);
	    sleep(1);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		pgi2c.i2cReg = silbhem.writechar;
		pgi2c.bVal   = toascii(j);
		PG_i2cset(pgi2c, xflag);
		delay(500);
		if (i == ncol)
		  {
		    pgi2c.i2cReg = silbhem.hd44780;
		    pgi2c.bVal   = LCD_CURSOR_UP_SHIFTED;
		    PG_i2cset(pgi2c, xflag);
		  }
		if (i == 25)
		  {
		    pgi2c.i2cReg = silbhem.hd44780;
		    pgi2c.bVal   = LCD_DISPLAY_SHIFT_LEFT;
		    PG_i2cset(pgi2c, xflag);
		  }
	      }
	    sleep(1);
	    k = (26 - ncol);
	    pgi2c.i2cReg = silbhem.hd44780;
	    pgi2c.bVal   = LCD_DISPLAY_SHIFT_RIGHT;
	    for (i=0; i<k; i++)
	      {
		PG_i2cset(pgi2c, xflag);
		delay(1);
	      }
	    pgi2c.i2cReg = silbhem.cursorposn;
	    pgi2c.wVal   = 1;
	    pgi2c.wVal   = (pgi2c.wVal * 0x100) + 2; 
	    PG_i2cset2(pgi2c, xflag);
	    pgi2c.i2cReg = silbhem.hd44780;
	    pgi2c.bVal   = LCD_CURSOR_UP_NO_SHIFT;
	    PG_i2cset(pgi2c, xflag);
	    sleep(2);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		pgi2c.i2cReg = silbhem.writechar;
		pgi2c.bVal   = toascii(j);
		PG_i2cset(pgi2c, xflag);
		delay(500);
		if (i == ncol)
		  {
		    pgi2c.i2cReg = silbhem.hd44780;
		    pgi2c.bVal   = LCD_CURSOR_UP_SHIFTED;
		    PG_i2cset(pgi2c, xflag);
		  }
		if (i == 25)
		  {
		    pgi2c.i2cReg = silbhem.hd44780;
		    pgi2c.bVal   = LCD_DISPLAY_SHIFT_LEFT;
		    PG_i2cset(pgi2c, xflag);
		  }
	      }
	    sleep(1);
	    k = (26 - ncol);
	    pgi2c.i2cReg = silbhem.hd44780;
	    pgi2c.bVal   = LCD_DISPLAY_SHIFT_RIGHT;
	    for (i=0; i<k; i++)
	      {
		PG_i2cset(pgi2c, xflag);
		delay(1);
	      }
	    pgi2c.i2cReg = silbhem.cursorposn;
	    pgi2c.wVal   = 1;
	    pgi2c.wVal   = (pgi2c.wVal * 0x100) + 1; 
	    PG_i2cset2(pgi2c, xflag);
	    pgi2c.i2cReg = silbhem.hd44780;
	    pgi2c.bVal   = LCD_CURSOR_UP_NO_SHIFT;
	    PG_i2cset(pgi2c, xflag);
	    sleep(1);
	    break;
	  }
	case WLINTST:
	  {
	    switch(lcdmodel)
	      {
	      case LCM1602:
	      default:
		{
		  pgi2c.i2cReg = silbhem.clearline;
		  pgi2c.bVal   = 1;
		  PG_i2cset(pgi2c, xflag);
		  delay(500);
		  strcpy(buffer4, "CL1 LCM1602 #1");
		  j = strlen(buffer4);
		  if (j > PG_BLOCK_SIZE) j = PG_BLOCK_SIZE;
		  for (i=0; i<j; i++) pgi2c.block_buf[i] = toascii((int)(buffer4[i]));
		  pgi2c.i2cReg = silbhem.printstr;
		  PG_i2cseti(pgi2c, (unsigned int)(j), xflag);
		  delay(500);
		  pgi2c.i2cReg = silbhem.clearline;
		  pgi2c.bVal   = 2;
		  PG_i2cset(pgi2c, xflag);
		  delay(500);
		  strcpy(buffer4, "CL2 LCM1602 #2");
		  j = strlen(buffer4);
		  if (j > PG_BLOCK_SIZE) j = PG_BLOCK_SIZE;
		  for (i=0; i<j; i++) pgi2c.block_buf[i] = toascii((int)(buffer4[i]));
		  pgi2c.i2cReg = silbhem.printstr;
		  PG_i2cseti(pgi2c, (unsigned int)(j), xflag);
		  break;
		}
	      }
	    break;
	  }
	case CLRHOME: /* Do nothing as this is done earlier! */
	default: break;
	}
    }
#endif /* HAVE_PIGPIO_H */

  /* Tidy up ... */
  if (i2cmode == SYSMODE)
    {
      /* Return Home */
      C2S_i2cset(PI_B2,
		 silbhem.addr,
		 silbhem.hd44780,
		 LCD_RETURN_HOME,
		 xflag);
      sleep(1);
      /* Turn backlight 60% on */
      C2S_i2cset(PI_B2,
		 silbhem.addr,
		 silbhem.backlight,
		 150,
		 xflag);
      sleep(2);
      /* Cursor off */
      C2S_i2cset(PI_B2,
		 silbhem.addr,
		 silbhem.hd44780,
		 LCD_DISPLAY_ON_NO_CURSOR,
		 xflag);
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      /* Return Home */
      i2c_chip[0].reg  = (int)(silbhem.hd44780);
      i2c_chip[0].data = LCD_RETURN_HOME;
      WP_i2cset(i2c_chip[0], xflag);
      delay(500);
      /* Turn backlight 60%-on */
      i2c_chip[0].reg  = (int)(silbhem.backlight);
      i2c_chip[0].data = 150;
      WP_i2cset(i2c_chip[0], xflag);
      sleep(2);
      /* Cursor off */
      i2c_chip[0].reg  = (int)(silbhem.hd44780);
      i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR;
      WP_i2cset(i2c_chip[0], xflag);
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_PIGPIO_H
  if (i2cmode == PIGMODE)
    {
      /* Return Home */
      pgi2c.i2cReg = silbhem.hd44780;
      pgi2c.bVal   = LCD_RETURN_HOME;
      PG_i2cset(pgi2c, xflag);
      delay(500);
      /* Turn backlight 60%-on */
      pgi2c.i2cReg = silbhem.backlight;
      pgi2c.bVal   = 150;
      PG_i2cset(pgi2c, xflag);
      sleep(2);
      /* Cursor off */
      pgi2c.i2cReg = silbhem.hd44780;
      pgi2c.bVal   = LCD_DISPLAY_ON_NO_CURSOR;
      PG_i2cset(pgi2c, xflag);
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

  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: lcd_hi2c_demo.c,v $
 * Revision 1.7  2018/04/29 23:30:27  pi
 * Interim commit - -W with -CH & -WMT issues
 *
 * Revision 1.6  2018/04/29 22:16:40  pi
 * -CH and -WMT options implemented (latter for -S mode only)
 *
 * Revision 1.5  2018/04/29 17:13:31  pi
 * Interim commit
 *
 * Revision 1.4  2018/04/29 15:02:36  pi
 * Interim commit
 *
 * Revision 1.3  2018/04/25 21:29:34  pi
 * Beginning to add -1604 support
 *
 * Revision 1.2  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.9  2015/04/27 21:21:30  pi
 * Interim commit
 *
 * Revision 1.8  2015/04/26 19:39:01  pi
 * Interim commit
 *
 * Revision 1.7  2015/04/26 19:11:22  pi
 * Interim commit
 *
 * Revision 1.6  2015/04/25 16:47:05  pi
 * Interim commit
 *
 * Revision 1.5  2015/04/24 21:20:08  pi
 * Interim commit
 *
 * Revision 1.4  2015/04/24 15:41:08  pi
 * Beginning to try out pigpio methods
 *
 * Revision 1.3  2015/04/23 14:46:14  pi
 * SCMVersion 0.01
 *
 * Revision 1.2  2015/04/21 15:21:12  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:13:46  pi
 * Initial revision
 *
 * Revision 1.2  2014/06/02 22:09:21  pi
 * Minor tidy up
 *
 * Revision 1.1  2014/05/26 21:45:35  pi
 * Initial revision
 *
 * Revision 1.11  2014/05/24 17:06:37  pi
 * Preliminary LCM0802 support
 *
 * Revision 1.10  2014/04/29 21:17:35  pi
 * -WDT option now working with -W
 *
 * Revision 1.9  2014/04/29 20:25:29  pi
 * Interim commit
 *
 * Revision 1.8  2014/04/25 21:17:33  pi
 * -WDT option with -S now appears to be working as required
 *
 * Revision 1.7  2014/04/23 21:05:56  pi
 * -WDT option partially implemented
 *
 * Revision 1.6  2014/04/22 20:45:45  pi
 * Interim commit
 *
 * Revision 1.5  2014/04/22 20:27:32  pi
 * Interim commit
 *
 * Revision 1.4  2014/04/22 19:55:22  pi
 * Interim commit
 *
 * Revision 1.3  2014/04/22 14:30:30  pi
 * Interim commit
 *
 * Revision 1.2  2014/04/22 13:44:07  pi
 * Interim commit
 *
 * Revision 1.1  2014/04/22 13:13:42  pi
 * Initial revision
 *
 *
 */
