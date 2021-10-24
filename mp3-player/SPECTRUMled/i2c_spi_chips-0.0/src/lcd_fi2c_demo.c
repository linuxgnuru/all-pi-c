/* LCD Funduino I2C backpack DEMOnstration code
 * $Id: lcd_fi2c_demo.c,v 1.10 2018/04/13 16:09:21 pi Exp $
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
static char RCSFile[]     = "$RCSfile: lcd_fi2c_demo.c,v $";
static char RCSId[]       = "$Id: lcd_fi2c_demo.c,v 1.10 2018/04/13 16:09:21 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.10 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.08 $";

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
  DSPLMSG,   /* -dmsg 's'            */
  SCROLLR,   /* +S                   */
  WLINTST,   /* -WLT                 */
  WMSGTST,   /* -WMT                 */
  W80CTST,   /* -W80T                */
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
  arglist[DSPLMSG]  = "-dmsg";
  arglist[SCROLLR]  = "+S";
  arglist[WLINTST]  = "-WLT";
  arglist[WMSGTST]  = "-WMT";
  arglist[W80CTST]  = "-W80T";
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
	    case DSPLMSG:
	      {
		demode=DSPLMSG;
		i2cmode=WPIMODE;
		strcpy(message_buffer, buffer2);
		break;
	      }
	    case SCROLLR: srflag=TRUE;  ; break;
	    case WLINTST: demode=WLINTST; i2cmode=WPIMODE; break;
	    case WMSGTST: demode=WMSGTST; i2cmode=WPIMODE; break;
	    case W80CTST: demode=W80CTST; i2cmode=WPIMODE; break;
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
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -dmsg 's' {Display a MeSsaGe 'like_this_one'... - implies -W},\n");
  strcat(buffer1, "         +S        {... and Scroll right},\n");
#endif /* HAVE_WIRINGPII2C_H */
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
  strcat(buffer1, "         -W80T  {Write 80 characters Test - implies -W},\n");
#endif /* HAVE_WIRINGPII2C_H */
  strcat(buffer1, "         -F4T   {Func set init 4-bit Test*},\n");
  strcat(buffer1, "         -P     {Preload lcd fi2c},\n");
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

void C2S_FunLcdStrobe_Test(char qx)
{
  unsigned int lqiop;
  lqiop = fun_yellow_1602.lcd_data.qiop;
  /* fun_yellow_1602.lcd_data.qiop = lqiop & 0xfb; */
  fun_yellow_1602.lcd_data.qiop = lqiop | LCD_FI2C_ESMASK_H;
  C2S_i2cout(PI_B2,
	     fun_yellow_1602.lcd_data.addr,
	     fun_yellow_1602.lcd_data.qiop,
	     qx);
  /*  fun_yellow_1602.lcd_data.qiop = lqiop | 0x04; */
  fun_yellow_1602.lcd_data.qiop = lqiop & LCD_FI2C_ESMASK_L;
  sleep(1);
  C2S_i2cout(PI_B2,
	     fun_yellow_1602.lcd_data.addr,
	     fun_yellow_1602.lcd_data.qiop,
	     qx);
  sleep(2);
}

void C2S_FunLcdF4TStrobe_Test(int instance,
			      char qx)
{
  unsigned int lqiop;
  if (instance > 3) return;
  lqiop = LCD_FI2C_RMASK + (LCD_FUNC_SET_4BIT_INIT * 0x10);
  if (instance > 2) lqiop = LCD_FI2C_RMASK + (LCD_FUNC_SET_4BIT_NF28_H * 0x10);
  if (instance == 0) sleep(3);
  /*  fun_yellow_1602.lcd_data.qiop = lqiop & 0xfb; */
  fun_yellow_1602.lcd_data.qiop = lqiop | LCD_FI2C_ESMASK_H;
  C2S_i2cout(PI_B2,
	     fun_yellow_1602.lcd_data.addr,
	     fun_yellow_1602.lcd_data.qiop,
	     qx);
  /* fun_yellow_1602.lcd_data.qiop = lqiop | 0x04;*/
  fun_yellow_1602.lcd_data.qiop = lqiop & LCD_FI2C_ESMASK_L;
  sleep(1);
  C2S_i2cout(PI_B2,
	     fun_yellow_1602.lcd_data.addr,
	     fun_yellow_1602.lcd_data.qiop,
	     qx);
  if (instance == 0) sleep(2);
  else if (instance > 0) sleep(1);
}

void C2S_FunLcdWriteRegister_Test(unsigned int regh,
				  unsigned int regl,
				  char qx)
{
  fun_yellow_1602.lcd_data.qiop = LCD_FI2C_RMASK + (regh * 0x10);
  C2S_FunLcdStrobe_Test(qx);
  fun_yellow_1602.lcd_data.qiop = LCD_FI2C_RMASK + (regl * 0x10);
  C2S_FunLcdStrobe_Test(qx);
}

