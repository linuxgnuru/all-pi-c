/* LCD Mjkdz I2C lcd1602 backpack DEMOnstration code
 * $Id: lcd_mi2c_demo.c,v 1.10 2018/04/13 16:09:21 pi Exp $
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
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "parsel.h"
#include "i2c_chips.h"
#include "i2c_wraps.h"
#include "lcd_disptools.h"
/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/04/13 16:09:21 $";
static char RCSFile[]     = "$RCSfile: lcd_mi2c_demo.c,v $";
static char RCSId[]       = "$Id: lcd_mi2c_demo.c,v 1.10 2018/04/13 16:09:21 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.10 $";
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
  INQUIRE,   /* -I                   */
  LCD0802,   /* -0802                */
  LCD1602,   /* -1602                */
  LCD1604,   /* -1604                */
  LCD2004,   /* -2004                */
  HVI2C,     /* -HVI2C               */
  LVI2C,     /* -LVI2C               */
  BKLTON,    /* -B1                  */
  BKLTOFF,   /* -B0                  */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  CLRHOME,   /* -CH                  */
  WLINTST,   /* -WLT                 */
  WMSGTST,   /* -WMT                 */
  WTIMTST,   /* -WTT                 */
#endif /* HAVE_WIRINGPII2C_H */
  FSI4TST,   /* -F4T                 */
  PRELOAD,   /* -P                   */
  STRBTST,   /* -ST                  */
  WDTATST,   /* -WDT                 */
  WRGRTST,   /* -WRT                 */
#endif /* HAVE_WIRINGPI_H */
  QUIET,     /* -Q                   */
  SYSMODE,   /* -S                   */
  TSTMODE,   /* -T                   */
  VERBOSE,   /* -V                   */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,i2cmode,i2cvcc;
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
  arglist[HVI2C]    = "-HVI2C";
  arglist[LVI2C]    = "-LVI2C";
  arglist[BKLTON]   = "-B1";
  arglist[BKLTOFF]  = "-B0";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  arglist[CLRHOME]  = "-CH";
  arglist[WLINTST]  = "-WLT";
  arglist[WMSGTST]  = "-WMT";
  arglist[WTIMTST]  = "-WTT";
#endif /* HAVE_WIRINGPII2C_H */
  arglist[FSI4TST]  = "-F4T";
  arglist[PRELOAD]  = "-P";
  arglist[STRBTST]  = "-ST";
  arglist[WDTATST]  = "-WDT";
  arglist[WRGRTST]  = "-WRT";
#endif /* HAVE_WIRINGPI_H */
  arglist[QUIET]    = "-Q";
  arglist[SYSMODE]  = "-S";
  arglist[TSTMODE]  = "-T";
  arglist[VERBOSE]  = "-V";
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
char blflag,eflag,iqflag,oflag,srflag,tflag,vflag,xflag;
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
	    case HVI2C:   i2cvcc=HVI2C;     break;
	    case LVI2C:   i2cvcc=LVI2C;     break;
	    case BKLTON:  blflag=TRUE;      break;
	    case BKLTOFF: blflag=FALSE;     break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case CLRHOME: demode=CLRHOME; i2cmode=WPIMODE; break;
	    case WLINTST: demode=WLINTST; i2cmode=WPIMODE; break;
	    case WMSGTST: demode=WMSGTST; i2cmode=WPIMODE; break;
	    case WTIMTST: demode=WTIMTST; i2cmode=WPIMODE; break;
#endif /* HAVE_WIRINGPII2C_H */
	    case FSI4TST: demode=FSI4TST; break;
	    case PRELOAD: demode=PRELOAD; break;
	    case STRBTST: demode=STRBTST; break;
	    case WDTATST: demode=WDTATST; break;
	    case WRGRTST: demode=WRGRTST; break;
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;     break;
	    case SYSMODE: i2cmode=SYSMODE; break;
	    case TSTMODE: tflag=TRUE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
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
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -E     {Echo command-line arguments},\n");
  strcat(buffer1, "         -I     {Inquire about i2c devices},\n");
  strcat(buffer1, "         -0802  {LCD display type 0802},\n");
  strcat(buffer1, "         -1602  {LCD display type 1602 - default},\n");
  strcat(buffer1, "         -1604  {LCD display type 1604},\n");
  strcat(buffer1, "         -2004  {LCD display type 2004},\n");
  strcat(buffer1, "         -HVI2C {High Voltage I2C (5V - default) },\n");
  strcat(buffer1, "         -LVI2C {Low  Voltage I2C (4.4V)},\n");
  strcat(buffer1, "         -Bn    {Backlight mode (n=0->Off, n=1->On - default)},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -CH    {Clear display & return Home - implies -W},\n");
  strcat(buffer1, "         -WLT   {Write Line info. Test - implies -W},\n");
  strcat(buffer1, "         -WMT   {Write Message Test - implies -W},\n");
  strcat(buffer1, "         -WTT   {Write Timing data Test - implies -W},\n");
