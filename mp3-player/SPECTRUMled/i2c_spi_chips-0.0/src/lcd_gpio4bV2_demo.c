/* LCD GPIO 4-Bit DEMOnstration code Version 2 (similar to that in wiringPi)
 * $Id: lcd_gpio4bV2_demo.c,v 1.8 2018/03/31 21:32:45 pi Exp $
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
#include <unistd.h>
#include <sys/utsname.h>
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_WIRINGPI_H
#include <wiringPi.h>
#endif /* HAVE_WIRINGPI_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "parsel.h"
#include "i2c_wraps.h"
#include "lcd_disptools.h"
/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: lcd_gpio4bV2_demo.c,v $";
static char RCSId[]       = "$Id: lcd_gpio4bV2_demo.c,v 1.8 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.8 $";
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
  LCD0802,   /* -0802                */
#ifdef HAVE_WIRINGPI_H
  CLRHOME,   /* -CH                  */
  FSI4TST,   /* -F4T                 */
  INITGPIO,  /* -IG                  */
  PRELOAD,   /* -P                   */
  STRBTST,   /* -ST                  */
  WDTATST,   /* -WDT                 */
  WLINTST,   /* -WLT                 */
  WMSGTST,   /* -WMT                 */
  WRGRTST,   /* -WRT                 */
#endif /* HAVE_WIRINGPI_H */
  QUIET,     /* -Q                   */
  SYSMODE,   /* -S                   */
  TSTMODE,   /* -T                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPI_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,gpiomode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[LCD0802]  = "-0802";
#ifdef HAVE_WIRINGPI_H
  arglist[CLRHOME]  = "-CH";
  arglist[FSI4TST]  = "-F4T";
  arglist[INITGPIO] = "-IG";
  arglist[PRELOAD]  = "-P";
  arglist[STRBTST]  = "-ST";
  arglist[WDTATST]  = "-WDT";
  arglist[WLINTST]  = "-WLT";
  arglist[WMSGTST]  = "-WMT";
  arglist[WRGRTST]  = "-WRT";
#endif /* HAVE_WIRINGPI_H */
  arglist[QUIET]    = "-Q";
  arglist[SYSMODE]  = "-S";
  arglist[TSTMODE]  = "-T";
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
char eflag,igflag,oflag,tflag,vflag,xflag;
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
	    case LCD0802: lcdmodel=LCM0802; break;
#ifdef HAVE_WIRINGPI_H
	    case CLRHOME: demode=CLRHOME;  gpiomode=WPIMODE; break;
	    case FSI4TST: demode=FSI4TST;  gpiomode=WPIMODE; break;
	    case INITGPIO: igflag=TRUE;    break;
	    case PRELOAD: demode=PRELOAD;  break;
	    case STRBTST: demode=STRBTST;  break;
	    case WDTATST: demode=WDTATST;  break;
	    case WLINTST: demode=WLINTST;  gpiomode=WPIMODE; break;
	    case WMSGTST: demode=WMSGTST;  gpiomode=WPIMODE; break;
	    case WRGRTST: demode=WRGRTST;  break;
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;      break;
	    case SYSMODE: gpiomode=SYSMODE; xflag=FALSE; break;
	    case TSTMODE: tflag=TRUE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPI_H
	    case WPIMODE: gpiomode=WPIMODE; break;
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
  strcat(buffer1, "         -E    {Echo command-line arguments},\n");
  strcat(buffer1, "         -0802 {LCD display type 0802} - default,\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -CH   {Clear display & return Home - implies -W*},\n");
  strcat(buffer1, "         -F4T  {Func set init 4-bit Test - implies -W*},\n");
  strcat(buffer1, "         -IG   {Initialise used gpio's & Gpio readall},\n");
  strcat(buffer1, "         -P    {Preload lcd iface},\n");
  strcat(buffer1, "         -ST   {Strobe Test},\n");
  strcat(buffer1, "         -WDT  {Write Data Test},\n");
  strcat(buffer1, "         -WLT  {Write Line info. Test - implies -W*},\n");
  strcat(buffer1, "         -WMT  {Write Message Test - implies -W*},\n");
  strcat(buffer1, "         -WRT  {Write Register Test*},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q    {Quiet - default},\n");
  strcat(buffer1, "         -S    {use System call methods - default, diagnostics only!},\n");
  strcat(buffer1, "         -T    {using Test mode timings (10x slower)},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -W    {use Wiring pi \"byte write\" method**},\n");
  strcat(buffer1, "               {*NB: requires sudo \"prefix\" with -X!},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X    {eXecute demonstration (-IG, -P or -W active)}.\n");
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
void C2S_LcdG4V2Strobe_Test(char qx)
{
  unsigned int lG4data;
  lG4data = current_gpio_4bitV2.lcd_G4data;
  current_gpio_4bitV2.lcd_G4data = lG4data | GPIO4BITV2_LCD_E_HIGH;
  C2S_gpio_wb(current_gpio_4bitV2.lcd_G4data, xflag);
  sleep(1);
  current_gpio_4bit.lcd_G4data = lG4data & GPIO4BIT_LCD_E_LOW;
  C2S_gpio_wb(current_gpio_4bit.lcd_G4data, xflag);
  current_gpio_4bit.lcd_G4data = lG4data;
  sleep(1);
}

