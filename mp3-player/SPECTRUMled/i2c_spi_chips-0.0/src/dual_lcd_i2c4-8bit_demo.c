/* DUAL LCD with I2C 4 or 8 BIT interface DEMOnstration code
 * $Id: dual_lcd_i2c4-8bit_demo.c,v 1.5 2018/03/31 21:32:45 pi Exp $
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
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: dual_lcd_i2c4-8bit_demo.c,v $";
static char RCSId[]       = "$Id: dual_lcd_i2c4-8bit_demo.c,v 1.5 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.5 $";
static char RCSState[]    = "$State: Exp $";
static char SCMVersion[]  = "$SCMversion: 0.02 $";

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
  BKLTSOF,   /* -BL0                 */
  BKLTSON,   /* -BL1                 */
  DSPLYA8,   /* -A8                  */
  DSPLYB4,   /* -B4                  */
  ECHO,      /* -E                   */
  INQUIRE,   /* -I                   */
  LCD0802,   /* -0802                */
  LCD1602,   /* -1602                */
  LCD1604,   /* -1604                */
  LCD2004,   /* -2004                */
#ifdef HAVE_WIRINGPI_H
  FSI8TST,   /* -F8T                 */
  INQGPIO,   /* -IG                  */
  PRELOAD,   /* -P                   */
  STRBTST,   /* -ST                  */
  WDTATST,   /* -WDT                 */
  WRGRTST,   /* -WRT                 */
#ifdef HAVE_WIRINGPII2C_H
  CLRHOME,   /* -CH                  */
  FSI4TST,   /* -F4T                 */
  WLINTST,   /* -WLT                 */
  WMSGTST,   /* -WMT                 */
  W80CTST,   /* -W80T                */
#endif /* HAVE_WIRINGPII2C_H */
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

argnames what,which,bkltmode,demode,dsplymode,i2cmode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[BKLTSOF]  = "-BL0";
  arglist[BKLTSON]  = "-BL1";
  arglist[DSPLYA8]  = "-A8";
  arglist[DSPLYB4]  = "-B4";
  arglist[ECHO]     = "-E";
  arglist[INQUIRE]  = "-I";
  arglist[LCD0802]  = "-0802";
  arglist[LCD1602]  = "-1602";
  arglist[LCD1604]  = "-1604";
  arglist[LCD2004]  = "-2004";
#ifdef HAVE_WIRINGPI_H
  arglist[INQGPIO]  = "-IG";
  arglist[PRELOAD]  = "-P";
  arglist[STRBTST]  = "-ST";
  arglist[FSI8TST]  = "-F8T";
  arglist[WDTATST]  = "-WDT";
  arglist[WRGRTST]  = "-WRT";
#ifdef HAVE_WIRINGPII2C_H
  arglist[CLRHOME]  = "-CH";
  arglist[FSI4TST]  = "-F4T";
  arglist[WLINTST]  = "-WLT";
  arglist[WMSGTST]  = "-WMT";
  arglist[W80CTST]  = "-W80T";
#endif /* HAVE_WIRINGPII2C_H */
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
char eflag,igflag,iqflag,oflag,srflag,tflag,vflag,xflag;
char i2caddr[7];
char message_buffer[BUF_LEN];
char line_buffer1[LCD_DDRAM_SIZE],line_buffer2[LCD_DDRAM_SIZE];
unsigned int i2ca;

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
	    case BKLTSOF: bkltmode=BKLTSOF;  break;
	    case BKLTSON: bkltmode=BKLTSON;  break;
	    case DSPLYA8: dsplymode=DSPLYA8; break;
	    case DSPLYB4: dsplymode=DSPLYB4; break;
	    case ECHO:    eflag=TRUE;        break;
	    case INQUIRE: iqflag=TRUE;       break;
	    case LCD0802: lcdmodel=LCM0802;  break;
	    case LCD1602: lcdmodel=LCM1602;  break;
	    case LCD1604: lcdmodel=LCM1604;  break;
	    case LCD2004: lcdmodel=LCM2004;  break;
#ifdef HAVE_WIRINGPI_H
	    case INQGPIO: igflag=TRUE;    iqflag=TRUE; break;
	    case PRELOAD: demode=PRELOAD; break;
	    case STRBTST: demode=STRBTST; break;
	    case FSI8TST: demode=FSI8TST; dsplymode=DSPLYA8; break;
	    case WDTATST: demode=WDTATST; break;
	    case WRGRTST: demode=WRGRTST; break;
