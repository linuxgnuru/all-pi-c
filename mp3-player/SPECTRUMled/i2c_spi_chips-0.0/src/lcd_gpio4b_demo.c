/* LCD GPIO 4-Bit  DEMOnstration code (similar to that in wiringPi)
 * $Id: lcd_gpio4b_demo.c,v 1.9 2018/05/18 13:47:57 pi Exp pi $
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
static char RCSDate[]     = "$Date: 2018/05/18 13:47:57 $";
static char RCSFile[]     = "$RCSfile: lcd_gpio4b_demo.c,v $";
static char RCSId[]       = "$Id: lcd_gpio4b_demo.c,v 1.9 2018/05/18 13:47:57 pi Exp pi $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.9 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.03 $";

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
  LCD1602,   /* -1602                */
  LCD1604,   /* -1604                */
  LCD2004,   /* -2004                */
#ifdef HAVE_WIRINGPI_H
  BCKLT0,    /* -BL0                 */
  BCKLT1,    /* -BL1                 */
  BCKLT2,    /* -BL2                 */
  BCKLT3,    /* -BL3                 */
  BCKLTT,    /* -BLT                 */
  CLRHOME,   /* -CH                  */
  FSI4TST,   /* -F4T                 */
  INITGPIO,  /* -IG                  */
  PRELOAD,   /* -P                   */
  STRBTST,   /* -ST                  */
  WDTATST,   /* -WDT                 */
  WLINTST,   /* -WLT                 */
  WMSGTST,   /* -WMT                 */
  W80CTST,   /* -W80T                */
  WRGRTST,   /* -WRT                 */
#endif /* HAVE_WIRINGPI_H */
  QUIET,     /* -Q                   */
  SYSMODE,   /* -S                   */
  TSTMODE,   /* -T                   */
  VDD3V3,    /* -V3                  */
  VDD5V0,    /* -V5                  */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPI_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,bkltmode,demode,gpiomode,vdd;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[LCD0802]  = "-0802";
  arglist[LCD1602]  = "-1602";
  arglist[LCD1604]  = "-1604";
  arglist[LCD2004]  = "-2004";
#ifdef HAVE_WIRINGPI_H
  arglist[BCKLT0]   = "-BL0";
  arglist[BCKLT1]   = "-BL1";
  arglist[BCKLT2]   = "-BL2";
  arglist[BCKLT3]   = "-BL3";
  arglist[BCKLTT]   = "-BLT";
  arglist[CLRHOME]  = "-CH";
  arglist[FSI4TST]  = "-F4T";
  arglist[INITGPIO] = "-IG";
  arglist[PRELOAD]  = "-P";
  arglist[STRBTST]  = "-ST";
  arglist[WDTATST]  = "-WDT";
  arglist[WLINTST]  = "-WLT";
  arglist[WMSGTST]  = "-WMT";
  arglist[W80CTST]  = "-W80T";
  arglist[WRGRTST]  = "-WRT";
#endif /* HAVE_WIRINGPI_H */
  arglist[QUIET]    = "-Q";
  arglist[SYSMODE]  = "-S";
  arglist[TSTMODE]  = "-T";
  arglist[VDD3V3]   = "-V3";
  arglist[VDD5V0]   = "-V5";
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
	    case LCD1602: lcdmodel=LCM1602; break;
	    case LCD1604: lcdmodel=LCM1604; break;
	    case LCD2004: lcdmodel=LCM2004; break;