void C2S_FunLcdWriteData_Test(unsigned int data,
			      char qx)
{
  unsigned int datah,datal;
  datah = data & 0xf0;
  datal = data & 0x0f;
  fun_yellow_1602.lcd_data.qiop = LCD_FI2C_DMASK + datah;
  C2S_FunLcdStrobe_Test(qx);
  fun_yellow_1602.lcd_data.qiop = LCD_FI2C_DMASK + (datal * 0x10);
  C2S_FunLcdStrobe_Test(qx);
}

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
void WP_FunLcdStrobe(lcd_fi2c_4bit *any_4bit,
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
  /*  if ((*any_4bit).lcd_wait > (*any_4bit).lcd_ehmin) */
  if ((*any_4bit).lcd_wait > (*any_4bit).lcd_elmin)
    lwait = (*any_4bit).lcd_wait;
  else lwait = (*any_4bit).lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  /* lreg.data = reg.data | 0x04; */
  lreg.data = reg.data | LCD_FI2C_ESMASK_H;
  WP_i2cout(lreg, qx);
  /* delay((*any_4bit).lcd_ehmin); */
  delayMicroseconds((*any_4bit).lcd_ehmin);
  /* lreg.data = reg.data & 0xfb; */
  lreg.data = reg.data & LCD_FI2C_ESMASK_L;
  WP_i2cout(lreg, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_FunLcdF4TStrobe(lcd_fi2c_4bit *any_4bit,
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
  if ((*any_4bit).lcd_wait > (*any_4bit).lcd_elmin)
    lwait = (*any_4bit).lcd_wait;
  else lwait = (*any_4bit).lcd_elmin;
  /* Use lreg.reg as a local store */
  lreg.reg = LCD_FI2C_RMASK + (LCD_FUNC_SET_4BIT_INIT * 0x10);
  if (instance > 2) lreg.reg = LCD_FI2C_RMASK + (LCD_FUNC_SET_4BIT_NF28_H * 0x10);
  if (instance == 0) delay(15);
  uorm = TRUE;
  /* Duplicated code below? ...*/
  /*
  if ((*any_4bit).lcd_wait > (*any_4bit).lcd_elmin)
    lwait = (*any_4bit).lcd_wait;
  else lwait = (*any_4bit).lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  */
  if (instance == 0)
    {
      uorm = FALSE;
      lwait = 5;      
    }
  /* lreg.data = lreg.reg | 0x04; */
  lreg.data = lreg.reg | LCD_FI2C_ESMASK_H;
  WP_i2cout(lreg, qx);
  delayMicroseconds((*any_4bit).lcd_ehmin);
  /* lreg.data = lreg.reg & 0xfb; */
  lreg.data = lreg.reg & LCD_FI2C_ESMASK_L;
  WP_i2cout(lreg, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);  
}

void WP_FunLcdWriteRegister(lcd_fi2c_4bit *any_4bit,
			    wpi2cvb regh,
			    wpi2cvb regl,
			    char qx)
{
  unsigned int old_lcd_wait;
  wpi2cvb lreg;
  lreg.did  = regh.did;
  lreg.fd   = regh.fd;
  lreg.reg  = regh.reg;
  lreg.data =  LCD_FI2C_RMASK + (regh.data * 0x10);
  WP_FunLcdStrobe(any_4bit, lreg, qx);
  lreg.did  = regl.did;
  lreg.fd   = regl.fd;
  lreg.reg  = regl.reg;
  lreg.data =  LCD_FI2C_RMASK + (regl.data * 0x10);
  if (lreg.data == LCD_RETURN_HOME_L)
    {
      old_lcd_wait = (*any_4bit).lcd_wait;
      (*any_4bit).lcd_wait = (*any_4bit).lcd_elmax;;
    }
  WP_FunLcdStrobe(any_4bit, lreg, qx);
  /* Reset lcd_wait? */
  if (lreg.data == LCD_RETURN_HOME_L) (*any_4bit).lcd_wait = old_lcd_wait;
}

void WP_FunLcdWriteData(lcd_fi2c_4bit *any_4bit,
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
  lreg.data =  LCD_FI2C_DMASK + datah;
  WP_FunLcdStrobe(any_4bit, lreg, qx);
  lreg.data =  LCD_FI2C_DMASK + (datal * 0x10);
  WP_FunLcdStrobe(any_4bit, lreg, qx);
}

void WP_FunLcdHomeClear(lcd_fi2c_4bit *any_4bit,
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
  WP_FunLcdWriteRegister(any_4bit, lregh, lregl,qx);
  lregh.data = LCD_DISPLAY_ON_CURSOR_BLINK_H;
  lregl.data = LCD_DISPLAY_ON_CURSOR_BLINK_L;
  WP_FunLcdWriteRegister(any_4bit, lregh, lregl,qx);
  lregh.data = LCD_CLEAR_DISPLAY_H;
  lregl.data = LCD_CLEAR_DISPLAY_L;
  WP_FunLcdWriteRegister(any_4bit, lregh, lregl,qx);
  lregh.data = LCD_RETURN_HOME_H;
  lregl.data = LCD_RETURN_HOME_L;
  WP_FunLcdWriteRegister(any_4bit, lregh, lregl,qx);
  lregh.data = LCD_CURSOR_UP_NO_SHIFT_H;
  lregl.data = LCD_CURSOR_UP_NO_SHIFT_L;
  WP_FunLcdWriteRegister(any_4bit, lregh, lregl,qx);
}

void WP_FunLcdWriteMessage(lcd_fi2c_4bit *any_4bit,
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
      WP_FunLcdWriteData(any_4bit, ldata0, qx);
      lp1++;
      i++;
      if (i == (*any_4bit).lcd_nc)
	{
	  ldata0.data = LCD_RETURN_HOME_H;
	  ldata1.data = LCD_RETURN_HOME_L;
	  WP_FunLcdWriteRegister(any_4bit, 
				 ldata0, 
				 ldata1, 
				 qx);
	  bite = LCD_SET_DDRAM + ((*any_4bit).lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	  ldata0.data = (bite & 0xf0) / 0x10;
	  ldata1.data = bite & 0x0f;
	  WP_FunLcdWriteRegister(any_4bit, 
				 ldata0, 
				 ldata1, 
				 qx);
	}
    }
}

void WP_FunLcdWriteBuffer(lcd_fi2c_4bit *any_4bit,
			  wpi2cvb data,
			  char qx,
			  char scroll)
{
  int i,j,k,bite,ks1,ks2,ks3,ks4;
  wpi2cvb ldata0,ldata1;
  char lscroll,lbuf[LCD_DDRAM_SIZE+1],*lp1;
  ldata0.did = data.did;
  ldata0.fd  = data.fd;
  ldata0.reg = data.reg;
  ldata1.did = data.did;
  ldata1.fd  = data.fd;
  ldata1.reg = data.reg;
  lscroll    = scroll;
  strncpy(lbuf, (*any_4bit).lcd_buffer, LCD_DDRAM_SIZE);
  lbuf[LCD_DDRAM_SIZE] = NUL;
  lp1 = lbuf;  
  i = 0;
  j = LCD_DDRAM_SIZE / 2;
  ks1 = (*any_4bit).lcd_nc;
  ks2 = j + (*any_4bit).lcd_nc;
  /* if ((*any_4bit).lcd_type == LCM2004) */
  if (((*any_4bit).lcd_type == LCM1604) || ((*any_4bit).lcd_type == LCM2004)) 
    {
      ks2 = ks1 + (*any_4bit).lcd_nc;
      ks3 = ks2 + (*any_4bit).lcd_nc;
      if ((*any_4bit).lcd_type == LCM2004) ks4 = 20;
      else ks4 = 16;
    }
  while (*lp1 != NUL)
    {
      ldata0.data = toascii((int)(*lp1));
      WP_FunLcdWriteData(any_4bit, ldata0, qx);
      delay(50);
      lp1++;
      i++;
      /* if ((*any_4bit).lcd_type != LCM2004) */
      if (((*any_4bit).lcd_type != LCM2004) && ((*any_4bit).lcd_type != LCM1604))
	{
	  if (lscroll && ((i == ks1) || (i == ks2)))
	    {
	      ldata0.data = LCD_CURSOR_UP_SHIFTED_H;
	      ldata1.data = LCD_CURSOR_UP_SHIFTED_L;
	      WP_FunLcdWriteRegister(any_4bit, 
				     ldata0, 
				     ldata1, 
				     qx);
	    }
	  if (i == j)
	    {
	      if (lscroll) delay(500);
	      ldata0.data = LCD_RETURN_HOME_H;
	      ldata1.data = LCD_RETURN_HOME_L;
	      WP_FunLcdWriteRegister(any_4bit, 
				     ldata0, 
				     ldata1, 
				     qx);
	      if (lscroll && (i < ks2))
		{
		  ldata0.data = LCD_CURSOR_UP_NO_SHIFT_H;
		  ldata1.data = LCD_CURSOR_UP_NO_SHIFT_L;
		  WP_FunLcdWriteRegister(any_4bit, 
					 ldata0, 
					 ldata1, 
					 qx);
		}
	      bite = LCD_SET_DDRAM + 
		((*any_4bit).lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	      ldata0.data = (bite & 0xf0) / 0x10;
	      ldata1.data = bite & 0x0f;
	      WP_FunLcdWriteRegister(any_4bit, 
				     ldata0, 
				     ldata1, 
				     qx);
	    }	  
	  if (lscroll && ((i > ks1) || (i > ks2))) delay(500);
	}
      /* if ((*any_4bit).lcd_type != LCM2004) */
      if (((*any_4bit).lcd_type == LCM1604) || ((*any_4bit).lcd_type == LCM2004))
	{
	  if (i == ks1)
	    {
	      bite = LCD_SET_DDRAM + 
		((*any_4bit).lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	      ldata0.data = (bite & 0xf0) / 0x10;
	      ldata1.data = bite & 0x0f;
	      WP_FunLcdWriteRegister(any_4bit, 
				     ldata0, 
				     ldata1, 
				     qx);
	    }
	  if (i == ks2)
	    {
	      bite = LCD_SET_DDRAM + 
		((*any_4bit).lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
	      ldata0.data = (bite & 0xf0) / 0x10;
	      ldata1.data = bite & 0x0f;
	      WP_FunLcdWriteRegister(any_4bit, 
				     ldata0, 
				     ldata1, 
				     qx);
	    }
	  if (i == ks3)
	    {
	      bite = LCD_SET_DDRAM + 
		((*any_4bit).lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
	      ldata0.data = (bite & 0xf0) / 0x10;
	      ldata1.data = bite & 0x0f;
	      WP_FunLcdWriteRegister(any_4bit, 
				     ldata0, 
				     ldata1, 
				     qx);
	      ldata0.data = LCD_CURSOR_SHIFT_RIGHT_H;
	      ldata1.data = LCD_CURSOR_SHIFT_RIGHT_L;
	      /* for (i=0; i<20; i++) */
	      for (k=0; k<ks4; k++)
		{
		  delay(100);
		  WP_FunLcdWriteRegister(any_4bit,
					 ldata0,
					 ldata1, 
					 xflag);
		}
	    }
	}
    }
  ldata0.data = LCD_RETURN_HOME_H;
  ldata1.data = LCD_RETURN_HOME_L;
  WP_FunLcdWriteRegister(any_4bit, ldata0, ldata1, xflag);
  ldata0.data = LCD_DISPLAY_ON_NO_CURSOR_H;
  ldata1.data = LCD_DISPLAY_ON_NO_CURSOR_L;
  WP_FunLcdWriteRegister(any_4bit, ldata0, ldata1, xflag);
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
	    case DSPLMSG: 
	      {
		demode=DSPLMSG;
		i2cmode=WPIMODE;
		strcpy(message_buffer, p2);
		break;
	      }
	    case SCROLLR: srflag=TRUE;     i--; break;
	    case WLINTST: demode=WLINTST;  i2cmode=WPIMODE; i--; break;
	    case WMSGTST: demode=WMSGTST;  i2cmode=WPIMODE; i--; break;
	    case W80CTST: demode=W80CTST;  i2cmode=WPIMODE; i--; break;
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
#ifdef HAVE_WIRINGPII2C_H
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
	  /*
	  mca = init_fun_yellow_1602(lcdmodel, 0x27);
	  echo_lcd_fi2c_4bit_status(fun_yellow_1602);
	  */
	  mca = init_any_fi2c_4bit(&fun_yellow_1602,
				   lcdmodel,
				   0x27,
				   "fun_yellow_1602");
	  if (i2cvcc == LVI2C)
	    {
	      /* Increase ehmin, elmin & wait ...*/
	      fun_yellow_1602.lcd_ehmin = 
		fun_yellow_1602.lcd_ehmin + PCF8574_I2C_DRT;
	      fun_yellow_1602.lcd_elmin = 
		fun_yellow_1602.lcd_elmin + PCF8574_I2C_DRT;
	      fun_yellow_1602.lcd_wait = fun_yellow_1602.lcd_elmin;
	    }
	  echo_any_fi2c_4bit_status(&fun_yellow_1602);	  
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */

  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      /* NB: All tests done in '1602' mode */
      mca = init_any_fi2c_4bit(&fun_yellow_1602,
			       lcdmodel,
			       0x27,
			       "fun_yellow_1602");
      if (i2cvcc == LVI2C)
	{
	  /* Increase ehmin, elmin & wait ...*/
	  fun_yellow_1602.lcd_ehmin = 
	    fun_yellow_1602.lcd_ehmin + PCF8574_I2C_DRT;
	  fun_yellow_1602.lcd_elmin = 
	    fun_yellow_1602.lcd_elmin + PCF8574_I2C_DRT;
	  fun_yellow_1602.lcd_wait = fun_yellow_1602.lcd_elmin;
	}
      echo_any_fi2c_4bit_status(&fun_yellow_1602);
      switch(demode)
	{
	case PRELOAD:
	  {
	    fun_yellow_1602.lcd_data.qiop = LCD_FI2C_PRELOAD;
	    C2S_i2cout(PI_B2,
		       fun_yellow_1602.lcd_data.addr,
		       fun_yellow_1602.lcd_data.qiop,
		       xflag);
	    break;
	  }
	case STRBTST:
	  {
	    fun_yellow_1602.lcd_data.qiop = LCD_FI2C_STANDBY_L;
	    C2S_i2cout(PI_B2,
		       fun_yellow_1602.lcd_data.addr,
		       fun_yellow_1602.lcd_data.qiop,
		       xflag);
	    sleep(1);
	    break;
	  }
	case WDTATST:
	  {
	    fun_yellow_1602.lcd_data.qiop = LCD_FI2C_DMASK;
	    C2S_i2cout(PI_B2,
		       fun_yellow_1602.lcd_data.addr,
		       fun_yellow_1602.lcd_data.qiop,
		       xflag);
	    sleep(1);
	    break;
	  }
	case FSI4TST: 
	case WRGRTST:
	  {
	    fun_yellow_1602.lcd_data.qiop = LCD_FI2C_RMASK;
	    C2S_i2cout(PI_B2,
		       fun_yellow_1602.lcd_data.addr,
		       fun_yellow_1602.lcd_data.qiop,
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
      mca = init_any_fi2c_4bit(&current_fi2c_4bit,
			       lcdmodel,
			       0x27,
			       buffer4);
      if (i2cvcc == LVI2C)
	{
	  /* Increase ehmin, elmin & wait ...*/
	  current_fi2c_4bit.lcd_ehmin = 
	    current_fi2c_4bit.lcd_ehmin + PCF8574_I2C_DRT;
	  current_fi2c_4bit.lcd_elmin = 
	    current_fi2c_4bit.lcd_elmin + PCF8574_I2C_DRT;
	  current_fi2c_4bit.lcd_wait = current_fi2c_4bit.lcd_elmin;
	}
      echo_any_fi2c_4bit_status(&current_fi2c_4bit);
      i2c_chip[0].did = (int)(current_fi2c_4bit.lcd_data.addr);
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
	case PRELOAD:
	  {
	    current_fi2c_4bit.lcd_data.qiop = LCD_FI2C_PRELOAD;
	    i2c_chip[0].data = current_fi2c_4bit.lcd_data.qiop;
	    WP_i2cout(i2c_chip[0], xflag);	    
	    break;
	  }
	case STRBTST:
	  {
	    current_fi2c_4bit.lcd_data.qiop = LCD_FI2C_STANDBY_L;
	    i2c_chip[0].data = current_fi2c_4bit.lcd_data.qiop;
	    WP_i2cout(i2c_chip[0], xflag);	    
	    delay(500);
	    break;
	  }
	case DSPLMSG:
	case WDTATST:
	case WLINTST:
	case WMSGTST:
	case W80CTST:
	  {
	    current_fi2c_4bit.lcd_data.qiop = LCD_FI2C_DMASK;
	    i2c_chip[0].data = current_fi2c_4bit.lcd_data.qiop;
	    WP_i2cout(i2c_chip[0], xflag);	    
	    delay(500);
	    /*
	    current_fi2c_4bit.lcd_data.qiop = bkltmode;
	    i2c_chip[0].data = current_fi2c_4bit.lcd_data.qiop;
	    WP_i2cout(i2c_chip[0], xflag);	    
	    delay(500);
	    */
	    break;
	  }
	case CLRHOME:
	case FSI4TST: 
	case WRGRTST:
	  {
	    current_fi2c_4bit.lcd_data.qiop = LCD_FI2C_RMASK;
	    i2c_chip[0].data = current_fi2c_4bit.lcd_data.qiop;
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
	    for (i=0; i<4; i++) C2S_FunLcdF4TStrobe_Test(i, xflag);
	    C2S_FunLcdWriteRegister_Test(LCD_FUNC_SET_4BIT_NF28_H,
					 LCD_FUNC_SET_4BIT_NF28_L,
					 xflag);
	    C2S_FunLcdWriteRegister_Test(LCD_DISPLAY_OFF_H,
					 LCD_DISPLAY_OFF_L,
					 xflag);
	    C2S_FunLcdWriteRegister_Test(LCD_CLEAR_DISPLAY_H,
					 LCD_CLEAR_DISPLAY_L,
					 xflag);
	    C2S_FunLcdWriteRegister_Test(LCD_DISPLAY_ON_CURSOR_BLINK_H,
					 LCD_DISPLAY_ON_CURSOR_BLINK_L,
					 xflag);
	    C2S_FunLcdWriteRegister_Test(LCD_CURSOR_UP_NO_SHIFT_H,
					 LCD_CURSOR_UP_NO_SHIFT_L,
					 xflag);
	    break;
	  } 
	case STRBTST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_FunLcdStrobe_Test(xflag);
	    break;
	  }
	case WDTATST:
	  {
	    if (!tflag) xflag=FALSE;
	    for (i=0; i<10; i++)
	      {
		j = i +0x30;
		C2S_FunLcdWriteData_Test(j, xflag);
	      }
	    break;
	  }
	case WRGRTST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_FunLcdWriteRegister_Test(LCD_DISPLAY_OFF_H,
					 LCD_DISPLAY_OFF_L,
					 xflag);
	    C2S_FunLcdWriteRegister_Test(LCD_DISPLAY_ON_CURSOR_BLINK_H,
					 LCD_DISPLAY_ON_CURSOR_BLINK_L,
					 xflag);
	    C2S_FunLcdWriteRegister_Test(LCD_CLEAR_DISPLAY_H,
					 LCD_CLEAR_DISPLAY_L,
					 xflag);
	    break;
	  }
#ifdef HAVE_WIRINGPI_H
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      if (tflag) current_fi2c_4bit.lcd_wait = 500000; /* Wait for 500000 micro-seconds */
      switch(demode)
	{
	case DSPLMSG: 
	  {
	    strncpy(current_fi2c_4bit.lcd_buffer, 
		    message_buffer,
		    current_fi2c_4bit.lcd_bsize);
	    current_fi2c_4bit.lcd_buffer[current_fi2c_4bit.lcd_bsize] = NUL;
	    if (!xflag || eflag || vflag)
	      printf("LCD.BUFFER = %s\n", current_fi2c_4bit.lcd_buffer);
	    WP_FunLcdHomeClear(&current_fi2c_4bit, i2c_chip[0], xflag);
	    sleep(1);
	    WP_FunLcdWriteBuffer(&current_fi2c_4bit,
				 i2c_chip[0],
				 xflag,
				 srflag);
	    break;
	  }
	case FSI4TST:
	  {
	    
	    for (i=0; i<4; i++) WP_FunLcdF4TStrobe(&current_fi2c_4bit, 
						   i2c_chip[0], 
						   i,
						   xflag);
	    i2c_chip[0].data = LCD_DISPLAY_OFF_H;
	    i2c_chip[1].data = LCD_DISPLAY_OFF_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_CLEAR_DISPLAY_H;
	    i2c_chip[1].data = LCD_CLEAR_DISPLAY_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_CURSOR_BLINK_H;
	    i2c_chip[1].data = LCD_DISPLAY_ON_CURSOR_BLINK_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT_H;
	    i2c_chip[1].data = LCD_CURSOR_UP_NO_SHIFT_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    break;
	  } 
	case STRBTST:
	  {
	    WP_FunLcdStrobe(&current_fi2c_4bit, i2c_chip[0], xflag);
	    break;
	  }
	case WDTATST:
	  {
	    WP_FunLcdHomeClear(&current_fi2c_4bit, i2c_chip[0], xflag);
	    sleep(2);
	    for (i=0; i<26; i++)
	      {
		i2c_chip[0].data = i + 65;
		WP_FunLcdWriteData(&current_fi2c_4bit, i2c_chip[0], xflag);
		sleep(1);
		/*		if (i == 16) */
		if (i == current_fi2c_4bit.lcd_nc)
		  {
		    i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED_H;
		    i2c_chip[1].data = LCD_CURSOR_UP_SHIFTED_L;
		    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
					   i2c_chip[0], 
					   i2c_chip[1], 
					   xflag);
		  }
		if (i == 25)
		  {
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT_H;
		    i2c_chip[1].data = LCD_DISPLAY_SHIFT_LEFT_L;
		    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
					   i2c_chip[0], 
					   i2c_chip[1], 
					   xflag);
		  }
	      }
	    sleep(2);
	    i2c_chip[0].data = LCD_RETURN_HOME_H;
	    i2c_chip[1].data = LCD_RETURN_HOME_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    bite = LCD_SET_DDRAM + (40 & LCD_DDRAM_ADDR_MASK);
	    i2c_chip[0].data = (bite & 0xf0) / 0x10;
	    i2c_chip[1].data = bite & 0x0f;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT_H;
	    i2c_chip[1].data = LCD_CURSOR_UP_NO_SHIFT_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    for (i=0; i<26; i++)
	      {
		i2c_chip[0].data = i + 97;
		WP_FunLcdWriteData(&current_fi2c_4bit, i2c_chip[0], xflag);
		sleep(1);
		/*		if (i == 16) */
		if (i == current_fi2c_4bit.lcd_nc)
		  {
		    i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED_H;
		    i2c_chip[1].data = LCD_CURSOR_UP_SHIFTED_L;
		    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
					   i2c_chip[0], 
					   i2c_chip[1], 
					   xflag);
		  }
		if (i == 25)
		  {
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT_H;
		    i2c_chip[1].data = LCD_DISPLAY_SHIFT_LEFT_L;
		    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
					   i2c_chip[0], 
					   i2c_chip[1], 
					   xflag);
		  }
	      }
	    sleep(2);
	    i2c_chip[0].data = LCD_RETURN_HOME_H;
	    i2c_chip[1].data = LCD_RETURN_HOME_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR_H;
	    i2c_chip[1].data = LCD_DISPLAY_ON_NO_CURSOR_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, 
				   i2c_chip[0], 
				   i2c_chip[1], 
				   xflag);
	    break;
	  }
	case WRGRTST:
	  {
	    i2c_chip[0].data = LCD_CLEAR_DISPLAY_H;
	    i2c_chip[1].data = LCD_CLEAR_DISPLAY_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_RETURN_HOME_H;
	    i2c_chip[1].data = LCD_RETURN_HOME_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    break;
	  }
	case CLRHOME:
	  {
	    WP_FunLcdHomeClear(&current_fi2c_4bit, i2c_chip[0], xflag);
	    break;
	  }
	case W80CTST:
	  {
	    WP_FunLcdHomeClear(&current_fi2c_4bit, i2c_chip[0], xflag);
	    for (i=0; i<8; i++)
	      {
		for (j=0; j<10; j++)
		  {
		    if (j==0) k = i + 48;
		    else k = j + 48;
		    i2c_chip[0].data = k;
		    WP_FunLcdWriteData(&current_fi2c_4bit, i2c_chip[0], xflag);
		    sleep(1);
		  }
	      }
	    break;
	  }
	case WLINTST:
	  {
	    WP_FunLcdHomeClear(&current_fi2c_4bit, i2c_chip[0], xflag);
	    sleep(1);
	    switch (lcdmodel)
	      {
	      case LCM0802:
		{
		  sprintf(message_buffer, 
			  "0x%02x #1", 
			  current_fi2c_4bit.lcd_ddram_idx[0]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_fi2c_4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_FunLcdWriteRegister(&current_fi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x #2", 
			  current_fi2c_4bit.lcd_ddram_idx[1]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  break;
		}
	      case LCM1604:
		{
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #1", 
			  current_fi2c_4bit.lcd_ddram_idx[0]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_fi2c_4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_FunLcdWriteRegister(&current_fi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #2", 
			  current_fi2c_4bit.lcd_ddram_idx[1]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_fi2c_4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_FunLcdWriteRegister(&current_fi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #3", 
			  current_fi2c_4bit.lcd_ddram_idx[2]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_fi2c_4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_FunLcdWriteRegister(&current_fi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT_H;
		  i2c_chip[1].data = LCD_CURSOR_SHIFT_RIGHT_L;
		  for (i=0; i<16; i++)
		    {
		      delay(100);
		      WP_FunLcdWriteRegister(&current_fi2c_4bit,
					     i2c_chip[0],
					     i2c_chip[1], 
					     xflag);
		    }
		  sprintf(message_buffer, 
			  "0x%02x +CSRx16  #4", 
			  current_fi2c_4bit.lcd_ddram_idx[3]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #1", 
			  current_fi2c_4bit.lcd_ddram_idx[0]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_fi2c_4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_FunLcdWriteRegister(&current_fi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #2", 
			  current_fi2c_4bit.lcd_ddram_idx[1]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_fi2c_4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_FunLcdWriteRegister(&current_fi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #3", 
			  current_fi2c_4bit.lcd_ddram_idx[2]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_fi2c_4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_FunLcdWriteRegister(&current_fi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  for (i=0; i<20; i++)
		    {
		      delay(100);
		      i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT_H;
		      i2c_chip[1].data = LCD_CURSOR_SHIFT_RIGHT_L;
		      WP_FunLcdWriteRegister(&current_fi2c_4bit,
					     i2c_chip[0],
					     i2c_chip[1], 
					     xflag);
		    }
		  sprintf(message_buffer, 
			  "0x%02x SD +CSRx20   #4", 
			  current_fi2c_4bit.lcd_ddram_idx[3]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
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
			  current_fi2c_4bit.lcd_ddram_idx[0]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  bite = LCD_SET_DDRAM + 
		    (current_fi2c_4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  i2c_chip[0].data = (bite & 0xf0) / 0x10;
		  i2c_chip[1].data = bite & 0x0f;
		  WP_FunLcdWriteRegister(&current_fi2c_4bit,
					 i2c_chip[0],
					 i2c_chip[1], 
					 xflag);
		  sprintf(message_buffer, 
			  "0x%02x LCM1602 #2", 
			  current_fi2c_4bit.lcd_ddram_idx[1]);
		  WP_FunLcdWriteMessage(&current_fi2c_4bit,
					i2c_chip[0],
					xflag,
					message_buffer);
		  break;
		}
	      }
	    i2c_chip[0].data = LCD_RETURN_HOME_H;
	    i2c_chip[1].data = LCD_RETURN_HOME_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR_H;
	    i2c_chip[1].data = LCD_DISPLAY_ON_NO_CURSOR_L;
	    WP_FunLcdWriteRegister(&current_fi2c_4bit, i2c_chip[0], i2c_chip[1], xflag);
	    break;
	  }
	case WMSGTST:
	  {
	    WP_FunLcdHomeClear(&current_fi2c_4bit, i2c_chip[0], xflag);
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
		  strcpy(buffer4, "Funduino LCM1604");
		  message_buffer[16] = NUL;
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  strcpy(buffer4, "Funduino LCM2004....");
		  message_buffer[20] = NUL;
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  strcpy(buffer4, "Funduino LCM1602");
		  message_buffer[16] = NUL;
		  strcat(buffer4, message_buffer);
		  break;
		}
	      }
	    WP_FunLcdWriteMessage(&current_fi2c_4bit,
				  i2c_chip[0],
				  xflag,
				  buffer4);
	    if ((lcdmodel == LCM2004) || (lcdmodel == LCM1604))
	      {
		bite = LCD_SET_DDRAM + 
		  (current_fi2c_4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		i2c_chip[0].data = (bite & 0xf0) / 0x10;
		i2c_chip[1].data = bite & 0x0f;
		WP_FunLcdWriteRegister(&current_fi2c_4bit,
				       i2c_chip[0],
				       i2c_chip[1], 
				       xflag);
		get_date(buffer4);
		WP_FunLcdWriteMessage(&current_fi2c_4bit,
				      i2c_chip[0],
				      xflag,
				      buffer4);	
		for (j=0; j<30; j++)
		  {
		    bite = LCD_SET_DDRAM + 
		      (current_fi2c_4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		    i2c_chip[0].data = (bite & 0xf0) / 0x10;
		    i2c_chip[1].data = bite & 0x0f;
		    WP_FunLcdWriteRegister(&current_fi2c_4bit,
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
			WP_FunLcdWriteRegister(&current_fi2c_4bit,
					       i2c_chip[0],
					       i2c_chip[1], 
					       xflag);
		      }
		    get_timenow(timenowis);
		    WP_FunLcdWriteMessage(&current_fi2c_4bit,
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
	  /* fun_yellow_1602.lcd_data.qiop = bkltmode; */
	  fun_yellow_1602.lcd_data.qiop = LCD_FI2C_STANDBY_H;
	  C2S_i2cout(PI_B2,
		     fun_yellow_1602.lcd_data.addr,
		     fun_yellow_1602.lcd_data.qiop,
		     xflag);
	}
      /* Reset */
      if (tflag)
	{
	  sleep(2);
	  fun_yellow_1602.lcd_data.qiop = ALL_GPIO_BITS_HIGH;
	  C2S_i2cout(PI_B2,
		     fun_yellow_1602.lcd_data.addr,
		     fun_yellow_1602.lcd_data.qiop,
		     xflag);
	}
      else
	{
	  /* fun_yellow_1602.lcd_data.qiop = bkltmode; */
	  /* Standby mode with or w/out backlight? ... */
	  sleep(2);
	  if (blflag) fun_yellow_1602.lcd_data.qiop = LCD_FI2C_STANDBY_H;
	  else fun_yellow_1602.lcd_data.qiop = LCD_FI2C_STANDBY_H & LCD_FI2C_BLFMSK_L;
	  C2S_i2cout(PI_B2,
		     fun_yellow_1602.lcd_data.addr,
		     fun_yellow_1602.lcd_data.qiop,
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
	  /* current_fi2c_4bit.lcd_data.qiop = bkltmode; */
	  current_fi2c_4bit.lcd_data.qiop = LCD_FI2C_STANDBY_H;
	  i2c_chip[0].data = current_fi2c_4bit.lcd_data.qiop;
	  WP_i2cout(i2c_chip[0], xflag);
	}	    
      /* Reset */
      if (tflag)
	{
	  sleep(2);
	  current_fi2c_4bit.lcd_data.qiop = ALL_GPIO_BITS_HIGH;
	  i2c_chip[0].data = current_fi2c_4bit.lcd_data.qiop;
	  WP_i2cout(i2c_chip[0], xflag);
	}
      else
	{
	  /* current_fi2c_4bit.lcd_data.qiop = bkltmode; */
	  if (blflag) current_fi2c_4bit.lcd_data.qiop = LCD_FI2C_STANDBY_H;
	  else current_fi2c_4bit.lcd_data.qiop = LCD_FI2C_STANDBY_H & LCD_FI2C_BLFMSK_L;
	  i2c_chip[0].data = current_fi2c_4bit.lcd_data.qiop;
	  WP_i2cout(i2c_chip[0], xflag);
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: lcd_fi2c_demo.c,v $
 * Revision 1.10  2018/04/13 16:09:21  pi
 * Revised backlight control
 *
 * Revision 1.9  2018/04/07 19:51:54  pi
 * Improved -1604 mode behaviour (ported from lcd_demo.c)
 *
 * Revision 1.8  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.2  2018/03/22 19:03:18  pi
 * Typo. fix
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.7  2018/01/17 23:06:01  pi
 * Partial implementation of -Bn (backlight control) options
 *
 * Revision 1.6  2017/07/12 14:48:00  pi
 * Minor bug-fix
 *
 * Revision 1.5  2017/07/04 17:32:30  pi
 * Interim commit
 *
 * Revision 1.4  2017/07/01 14:36:23  pi
 * Interim commit
 *
 * Revision 1.3  2017/06/29 14:14:30  pi
 * Improved -1604 support
 *
 * Revision 1.2  2017/06/26 18:57:16  pi
 * -W80T & -1604 options added
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.13  2015/06/30 22:52:36  pi
 * Interim commit
 *
 * Revision 1.12  2015/06/29 19:12:28  pi
 * Added a delay into seconds check loop to improve CPU usage: ~85% --> ~1.3% !
 *
 * Revision 1.11  2015/06/29 18:33:43  pi
 * Date & Time added to -WMT option for 20x04 display
 *
 * Revision 1.10  2015/02/08 20:58:50  pi
 * Further tidying up of conditional compilation
 *
 * Revision 1.9  2015/02/06 21:08:39  pi
 * Some tidying up of comments and making system/wiringPi methods consistant
 *
 * Revision 1.8  2014/11/06 17:12:54  pi
 * Some improved pointer usage & tidying up
 * SCMVersion 0.06
 *
 * Revision 1.7  2014/11/04 20:57:49  pi
 * Some tidying up needed, not all done yet
 *
 * Revision 1.6  2014/10/11 21:52:29  pi
 * Revised strobe timings & mechanism
 *
 * Revision 1.5  2014/10/09 15:31:55  pi
 * 4-bit PCF8574-based strobe timings adjusted (+ bug-fix)
 *
 * Revision 1.4  2014/09/06 21:56:02  pi
 * Strobe delay bug-fix
 *
 * Revision 1.3  2014/08/28 21:56:26  pi
 * Improved strobe timing control mechanism (backported)
 * SCMVersion 0.04
 *
 * Revision 1.2  2014/08/01 21:05:00  pi
 * Minor tidy up
 *
 * Revision 1.1  2014/07/18 19:13:46  pi
 * Initial revision
 *
 * Revision 1.2  2014/05/26 22:40:13  pi
 * Bug-fix to, and improved -WMT option
 * SCMVersion 0.03
 *
 * Revision 1.1  2014/05/26 21:45:35  pi
 * Initial revision
 *
 * Revision 1.14  2014/05/23 20:57:50  pi
 * Preliminary 0802 display support and improved -WDT option
 * SCMVersion 0.02
 *
 * Revision 1.13  2014/04/22 13:13:42  pi
 * *** empty log message ***
 *
 * Revision 1.12  2014/04/20 21:14:12  pi
 * SCMVersion 0.01
 *
 * Revision 1.11  2014/04/20 19:56:36  pi
 * Interim commit
 *
 * Revision 1.10  2014/04/18 22:28:42  pi
 * Interim commit
 *
 * Revision 1.9  2014/04/17 22:41:23  pi
 * Interim commit
 *
 * Revision 1.8  2014/04/15 22:01:08  pi
 * Interim commit
 *
 * Revision 1.7  2014/04/15 20:06:56  pi
 * Interim commit
 *
 * Revision 1.6  2014/04/14 21:54:54  pi
 * Interim commit
 *
 * Revision 1.5  2014/04/14 13:51:18  pi
 * Interim commit
 *
 * Revision 1.4  2014/04/13 21:35:18  pi
 * Interim commit
 *
 * Revision 1.3  2014/04/13 20:46:14  pi
 * Interim commit
 *
 * Revision 1.2  2014/04/12 21:16:10  pi
 * Interim commit
 *
 * Revision 1.1  2014/04/12 19:11:15  pi
 * Initial revision
 *
 *
 */