#ifdef HAVE_WIRINGPII2C_H
	    case CLRHOME: demode=CLRHOME; i2cmode=WPIMODE; break;
	    case FSI4TST: demode=FSI4TST; dsplymode=DSPLYB4; i2cmode=WPIMODE; break;
	    case WLINTST: demode=WLINTST; i2cmode=WPIMODE; break;
	    case WMSGTST: demode=WMSGTST; i2cmode=WPIMODE; break;
	    case W80CTST: demode=W80CTST; i2cmode=WPIMODE; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;     break;
	    case SYSMODE: i2cmode=SYSMODE; break;
	    case TSTMODE: tflag=TRUE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPI_H
	    case WPIMODE: i2cmode=WPIMODE; break;
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
  strcat(buffer1, "         -A8   {use 8-bit interface via gpio A - default},\n");
  strcat(buffer1, "         -B4   {use 4-bit interface via gpio B},\n");
  strcat(buffer1, "         -BLn  {force BackLights off (n=0) or on (n=1)},\n");
  strcat(buffer1, "         -E    {Echo command-line arguments},\n");
  strcat(buffer1, "         -I    {Inquire about i2c devices},\n");
  strcat(buffer1, "         -0802 {LCD display type 0802},\n");
  strcat(buffer1, "         -1602 {LCD display type 1602 - default},\n");
  strcat(buffer1, "         -1604 {LCD display type 1604},\n");
  strcat(buffer1, "         -2004 {LCD display type 2004},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -IG   {Inquire about i2c devices & Gpio readall},\n");
  strcat(buffer1, "         -P    {Preload lcd iface's},\n");
  strcat(buffer1, "         -ST   {Strobe Test*},\n");
  strcat(buffer1, "         -F8T  {Func set init 8-bit Test* - implies -A8},\n");
  strcat(buffer1, "         -WDT  {Write Data Test*},\n");
  strcat(buffer1, "         -WRT  {Write Register Test*},\n");
  strcat(buffer1, "               {*NB: LED Test board req'd with -S},\n");
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -CH   {Clear display & return Home - implies -W},\n");
  strcat(buffer1, "         -F4T  {Func set init 4-bit Test - implies -B4 & -W},\n");
  strcat(buffer1, "         -WLT  {Write Line info. Test - implies -W},\n");
  strcat(buffer1, "         -WMT  {Write Message Test - implies -W},\n");
  strcat(buffer1, "         -W80T {Write 80 characters Test - implies -W},\n");
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q    {Quiet - default},\n");
  strcat(buffer1, "         -S    {use System call methods - default},\n");
  strcat(buffer1, "         -T    {using LED Test board},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -W    {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
void C2S_LcdStrobe_TestA(char qx)
{
  C2S_gpio_g_write(current_dual_lcd.lcd_e_gpioA,
		   GPIO_HIGH,
		   qx);
  sleep(2);
  C2S_gpio_g_write(current_dual_lcd.lcd_e_gpioA,
		   GPIO_LOW,
		   qx);
}

void C2S_LcdStrobe_TestB(char qx)
{
  unsigned int lB4data;
  lB4data = current_dual_lcd.lcd_B4data;
  current_dual_lcd.lcd_B4data = lB4data | DUAL_LCD_EB_HIGH;
  C2S_i2cset(PI_B2,
	     current_dual_lcd.lcd_data.addr,
	     current_dual_lcd.lcd_data.gpiob,
	     current_dual_lcd.lcd_B4data,
	     xflag);	    
  sleep(1);
  current_dual_lcd.lcd_B4data = lB4data & DUAL_LCD_EB_LOW;
  C2S_i2cset(PI_B2,
	     current_dual_lcd.lcd_data.addr,
	     current_dual_lcd.lcd_data.gpiob,
	     current_dual_lcd.lcd_B4data,
	     xflag);
  current_dual_lcd.lcd_B4data = lB4data;
  sleep(1);
}

void C2S_LcdWriteRegister_TestA(unsigned int reg,
				char qx)
{
  C2S_gpio_g_write(current_dual_lcd.lcd_rs_gpioA,
		   GPIO_LOW,
		   qx);
  C2S_i2cset(PI_B2,
	     current_dual_lcd.lcd_data.addr,
	     current_dual_lcd.lcd_data.gpioa,
	     reg,
	     xflag);
  C2S_LcdStrobe_TestA(qx);
}

void C2S_LcdWriteRegister_TestB(unsigned int regh,
				unsigned int regl,
				char qx)
{
  current_dual_lcd.lcd_B4data = DUAL_LCD_B4_RMASK + (regh * 0x10);
  C2S_LcdStrobe_TestB(qx);
  current_dual_lcd.lcd_B4data = DUAL_LCD_B4_RMASK + (regl * 0x10);
  C2S_LcdStrobe_TestB(qx);
}

void C2S_LcdWriteData_TestA(unsigned int data,
			    char qx)
{
  C2S_gpio_g_write(current_dual_lcd.lcd_rs_gpioA,
		   GPIO_HIGH,
		   qx);
  C2S_i2cset(PI_B2,
	     current_dual_lcd.lcd_data.addr,
	     current_dual_lcd.lcd_data.gpioa,
	     data,
	     xflag);
  C2S_LcdStrobe_TestA(qx);
}

void C2S_LcdWriteData_TestB(unsigned int data,
			    char qx)
{
  unsigned int datah,datal;
  datah = data & 0xf0;
  datal = data & 0x0f;
  current_dual_lcd.lcd_B4data = DUAL_LCD_B4_DMASK + datah;
  C2S_LcdStrobe_TestB(qx);
  current_dual_lcd.lcd_B4data = DUAL_LCD_B4_DMASK + (datal * 0x10);
  C2S_LcdStrobe_TestB(qx);
}

void WP_LcdStrobeA(dual_lcd *any_dual_lcd,
		   char qx)
{
  unsigned int lwait;
  char uorm;
  uorm = TRUE;
  if ((*any_dual_lcd).A8bit.lcd_wait > (*any_dual_lcd).A8bit.lcd_elmin)
    lwait = (*any_dual_lcd).A8bit.lcd_wait;
  else lwait = (*any_dual_lcd).A8bit.lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  WP_digitalWrite((*any_dual_lcd).lcd_e_gpioA, GPIO_HIGH, qx);
  delayMicroseconds((*any_dual_lcd).A8bit.lcd_ehmin);
  WP_digitalWrite((*any_dual_lcd).lcd_e_gpioA, GPIO_LOW, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_LcdStrobeB(dual_lcd *any_dual_lcd,
		   wpi2cvb reg,
		   char qx)
{
  wpi2cvb lreg;
  unsigned int lwait;
  char uorm;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = (*any_dual_lcd).lcd_data.gpiob; /* Force gpiob */
  uorm = TRUE;
  if ((*any_dual_lcd).B4bit.lcd_wait > (*any_dual_lcd).B4bit.lcd_elmin)
    lwait = (*any_dual_lcd).B4bit.lcd_wait;
  else lwait = (*any_dual_lcd).B4bit.lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  lreg.data = reg.data | DUAL_LCD_EB_HIGH;
  WP_i2cset(lreg, qx);
  delayMicroseconds((*any_dual_lcd).B4bit.lcd_ehmin);
  lreg.data = reg.data & DUAL_LCD_EB_LOW;
  WP_i2cset(lreg, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_LcdF4TStrobeB(dual_lcd *any_dual_lcd,
		      wpi2cvb reg,
		      int instance,
		      char qx)
{
  wpi2cvb lreg;
  unsigned int lwait,lstore;
  char uorm;
  if (instance > 3) return;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = (*any_dual_lcd).lcd_data.gpiob; /* Force gpiob */
  uorm = TRUE;
  if ((*any_dual_lcd).B4bit.lcd_wait > (*any_dual_lcd).B4bit.lcd_elmin)
    lwait = (*any_dual_lcd).B4bit.lcd_wait;
  else lwait = (*any_dual_lcd).B4bit.lcd_elmin;
  lstore = DUAL_LCD_B4_RMASK + (LCD_FUNC_SET_4BIT_INIT * 0x10);
  if (instance > 2) lstore = DUAL_LCD_B4_RMASK + (LCD_FUNC_SET_4BIT_NF28_H * 0x10);
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
  lreg.data = lstore | DUAL_LCD_EB_HIGH;
  WP_i2cset(lreg, qx);
  delayMicroseconds((*any_dual_lcd).B4bit.lcd_ehmin);
  lreg.data = lstore & DUAL_LCD_EB_LOW;
  WP_i2cset(lreg, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_LcdWriteRegisterA(dual_lcd *any_dual_lcd,
			  wpi2cvb reg,
			  char qx)
{
  unsigned int old_lcd_wait;
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = (*any_dual_lcd).lcd_data.gpioa; /* Force gpioa */
  lreg.data = reg.data;
  WP_digitalWrite((*any_dual_lcd).lcd_rs_gpioA, GPIO_LOW, qx);
  if (reg.data == LCD_RETURN_HOME) 
    {
      old_lcd_wait = (*any_dual_lcd).A8bit.lcd_wait;
      (*any_dual_lcd).A8bit.lcd_wait = (*any_dual_lcd).A8bit.lcd_elmax;
    }
  WP_i2cset(lreg, qx);
  WP_LcdStrobeA(any_dual_lcd, qx);
  /* Reset A8bit.lcd_wait? */
  if (reg.data == LCD_RETURN_HOME) (*any_dual_lcd).A8bit.lcd_wait = old_lcd_wait;
}

void WP_LcdWriteRegisterB(dual_lcd *any_dual_lcd,
			  wpi2cvb regh,
			  wpi2cvb regl,
			  char qx)
{
  unsigned int old_lcd_wait;
  wpi2cvb lreg;
  lreg.did  = regh.did;
  lreg.fd   = regh.fd;
  lreg.reg  = (*any_dual_lcd).lcd_data.gpiob; /* Force gpiob */
  lreg.data = DUAL_LCD_B4_RMASK + (regh.data * 0x10);
  WP_LcdStrobeB(any_dual_lcd, lreg, qx);
  lreg.did  = regl.did;
  lreg.fd   = regl.fd;
  lreg.reg  = (*any_dual_lcd).lcd_data.gpiob; /* Force gpiob */
  lreg.data = DUAL_LCD_B4_RMASK + (regl.data * 0x10);
  if (lreg.data == LCD_RETURN_HOME_L)
    {
      old_lcd_wait = (*any_dual_lcd).B4bit.lcd_wait;
      (*any_dual_lcd).B4bit.lcd_wait = (*any_dual_lcd).B4bit.lcd_elmax;;
    }
  WP_LcdStrobeB(any_dual_lcd, lreg, qx);
  /* Reset lcd_wait? */
  if (lreg.data == LCD_RETURN_HOME_L) (*any_dual_lcd).B4bit.lcd_wait = old_lcd_wait;
}

void WP_LcdWriteDataA(dual_lcd *any_dual_lcd,
		      wpi2cvb data,
		      char qx)
{
  wpi2cvb lreg;
  lreg.did  = data.did;
  lreg.fd   = data.fd;
  lreg.reg  = (*any_dual_lcd).lcd_data.gpioa; /* Force gpioa */
  lreg.data = data.data;
  WP_digitalWrite((*any_dual_lcd).lcd_rs_gpioA, GPIO_HIGH, qx);
  WP_i2cset(lreg, qx);
  WP_LcdStrobeA(any_dual_lcd, qx);
}

void WP_LcdWriteDataB(dual_lcd *any_dual_lcd,
		      wpi2cvb reg,
		      char qx)
{
  wpi2cvb lreg;
  unsigned int datah,datal;
  datah     = reg.data & 0xf0;
  datal     = reg.data & 0x0f;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = (*any_dual_lcd).lcd_data.gpiob; /* Force gpiob */
  lreg.data =  DUAL_LCD_B4_DMASK + datah;
  WP_LcdStrobeB(any_dual_lcd, lreg, qx);
  lreg.data =  DUAL_LCD_B4_DMASK + (datal * 0x10);
  WP_LcdStrobeB(any_dual_lcd, lreg, qx);
}

void WP_LcdHomeClearA(dual_lcd *any_dual_lcd,
		      wpi2cvb data,
		      char qx)
{ 
  wpi2cvb ldata;
  ldata.did  = data.did;
  ldata.fd   = data.fd;
  ldata.reg  = data.reg;
  ldata.data = LCD_DISPLAY_OFF;
  WP_LcdWriteRegisterA(any_dual_lcd, ldata, qx);
  ldata.data = LCD_DISPLAY_ON_CURSOR_BLINK;
  WP_LcdWriteRegisterA(any_dual_lcd, ldata, qx);
  ldata.data = LCD_CLEAR_DISPLAY;
  WP_LcdWriteRegisterA(any_dual_lcd, ldata, qx);	    
  ldata.data = LCD_RETURN_HOME;
  WP_LcdWriteRegisterA(any_dual_lcd, ldata, qx);
  ldata.data = LCD_SET_DDRAM;
  WP_LcdWriteRegisterA(any_dual_lcd, ldata, qx);
  ldata.data = LCD_CURSOR_UP_NO_SHIFT;
  WP_LcdWriteRegisterA(any_dual_lcd, ldata, qx);	    
}

void WP_LcdHomeClearB(dual_lcd *any_dual_lcd,
		      wpi2cvb data,
		      char qx)
{
  wpi2cvb hdata,ldata;
  hdata.did  = data.did;
  hdata.fd   = data.fd;
  hdata.reg  = data.reg;
  ldata.did  = data.did;
  ldata.fd   = data.fd;
  ldata.reg  = data.reg;
  hdata.data = LCD_DISPLAY_OFF_H;
  ldata.data = LCD_DISPLAY_OFF_L;
  WP_LcdWriteRegisterB(any_dual_lcd, hdata, ldata, qx);
  hdata.data = LCD_DISPLAY_ON_CURSOR_BLINK_H;
  ldata.data = LCD_DISPLAY_ON_CURSOR_BLINK_L;
  WP_LcdWriteRegisterB(any_dual_lcd, hdata, ldata, qx);
  hdata.data = LCD_CLEAR_DISPLAY_H;
  ldata.data = LCD_CLEAR_DISPLAY_L;
  WP_LcdWriteRegisterB(any_dual_lcd, hdata, ldata, qx);
  hdata.data = LCD_RETURN_HOME_H;
  ldata.data = LCD_RETURN_HOME_L;
  WP_LcdWriteRegisterB(any_dual_lcd, hdata, ldata, qx);
  hdata.data = LCD_SET_DDRAM_H;
  ldata.data = 0x00;
  WP_LcdWriteRegisterB(any_dual_lcd, hdata, ldata, qx);
  hdata.data = LCD_CURSOR_UP_NO_SHIFT_H;
  ldata.data = LCD_CURSOR_UP_NO_SHIFT_L;
  WP_LcdWriteRegisterB(any_dual_lcd, hdata, ldata, qx);
}

void WP_LcdWriteMessageA(dual_lcd *any_dual_lcd,
			 wpi2cvb data,
			 char qx,
			 char *message)
{
  int i;
  wpi2cvb ldata;
  char lmsg[41],*lp1;
  ldata.did = data.did;
  ldata.fd  = data.fd;
  ldata.reg = data.reg;
  strncpy(lmsg, message, 40);
  lmsg[40] = NUL;
  lp1 = lmsg;
  i = 0;
  while (*lp1 != NUL)
    {
      ldata.data = toascii((int)(*lp1));
      WP_LcdWriteDataA(any_dual_lcd, ldata, qx);
      lp1++;
      i++;
      if (i == (*any_dual_lcd).A8bit.lcd_nc)
	{
	  ldata.data = LCD_RETURN_HOME;
	  WP_LcdWriteRegisterA(any_dual_lcd, ldata, qx);
	  ldata.data = LCD_SET_DDRAM + 
	    ((*any_dual_lcd).A8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	  WP_LcdWriteRegisterA(any_dual_lcd, ldata, qx);
	}
    }
}

void WP_LcdWriteMessageB(dual_lcd *any_dual_lcd,
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
      WP_LcdWriteDataB(any_dual_lcd, ldata0, qx);
      lp1++;
      i++;
      if (i == (*any_dual_lcd).B4bit.lcd_nc)
	{
	  ldata0.data = LCD_RETURN_HOME_H;
	  ldata1.data = LCD_RETURN_HOME_L;
	  WP_LcdWriteRegisterB(any_dual_lcd, 
			       ldata0, 
			       ldata1, 
			       qx);
	  bite = LCD_SET_DDRAM + 
	    ((*any_dual_lcd).B4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	  ldata0.data = (bite & 0xf0) / 0x10;
	  ldata1.data = bite & 0x0f;
	  WP_LcdWriteRegisterB(any_dual_lcd, 
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
  char *p1,*p2;
  int i,j,k,l;
  int bite,data[16];
  int mehminA,mehminB,mwaitA,mwaitB;
  unsigned int mca;
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
  eflag     = srflag = tflag = vflag = xflag = FALSE;
  bkltmode  = LAST_ARG; /* IE. not yet defined */
  dsplymode = DSPLYA8; /* MCP23017 GPIO_A, 8-bit interface */
  demode    = LAST_ARG;
  i2ca      = 0x20; /* > 0, MCP23017 address range = 0x20 - 0x27 */
  i2cmode   = SYSMODE;
  lcdmodel  = LCM1602;
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
	    case BKLTSOF: bkltmode=BKLTSOF;  i--; break;
	    case BKLTSON: bkltmode=BKLTSON;  i--; break;
	    case DSPLYA8: dsplymode=DSPLYA8; i--; break;
	    case DSPLYB4: dsplymode=DSPLYB4; i--; break;
	    case ECHO:    eflag=TRUE;        i--; break;
	    case INQUIRE: iqflag=TRUE;       i--; break;
	    case LCD0802: lcdmodel=LCM0802;  i--; break;
	    case LCD1602: lcdmodel=LCM1602;  i--; break;
	    case LCD1604: lcdmodel=LCM1604;  i--; break;
	    case LCD2004: lcdmodel=LCM2004;  i--; break;
#ifdef HAVE_WIRINGPI_H
	    case CLRHOME: demode=CLRHOME;  i2cmode=WPIMODE; i--; break;
	    case FSI4TST: demode=FSI4TST;  dsplymode=DSPLYB4; i2cmode=WPIMODE; i--; break;
	    case FSI8TST: demode=FSI8TST;  dsplymode=DSPLYA8; i--; break;
	    case INQGPIO: igflag= TRUE;    iqflag=TRUE; i--; break;
	    case PRELOAD: demode=PRELOAD;  i--; break;
	    case STRBTST: demode=STRBTST;  i--; break;
	    case WDTATST: demode=WDTATST;  i--; break;
	    case WLINTST: demode=WLINTST;  i2cmode=WPIMODE; i--; break;
	    case WMSGTST: demode=WMSGTST;  i2cmode=WPIMODE; i--; break;
	    case W80CTST: demode=W80CTST;  i2cmode=WPIMODE; i--; break;
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

  /* -I or -IG options lead to an early exit after the system call ... */
  if (iqflag)
    {
      if (!xflag)
	{
	  mca = init_any_dual_lcd(&current_dual_lcd,
				  i2ca,
				  lcdmodel,
				  22,
				  17,
				  lcdmodel,
				  23,
				  18,
				  "dual4or8bit_pio");
	  echo_any_dual_lcd_status(&current_dual_lcd);
	}
      C2S_i2cdetect(PI_B2, xflag);
#ifdef HAVE_WIRINGPI_H
      if (igflag) C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
  mca = init_any_dual_lcd(&current_dual_lcd,
			  i2ca,
			  lcdmodel,
			  22,
			  17,
			  lcdmodel,
			  23,
			  18,
			  "dual4or8bit_pio");
  echo_any_dual_lcd_status(&current_dual_lcd);
  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      switch(demode)
	{
#ifdef HAVE_WIRINGPI_H
	case FSI8TST:
	case PRELOAD:
	case STRBTST:
	case WDTATST:
	case WRGRTST:
	  {
	    C2S_gpio_g_mode(current_dual_lcd.lcd_rs_gpioA,
			    GPIO_MODE_OUT,
			    xflag);
	    /* Register Select = instruction register */
	    C2S_gpio_g_write(current_dual_lcd.lcd_rs_gpioA,
			     GPIO_LOW,
			     xflag);
	    C2S_gpio_g_mode(current_dual_lcd.lcd_e_gpioA,
			    GPIO_MODE_OUT,
			    xflag);
	    /* Enable disabled */
	    C2S_gpio_g_write(current_dual_lcd.lcd_e_gpioA,
			     GPIO_LOW,
			     xflag);
	    C2S_gpio_g_mode(current_dual_lcd.lcd_rs_gpioB,
			    GPIO_MODE_IN,
			    xflag);
	    C2S_gpio_g_mode(current_dual_lcd.lcd_e_gpioB,
			    GPIO_MODE_IN,
			    xflag);
	    C2S_i2cset(PI_B2,
		       current_dual_lcd.lcd_data.addr,
		       current_dual_lcd.lcd_data.iodira,
		       ALL_GPIO_BITS_OUT,
		       xflag);
	    C2S_i2cset(PI_B2,
		       current_dual_lcd.lcd_data.addr,
		       current_dual_lcd.lcd_data.iodirb,
		       ALL_GPIO_BITS_OUT,
		       xflag);
	    current_dual_lcd.lcd_B4data = DUAL_LCD_B4_STANDBY;
	    if (dsplymode == DSPLYA8) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLB_OFF;
	    if (dsplymode == DSPLYB4) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLA_OFF;
	    if (bkltmode == BKLTSOF) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLAB_OFF;
	    if (bkltmode == BKLTSON) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data & DUAL_LCD_BLAB_ON;
	    C2S_i2cset(PI_B2,
		       current_dual_lcd.lcd_data.addr,
		       current_dual_lcd.lcd_data.gpiob,
		       current_dual_lcd.lcd_B4data,
		       xflag);	    
	    break;
	  }
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      printf("Using wiringPiI2C methods: ...\n");
      i2c_chip[0].did = (int)(current_dual_lcd.lcd_data.addr);
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
      if (dsplymode == DSPLYB4)
	{
	  /* Create a copy for lower nibble's use */
	  i2c_chip[1].did = i2c_chip[0].did;
	  i2c_chip[1].fd  = i2c_chip[0].fd;
	}
      switch(demode)
	{
	case CLRHOME:
	case FSI4TST:
	case FSI8TST:
	case PRELOAD:
	case STRBTST:
	case WDTATST:
	case WLINTST:
	case WMSGTST:
	case W80CTST:
	case WRGRTST:
	  {
	    i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.iodira);
	    i2c_chip[0].data = ALL_GPIO_BITS_OUT;
	    WP_i2cset(i2c_chip[0], xflag);
	    i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.iodirb);
	    i2c_chip[0].data = ALL_GPIO_BITS_OUT;
	    WP_i2cset(i2c_chip[0], xflag);
	    if (dsplymode == DSPLYA8) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLB_OFF;
	    if (dsplymode == DSPLYB4) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLA_OFF;
	    if (bkltmode == BKLTSOF) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLAB_OFF;
	    if (bkltmode == BKLTSON) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data & DUAL_LCD_BLAB_ON;
	    i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpiob);
	    i2c_chip[0].data = current_dual_lcd.lcd_B4data;
	    WP_i2cset(i2c_chip[0], xflag);
	    printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
	    C2S_gpio_export(current_dual_lcd.lcd_rs_gpioA, GPIO_XOUT, xflag);
	    C2S_gpio_export(current_dual_lcd.lcd_e_gpioA, GPIO_XOUT, xflag);
	    C2S_gpio_export(current_dual_lcd.lcd_rs_gpioB, GPIO_XIN, xflag);
	    C2S_gpio_export(current_dual_lcd.lcd_e_gpioB, GPIO_XIN, xflag);
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
	    /* Register Select = instruction register */
	    WP_digitalWrite(current_dual_lcd.lcd_rs_gpioA, GPIO_LOW, xflag);
	    /* Enable disabled */
	    WP_digitalWrite(current_dual_lcd.lcd_e_gpioA, GPIO_LOW, xflag);
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
#ifdef HAVE_WIRINGPI_H
	case FSI8TST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_LcdWriteRegister_TestA(LCD_FUNC_SET_8BIT_INIT, xflag);
	    sleep(3);
	    C2S_LcdWriteRegister_TestA(LCD_FUNC_SET_8BIT_INIT, xflag);
	    C2S_LcdWriteRegister_TestA(LCD_FUNC_SET_8BIT_INIT, xflag);
	    C2S_LcdWriteRegister_TestA(current_dual_lcd.A8bit.lcd_nfb, xflag);
	    C2S_LcdWriteRegister_TestA(LCD_DISPLAY_OFF, xflag);
	    C2S_LcdWriteRegister_TestA(LCD_CLEAR_DISPLAY, xflag);
	    C2S_LcdWriteRegister_TestA(LCD_DISPLAY_ON_CURSOR_BLINK, xflag);
	    C2S_LcdWriteRegister_TestA(LCD_CURSOR_UP_NO_SHIFT, xflag);
	    break;
	  }
	case PRELOAD:
	  {
	    C2S_i2cset(PI_B2,
		       current_dual_lcd.lcd_data.addr,
		       current_dual_lcd.lcd_data.gpioa,
		       0xff,
		       xflag);
	    current_dual_lcd.lcd_B4data = DUAL_LCD_B4_STANDBY;
	    if (dsplymode == DSPLYA8) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLB_OFF;
	    if (dsplymode == DSPLYB4) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLA_OFF;
	    if (bkltmode == BKLTSOF) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data | DUAL_LCD_BLAB_OFF;
	    if (bkltmode == BKLTSON) current_dual_lcd.lcd_B4data = 
					current_dual_lcd.lcd_B4data & DUAL_LCD_BLAB_ON;
	    C2S_i2cset(PI_B2,
		       current_dual_lcd.lcd_data.addr,
		       current_dual_lcd.lcd_data.gpiob,
		       current_dual_lcd.lcd_B4data,
		       xflag);	    
	    echo_any_dual_lcd_status(&current_dual_lcd);
	    C2S_gpio("readall", xflag);
	    sleep(5);
	    C2S_i2cset(PI_B2,
		       current_dual_lcd.lcd_data.addr,
		       current_dual_lcd.lcd_data.gpioa,
		       0x00,
		       xflag);
	    C2S_i2cset(PI_B2,
		       current_dual_lcd.lcd_data.addr,
		       current_dual_lcd.lcd_data.gpiob,
		       0x00,
		       xflag);
	    C2S_gpio("readall", xflag);
	    break;
	  }
	case STRBTST:
	  {
	    if (!tflag) xflag=FALSE;
	    if (dsplymode == DSPLYA8) C2S_LcdStrobe_TestA(xflag);
	    if (dsplymode == DSPLYB4) C2S_LcdStrobe_TestB(xflag);
	    break;
	  }
	case WDTATST:
	  {
	    if (!tflag) xflag=FALSE;
	    if (dsplymode == DSPLYA8)
	      {
		for (i=0; i<16; i++)
		  {
		    j = i +0x30;
		    C2S_LcdWriteData_TestA(j, xflag);
		  }
		/* Register Select = instruction register */
		C2S_gpio_g_write(current_dual_lcd.lcd_rs_gpioA,
				 GPIO_LOW,
				 xflag);
	      }
	    if (dsplymode == DSPLYB4) 
	      {
		for (i=0; i<16; i++)
		  {
		    j = i +0x30;
		    C2S_LcdWriteData_TestB(j, xflag);
		  }
		/* Register Select = instruction register */
		current_dual_lcd.lcd_B4data = 
		  current_dual_lcd.lcd_B4data & DUAL_LCD_RSB_LOW;
		/* Clear data nibble ... */
		current_dual_lcd.lcd_B4data = 
		  current_dual_lcd.lcd_B4data & 0x0f;
		C2S_i2cset(PI_B2,
			   current_dual_lcd.lcd_data.addr,
			   current_dual_lcd.lcd_data.gpiob,
			   current_dual_lcd.lcd_B4data,
			   xflag);
	      }
	    break;
	  }
	case WRGRTST:
	  {
	    if (!tflag) xflag=FALSE;
	    if (dsplymode == DSPLYA8) 
	      {
		C2S_LcdWriteRegister_TestA(LCD_DISPLAY_OFF, xflag);
		C2S_LcdWriteRegister_TestA(LCD_DISPLAY_ON_CURSOR_BLINK, xflag);
		C2S_LcdWriteRegister_TestA(LCD_CLEAR_DISPLAY, xflag);
	      }
	    if (dsplymode == DSPLYB4) 
	      {
		C2S_LcdWriteRegister_TestB(LCD_DISPLAY_OFF_H,
					   LCD_DISPLAY_OFF_L,
					   xflag);
		C2S_LcdWriteRegister_TestB(LCD_DISPLAY_ON_CURSOR_BLINK_H,
					   LCD_DISPLAY_ON_CURSOR_BLINK_L,
					   xflag);
		C2S_LcdWriteRegister_TestB(LCD_CLEAR_DISPLAY_H,
					   LCD_CLEAR_DISPLAY_L,
					   xflag);	      
		/* Clear register nibble ... */
		current_dual_lcd.lcd_B4data = 
		  current_dual_lcd.lcd_B4data & 0x0f;
		C2S_i2cset(PI_B2,
			   current_dual_lcd.lcd_data.addr,
			   current_dual_lcd.lcd_data.gpiob,
			   current_dual_lcd.lcd_B4data,
			   xflag);
	      }
	    break;
	  }
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      if (tflag)
	{
	  /* Extend the high pulse ... */
	  mehminA = current_dual_lcd.A8bit.lcd_ehmin;
	  mehminB = current_dual_lcd.B4bit.lcd_ehmin;
	  current_dual_lcd.A8bit.lcd_ehmin = 50000;
	  current_dual_lcd.B4bit.lcd_ehmin = 50000;
	  /* and wait for 500000 micro-seconds */
	  mwaitA = current_dual_lcd.A8bit.lcd_wait;
	  mwaitB = current_dual_lcd.B4bit.lcd_wait;
	  current_dual_lcd.A8bit.lcd_wait = 500000;
	  current_dual_lcd.B4bit.lcd_wait = 500000;
	}
      switch(demode)
	{
	case FSI4TST:
	  {
	    i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpiob);
	    i2c_chip[1].reg  = i2c_chip[0].reg;
	    for (i=0; i<4; i++) WP_LcdF4TStrobeB(&current_dual_lcd, 
						 i2c_chip[0], 
						 i,
						 xflag);
	    i2c_chip[0].data = LCD_DISPLAY_OFF_H;
	    i2c_chip[1].data = LCD_DISPLAY_OFF_L;
	    WP_LcdWriteRegisterB(&current_dual_lcd, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_CURSOR_BLINK_H;
	    i2c_chip[1].data = LCD_DISPLAY_ON_CURSOR_BLINK_L;
	    WP_LcdWriteRegisterB(&current_dual_lcd, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_CLEAR_DISPLAY_H;
	    i2c_chip[1].data = LCD_CLEAR_DISPLAY_L;
	    WP_LcdWriteRegisterB(&current_dual_lcd, i2c_chip[0], i2c_chip[1], xflag);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT_H;
	    i2c_chip[1].data = LCD_CURSOR_UP_NO_SHIFT_L;
	    WP_LcdWriteRegisterB(&current_dual_lcd, i2c_chip[0], i2c_chip[1], xflag);
	    break;
	  }
	case FSI8TST:
	  {
	    i2c_chip[0].data = LCD_FUNC_SET_8BIT_INIT;
	    WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	    if (!tflag) delay(5); /* >4100 us */
	    else delay(500);
	    WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	    /* Add in an extra delay ... */
	    delayMicroseconds(120); /* >100 */
	    WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	    i2c_chip[0].data = current_dual_lcd.A8bit.lcd_nfb;
	    WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_OFF;
	    WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_CURSOR_BLINK;
	    WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_CLEAR_DISPLAY;
	    WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT;
	    WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	    break;
	  }
	case STRBTST:
	  {
	    if (dsplymode == DSPLYA8) 
	      for (i=0; i<5; i++) WP_LcdStrobeA(&current_dual_lcd, xflag);
	    if (dsplymode == DSPLYB4)
	      {
		i2c_chip[0].data = current_dual_lcd.lcd_B4data;
		for (i=0; i<5; i++) WP_LcdStrobeB(&current_dual_lcd,
						  i2c_chip[0], 
						  xflag);
	      }
	    break;
	  }
	case WDTATST:
	  {
	    if (dsplymode == DSPLYA8) 
	      {
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpioa);
		WP_LcdHomeClearA(&current_dual_lcd, i2c_chip[0], xflag);
		sleep(2);
		for (i=0; i<26; i++)
		  {
		    i2c_chip[0].data = i + 65;
		    WP_LcdWriteDataA(&current_dual_lcd, i2c_chip[0], xflag);
		    sleep(1);
		    if (i == current_dual_lcd.A8bit.lcd_nc)
		      {
			i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED;
			WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		      }
		    if (i == 25)
		      {
			i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT;
			WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		      }
		  }
		sleep(2);
		i2c_chip[0].data = LCD_RETURN_HOME;
		WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		i2c_chip[0].data = 
		  LCD_SET_DDRAM + 
		  (current_dual_lcd.A8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT;
		WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		for (i=0; i<26; i++)
		  {
		    i2c_chip[0].data = i + 97;
		    WP_LcdWriteDataA(&current_dual_lcd, i2c_chip[0], xflag);
		    sleep(1);
		    if (i == current_dual_lcd.A8bit.lcd_nc)
		      {
			i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED;
			WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		      }
		    if (i == 25)
		      {
			i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT;
			WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		      }
		  }
		sleep(2);
		i2c_chip[0].data = LCD_RETURN_HOME;
		WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		i2c_chip[0].data =  LCD_DISPLAY_ON_NO_CURSOR;
		WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		/* Register Select = instruction register */
		WP_digitalWrite(current_dual_lcd.lcd_rs_gpioA, GPIO_LOW, xflag);
	      }
	    if (dsplymode == DSPLYB4)
	      {	    
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpiob);
		i2c_chip[1].reg  = i2c_chip[0].reg;
		WP_LcdHomeClearB(&current_dual_lcd, i2c_chip[0], xflag);
		sleep(2);
		for (i=0; i<26; i++)
		  {
		    i2c_chip[0].data = i + 65;
		    WP_LcdWriteDataB(&current_dual_lcd, i2c_chip[0], xflag);
		    sleep(1);
		    if (i == current_dual_lcd.B4bit.lcd_nc)
		      {
			i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED_H;
			i2c_chip[1].data = LCD_CURSOR_UP_SHIFTED_L;
			WP_LcdWriteRegisterB(&current_dual_lcd, 
					     i2c_chip[0], 
					     i2c_chip[1], 
					     xflag);
		      }
		    if (i == 25)
		      {
			i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT_H;
			i2c_chip[1].data = LCD_DISPLAY_SHIFT_LEFT_L;
			WP_LcdWriteRegisterB(&current_dual_lcd,
					     i2c_chip[0],
					     i2c_chip[1],
					     xflag);
		      }
		  }
		sleep(2);
		i2c_chip[0].data = LCD_RETURN_HOME_H;
		i2c_chip[1].data = LCD_RETURN_HOME_L;
		WP_LcdWriteRegisterB(&current_dual_lcd,
				     i2c_chip[0],
				     i2c_chip[1],
				     xflag);
		bite = LCD_SET_DDRAM + 
		  (current_dual_lcd.B4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		i2c_chip[0].data = (bite & 0xf0) / 0x10;
		i2c_chip[1].data = bite & 0x0f;
		WP_LcdWriteRegisterB(&current_dual_lcd,
				     i2c_chip[0],
				     i2c_chip[1],
				     xflag);
		i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT_H;
		i2c_chip[1].data = LCD_CURSOR_UP_NO_SHIFT_L;
		WP_LcdWriteRegisterB(&current_dual_lcd,
				     i2c_chip[0],
				     i2c_chip[1],
				     xflag);
		for (i=0; i<26; i++)
		  {
		    i2c_chip[0].data = i + 97;
		    WP_LcdWriteDataB(&current_dual_lcd, i2c_chip[0], xflag);
		    sleep(1);
		    if (i == current_dual_lcd.B4bit.lcd_nc)
		      {
			i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED_H;
			i2c_chip[1].data = LCD_CURSOR_UP_SHIFTED_L;
			WP_LcdWriteRegisterB(&current_dual_lcd, 
					     i2c_chip[0], 
					     i2c_chip[1], 
					     xflag);
		      }
		    if (i == 25)
		      {
			i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT_H;
			i2c_chip[1].data = LCD_DISPLAY_SHIFT_LEFT_L;
			WP_LcdWriteRegisterB(&current_dual_lcd,
					     i2c_chip[0],
					     i2c_chip[1],
					     xflag);
		      }
		  }
		sleep(2);
		i2c_chip[0].data = LCD_RETURN_HOME_H;
		i2c_chip[1].data = LCD_RETURN_HOME_L;
		WP_LcdWriteRegisterB(&current_dual_lcd,
				     i2c_chip[0],
				     i2c_chip[1],
				     xflag);
		i2c_chip[0].data =  LCD_DISPLAY_ON_NO_CURSOR_H;
		i2c_chip[1].data =  LCD_DISPLAY_ON_NO_CURSOR_L;
		WP_LcdWriteRegisterB(&current_dual_lcd,
				     i2c_chip[0],
				     i2c_chip[1],
				     xflag);
		/* Register Select = instruction register */
		i2c_chip[0].data =  current_dual_lcd.lcd_B4data & DUAL_LCD_RSB_LOW;
		WP_i2cset(i2c_chip[0], xflag);
	      }
	    break;
	  }
	case WLINTST:
	  {
	    if (dsplymode == DSPLYA8)
	      { 
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpioa);
		WP_LcdHomeClearA(&current_dual_lcd, i2c_chip[0], xflag);
		sleep(1);
		switch(lcdmodel)
		  {
		  case LCM0802:
		    {
		      sprintf(message_buffer, 
			      "0x%02x #1", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[0]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      i2c_chip[0].data = LCD_SET_DDRAM + 
			(current_dual_lcd.A8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		      WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);	    
		      sprintf(message_buffer, 
			      "0x%02x #2", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[1]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      break;
		    }
		  case LCM1604:
		    {
		      sprintf(message_buffer, 
			      "0x%02x LCM1604  #1", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[0]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      i2c_chip[0].data = LCD_SET_DDRAM + 
			(current_dual_lcd.A8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		      WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);	    
		      sprintf(message_buffer, 
			      "0x%02x LCM1604  #2", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[1]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      i2c_chip[0].data = LCD_SET_DDRAM + 
			(current_dual_lcd.A8bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		      WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);	    
		      sprintf(message_buffer, 
			      "0x%02x LCM1604  #3", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[2]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      i2c_chip[0].data = LCD_SET_DDRAM + 
			(current_dual_lcd.A8bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		      WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		      i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT;
		      for (i=0; i<16; i++)
			{
			  delay(100);
			  WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
			}
		      sprintf(message_buffer, 
			      "0x%02x +CSRx16  #4", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[3]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      break;
		    }
		  case LCM2004:
		    {
		      sprintf(message_buffer, 
			      "0x%02x SD LCM2004   #1", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[0]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      i2c_chip[0].data = LCD_SET_DDRAM + 
			(current_dual_lcd.A8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		      WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);	    
		      sprintf(message_buffer, 
			      "0x%02x SD LCM2004   #2", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[1]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      i2c_chip[0].data = LCD_SET_DDRAM + 
			(current_dual_lcd.A8bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		      WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);	    
		      sprintf(message_buffer, 
			      "0x%02x SD LCM2004   #3", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[2]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      i2c_chip[0].data = LCD_SET_DDRAM + 
			(current_dual_lcd.A8bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		      WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		      i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT;
		      for (i=0; i<20; i++)
			{
			  delay(100);
			  WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
			}
		      sprintf(message_buffer, 
			      "0x%02x SD +CSRx20   #4", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[3]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
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
			      current_dual_lcd.A8bit.lcd_ddram_idx[0]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      i2c_chip[0].data = LCD_SET_DDRAM + 
			(current_dual_lcd.A8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		      WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);	    
		      sprintf(message_buffer, 
			      "0x%02x LCM1602 #2", 
			      current_dual_lcd.A8bit.lcd_ddram_idx[1]);
		      WP_LcdWriteMessageA(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      break;
		    }
		  }
		i2c_chip[0].data = LCD_RETURN_HOME;
		WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
		i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR;
		WP_LcdWriteRegisterA(&current_dual_lcd, i2c_chip[0], xflag);
	      }
	    if (dsplymode == DSPLYB4)
	      { 
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpiob);
		WP_LcdHomeClearB(&current_dual_lcd, i2c_chip[0], xflag);
		sleep(1);
		switch(lcdmodel)
		  {
		  case LCM1604:
		    {
		      sprintf(message_buffer, 
			      "0x%02x LCM1604  #1", 
			      current_dual_lcd.B4bit.lcd_ddram_idx[0]);
		      WP_LcdWriteMessageB(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      bite = LCD_SET_DDRAM + 
			(current_dual_lcd.B4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		      i2c_chip[0].data = (bite & 0xf0) / 0x10;
		      i2c_chip[1].data = bite & 0x0f;
		      WP_LcdWriteRegisterB(&current_dual_lcd,
					   i2c_chip[0],
					   i2c_chip[1],
					   xflag);
		      sprintf(message_buffer, 
			      "0x%02x LCM1604  #2", 
			      current_dual_lcd.B4bit.lcd_ddram_idx[1]);
		      WP_LcdWriteMessageB(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      bite = LCD_SET_DDRAM + 
			(current_dual_lcd.B4bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		      i2c_chip[0].data = (bite & 0xf0) / 0x10;
		      i2c_chip[1].data = bite & 0x0f;
		      WP_LcdWriteRegisterB(&current_dual_lcd,
					   i2c_chip[0],
					   i2c_chip[1],
					   xflag);
		      sprintf(message_buffer, 
			      "0x%02x LCM1604  #3", 
			      current_dual_lcd.B4bit.lcd_ddram_idx[2]);
		      WP_LcdWriteMessageB(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      bite = LCD_SET_DDRAM + 
			(current_dual_lcd.B4bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		      i2c_chip[0].data = (bite & 0xf0) / 0x10;
		      i2c_chip[1].data = bite & 0x0f;
		      WP_LcdWriteRegisterB(&current_dual_lcd,
					   i2c_chip[0],
					   i2c_chip[1],
					   xflag);
		      i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT_H;
		      i2c_chip[1].data = LCD_CURSOR_SHIFT_RIGHT_L;
		      for (i=0; i<16; i++)
			{
			  delay(100);
			  WP_LcdWriteRegisterB(&current_dual_lcd,
					       i2c_chip[0],
					       i2c_chip[1],
					       xflag);
			}
		      sprintf(message_buffer, 
			      "0x%02x +CSRx16  #4", 
			      current_dual_lcd.B4bit.lcd_ddram_idx[3]);
		      WP_LcdWriteMessageB(&current_dual_lcd,
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
			      current_dual_lcd.B4bit.lcd_ddram_idx[0]);
		      WP_LcdWriteMessageB(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      bite = LCD_SET_DDRAM + 
			(current_dual_lcd.B4bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		      i2c_chip[0].data = (bite & 0xf0) / 0x10;
		      i2c_chip[1].data = bite & 0x0f;
		      WP_LcdWriteRegisterB(&current_dual_lcd,
					   i2c_chip[0],
					   i2c_chip[1],
					   xflag);
		      sprintf(message_buffer, 
			      "0x%02x LCM1602 #2", 
			      current_dual_lcd.B4bit.lcd_ddram_idx[1]);
		      WP_LcdWriteMessageB(&current_dual_lcd,
					  i2c_chip[0],
					  xflag,
					  message_buffer);
		      break;
		    }
		    i2c_chip[0].data = LCD_RETURN_HOME_H;
		    i2c_chip[1].data = LCD_RETURN_HOME_L;
		    WP_LcdWriteRegisterB(&current_dual_lcd, i2c_chip[0], i2c_chip[1], xflag);
		    i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR_H;
		    i2c_chip[1].data = LCD_DISPLAY_ON_NO_CURSOR_L;
		    WP_LcdWriteRegisterB(&current_dual_lcd, i2c_chip[0], i2c_chip[1], xflag);
		  }
		break;
	      }
	    break;
	  }
	case WMSGTST:
	  {
	    if (dsplymode == DSPLYA8)
	      { 
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpioa);
		WP_LcdHomeClearA(&current_dual_lcd, i2c_chip[0], xflag);
	      }
	    if (dsplymode == DSPLYB4)
	      { 
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpiob);
		WP_LcdHomeClearB(&current_dual_lcd, i2c_chip[0], xflag);
	      }
	    sleep(1);
	    if (dsplymode == DSPLYA8) strcpy(buffer4, "-A8 "); 
	    if (dsplymode == DSPLYB4) strcpy(buffer4, "-B4 "); 
	    switch(lcdmodel)
	      {
	      case LCM0802:
		{
		  message_buffer[8] = NUL;
		  strcat(buffer4, "0802");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM1604:
		{
		  message_buffer[16] = NUL;
		  strcat(buffer4, "LCM1604...  ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  message_buffer[20] = NUL;
		  strcat(buffer4, "LCM2004...      ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  message_buffer[16] = NUL;
		  strcat(buffer4, "LCM1602...  ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      }
	    if (dsplymode == DSPLYA8) WP_LcdWriteMessageA(&current_dual_lcd,
							  i2c_chip[0],
							  xflag,
							  buffer4);
	    if (dsplymode == DSPLYB4) WP_LcdWriteMessageB(&current_dual_lcd,
							  i2c_chip[0],
							  xflag,
							  buffer4);
	    break;
	  }
	case W80CTST:
	  {
	    if (dsplymode == DSPLYA8)
	      { 
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpioa);
		WP_LcdHomeClearA(&current_dual_lcd, i2c_chip[0], xflag);
	      }
	    if (dsplymode == DSPLYB4)
	      { 
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpiob);
		WP_LcdHomeClearB(&current_dual_lcd, i2c_chip[0], xflag);
	      }
	    for (i=0; i<8; i++)
	      {
		for (j=0; j<10; j++)
		  {
		    if (j==0) k = i + 48;
		    else k = j + 48;
		    i2c_chip[0].data = k;
		    if (dsplymode == DSPLYA8)
		      WP_LcdWriteDataA(&current_dual_lcd, i2c_chip[0], xflag);
		    if (dsplymode == DSPLYB4) 
		      WP_LcdWriteDataB(&current_dual_lcd, i2c_chip[0], xflag);
		    sleep(1);
		  }
	      }
	    break;
	  }
	case CLRHOME:
	case WRGRTST:
	  {
	    if (dsplymode == DSPLYA8)
	      { 
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpioa);
		WP_LcdHomeClearA(&current_dual_lcd, i2c_chip[0], xflag);
	      }
	    if (dsplymode == DSPLYB4) 
	      {
		i2c_chip[0].reg  = (int)(current_dual_lcd.lcd_data.gpiob);
		WP_LcdHomeClearB(&current_dual_lcd, i2c_chip[0], xflag);
	      }
	    break;
	  }
	default: break;
	}
      if (tflag)
	{
	  /* Recover stored values ... */
	  current_dual_lcd.A8bit.lcd_ehmin = mehminA;
	  current_dual_lcd.B4bit.lcd_ehmin = mehminB;
	  current_dual_lcd.A8bit.lcd_wait  = mwaitA;
	  current_dual_lcd.B4bit.lcd_wait  = mwaitB;
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  
#ifdef HAVE_WIRINGPI_H
  /* Tidy up ... */
  current_dual_lcd.lcd_B4data = DUAL_LCD_B4_STANDBY;
  if (dsplymode == DSPLYA8) current_dual_lcd.lcd_B4data = 
			      current_dual_lcd.lcd_B4data | DUAL_LCD_BLB_OFF;
  if (dsplymode == DSPLYB4) current_dual_lcd.lcd_B4data = 
			      current_dual_lcd.lcd_B4data | DUAL_LCD_BLA_OFF;
  if (bkltmode == BKLTSOF) current_dual_lcd.lcd_B4data = 
			     current_dual_lcd.lcd_B4data | DUAL_LCD_BLAB_OFF;
  if (bkltmode == BKLTSON) current_dual_lcd.lcd_B4data = 
			     current_dual_lcd.lcd_B4data & DUAL_LCD_BLAB_ON;
  C2S_i2cset(PI_B2,
	     current_dual_lcd.lcd_data.addr,
	     current_dual_lcd.lcd_data.gpiob,
	     current_dual_lcd.lcd_B4data,
	     xflag);	    

  if (i2cmode == WPIMODE)
    {
      C2S_gpio_unexportall(xflag);
    }
#endif /* HAVE_WIRINGPI_H */
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: dual_lcd_i2c4-8bit_demo.c,v $
 * Revision 1.5  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.4  2017/06/26 18:57:16  pi
 * -W80T & -1604 options added
 *
 * Revision 1.3  2017/06/24 20:21:05  pi
 * Interim commit
 *
 * Revision 1.2  2017/06/24 02:48:03  pi
 * Interim commit. Testing 1604 LCD
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.16  2014/11/20 22:10:27  pi
 * -BLn modes added
 *
 * Revision 1.15  2014/11/10 23:02:04  pi
 * Minor tidying up
 *
 * Revision 1.14  2014/11/09 19:29:12  pi
 * Minor revision
 *
 * Revision 1.13  2014/11/06 17:12:54  pi
 * Some tidying up
 *
 * Revision 1.12  2014/11/05 22:58:25  pi
 * -B4 mode mostly working. Some tidying up needed
 *
 * Revision 1.11  2014/11/04 20:57:49  pi
 * -B4 options beginning to work after doubling ehmin
 *
 * Revision 1.10  2014/11/04 15:55:00  pi
 * Beginning to test 4-bit mode
 *
 * Revision 1.9  2014/11/03 19:47:03  pi
 * All common tests now working in -A8 mode
 * SCMVersion 0.01
 *
 * Revision 1.8  2014/11/03 18:03:16  pi
 * Basic options (-F8T, -WDT & -WRT) all working with -A8
 *
 * Revision 1.7  2014/11/01 22:53:55  pi
 * Interim commit. Beginning to clone wiringPi option versions
 *
 * Revision 1.6  2014/11/01 21:50:44  pi
 * Interim commit
 *
 * Revision 1.5  2014/10/29 18:49:58  pi
 * Interim commit
 *
 * Revision 1.4  2014/10/29 16:04:38  pi
 * -P option working for -S mode
 *
 * Revision 1.3  2014/10/28 23:07:02  pi
 * Interim commit
 *
 * Revision 1.2  2014/10/28 20:22:47  pi
 * Interim commit
 *
 * Revision 1.1  2014/10/28 18:47:49  pi
 * Initial revision
 *
 *
 *
 */