#ifdef HAVE_WIRINGPI_H
	    case BCKLT0:  bkltmode=BCKLT0; break;
	    case BCKLT1:  bkltmode=BCKLT1; break;
	    case BCKLT2:  bkltmode=BCKLT2; break;
	    case BCKLT3:  bkltmode=BCKLT3; break;
	    case BCKLTT:  demode=BCKLTT;   gpiomode=WPIMODE; break;
	    case CLRHOME: demode=CLRHOME;  gpiomode=WPIMODE; break;
	    case FSI4TST: demode=FSI4TST;  gpiomode=WPIMODE; break;
	    case INITGPIO: igflag=TRUE;    break;
	    case PRELOAD: demode=PRELOAD;  break;
	    case STRBTST: demode=STRBTST;  break;
	    case WDTATST: demode=WDTATST;  break;
	    case WLINTST: demode=WLINTST;  gpiomode=WPIMODE; break;
	    case WMSGTST: demode=WMSGTST;  gpiomode=WPIMODE; break;
	    case W80CTST: demode=W80CTST;  gpiomode=WPIMODE; break;
	    case WRGRTST: demode=WRGRTST;  break;
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;     break;
	    case SYSMODE: gpiomode=SYSMODE; break;
	    case TSTMODE: tflag=TRUE; break;
	    case VDD3V3:  vdd=VDD3V3; break;
	    case VDD5V0:  vdd=VDD5V0; break;
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
  strcat(buffer1, "         -0802 {LCD display type 0802},\n");
  strcat(buffer1, "         -1602 {LCD display type 1602 - default},\n");
  strcat(buffer1, "         -1604 {LCD display type 1604},\n");
  strcat(buffer1, "         -2004 {LCD display type 2004},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -BLn  {BackLight mode n: n=0,1,2 or 3, default 2},\n");
  strcat(buffer1, "         -BLT  {BackLight Test - implies -W},\n");
  strcat(buffer1, "         -CH   {Clear display & return Home - implies -W},\n");
  strcat(buffer1, "         -F4T  {Func set init 4-bit Test - implies -W},\n");
  strcat(buffer1, "         -IG   {Initialise used gpio's & Gpio readall},\n");
  strcat(buffer1, "         -P    {Preload lcd iface},\n");
  strcat(buffer1, "         -ST   {Strobe Test*},\n");
  strcat(buffer1, "         -WDT  {Write Data Test*},\n");
  strcat(buffer1, "         -WLT  {Write Line info. Test - implies -W},\n");
  strcat(buffer1, "         -WMT  {Write Message Test - implies -W},\n");
  strcat(buffer1, "         -WRT  {Write Register Test*},\n");
  strcat(buffer1, "         -W80T {Write 80 characters Test - implies -W},\n");
  strcat(buffer1, "               {*NB: LED Test board req'd with -S},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q    {Quiet - default},\n");
  strcat(buffer1, "         -S    {use System call method - default},\n");
  strcat(buffer1, "         -T    {using LED Test board},\n");
  strcat(buffer1, "         -V3   {using 3.3V LCD},\n");
  strcat(buffer1, "         -V5   {using 5.0V LCD - default},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
  /*
  strcat(buffer1, "         -W    {use Wiring pi \"byte write\" method**},\n");
  strcat(buffer1, "               {**NB: requires sudo \"prefix\" with -X!},\n");
  */
  strcat(buffer1, "         -W    {use Wiring pi \"byte write\" method},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.\n");
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
void C2S_LcdG4Strobe_Test(char qx)
{
  unsigned int lG4data;
  lG4data = current_gpio_4bit.lcd_G4data;
  current_gpio_4bit.lcd_G4data = lG4data | GPIO4BIT_LCD_E_HIGH;
  C2S_gpio_wb(current_gpio_4bit.lcd_G4data, xflag);
  sleep(1);
  current_gpio_4bit.lcd_G4data = lG4data & GPIO4BIT_LCD_E_LOW;
  C2S_gpio_wb(current_gpio_4bit.lcd_G4data, xflag);
  current_gpio_4bit.lcd_G4data = lG4data;
  sleep(1);
}

void C2S_LcdG4WriteRegister_Test(unsigned int regh,
				 unsigned int regl,
				 char qx)
{
  current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK + (regh * 0x10);
  C2S_LcdG4Strobe_Test(qx);
  current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK + (regl * 0x10);
  C2S_LcdG4Strobe_Test(qx);
}

void C2S_LcdG4WriteData_Test(unsigned int data,
			     char qx)
{
  unsigned int datah,datal;
  datah = data & 0xf0;
  datal = data & 0x0f;
  current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_DMASK + (datah);
  C2S_LcdG4Strobe_Test(qx);
  current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_DMASK + (datal * 0x10);
  C2S_LcdG4Strobe_Test(qx);
}

void WP_LcdG4Strobe(lcd_gpio_4bit *any_gpio_4bit,
		    char qx)
{
  unsigned int lG4data,lwait;
  char uorm;
  uorm = TRUE;
  lG4data = (*any_gpio_4bit).lcd_G4data;
  if ((*any_gpio_4bit).G4bit.lcd_wait > (*any_gpio_4bit).G4bit.lcd_elmin)
    lwait = (*any_gpio_4bit).G4bit.lcd_wait;
  else lwait = (*any_gpio_4bit).G4bit.lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  (*any_gpio_4bit).lcd_G4data = lG4data | GPIO4BIT_LCD_E_HIGH;
  WP_digitalWriteByte((*any_gpio_4bit).lcd_G4data, qx);
  delayMicroseconds((*any_gpio_4bit).G4bit.lcd_ehmin);
  (*any_gpio_4bit).lcd_G4data = lG4data & GPIO4BIT_LCD_E_LOW;
  WP_digitalWriteByte((*any_gpio_4bit).lcd_G4data, qx);
  (*any_gpio_4bit).lcd_G4data = lG4data;
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_LcdG4F4TStrobe(lcd_gpio_4bit *any_gpio_4bit,
		       int instance,
		       char qx)
{
  unsigned int lG4data,lwait;
  char uorm;
  if (instance > 3) return;
  uorm = TRUE;
  lG4data = (*any_gpio_4bit).lcd_G4data;
  if ((*any_gpio_4bit).G4bit.lcd_wait > (*any_gpio_4bit).G4bit.lcd_elmin)
    lwait = (*any_gpio_4bit).G4bit.lcd_wait;
  else lwait = (*any_gpio_4bit).G4bit.lcd_elmin;
  (*any_gpio_4bit).lcd_G4data = GPIO4BIT_LCD_RMASK + (LCD_FUNC_SET_4BIT_INIT * 0x10);
  if (instance > 2) (*any_gpio_4bit).lcd_G4data = GPIO4BIT_LCD_RMASK +
		      (LCD_FUNC_SET_4BIT_NF28_H * 0x10);  
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
  (*any_gpio_4bit).lcd_G4data = (*any_gpio_4bit).lcd_G4data | GPIO4BIT_LCD_E_HIGH;
  WP_digitalWriteByte((*any_gpio_4bit).lcd_G4data, qx);
  delayMicroseconds((*any_gpio_4bit).G4bit.lcd_ehmin);
  (*any_gpio_4bit).lcd_G4data = (*any_gpio_4bit).lcd_G4data & GPIO4BIT_LCD_E_LOW;
  WP_digitalWriteByte((*any_gpio_4bit).lcd_G4data, qx);
  (*any_gpio_4bit).lcd_G4data = lG4data;
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_LcdG4WriteRegister(lcd_gpio_4bit *any_gpio_4bit,
			   unsigned int regh,
			   unsigned int regl,
			   char qx)
{
  unsigned int old_lcd_wait;
  (*any_gpio_4bit).lcd_G4data = GPIO4BIT_LCD_RMASK + (regh * 0x10);
  WP_LcdG4Strobe(any_gpio_4bit, qx);
  (*any_gpio_4bit).lcd_G4data = GPIO4BIT_LCD_RMASK + (regl * 0x10);
  if (regl == LCD_RETURN_HOME_L)
    {
      old_lcd_wait = (*any_gpio_4bit).G4bit.lcd_wait;
      (*any_gpio_4bit).G4bit.lcd_wait = (*any_gpio_4bit).G4bit.lcd_elmax;;
    }
  WP_LcdG4Strobe(any_gpio_4bit, qx);
  /* Reset lcd_wait? */
  if (regl == LCD_RETURN_HOME_L) (*any_gpio_4bit).G4bit.lcd_wait = old_lcd_wait;
}

void WP_LcdG4WriteData(lcd_gpio_4bit *any_gpio_4bit,
		       unsigned int data,
		       char qx)
{
  unsigned int datah,datal;
  datah = data & 0xf0;
  datal = data & 0x0f;
  (*any_gpio_4bit).lcd_G4data = GPIO4BIT_LCD_DMASK + datah;
  WP_LcdG4Strobe(any_gpio_4bit, qx);
  (*any_gpio_4bit).lcd_G4data = GPIO4BIT_LCD_DMASK + (datal * 0x10);
  WP_LcdG4Strobe(any_gpio_4bit, qx);
}

void WP_LcdG4HomeClear(lcd_gpio_4bit *any_gpio_4bit,
		       char qx)
{
  unsigned int regh,regl;
  regh = LCD_DISPLAY_OFF_H;
  regl = LCD_DISPLAY_OFF_L;
  WP_LcdG4WriteRegister(any_gpio_4bit, regh, regl, qx);
  regh = LCD_DISPLAY_ON_CURSOR_BLINK_H;
  regl = LCD_DISPLAY_ON_CURSOR_BLINK_L;
  WP_LcdG4WriteRegister(any_gpio_4bit, regh, regl, qx);
  regh = LCD_CLEAR_DISPLAY_H;
  regl = LCD_CLEAR_DISPLAY_L;
  WP_LcdG4WriteRegister(any_gpio_4bit, regh, regl, qx);
  regh = LCD_RETURN_HOME_H;
  regl = LCD_RETURN_HOME_L;
  WP_LcdG4WriteRegister(any_gpio_4bit, regh, regl, qx);
  regh = LCD_SET_DDRAM_H;
  regl = 0x00;
  WP_LcdG4WriteRegister(any_gpio_4bit, regh, regl, qx);
  regh = LCD_CURSOR_UP_NO_SHIFT_H;
  regl = LCD_CURSOR_UP_NO_SHIFT_L;
  WP_LcdG4WriteRegister(any_gpio_4bit, regh, regl, qx);
}

void WP_LcdG4WriteMessage(lcd_gpio_4bit *any_gpio_4bit,
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
      WP_LcdG4WriteData(any_gpio_4bit, data, qx);
      lp1++;
      i++;
      if (i == (*any_gpio_4bit).G4bit.lcd_nc)
	{
	  regh = LCD_RETURN_HOME_H;
	  regl = LCD_RETURN_HOME_L;
	  WP_LcdG4WriteRegister(any_gpio_4bit, regh, regl, qx);
	  bite = LCD_SET_DDRAM + 
	    ((*any_gpio_4bit).G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	  regh = (bite & 0xf0) / 0x10;
	  regl = bite & 0x0f;
	  WP_LcdG4WriteRegister(any_gpio_4bit, regh, regl, qx);
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
      get_dateandtime(dateandtimeis);
      printf("\nCheck1: %s", dateandtimeis);
      sleep(2);
      get_timenow(timenowis);
      printf("Check2: %s\n", timenowis);
      /* ... */
      exit(EXIT_SUCCESS);
    }

  /* Set up defaults */
  eflag    = igflag = tflag = vflag = xflag = FALSE;
  bkltmode = BCKLT2;
  demode   = INITGPIO; /* Dummy mode */
  gpiomode = SYSMODE;
  lcdmodel = LCM1602;
  vdd      = VDD5V0;

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
	    case LCD1602: lcdmodel=LCM1602; i--; break;
	    case LCD1604: lcdmodel=LCM1604;  i--; break;
	    case LCD2004: lcdmodel=LCM2004; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case BCKLT0:  bkltmode=BCKLT0; i--; break;
	    case BCKLT1:  bkltmode=BCKLT1; i--; break;
	    case BCKLT2:  bkltmode=BCKLT2; i--; break;
	    case BCKLT3:  bkltmode=BCKLT3; i--; break;
	    case BCKLTT:  demode=BCKLTT;   gpiomode=WPIMODE; i--; break;
	    case CLRHOME: demode=CLRHOME;  gpiomode=WPIMODE; i--; break;
	    case FSI4TST: demode=FSI4TST;  gpiomode=WPIMODE; i--; break;
	    case INITGPIO: igflag=TRUE;    i--; break;
	    case PRELOAD: demode=PRELOAD;  i--; break;
	    case STRBTST: demode=STRBTST;  i--; break;
	    case WDTATST: demode=WDTATST;  i--; break;
	    case WLINTST: demode=WLINTST;  gpiomode=WPIMODE; i--; break;
	    case WMSGTST: demode=WMSGTST;  gpiomode=WPIMODE; i--; break;
	    case W80CTST: demode=W80CTST;  gpiomode=WPIMODE; i--; break;
	    case WRGRTST: demode=WRGRTST;  i--; break;
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;     i--; break;
	    case SYSMODE: gpiomode=SYSMODE; i--; break;
	    case TSTMODE: tflag=TRUE; i--; break;
	    case VDD3V3:  vdd=VDD3V3; i--; break;
	    case VDD5V0:  vdd=VDD5V0; i--; break;
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
      {
	strcpy(buffer4, "lcd_0802");
	break;
      }
    case LCM1604:
      {
	strcpy(buffer4, "lcd_1604");
	break;
      }
    case LCM2004:
      {
	strcpy(buffer4, "lcd_2004");
	if (vdd == VDD3V3) strcpy(buffer4, "lcd_3v3_2004");
	break;
      }
    case LCM1602:
    default:
      {
	strcpy(buffer4, "lcd_1602");
	break;
      }
    }

  /* -IG option leads to an early exit after the system call ... */
  if (igflag)
    {  
      init_current_gpio_4bit(&current_gpio_4bit,
			     lcdmodel,
			     17,
			     18,
			     27,
			     22,
			     23,
			     24,
			     25,
			     4,
			     buffer4);
      echo_any_gpio_4bit_status(&current_gpio_4bit);
#ifdef HAVE_WIRINGPI_H
      C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  init_current_gpio_4bit(&current_gpio_4bit,
			 lcdmodel,
			 17,
			 18,
			 27,
			 22,
			 23,
			 24,
			 25,
			 4,
			 buffer4);
  echo_any_gpio_4bit_status(&current_gpio_4bit);

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
	    C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.rs_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.e_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.bl0_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.bl1_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.d4_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.d5_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.d6_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.d7_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
	    if (bkltmode == BCKLT0) 
	      current_gpio_4bit.lcd_G4data =
		current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
	    if (bkltmode == BCKLT1)
	      { 
		current_gpio_4bit.lcd_G4data =
		  current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL2_OFF;
		current_gpio_4bit.lcd_G4data =
		  current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL1_ON;
	      }
	    if (bkltmode == BCKLT3)
	      current_gpio_4bit.lcd_G4data =
		current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL3_ON;
	    C2S_gpio_wb(current_gpio_4bit.lcd_G4data, xflag);
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
      C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.rs_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.e_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.bl0_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.bl1_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.d4_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.d5_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.d6_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_gpio_4bit.lcd_condat.d7_gpio,
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
	case W80CTST:
	case WRGRTST:
	  {
	    current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
	    if (bkltmode == BCKLT0) 
	      current_gpio_4bit.lcd_G4data =
		current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
	    if (bkltmode == BCKLT1)
	      { 
		current_gpio_4bit.lcd_G4data =
		  current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL2_OFF;
		current_gpio_4bit.lcd_G4data =
		  current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL1_ON;
	      }
	    if (bkltmode == BCKLT3)
	      current_gpio_4bit.lcd_G4data =
		current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL3_ON;
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    break;
	  }
	case BCKLTT:
	  {
	    current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
	    /* BCKLT0 */
	    current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    printf("Initialised Backlight ...\n");
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
	    if (!tflag) xflag=FALSE;
	    C2S_LcdG4Strobe_Test(xflag);
	    break;
	  }
	case WDTATST:
	  {
	    if (!tflag) xflag=FALSE;
	    for (i=0; i<16; i++)
	      {
		j = i +0x30;
		C2S_LcdG4WriteData_Test(j, xflag);
	      }
	    /* Register Select = instruction register */
	    current_gpio_4bit.lcd_G4data = 
	      current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_RS_LOW;
	    /* Clear data nibble ... */
	    current_gpio_4bit.lcd_G4data = current_gpio_4bit.lcd_G4data & 0x0f;
	    C2S_gpio_wb(current_gpio_4bit.lcd_G4data, xflag);
	    break;
	  }
	case WRGRTST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_LcdG4WriteRegister_Test(LCD_DISPLAY_OFF_H,
					LCD_DISPLAY_OFF_L,
					xflag);
	    C2S_LcdG4WriteRegister_Test(LCD_DISPLAY_ON_CURSOR_BLINK_H,
					LCD_DISPLAY_ON_CURSOR_BLINK_L,
					xflag);
	    C2S_LcdG4WriteRegister_Test(LCD_CLEAR_DISPLAY_H,
					LCD_CLEAR_DISPLAY_L,
					xflag);	      
	    /* Clear register nibble ... */
	    current_gpio_4bit.lcd_G4data = current_gpio_4bit.lcd_G4data & 0x0f;
	    C2S_gpio_wb(current_gpio_4bit.lcd_G4data, xflag);
	    break;
	  }
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
  if (gpiomode == WPIMODE)
    {
      if (tflag)
	{
	  /* Extend the high pulse ... */
	  mehmin = current_gpio_4bit.G4bit.lcd_ehmin;
	  current_gpio_4bit.G4bit.lcd_ehmin = 50000;
	  /* and wait for 500000 micro-seconds */
	  mwait = current_gpio_4bit.G4bit.lcd_wait;
	  current_gpio_4bit.G4bit.lcd_wait = 500000;
	}
      switch(demode)
	{
	case FSI4TST:
	  {
	    for (i=0; i<4; i++) WP_LcdG4F4TStrobe(&current_gpio_4bit, i, xflag);
	    WP_LcdG4WriteRegister(&current_gpio_4bit, 
				  LCD_DISPLAY_OFF_H,
				  LCD_DISPLAY_OFF_L,
				  xflag);
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_DISPLAY_ON_CURSOR_BLINK_H,
				  LCD_DISPLAY_ON_CURSOR_BLINK_L, 
				  xflag);
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_CLEAR_DISPLAY_H,
				  LCD_CLEAR_DISPLAY_L, 
				  xflag);
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_CURSOR_UP_NO_SHIFT_H,
				  LCD_CURSOR_UP_NO_SHIFT_L, 
				  xflag);
	    break;
	  }
	case STRBTST:
	  {
	    for (i=0; i<5; i++) WP_LcdG4Strobe(&current_gpio_4bit, xflag);
	    break;
	  }
	case WDTATST:
	  {
	    WP_LcdG4HomeClear(&current_gpio_4bit, xflag);
	    sleep(1);
	    for (i=0; i<26; i++)
	      {
		j = i + 65;
		WP_LcdG4WriteData(&current_gpio_4bit,
				  j,
				  xflag);
		sleep(1);
		if (i == current_gpio_4bit.G4bit.lcd_nc)
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					LCD_CURSOR_UP_SHIFTED_H,
					LCD_CURSOR_UP_SHIFTED_L, 
					xflag);
		if (i == 25)
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					LCD_DISPLAY_SHIFT_LEFT_H,
					LCD_DISPLAY_SHIFT_LEFT_L, 
					xflag);		
	      }
	    sleep(2);
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_RETURN_HOME_H,
				  LCD_RETURN_HOME_L, 
				  xflag);		
	    bite = LCD_SET_DDRAM + 
	      (current_gpio_4bit.G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	    mregh = (bite & 0xf0) / 0x10;
	    mregl = bite & 0x0f;
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  mregh,
				  mregl, 
				  xflag);		
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_CURSOR_UP_NO_SHIFT_H,
				  LCD_CURSOR_UP_NO_SHIFT_L, 
				  xflag);
	    for (i=0; i<26; i++)
	      {
		j = i + 97;
		WP_LcdG4WriteData(&current_gpio_4bit,
				  j,
				  xflag);
		sleep(1);
		if (i == current_gpio_4bit.G4bit.lcd_nc)
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					LCD_CURSOR_UP_SHIFTED_H,
					LCD_CURSOR_UP_SHIFTED_L, 
					xflag);
		if (i == 25)
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					LCD_DISPLAY_SHIFT_LEFT_H,
					LCD_DISPLAY_SHIFT_LEFT_L, 
					xflag);		
	      }
	    sleep(2);
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_RETURN_HOME_H,
				  LCD_RETURN_HOME_L, 
				  xflag);		
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_DISPLAY_ON_NO_CURSOR_H,
				  LCD_DISPLAY_ON_NO_CURSOR_L, 
				  xflag);
	    break;
	  }
	case WLINTST:
	  {
	    WP_LcdG4HomeClear(&current_gpio_4bit, xflag);
	    sleep(1);
	    switch (lcdmodel)
	      {
	      case LCM0802:
		{
		  sprintf(message_buffer, 
			  "0x%02x #1", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[0]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bit.G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					mregh,
					mregl, 
					xflag);		
		  sprintf(message_buffer, 
			  "0x%02x #2", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[0]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  break;
		}
	      case LCM1604:
		{
		  sprintf(message_buffer, 
			  "0x%02x LCM1604 #1", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[0]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bit.G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					mregh,
					mregl, 
					xflag);	
		  sprintf(message_buffer, 
			  "0x%02x LCM1604 #2", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[1]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bit.G4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					mregh,
					mregl, 
					xflag);	
		  sprintf(message_buffer, 
			  "0x%02x LCM1604 #3", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[2]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bit.G4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					mregh,
					mregl, 
					xflag);	
		  for (i=0; i<16; i++)
		    {
		      mregh = LCD_CURSOR_SHIFT_RIGHT_H;
		      mregl = LCD_CURSOR_SHIFT_RIGHT_L;
		      WP_LcdG4WriteRegister(&current_gpio_4bit,
					    mregh,
					    mregl, 
					    xflag);		
		      delay(100);
		    }
		  sprintf(message_buffer, 
			  "0x%02x +CSRx16 #4", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[3]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #1", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[0]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bit.G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					mregh,
					mregl, 
					xflag);	
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #2", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[1]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bit.G4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					mregh,
					mregl, 
					xflag);	
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #3", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[2]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bit.G4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					mregh,
					mregl, 
					xflag);	
		  for (i=0; i<20; i++)
		    {
		      mregh = LCD_CURSOR_SHIFT_RIGHT_H;
		      mregl = LCD_CURSOR_SHIFT_RIGHT_L;
		      WP_LcdG4WriteRegister(&current_gpio_4bit,
					    mregh,
					    mregl, 
					    xflag);		
		      delay(100);
		    }
		  sprintf(message_buffer, 
			  "0x%02x SD +CSRx20   #4", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[3]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  break;
		}	
	      case LCM1602:
	      default:
		{
		  sprintf(message_buffer, 
			  "0x%02x LCM1602 #1", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[0]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_gpio_4bit.G4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  mregh = (bite & 0xf0) / 0x10;
		  mregl = bite & 0x0f;
		  WP_LcdG4WriteRegister(&current_gpio_4bit,
					mregh,
					mregl, 
					xflag);		
		  sprintf(message_buffer, 
			  "0x%02x LCM1602 #2", 
			  current_gpio_4bit.G4bit.lcd_ddram_idx[1]);
		  WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, message_buffer);
		  break;
		}
	      }
	    break;
	  }
	case WMSGTST:
	  {
	    WP_LcdG4HomeClear(&current_gpio_4bit, xflag);
	    sleep(1);
	    strcpy(buffer4, "G4");
	    switch(lcdmodel)
	      {
	      case LCM0802:
		{
		  message_buffer[8] = NUL;
		  strcat(buffer4, " 0802 ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM1604:
		{
		  message_buffer[16] = NUL;
		  strcat(buffer4, " LCM1604...   ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  message_buffer[20] = NUL;
		  if (vdd == VDD5V0) strcat(buffer4, " LCM2004...       ");
		  else strcat(buffer4, " 3V3 LCM2004...   ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  message_buffer[16] = NUL;
		  strcat(buffer4, " LCM1602...   ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      }
	    WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, buffer4);
	    if ((lcdmodel == LCM2004) || (lcdmodel == LCM1604))
	      {
		bite = LCD_SET_DDRAM + 
		  (current_gpio_4bit.G4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		mregh = (bite & 0xf0) / 0x10;
		mregl = bite & 0x0f;
		WP_LcdG4WriteRegister(&current_gpio_4bit,
				      mregh,
				      mregl, 
				      xflag);		
		get_date(buffer4);
		WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, buffer4);
		for (j=0; j<30; j++)
		  {
		    bite = LCD_SET_DDRAM + 
		      (current_gpio_4bit.G4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		    mregh = (bite & 0xf0) / 0x10;
		    mregl = bite & 0x0f;
		    WP_LcdG4WriteRegister(&current_gpio_4bit,
					  mregh,
					  mregl, 
					  xflag);		
		    if (lcdmodel == LCM2004) k = 20;
		    else k = 16;
		    mregh = LCD_CURSOR_SHIFT_RIGHT_H;
		    mregl = LCD_CURSOR_SHIFT_RIGHT_L;
		    for (i=0; i<k; i++)
		      {
			delay(2);
			WP_LcdG4WriteRegister(&current_gpio_4bit,
					      mregh,
					      mregl, 
					      xflag);		
		      }
		    get_timenow(timenowis);
		    WP_LcdG4WriteMessage(&current_gpio_4bit, xflag, timenowis);
		    sc = timenowis[7];
		    while (timenowis[7] == sc)
		      {
			delay(60);
			get_timenow(timenowis);
		      }	    
		  }
	      }
	    break;
	  }
	case W80CTST:
	  {
	    WP_LcdG4HomeClear(&current_gpio_4bit, xflag);
	    for (i=0; i<8; i++)
	      {
		for (j=0; j<10; j++)
		  {
		    if (j==0) k = i + 48;
		    else k = j + 48;
		    WP_LcdG4WriteData(&current_gpio_4bit,
				      k,
				      xflag);
		    sleep(1);
		  }
	      }
	    break;
	  }
	case CLRHOME:
	case WRGRTST:
	  {
	    /*
	    WP_LcdG4WriteRegister(&current_gpio_4bit, 
				  LCD_DISPLAY_OFF_H,
				  LCD_DISPLAY_OFF_L,
				  xflag);
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_DISPLAY_ON_CURSOR_BLINK_H,
				  LCD_DISPLAY_ON_CURSOR_BLINK_L, 
				  xflag);
	    WP_LcdG4WriteRegister(&current_gpio_4bit,
				  LCD_CLEAR_DISPLAY_H,
				  LCD_CLEAR_DISPLAY_L, 
				  xflag);
	    */
	    WP_LcdG4HomeClear(&current_gpio_4bit, xflag);
	    break;
	  }
	case BCKLTT:
	  {
	    printf("Testing Backlight ...\n");
	    current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
	    /* BCKLT0 */
	    current_gpio_4bit.lcd_G4data =
	      current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    sleep(1);
	    /* BCKLT1 */
	    current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
	    current_gpio_4bit.lcd_G4data =
	      current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL2_OFF;
	    current_gpio_4bit.lcd_G4data =
	      current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL1_ON;
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    printf("BL1 ...\n");
	    sleep(3);
	    /* BCKLT0 */
	    current_gpio_4bit.lcd_G4data =
	      current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    sleep(1);
	    /* BCKLT2 */
	    current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    printf("BL2 ...\n");
	    sleep(3);
	    /* BCKLT0 */
	    current_gpio_4bit.lcd_G4data =
	      current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    sleep(1);
	    /* BCKLT3 */
	    current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
	      current_gpio_4bit.lcd_G4data =
		current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL3_ON;
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    printf("BL3 ...\n");
	    sleep(3);
	    /* BCKLT0 */
	    current_gpio_4bit.lcd_G4data =
	      current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
	    WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
	    printf("BL0 ...\n");
	    break;
	  }
	default: break;
	}
      if (tflag)
	{
	  /* Recover stored values ... */
	  current_gpio_4bit.G4bit.lcd_ehmin = mehmin;
	  current_gpio_4bit.G4bit.lcd_wait  = mwait;
	}
    }
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */

  if (gpiomode == SYSMODE)
    {
#ifdef HAVE_WIRINGPI_H
      current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
      if (bkltmode == BCKLT0) 
	current_gpio_4bit.lcd_G4data =
	  current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
      if (bkltmode == BCKLT1)
	{ 
	  current_gpio_4bit.lcd_G4data =
	    current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL2_OFF;
	  current_gpio_4bit.lcd_G4data =
	    current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL1_ON;
	}
      if (bkltmode == BCKLT3)
	current_gpio_4bit.lcd_G4data =
	  current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL3_ON;
      C2S_gpio_wb(current_gpio_4bit.lcd_G4data, xflag);
#endif /* HAVE_WIRINGPI_H */
    }
  
#ifdef HAVE_WIRINGPI_H
  if (gpiomode == WPIMODE)
    {
      current_gpio_4bit.lcd_G4data = GPIO4BIT_LCD_RMASK; /* Implies BCKLT2 */
      if (demode == BCKLTT) bkltmode = BCKLT0;
      if (bkltmode == BCKLT0) 
	current_gpio_4bit.lcd_G4data =
	  current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL0_OFF;
      if (bkltmode == BCKLT1)
	{ 
	  current_gpio_4bit.lcd_G4data =
	    current_gpio_4bit.lcd_G4data | GPIO4BIT_LCD_BL_LVL2_OFF;
	  current_gpio_4bit.lcd_G4data =
	    current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL1_ON;
	}
      if (bkltmode == BCKLT3)
	current_gpio_4bit.lcd_G4data =
	  current_gpio_4bit.lcd_G4data & GPIO4BIT_LCD_BL_LVL3_ON;
      WP_digitalWriteByte(current_gpio_4bit.lcd_G4data, xflag);
    }
#endif /* HAVE_WIRINGPI_H */
      
  exit(EXIT_SUCCESS);
}
/*
 * $Log: lcd_gpio4b_demo.c,v $
 * Revision 1.9  2018/05/18 13:47:57  pi
 * Minor bug-fixes to -1604 mode
 *
 * Revision 1.8  2018/05/17 17:32:52  pi
 * 3.3V LCD and improved backlight control support added
 * SCMVersion 0.03
 *
 * Revision 1.7  2018/05/14 23:01:23  pi
 * 3V3 2004 LCD modes now supported. sudo no longer required for -W
 *
 * Revision 1.6  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.5  2017/07/03 19:19:07  pi
 * -1604 support etc. added. SCMVersion 0.02
 *
 * Revision 1.4  2017/07/03 18:57:36  pi
 * Interim commit
 *
 * Revision 1.3  2017/07/03 18:24:24  pi
 * Interim commit
 *
 * Revision 1.2  2017/07/01 19:37:30  pi
 * Interim commit
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.5  2014/12/21 20:06:34  pi
 * Interim commit
 *
 * Revision 1.4  2014/12/05 20:05:48  pi
 * SCMVersion 0.01
 *
 * Revision 1.3  2014/12/05 19:14:44  pi
 * Interim commit
 *
 * Revision 1.2  2014/12/04 23:01:26  pi
 * Interim commit
 *
 * Revision 1.1  2014/12/04 21:29:28  pi
 * Initial revision
 *
 *
 *
 */