void C2S_LcdG4V2WriteRegister_Test(unsigned int regh,
				   unsigned int regl,
				   char qx)
{
  current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK + regh;
  C2S_LcdG4V2Strobe_Test(qx);
  current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK + regl;
  C2S_LcdG4V2Strobe_Test(qx);
}

void C2S_LcdG4V2WriteData_Test(unsigned int data,
			       char qx)
{
  unsigned int datah,datal;
  datah = data & 0xf0;
  datal = data & 0x0f;
  current_gpio_4bitV2.lcd_G4data = GPIO4BITV2_LCD_DMASK + (datah / 0x10);
  C2S_LcdG4V2Strobe_Test(qx);
  current_gpio_4bitV2.lcd_G4data = GPIO4BITV2_LCD_DMASK + datal;
  C2S_LcdG4V2Strobe_Test(qx);
}

void WP_LcdG4V2Strobe(lcd_gpio_4bitV2 *any_gpio_4bitV2,
		      char qx)
{
  unsigned int lG4data,lwait;
  char uorm;
  uorm = TRUE;
  lG4data = (*any_gpio_4bitV2).lcd_G4data;
  if ((*any_gpio_4bitV2).G4bit.lcd_wait > (*any_gpio_4bitV2).G4bit.lcd_elmin)
    lwait = (*any_gpio_4bitV2).G4bit.lcd_wait;
  else lwait = (*any_gpio_4bitV2).G4bit.lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  (*any_gpio_4bitV2).lcd_G4data = lG4data | GPIO4BITV2_LCD_E_HIGH;
  WP_digitalWriteByte((*any_gpio_4bitV2).lcd_G4data, qx);
  delayMicroseconds((*any_gpio_4bitV2).G4bit.lcd_ehmin);
  (*any_gpio_4bitV2).lcd_G4data = lG4data & GPIO4BITV2_LCD_E_LOW;
  WP_digitalWriteByte((*any_gpio_4bitV2).lcd_G4data, qx);
  (*any_gpio_4bitV2).lcd_G4data = lG4data;
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_LcdG4V2F4TStrobe(lcd_gpio_4bitV2 *any_gpio_4bitV2,
			 int instance,
			 char qx)
{
  unsigned int lG4data,lwait;
  char uorm;
  if (instance > 3) return;
  uorm = TRUE;
  lG4data = (*any_gpio_4bitV2).lcd_G4data;
  if ((*any_gpio_4bitV2).G4bit.lcd_wait > (*any_gpio_4bitV2).G4bit.lcd_elmin)
    lwait = (*any_gpio_4bitV2).G4bit.lcd_wait;
  else lwait = (*any_gpio_4bitV2).G4bit.lcd_elmin;
  (*any_gpio_4bitV2).lcd_G4data = GPIO4BITV2_LCD_RMASK + LCD_FUNC_SET_4BIT_INIT;
  if (instance > 2) (*any_gpio_4bitV2).lcd_G4data = GPIO4BITV2_LCD_RMASK +
		      LCD_FUNC_SET_4BIT_NF28_H;  
  if (instance == 0) delay(15);
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  if (instance == 0)
    {
      uorm = FALSE;
      lwait = 5;      
    }
  (*any_gpio_4bitV2).lcd_G4data = (*any_gpio_4bitV2).lcd_G4data | GPIO4BITV2_LCD_E_HIGH;
  WP_digitalWriteByte((*any_gpio_4bitV2).lcd_G4data, qx);
  delayMicroseconds((*any_gpio_4bitV2).G4bit.lcd_ehmin);
  (*any_gpio_4bitV2).lcd_G4data = (*any_gpio_4bitV2).lcd_G4data & GPIO4BITV2_LCD_E_LOW;
  WP_digitalWriteByte((*any_gpio_4bitV2).lcd_G4data, qx);
  (*any_gpio_4bitV2).lcd_G4data = lG4data;
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_LcdG4V2WriteRegister(lcd_gpio_4bitV2 *any_gpio_4bitV2,
			     unsigned int regh,
			     unsigned int regl,
			     char qx)
{
  unsigned int old_lcd_wait;
  (*any_gpio_4bitV2).lcd_G4data = GPIO4BITV2_LCD_RMASK + regh;
  WP_LcdG4V2Strobe(any_gpio_4bitV2, qx);
  (*any_gpio_4bitV2).lcd_G4data = GPIO4BITV2_LCD_RMASK + regl;
  if (regl == LCD_RETURN_HOME_L)
    {
      old_lcd_wait = (*any_gpio_4bitV2).G4bit.lcd_wait;
      (*any_gpio_4bitV2).G4bit.lcd_wait = (*any_gpio_4bitV2).G4bit.lcd_elmax;;
    }
  WP_LcdG4V2Strobe(any_gpio_4bitV2, qx);
  /* Reset lcd_wait? */
  if (regl == LCD_RETURN_HOME_L) (*any_gpio_4bitV2).G4bit.lcd_wait = old_lcd_wait;
}

void WP_LcdG4V2WriteData(lcd_gpio_4bitV2 *any_gpio_4bitV2,
			 unsigned int data,
			 char qx)
{
  unsigned int datah,datal;
  datah = data & 0xf0;
  datal = data & 0x0f;
  (*any_gpio_4bitV2).lcd_G4data = GPIO4BITV2_LCD_DMASK + (datah / 0x10);
  WP_LcdG4V2Strobe(any_gpio_4bitV2, qx);
  (*any_gpio_4bitV2).lcd_G4data = GPIO4BITV2_LCD_DMASK + datal;
  WP_LcdG4V2Strobe(any_gpio_4bitV2, qx);
}

void WP_LcdG4V2HomeClear(lcd_gpio_4bitV2 *any_gpio_4bitV2,
			 char qx)
{
  unsigned int regh,regl;
  regh = LCD_DISPLAY_OFF_H;
  regl = LCD_DISPLAY_OFF_L;
  WP_LcdG4V2WriteRegister(any_gpio_4bitV2, regh, regl, qx);
  regh = LCD_DISPLAY_ON_CURSOR_BLINK_H;
  regl = LCD_DISPLAY_ON_CURSOR_BLINK_L;
  WP_LcdG4V2WriteRegister(any_gpio_4bitV2, regh, regl, qx);
  regh = LCD_CLEAR_DISPLAY_H;
  regl = LCD_CLEAR_DISPLAY_L;
  WP_LcdG4V2WriteRegister(any_gpio_4bitV2, regh, regl, qx);
  regh = LCD_RETURN_HOME_H;
  regl = LCD_RETURN_HOME_L;
  WP_LcdG4V2WriteRegister(any_gpio_4bitV2, regh, regl, qx);
  regh = LCD_SET_DDRAM_H;
  regl = 0x00;
  WP_LcdG4V2WriteRegister(any_gpio_4bitV2, regh, regl, qx);
  regh = LCD_CURSOR_UP_NO_SHIFT_H;
  regl = LCD_CURSOR_UP_NO_SHIFT_L;
  WP_LcdG4V2WriteRegister(any_gpio_4bitV2, regh, regl, qx);
}

void WP_LcdG4V2WriteMessage(lcd_gpio_4bitV2 *any_gpio_4bitV2,
			    char qx,
			    char *message)
{
  int i,bite;
  unsigned int data,regh,regl;
  char lmsg[41],*lp1;
  strncpy(lmsg, message, 40);
  lmsg[40] = NUL;
  lp1 = lmsg;
  i = 0;
  while (*lp1 != NUL)
    {
      data = toascii((int)(*lp1));
      WP_LcdG4V2WriteData(any_gpio_4bitV2, data, qx);
      lp1++;
      i++;
      if (i == (*any_gpio_4bitV2).G4bit.lcd_nc)
	{
	  regh = LCD_RETURN_HOME_H;
	  regl = LCD_RETURN_HOME_L;
	  WP_LcdG4V2WriteRegister(any_gpio_4bitV2, regh, regl, qx);
	  bite = LCD_SET_DDRAM + 
	    ((*any_gpio_4bitV2).G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	  regh = (bite & 0xf0) / 0x10;
	  regl = bite & 0x0f;
	  WP_LcdG4V2WriteRegister(any_gpio_4bitV2, regh, regl, qx);
	}
    }
}
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char sc,*p1,*p2;
  char dateandtimeis[DATEANDTIME_BUF_LEN],timenowis[TIMENOW_BUF_LEN];
  int i,j,k,l;
  int bite,mehmin,mwait;
  int mregh,mregl;
#ifdef HAVE_WIRINGPI_H
  int wPS;
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
      /* Testing new functions ... */
      /*
      get_dateandtime(dateandtimeis);
      printf("\nCheck1: %s", dateandtimeis);
      sleep(2);
      get_timenow(timenowis);
      printf("Check2: %s\n", timenowis);
      */
      /* ... */
      exit(EXIT_SUCCESS);
    }

  /* Set up defaults */
  eflag    = igflag = tflag = vflag = xflag = FALSE;
  demode   = INITGPIO; /* Dummy mode */
  gpiomode = SYSMODE;
  lcdmodel = LCM0802;

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
	    case ECHO:    eflag=TRUE;       i--; break;
	    case LCD0802: lcdmodel=LCM0802; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case CLRHOME: demode=CLRHOME;  gpiomode=WPIMODE; i--; break;
	    case FSI4TST: demode=FSI4TST;  gpiomode=WPIMODE; i--; break;
	    case INITGPIO: igflag=TRUE;    i--; break;
	    case PRELOAD: demode=PRELOAD;  i--; break;
	    case STRBTST: demode=STRBTST;  i--; break;
	    case WDTATST: demode=WDTATST;  i--; break;
	    case WLINTST: demode=WLINTST;  gpiomode=WPIMODE; i--; break;
	    case WMSGTST: demode=WMSGTST;  gpiomode=WPIMODE; i--; break;
	    case WRGRTST: demode=WRGRTST;  i--; break;
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;      i--; break;
	    case SYSMODE: gpiomode=SYSMODE; xflag=FALSE; i--; break;
	    case TSTMODE: tflag=TRUE; i--;  break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case WPIMODE: gpiomode=WPIMODE; i--; break;
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

  /* Store default display name in buffer4 ... */
  switch (lcdmodel)
    {
    case LCM0802:
    default:
      {
	strcpy(buffer4, "blue_0802");
	break;
      }
    }

  /* -IG option leads to an early exit after the system call ... */
  if (igflag)
    {  
      init_current_gpio_4bitV2(&current_gpio_4bitV2,
			       lcdmodel,
			       4,
			       23,
			       17,
			       18,
			       27,
			       22,
			       buffer4);
      if (tflag)
	{
	  current_gpio_4bitV2.G4bit.lcd_ehmin = current_gpio_4bitV2.G4bit.lcd_ehmin * 10.0;
	  current_gpio_4bitV2.G4bit.lcd_elmin = current_gpio_4bitV2.G4bit.lcd_elmin * 10.0;
	  current_gpio_4bitV2.G4bit.lcd_elmax = current_gpio_4bitV2.G4bit.lcd_elmax * 10.0;
	  current_gpio_4bitV2.G4bit.lcd_wait  = current_gpio_4bitV2.G4bit.lcd_wait * 10.0;
	}
      echo_any_gpio_4bitV2_status(&current_gpio_4bitV2);
#ifdef HAVE_WIRINGPI_H
      C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  init_current_gpio_4bitV2(&current_gpio_4bitV2,
			   lcdmodel,
			   4,
			   23,
			   17,
			   18,
			   27,
			   22,
			   buffer4);
  if (tflag)
    {
      current_gpio_4bitV2.G4bit.lcd_ehmin = current_gpio_4bitV2.G4bit.lcd_ehmin * 10.0;
      current_gpio_4bitV2.G4bit.lcd_elmin = current_gpio_4bitV2.G4bit.lcd_elmin * 10.0;
      current_gpio_4bitV2.G4bit.lcd_elmax = current_gpio_4bitV2.G4bit.lcd_elmax * 10.0;
      current_gpio_4bitV2.G4bit.lcd_wait  = current_gpio_4bitV2.G4bit.lcd_wait * 10.0;
    }
  echo_any_gpio_4bitV2_status(&current_gpio_4bitV2);

  if (gpiomode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      switch(demode)
	{
#ifdef HAVE_WIRINGPI_H
	case PRELOAD:
	case STRBTST:
	case WDTATST:
	case WRGRTST:
	  {
	    C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.rs_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.e_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.d4_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.d5_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.d6_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.d7_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    current_gpio_4bitV2.lcd_G4data = GPIO4BITV2_LCD_RMASK;
	    C2S_gpio_wb(current_gpio_4bitV2.lcd_G4data, xflag);
	    break;
	  }
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }
  
#ifdef HAVE_WIRINGPI_H
  if (gpiomode == WPIMODE)
    {
      printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
      C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.rs_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.e_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.d4_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.d5_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.d6_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bitV2.lcd_condat.d7_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      if (xflag)
	{
	  if ((wPS=wiringPiSetup()) < 0)
	    {
	      fprintf(stderr,
		      "Could not use wiringPiSetup!");
	      perror("wiringPiSetup");
	      exit(EXIT_FAILURE);
	    }
	  else printf("Executed wiringPiSetup ...\n");
	}
      switch(demode)
	{
	case CLRHOME:
	case FSI4TST:
	case STRBTST:
	case WDTATST:
	case WLINTST:
	case WMSGTST:
	case WRGRTST:
	  {
	    current_gpio_4bitV2.lcd_G4data = GPIO4BITV2_LCD_RMASK;
	    WP_digitalWriteByte(current_gpio_4bitV2.lcd_G4data, xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPI_H */

  /* Demonstration ... */
  if (gpiomode == SYSMODE)
    {
      switch(demode)
	{
#ifdef HAVE_WIRINGPI_H
	case STRBTST:
	  {
	    C2S_LcdG4V2Strobe_Test(xflag);
	    break;
	  }
	case WDTATST:
	  {
	    for (i=0; i<16; i++)
	      {
		j = i +0x30;
		C2S_LcdG4V2WriteData_Test(j, xflag);
	      }
	    /* Register Select = instruction register */
	    current_gpio_4bitV2.lcd_G4data = 
	      current_gpio_4bitV2.lcd_G4data & GPIO4BITV2_LCD_RS_LOW;
	    /* Clear data nibble ... */
	    current_gpio_4bitV2.lcd_G4data = current_gpio_4bitV2.lcd_G4data & 0x0f;
	    C2S_gpio_wb(current_gpio_4bitV2.lcd_G4data, xflag);
	    break;
	  }
	case WRGRTST:
	  {
	    C2S_LcdG4V2WriteRegister_Test(LCD_DISPLAY_OFF_H,
					  LCD_DISPLAY_OFF_L,
					  xflag);
	    C2S_LcdG4V2WriteRegister_Test(LCD_DISPLAY_ON_CURSOR_BLINK_H,
					  LCD_DISPLAY_ON_CURSOR_BLINK_L,
					  xflag);
	    C2S_LcdG4V2WriteRegister_Test(LCD_CLEAR_DISPLAY_H,
					  LCD_CLEAR_DISPLAY_L,
					  xflag);	      
	    /* Clear register nibble ... */
	    current_gpio_4bitV2.lcd_G4data = current_gpio_4bitV2.lcd_G4data & 0x0f;
	    C2S_gpio_wb(current_gpio_4bitV2.lcd_G4data, xflag);
	    break;
	  }
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
  if (gpiomode == WPIMODE)
    {
      switch(demode)
	{
	case FSI4TST:
	  {
	    for (i=0; i<4; i++) WP_LcdG4V2F4TStrobe(&current_gpio_4bitV2, i, xflag);
	    /*
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2, 
				    LCD_DISPLAY_OFF_H,
				    LCD_DISPLAY_OFF_L,
				    xflag);
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				    LCD_DISPLAY_ON_CURSOR_BLINK_H,
				    LCD_DISPLAY_ON_CURSOR_BLINK_L, 
				    xflag);
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				    LCD_CLEAR_DISPLAY_H,
				    LCD_CLEAR_DISPLAY_L, 
				    xflag);
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				    LCD_CURSOR_UP_NO_SHIFT_H,
				    LCD_CURSOR_UP_NO_SHIFT_L, 
				    xflag);
	    */
	    break;
	  }
	case STRBTST:
	  {
	    for (i=0; i<5; i++) WP_LcdG4V2Strobe(&current_gpio_4bitV2, xflag);
	    break;
	  }
	case WDTATST:
	  {
	    WP_LcdG4V2HomeClear(&current_gpio_4bitV2, xflag);
	    sleep(1);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		WP_LcdG4V2WriteData(&current_gpio_4bitV2,
				  j,
				  xflag);
		sleep(1);
		if (i == current_gpio_4bitV2.G4bit.lcd_nc)
		  WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
					  LCD_CURSOR_UP_SHIFTED_H,
					  LCD_CURSOR_UP_SHIFTED_L, 
					  xflag);
		if (i == 25)
		  WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
					  LCD_DISPLAY_SHIFT_LEFT_H,
					  LCD_DISPLAY_SHIFT_LEFT_L, 
					  xflag);		
	      }
	    sleep(2);
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				    LCD_RETURN_HOME_H,
				    LCD_RETURN_HOME_L, 
				    xflag);		
	    bite = LCD_SET_DDRAM + 
	      (current_gpio_4bitV2.G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	    mregh = (bite & 0xf0) / 0x10;
	    mregl = bite & 0x0f;
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				    mregh,
				    mregl, 
				    xflag);		
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				    LCD_CURSOR_UP_NO_SHIFT_H,
				    LCD_CURSOR_UP_NO_SHIFT_L, 
				    xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		WP_LcdG4V2WriteData(&current_gpio_4bitV2,
				    j,
				    xflag);
		sleep(1);
		if (i == current_gpio_4bitV2.G4bit.lcd_nc)
		  WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
					  LCD_CURSOR_UP_SHIFTED_H,
					  LCD_CURSOR_UP_SHIFTED_L, 
					  xflag);
		if (i == 25)
		  WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
					  LCD_DISPLAY_SHIFT_LEFT_H,
					  LCD_DISPLAY_SHIFT_LEFT_L, 
					  xflag);		
	      }
	    sleep(2);
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				    LCD_RETURN_HOME_H,
				    LCD_RETURN_HOME_L, 
				    xflag);		
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				    LCD_DISPLAY_ON_NO_CURSOR_H,
				    LCD_DISPLAY_ON_NO_CURSOR_L, 
				    xflag);
	    break;
	  }
	case WLINTST:
	  {
	    WP_LcdG4V2HomeClear(&current_gpio_4bitV2, xflag);
	    sleep(1);
	    switch (lcdmodel)
	      {
	      case LCM0802:
	      default:
		{
		  sprintf(message_buffer, 
			  "0x%02x #1", 
			  current_gpio_4bitV2.G4bit.lcd_ddram_idx[0]);
		  WP_LcdG4V2WriteMessage(&current_gpio_4bitV2, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bitV2.G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
					  mregh,
					  mregl, 
					  xflag);		
		  sprintf(message_buffer, 
			  "0x%02x #2", 
			  current_gpio_4bitV2.G4bit.lcd_ddram_idx[0]);
		  WP_LcdG4V2WriteMessage(&current_gpio_4bitV2, xflag, message_buffer);
		  break;
		}
	      }
	    break;
	  }
	case WMSGTST:
	  {
	    WP_LcdG4V2HomeClear(&current_gpio_4bitV2, xflag);
	    sleep(1);
	    strcpy(buffer4, "G4");
	    switch(lcdmodel)
	      {
	      case LCM0802:
	      default:
		{
		  message_buffer[8] = NUL;
		  strcat(buffer4, " 0802 ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      }
	    WP_LcdG4V2WriteMessage(&current_gpio_4bitV2, xflag, buffer4);
	    break;
	  }
	case CLRHOME:
	case WRGRTST:
	  {
	    /*
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2, 
	                          LCD_DISPLAY_OFF_H,
				  LCD_DISPLAY_OFF_L,
				  xflag);
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				  LCD_DISPLAY_ON_CURSOR_BLINK_H,
				  LCD_DISPLAY_ON_CURSOR_BLINK_L, 
				  xflag);
	    WP_LcdG4V2WriteRegister(&current_gpio_4bitV2,
				  LCD_CLEAR_DISPLAY_H,
				  LCD_CLEAR_DISPLAY_L, 
				  xflag);
	    */
	    WP_LcdG4V2HomeClear(&current_gpio_4bitV2, xflag);
	    break;
	  }
	default: break;
	}
   }
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */

  if (gpiomode == SYSMODE)
    {
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
    }
  
#ifdef HAVE_WIRINGPI_H
  if (gpiomode == WPIMODE)
    {
    }
#endif /* HAVE_WIRINGPI_H */
      
  exit(EXIT_SUCCESS);
}
/*
 * $Log: lcd_gpio4bV2_demo.c,v $
 * Revision 1.8  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.7  2017/09/25 18:41:42  pi
 * Interim commit of some test code (for use with piscope)
 *
 * Revision 1.6  2017/09/19 21:47:23  pi
 * Interim commit
 *
 * Revision 1.5  2017/08/04 20:37:24  pi
 * Interim Commit
 *
 * Revision 1.4  2017/08/01 21:13:31  pi
 * Interim Commit
 *
 * Revision 1.3  2017/07/31 17:02:48  pi
 * Interim Commit
 *
 * Revision 1.2  2017/07/31 16:09:16  pi
 * Interim Commit
 *
 * Revision 1.1  2017/07/31 16:02:04  pi
 * Initial revision
 *
 *
 *
 *
 *
 */