#endif /* HAVE_WIRINGPII2C_H */
  strcat(buffer1, "         -F4T   {Func set init 4-bit Test*},\n");
  strcat(buffer1, "         -P     {Preload lcd mi2c},\n");
  strcat(buffer1, "         -ST    {Strobe Test*},\n");
  strcat(buffer1, "         -WDT   {Write Data Test*},\n");
  strcat(buffer1, "         -WRT   {Write Register Test*},\n");
  strcat(buffer1, "                {*NB: LED Test board req'd with -S},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q     {Quiet - default},\n");
  strcat(buffer1, "         -S     {use System call methods - default},\n");
  strcat(buffer1, "         -T     {using LED Test board},\n");
  strcat(buffer1, "         -V     {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -W     {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X     {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

void C2S_MjkLcdStrobe_Test(lcd_mi2c_4bit *any_mi2c_4bit,
			   char qx)
{
  unsigned int lqiop;
  lqiop = (*any_mi2c_4bit).lcd_data.qiop;
  (*any_mi2c_4bit).lcd_data.qiop = lqiop | LCD_MI2C_ESMASK_H;
  C2S_i2cout(PI_B2,
	     (*any_mi2c_4bit).lcd_data.addr,
	     (*any_mi2c_4bit).lcd_data.qiop,
	     qx);
  (*any_mi2c_4bit).lcd_data.qiop = lqiop & LCD_MI2C_ESMASK_L;
  sleep(1);
  C2S_i2cout(PI_B2,
	     (*any_mi2c_4bit).lcd_data.addr,
	     (*any_mi2c_4bit).lcd_data.qiop,
	     qx);
  sleep(2);
}

void C2S_MjkLcdF4TStrobe_Test(lcd_mi2c_4bit *any_mi2c_4bit,
			      int instance,
			      char qx)
{
  unsigned int lqiop;
  if (instance > 3) return;
  lqiop = LCD_MI2C_RMASK + LCD_FUNC_SET_4BIT_INIT;
  if (instance > 2) lqiop = LCD_MI2C_RMASK + LCD_FUNC_SET_4BIT_NF28_H;
  if (instance == 0) sleep(3);
  (*any_mi2c_4bit).lcd_data.qiop = lqiop | LCD_MI2C_ESMASK_H;
  C2S_i2cout(PI_B2,
	     (*any_mi2c_4bit).lcd_data.addr,
	     (*any_mi2c_4bit).lcd_data.qiop,
	     qx);
  (*any_mi2c_4bit).lcd_data.qiop = lqiop & LCD_MI2C_ESMASK_L;
  sleep(1);
  C2S_i2cout(PI_B2,
	     (*any_mi2c_4bit).lcd_data.addr,
	     (*any_mi2c_4bit).lcd_data.qiop,
	     qx);
  if (instance == 0) sleep(2);
  else if (instance > 0) sleep(1);
}

void C2S_MjkLcdWriteRegister_Test(lcd_mi2c_4bit *any_mi2c_4bit,
				  unsigned int regh,
				  unsigned int regl,
				  char qx)
{
  (*any_mi2c_4bit).lcd_data.qiop = LCD_MI2C_RMASK + regh;
  C2S_MjkLcdStrobe_Test(any_mi2c_4bit, qx);
  (*any_mi2c_4bit).lcd_data.qiop = LCD_MI2C_RMASK + regl;
  C2S_MjkLcdStrobe_Test(any_mi2c_4bit, qx);
}

void C2S_MjkLcdWriteData_Test(lcd_mi2c_4bit *any_mi2c_4bit,
			      unsigned int data,
			      char qx)
{
  unsigned int datah,datal;
  datah = data & 0xf0;
  datal = data & 0x0f;
  (*any_mi2c_4bit).lcd_data.qiop = LCD_MI2C_DMASK + (datah / 0x10);
  C2S_MjkLcdStrobe_Test(any_mi2c_4bit, qx);
  (*any_mi2c_4bit).lcd_data.qiop = LCD_MI2C_DMASK + datal;
  C2S_MjkLcdStrobe_Test(any_mi2c_4bit, qx);
}

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
void WP_MjkLcdStrobe(lcd_mi2c_4bit *any_mi2c_4bit,
		     wpi2cvb reg,
		     char qx)
{
  wpi2cvb lreg;
  unsigned int lwait;
  char uorm;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  uorm = TRUE;
  if ((*any_mi2c_4bit).M4bit.lcd_wait > (*any_mi2c_4bit).M4bit.lcd_elmin)
    lwait = (*any_mi2c_4bit).M4bit.lcd_wait;
  else lwait = (*any_mi2c_4bit).M4bit.lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  lreg.data = reg.data | LCD_MI2C_ESMASK_H;
  WP_i2cout(lreg, qx);
  delayMicroseconds((*any_mi2c_4bit).M4bit.lcd_ehmin);
  lreg.data = reg.data & LCD_MI2C_ESMASK_L;
  WP_i2cout(lreg, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_MjkLcdF4TStrobe(lcd_mi2c_4bit *any_4bit,
			wpi2cvb reg,
			int instance,
			char qx)
{
  wpi2cvb lreg;
  unsigned int lwait;
  char uorm;
  if (instance > 3) return;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  uorm = TRUE;
  if ((*any_4bit).M4bit.lcd_wait > (*any_4bit).M4bit.lcd_elmin)
    lwait = (*any_4bit).M4bit.lcd_wait;
  else lwait = (*any_4bit).M4bit.lcd_elmin;
  /* Use lreg.reg as a local store */
  lreg.reg = LCD_MI2C_RMASK + LCD_FUNC_SET_4BIT_INIT;
  if (instance > 2) lreg.reg = LCD_MI2C_RMASK + LCD_FUNC_SET_4BIT_NF28_H;
  if (instance == 0) delay(15);
  uorm = TRUE;
  if (instance == 0)
    {
      uorm = FALSE;
      lwait = 5;      
    }
  lreg.data = lreg.reg | LCD_MI2C_ESMASK_H;
  WP_i2cout(lreg, qx);
  delayMicroseconds((*any_4bit).M4bit.lcd_ehmin);
  lreg.data = lreg.reg & LCD_MI2C_ESMASK_L;
  WP_i2cout(lreg, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);  
}

void WP_MjkLcdWriteRegister(lcd_mi2c_4bit *any_4bit,
			    wpi2cvb regh,
			    wpi2cvb regl,
			    char qx)
{
  unsigned int old_lcd_wait;
  wpi2cvb lreg;
  lreg.did  = regh.did;
  lreg.fd   = regh.fd;
  lreg.reg  = regh.reg;
  lreg.data =  LCD_MI2C_RMASK + regh.data;
  WP_MjkLcdStrobe(any_4bit, lreg, qx);
  lreg.did  = regl.did;
  lreg.fd   = regl.fd;
  lreg.reg  = regl.reg;
  lreg.data =  LCD_MI2C_RMASK + regl.data;
  if (lreg.data == LCD_RETURN_HOME_L)
    {
      old_lcd_wait = (*any_4bit).M4bit.lcd_wait;
      (*any_4bit).M4bit.lcd_wait = (*any_4bit).M4bit.lcd_elmax;;
    }
  WP_MjkLcdStrobe(any_4bit, lreg, qx);
  /* Reset lcd_wait? */
  if (lreg.data == LCD_RETURN_HOME_L) (*any_4bit).M4bit.lcd_wait = old_lcd_wait;
}

void WP_MjkLcdWriteData(lcd_mi2c_4bit *any_4bit,
			wpi2cvb reg,
			char qx)
{
  wpi2cvb lreg;
  unsigned int datah,datal;
  datah     = reg.data & 0xf0;
  datal     = reg.data & 0x0f;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg;
  lreg.data =  LCD_MI2C_DMASK + (datah / 0x10);
  WP_MjkLcdStrobe(any_4bit, lreg, qx);
  lreg.data =  LCD_MI2C_DMASK + datal;
  WP_MjkLcdStrobe(any_4bit, lreg, qx);
}

void WP_MjkLcdHomeClear(lcd_mi2c_4bit *any_4bit,
			wpi2cvb reg,
			char qx)
{
  wpi2cvb lregh,lregl;
  lregh.did  = reg.did;
  lregh.fd   = reg.fd;
  lregh.reg  = reg.reg;
  lregl.did  = reg.did;
  lregl.fd   = reg.fd;
  lregl.reg  = reg.reg;
  lregh.data = LCD_DISPLAY_OFF_H;
  lregl.data = LCD_DISPLAY_OFF_L;
  WP_MjkLcdWriteRegister(any_4bit, lregh, lregl,qx);
  lregh.data = LCD_DISPLAY_ON_CURSOR_BLINK_H;
  lregl.data = LCD_DISPLAY_ON_CURSOR_BLINK_L;
  WP_MjkLcdWriteRegister(any_4bit, lregh, lregl,qx);
  lregh.data = LCD_CLEAR_DISPLAY_H;
  lregl.data = LCD_CLEAR_DISPLAY_L;
  WP_MjkLcdWriteRegister(any_4bit, lregh, lregl,qx);
  lregh.data = LCD_RETURN_HOME_H;
  lregl.data = LCD_RETURN_HOME_L;
  WP_MjkLcdWriteRegister(any_4bit, lregh, lregl,qx);
  lregh.data = LCD_CURSOR_UP_NO_SHIFT_H;
  lregl.data = LCD_CURSOR_UP_NO_SHIFT_L;
  WP_MjkLcdWriteRegister(any_4bit, lregh, lregl,qx);
}

void WP_MjkLcdWriteMessage(lcd_mi2c_4bit *any_4bit,
			   wpi2cvb data,
			   char qx,
			   char *message)
{
  int i,bite;
  wpi2cvb ldata0,ldata1;
  char lmsg[41],*lp1;
  ldata0.did = data.did;
  ldata0.fd  = data.fd;
  ldata0.reg = data.reg;
  ldata1.did = data.did;
  ldata1.fd  = data.fd;
  ldata1.reg = data.reg;
  strncpy(lmsg, message, 40);
  lmsg[40] = NUL;
  lp1 = lmsg;
  i = 0;
  while (*lp1 != NUL)
    {
      ldata0.data = toascii((int)(*lp1));
      WP_MjkLcdWriteData(any_4bit, ldata0, qx);
      lp1++;
      i++;
      if (i == (*any_4bit).M4bit.lcd_nc)
	{
	  ldata0.data = LCD_RETURN_HOME_H;
	  ldata1.data = LCD_RETURN_HOME_L;
	  WP_MjkLcdWriteRegister(any_4bit, 
				 ldata0, 
				 ldata1, 
				 qx);
	  bite = LCD_SET_DDRAM + ((*any_4bit).M4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	  ldata0.data = (bite & 0xf0) / 0x10;
	  ldata1.data = bite & 0x0f;
	  WP_MjkLcdWriteRegister(any_4bit, 
				 ldata0, 
				 ldata1, 
				 qx);
	}
    }
}
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char sc,*p1,*p2;
  char dateandtimeis[DATEANDTIME_BUF_LEN],timenowis[TIMENOW_BUF_LEN];
  int i,j,k,l,mca,mcb;
  int bite,data[16];
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#ifdef HAVE_WIRINGPII2C_H
  wpi2cvb i2c_chip[128];
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
  eflag    = srflag = tflag = vflag = xflag = FALSE;
  blflag   = TRUE;
  demode   = LAST_ARG;
  i2cmode  = SYSMODE;
  i2cvcc   = HVI2C;
  lcdmodel = LCM1602;
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
	    case HVI2C:   i2cvcc=HVI2C;     i--; break;
	    case LVI2C:   i2cvcc=LVI2C;     i--; break;
	    case BKLTOFF: blflag=FALSE;     i--; break;
	    case BKLTON:  blflag=TRUE;      i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case CLRHOME: demode=CLRHOME;  i2cmode=WPIMODE; i--; break;
	    case WLINTST: demode=WLINTST;  i2cmode=WPIMODE; i--; break;
	    case WMSGTST: demode=WMSGTST;  i2cmode=WPIMODE; i--; break;
	    case WTIMTST: demode=WTIMTST;  i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
	    case FSI4TST: demode=FSI4TST;  i--; break;
	    case PRELOAD: demode=PRELOAD;  i--; break;
	    case STRBTST: demode=STRBTST;  i--; break;
	    case WDTATST: demode=WDTATST;  i--; break;
	    case WRGRTST: demode=WRGRTST;  i--; break;
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;     i--; break;
	    case SYSMODE: i2cmode=SYSMODE; i--; break;
	    case TSTMODE: tflag=TRUE; i--; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE: i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
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

  /* -I option leads to an early exit after the system call ... */
  if (iqflag)
    {
      if (!xflag)
	{
	  mca = init_any_mi2c_4bit(&current_mi2c_4bit,
				   lcdmodel,
				   "HD44780U");
	  if (i2cvcc == LVI2C)
	    {
	      /* Increase ehmin, elmin & wait ...*/
	      current_mi2c_4bit.M4bit.lcd_ehmin = 
		current_mi2c_4bit.M4bit.lcd_ehmin + PCF8574_I2C_DRT;
	      current_mi2c_4bit.M4bit.lcd_elmin = 
		current_mi2c_4bit.M4bit.lcd_elmin + PCF8574_I2C_DRT;
	      current_mi2c_4bit.M4bit.lcd_wait = current_mi2c_4bit.M4bit.lcd_elmin;
	    }
	  echo_any_mi2c_4bit_status(&current_mi2c_4bit);	  
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */

  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      mca = init_any_mi2c_4bit(&current_mi2c_4bit,
			       lcdmodel,
			       "HD44780U");
      if (i2cvcc == LVI2C)
	{
	  /* Increase ehmin, elmin & wait ...*/
	  current_mi2c_4bit.M4bit.lcd_ehmin = 
	    current_mi2c_4bit.M4bit.lcd_ehmin + PCF8574_I2C_DRT;
	  current_mi2c_4bit.M4bit.lcd_elmin = 
	    current_mi2c_4bit.M4bit.lcd_elmin + PCF8574_I2C_DRT;
	  current_mi2c_4bit.M4bit.lcd_wait = current_mi2c_4bit.M4bit.lcd_elmin;
	}
      echo_any_mi2c_4bit_status(&current_mi2c_4bit);
      switch(demode)
	{
	case FSI4TST: 
	case WRGRTST:
	  {
	    current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_RMASK;
	    C2S_i2cout(PI_B2,
		       current_mi2c_4bit.lcd_data.addr,
		       current_mi2c_4bit.lcd_data.qiop,
		       xflag);
	    sleep(1);
	    break;
	  }
	case PRELOAD:
	  {
	    current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_PRELOAD;
	    C2S_i2cout(PI_B2,
		       current_mi2c_4bit.lcd_data.addr,
		       current_mi2c_4bit.lcd_data.qiop,
		       xflag);
	    break;
	  }
	case STRBTST:
	  {
	    current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_STANDBY_L;
	    C2S_i2cout(PI_B2,
		       current_mi2c_4bit.lcd_data.addr,
		       current_mi2c_4bit.lcd_data.qiop,
		       xflag);
	    sleep(1);
	    break;
	  }
	case WDTATST:
	  {
	    current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_DMASK;
	    C2S_i2cout(PI_B2,
		       current_mi2c_4bit.lcd_data.addr,
		       current_mi2c_4bit.lcd_data.qiop,
		       xflag);
	    sleep(1);
	    break;
	  }
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      printf("Using wiringPiI2C methods: ...\n");
      switch(lcdmodel)
	{
	case LCM0802: strcpy(buffer4, "current_0802"); break;
	case LCM1604: strcpy(buffer4, "current_1604"); break;
	case LCM2004: strcpy(buffer4, "current_2004"); break;
	case LCM1602:
	default: strcpy(buffer4, "current_1602"); break;
	}
      mca = init_any_mi2c_4bit(&current_mi2c_4bit,
			       lcdmodel,
			       buffer4);
      if (i2cvcc == LVI2C)
	{
	  /* Increase ehmin, elmin & wait ...*/
	  current_mi2c_4bit.M4bit.lcd_ehmin = 
	    current_mi2c_4bit.M4bit.lcd_ehmin + PCF8574_I2C_DRT;
	  current_mi2c_4bit.M4bit.lcd_elmin = 
	    current_mi2c_4bit.M4bit.lcd_elmin + PCF8574_I2C_DRT;
	  current_mi2c_4bit.M4bit.lcd_wait = current_mi2c_4bit.M4bit.lcd_elmin;
	}
      echo_any_mi2c_4bit_status(&current_mi2c_4bit);
      i2c_chip[0].did = (int)(current_mi2c_4bit.lcd_data.addr);
      if ((i2c_chip[0].fd=wiringPiI2CSetup(i2c_chip[0].did)) < 0)
	{
	  fprintf(stderr,
		  "Could not setup i2c_chip[0].did %d!",
		  i2c_chip[0].did);
	  perror("wiringPiI2CSetup");
	  exit(EXIT_FAILURE);
	}
      else 
	{
	  printf("I2C Device 0x%02x has i2c_chip[0].fd of %d\n",
		 i2c_chip[0].did, i2c_chip[0].fd);
	  /* Create a copy for lower nibble's use */
	  i2c_chip[1].did = i2c_chip[0].did;
	  i2c_chip[1].fd  = i2c_chip[0].fd;
	}
      switch(demode)
	{
	case CLRHOME:
	case FSI4TST:
	case WRGRTST:
	  {
	    current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_RMASK;
	    i2c_chip[0].data = current_mi2c_4bit.lcd_data.qiop;
	    WP_i2cout(i2c_chip[0], xflag);	    
	    delay(500);
	    break;
	  }
	case PRELOAD:
	  {
	    current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_PRELOAD;
	    i2c_chip[0].data = current_mi2c_4bit.lcd_data.qiop;
	    WP_i2cout(i2c_chip[0], xflag);	    
	    break;
	  }
	case STRBTST:
	  {
	    current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_STANDBY_L;
	    i2c_chip[0].data = current_mi2c_4bit.lcd_data.qiop;
	    WP_i2cout(i2c_chip[0], xflag);	    
	    delay(500);
	    break;
	  }
	default: break;
	}
   }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

  /* Demonstration/Test ... */
  if (i2cmode == SYSMODE)
    {
      switch(demode)
	{
	case FSI4TST:
	  {
	    if (!tflag) xflag=FALSE;
	    for (i=0; i<4; i++) C2S_MjkLcdF4TStrobe_Test(&current_mi2c_4bit, i, xflag);
	    C2S_MjkLcdWriteRegister_Test(&current_mi2c_4bit,
					 LCD_FUNC_SET_4BIT_NF28_H,
					 LCD_FUNC_SET_4BIT_NF28_L,
					 xflag);
	    C2S_MjkLcdWriteRegister_Test(&current_mi2c_4bit,
					 LCD_DISPLAY_OFF_H,
					 LCD_DISPLAY_OFF_L,
					 xflag);
	    C2S_MjkLcdWriteRegister_Test(&current_mi2c_4bit,
					 LCD_CLEAR_DISPLAY_H,
					 LCD_CLEAR_DISPLAY_L,
					 xflag);
	    C2S_MjkLcdWriteRegister_Test(&current_mi2c_4bit,
					 LCD_DISPLAY_ON_CURSOR_BLINK_H,
					 LCD_DISPLAY_ON_CURSOR_BLINK_L,
					 xflag);
	    C2S_MjkLcdWriteRegister_Test(&current_mi2c_4bit,
					 LCD_CURSOR_UP_NO_SHIFT_H,
					 LCD_CURSOR_UP_NO_SHIFT_L,
					 xflag);
	    break;
	  } 
	case STRBTST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_MjkLcdStrobe_Test(&current_mi2c_4bit,
				  xflag);
	    break;
	  }
	case WDTATST:
	  {
	    if (!tflag) xflag=FALSE;
	    for (i=0; i<10; i++)
	      {
		j = i +0x30;
		C2S_MjkLcdWriteData_Test(&current_mi2c_4bit, j, xflag);
	      }
	    break;
	  }
	case WRGRTST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_MjkLcdWriteRegister_Test(&current_mi2c_4bit,
					 LCD_DISPLAY_OFF_H,
					 LCD_DISPLAY_OFF_L,
					 xflag);
	    C2S_MjkLcdWriteRegister_Test(&current_mi2c_4bit,
					 LCD_DISPLAY_ON_CURSOR_BLINK_H,
					 LCD_DISPLAY_ON_CURSOR_BLINK_L,
					 xflag);
	    C2S_MjkLcdWriteRegister_Test(&current_mi2c_4bit,
					 LCD_CLEAR_DISPLAY_H,
					 LCD_CLEAR_DISPLAY_L,
					 xflag);
	    break;
	  }
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      if (tflag) current_mi2c_4bit.M4bit.lcd_wait = 500000;
      /* Wait for 500000 micro-seconds */
      switch(demode)
	{
	case CLRHOME:
	  {
	    WP_MjkLcdHomeClear(&current_mi2c_4bit, i2c_chip[0], xflag);
	    break;
	  }
	case FSI4TST:
	  {
	    
	    for (i=0; i<4; i++) WP_MjkLcdF4TStrobe(&current_mi2c_4bit, 
						   i2c_chip[0], 
						   i,
						   xflag);
	    i2c_chip[0].data = LCD_DISPLAY_OFF_H;
	    i2c_chip[1].data = LCD_DISPLAY_OFF_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_CLEAR_DISPLAY_H;
	    i2c_chip[1].data = LCD_CLEAR_DISPLAY_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_CURSOR_BLINK_H;
	    i2c_chip[1].data = LCD_DISPLAY_ON_CURSOR_BLINK_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT_H;
	    i2c_chip[1].data = LCD_CURSOR_UP_NO_SHIFT_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    break;
	  } 
	case STRBTST:
	  {
	    WP_MjkLcdStrobe(&current_mi2c_4bit, i2c_chip[0], xflag);
	    break;
	  }
	case WDTATST:
	  {
	    WP_MjkLcdHomeClear(&current_mi2c_4bit, i2c_chip[0], xflag);
	    sleep(2);
	    for (i=0; i<26; i++)
	      {
		i2c_chip[0].data = i + 65;
		WP_MjkLcdWriteData(&current_mi2c_4bit, i2c_chip[0], xflag);
		sleep(1);
		/*		if (i == 16) */
		if (i == current_mi2c_4bit.M4bit.lcd_nc)
		  {
		    i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED_H;
		    i2c_chip[1].data = LCD_CURSOR_UP_SHIFTED_L;
		    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
					   i2c_chip[0], 
					   i2c_chip[1], 
					   xflag);
		  }
		if (i == 25)
		  {
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT_H;
		    i2c_chip[1].data = LCD_DISPLAY_SHIFT_LEFT_L;
		    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
					   i2c_chip[0], 
					   i2c_chip[1], 
					   xflag);
		  }
	      }
	    sleep(2);
	    i2c_chip[0].data = LCD_RETURN_HOME_H;
	    i2c_chip[1].data = LCD_RETURN_HOME_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    bite = LCD_SET_DDRAM + (40 & LCD_DDRAM_ADDR_MASK);
	    i2c_chip[0].data = (bite & 0xf0) / 0x10;
	    i2c_chip[1].data = bite & 0x0f;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT_H;
	    i2c_chip[1].data = LCD_CURSOR_UP_NO_SHIFT_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    for (i=0; i<26; i++)
	      {
		i2c_chip[0].data = i + 97;
		WP_MjkLcdWriteData(&current_mi2c_4bit, i2c_chip[0], xflag);
		sleep(1);
		if (i == current_mi2c_4bit.M4bit.lcd_nc)
		  {
		    i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED_H;
		    i2c_chip[1].data = LCD_CURSOR_UP_SHIFTED_L;
		    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
					   i2c_chip[0], 
					   i2c_chip[1], 
					   xflag);
		  }
		if (i == 25)
		  {
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT_H;
		    i2c_chip[1].data = LCD_DISPLAY_SHIFT_LEFT_L;
		    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
					   i2c_chip[0], 
					   i2c_chip[1], 
					   xflag);
		  }
	      }
	    sleep(2);
	    i2c_chip[0].data = LCD_RETURN_HOME_H;
	    i2c_chip[1].data = LCD_RETURN_HOME_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR_H;
	    i2c_chip[1].data = LCD_DISPLAY_ON_NO_CURSOR_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    break;
	  }
	case WLINTST:
	  {
	    WP_MjkLcdHomeClear(&current_mi2c_4bit, i2c_chip[0], xflag);
	    sleep(1);
	    switch (lcdmodel)
	      {
	      case LCM0802:
		{
		  sprintf(message_buffer, 
			  "0x%02x #1", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[0]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_mi2c_4bit.M4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x #2", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[1]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  break;
		}
	      case LCM1604:
		{
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #1", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[0]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_mi2c_4bit.M4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #2", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[1]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_mi2c_4bit.M4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #3", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[2]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_mi2c_4bit.M4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  for (i=0; i<16; i++)
		    {
		      delay(100);
		      i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT_H;
		      i2c_chip[1].data = LCD_CURSOR_SHIFT_RIGHT_L;
		      WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					     i2c_chip[0],
					     i2c_chip[1], 
					     xflag);
		    }
		  sprintf(message_buffer, 
			  "0x%02x +CSRx16  #4", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[3]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #1", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[0]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_mi2c_4bit.M4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #2", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[1]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_mi2c_4bit.M4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #3", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[2]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_mi2c_4bit.M4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  for (i=0; i<20; i++)
		    {
		      delay(100);
		      i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT_H;
		      i2c_chip[1].data = LCD_CURSOR_SHIFT_RIGHT_L;
		      WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					     i2c_chip[0],
					     i2c_chip[1], 
					     xflag);
		    }
		  sprintf(message_buffer, 
			  "0x%02x SD +CSRx20   #4", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[3]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  sprintf(message_buffer, 
			  "0x%02x LCM1602 #1", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[0]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_mi2c_4bit.M4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x LCM1602 #2", 
			  current_mi2c_4bit.M4bit.lcd_ddram_idx[1]);
		  WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  break;
		}
	      }
	    i2c_chip[0].data = LCD_RETURN_HOME_H;
	    i2c_chip[1].data = LCD_RETURN_HOME_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR_H;
	    i2c_chip[1].data = LCD_DISPLAY_ON_NO_CURSOR_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    break;
	  }
	case WMSGTST:
	  {
	    WP_MjkLcdHomeClear(&current_mi2c_4bit, i2c_chip[0], xflag);
	    sleep(1);
	    printf("%s\n", message_buffer);
	    switch(lcdmodel)
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
		  strcpy(buffer4, "Mjkdz LCM1604...");
		  message_buffer[20] = NUL;
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  strcpy(buffer4, "Mjkdz LCM2004.......");
		  message_buffer[20] = NUL;
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  strcpy(buffer4, "Mjkdz LCM1602...");
		  message_buffer[16] = NUL;
		  strcat(buffer4, message_buffer);
		  break;
		}
	      }
	    WP_MjkLcdWriteMessage(&current_mi2c_4bit,
				  i2c_chip[0],
				  xflag,
				  buffer4);
	    if ((lcdmodel == LCM2004) || (lcdmodel == LCM1604))
	      {
		bite = LCD_SET_DDRAM + 
		  (current_mi2c_4bit.M4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		i2c_chip[0].data = (bite & 0xf0) / 0x10;
		i2c_chip[1].data = bite & 0x0f;
		WP_MjkLcdWriteRegister(&current_mi2c_4bit,
				       i2c_chip[0],
				       i2c_chip[1], 
				       xflag);
		get_date(buffer4);
		WP_MjkLcdWriteMessage(&current_mi2c_4bit,
				      i2c_chip[0],
				      xflag,
				      buffer4);	
		for (j=0; j<30; j++)
		  {
		    bite = LCD_SET_DDRAM + 
		      (current_mi2c_4bit.M4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		    i2c_chip[0].data = (bite & 0xf0) / 0x10;
		    i2c_chip[1].data = bite & 0x0f;
		    WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					   i2c_chip[0],
					   i2c_chip[1], 
					   xflag);
		    if (lcdmodel == LCM2004) k = 20;
		    else k = 16;
		    i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT_H;
		    i2c_chip[1].data = LCD_CURSOR_SHIFT_RIGHT_L;
		    for (i=0; i<k; i++)
		      {
			delay(2);
			WP_MjkLcdWriteRegister(&current_mi2c_4bit,
					       i2c_chip[0],
					       i2c_chip[1], 
					       xflag);
		      }
		    get_timenow(timenowis);
		    WP_MjkLcdWriteMessage(&current_mi2c_4bit,
					  i2c_chip[0],
					  xflag,
					  timenowis);	
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
	case WTIMTST:
	  {
	    WP_MjkLcdHomeClear(&current_mi2c_4bit, i2c_chip[0], xflag);
	    sleep(1);
	    strcpy(message_buffer, getRCSFile(FALSE));
	    switch(lcdmodel)
	      {
	      case LCM0802:
		{
		  message_buffer[8] = NUL;
		  break;
		}
	      case LCM2004:
		{
		  message_buffer[20] = NUL;
		  break;
		}
	      case LCM1602:
	      case LCM1604:
	      default:
		{
		  message_buffer[16] = NUL;
		  break;
		}
	      }
	    WP_MjkLcdWriteMessage(&current_mi2c_4bit,
				  i2c_chip[0],
				  xflag,
				  message_buffer);
	    bite = LCD_SET_DDRAM + 
	      (current_mi2c_4bit.M4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	    i2c_chip[0].data = (bite & 0xf0) / 0x10;
	    i2c_chip[1].data = bite & 0x0f;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit,
				   i2c_chip[0],
				   i2c_chip[1], 
				   xflag);
	    if (lcdmodel != LCM0802)
	      {
		if (i2cvcc == HVI2C)
		  sprintf(message_buffer,
			  " %4duS | %4duS",
			  current_mi2c_4bit.M4bit.lcd_ehmin,
			  current_mi2c_4bit.M4bit.lcd_elmin);
		else
		  sprintf(message_buffer,
			  "*%4duS | %4duS",
			  current_mi2c_4bit.M4bit.lcd_ehmin,
			  current_mi2c_4bit.M4bit.lcd_elmin);
		message_buffer[16] = NUL;
	      }
	    else
	      {
		sprintf(message_buffer,
			"%4d|%3d",
			current_mi2c_4bit.M4bit.lcd_ehmin,
			current_mi2c_4bit.M4bit.lcd_elmin);
		message_buffer[8] = NUL;
	      }
	    WP_MjkLcdWriteMessage(&current_mi2c_4bit,
				  i2c_chip[0],
				  xflag,
				  message_buffer);
	    break;
	  }
	case WRGRTST:
	  {
	    i2c_chip[0].data = LCD_CLEAR_DISPLAY_H;
	    i2c_chip[1].data = LCD_CLEAR_DISPLAY_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_RETURN_HOME_H;
	    i2c_chip[1].data = LCD_RETURN_HOME_L;
	    WP_MjkLcdWriteRegister(&current_mi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */
  if (i2cmode == SYSMODE)
    {
      if (demode == PRELOAD)
	{
	  /* Standby mode? ... */
	  sleep(2);
	  current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_STANDBY_H;
	  C2S_i2cout(PI_B2,
		     current_mi2c_4bit.lcd_data.addr,
		     current_mi2c_4bit.lcd_data.qiop,
		     xflag);
	}
      /* Reset */
      if (tflag)
	{
	  sleep(2);
	  current_mi2c_4bit.lcd_data.qiop = ALL_GPIO_BITS_HIGH;
	  C2S_i2cout(PI_B2,
		     current_mi2c_4bit.lcd_data.addr,
		     current_mi2c_4bit.lcd_data.qiop,
		     xflag);
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      if (demode == PRELOAD)
	{
	  /* Standby mode? ... */
	  sleep(2);
	  current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_STANDBY_H;
	  i2c_chip[0].data = current_mi2c_4bit.lcd_data.qiop;
	  WP_i2cout(i2c_chip[0], xflag);
	}	    
      /* Reset */
      if (tflag)
	{
	  sleep(2);
	  current_mi2c_4bit.lcd_data.qiop = ALL_GPIO_BITS_HIGH;
	  i2c_chip[0].data = current_mi2c_4bit.lcd_data.qiop;
	  WP_i2cout(i2c_chip[0], xflag);
	}
      else
	{
	  /* Standby mode with or w/out backlight? ... */
	  sleep(2);
	  if (blflag) current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_STANDBY_H;
	  else current_mi2c_4bit.lcd_data.qiop = LCD_MI2C_STANDBY_H | LCD_MI2C_BLFMSK_H; 
	  i2c_chip[0].data = current_mi2c_4bit.lcd_data.qiop;
	  WP_i2cout(i2c_chip[0], xflag);
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: lcd_mi2c_demo.c,v $
 * Revision 1.10  2018/04/13 16:09:21  pi
 * Revised backlight control
 *
 * Revision 1.9  2018/04/12 17:32:21  pi
 * -B & +B options added (simple Backlight control)
 *
 * Revision 1.8  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.7  2017/07/12 20:09:43  pi
 * Interim commit
 *
 * Revision 1.6  2017/07/12 17:07:46  pi
 * Interim commit
 *
 * Revision 1.5  2017/07/04 19:42:27  pi
 * -1604 & -LVI2C support added. SCMVersion 0.02
 *
 * Revision 1.4  2017/07/04 19:27:47  pi
 * Interim commit
 *
 * Revision 1.3  2017/07/04 17:32:30  pi
 * Interim commit
 *
 * Revision 1.2  2016/11/10 17:53:23  pi
 * Interim commit
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.4  2015/02/08 20:58:50  pi
 * All main options implemented. SCMVersion 0.01
 *
 * Revision 1.3  2015/02/06 22:24:58  pi
 * Interim commit
 *
 * Revision 1.2  2015/02/06 21:08:39  pi
 * System method tests implemented. Beginning wiringPi methods.
 * (Ported from lcd_fi2c_demo.c)
 *
 * Revision 1.1  2015/02/06 14:12:36  pi
 * Initial revision
 *
 *
 *
 */
