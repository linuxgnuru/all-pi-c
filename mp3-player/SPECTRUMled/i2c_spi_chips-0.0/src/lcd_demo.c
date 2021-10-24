/* LCD DEMOnstration code
 * $Id: lcd_demo.c,v 1.4 2018/04/04 20:11:43 pi Exp $
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
static char RCSDate[]     = "$Date: 2018/04/04 20:11:43 $";
static char RCSFile[]     = "$RCSfile: lcd_demo.c,v $";
static char RCSId[]       = "$Id: lcd_demo.c,v 1.4 2018/04/04 20:11:43 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.4 $";
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
  ECHO,      /* -E                   */
  INQUIRE,   /* -I                   */
  I2CADDR,   /* -i2c 0x??            */
  ICISAP,    /* -AP                  */
  LCD0802,   /* -0802                */
  LCD1602,   /* -1602                */
  LCD1604,   /* -1604                */
  LCD2004,   /* -2004                */
#ifdef HAVE_WIRINGPI_H
  CLRHOME,   /* -CH                  */
  DSPLMSG,   /* -dmsg 's'            */
  FSI8TST,   /* -F8T                 */
  INQGPIO,   /* -IG                  */
  PRELOAD,   /* -P                   */
  SCROLLR,   /* +S                   */
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
#ifdef HAVE_WIRINGPII2C_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPII2C_H */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode,i2cmode,ictype;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[INQUIRE]  = "-I";
  arglist[I2CADDR]  = "-i2c";
  arglist[ICISAP]   = "-AP";
  arglist[LCD0802]  = "-0802";
  arglist[LCD1602]  = "-1602";
  arglist[LCD1604]  = "-1604";
  arglist[LCD2004]  = "-2004";
#ifdef HAVE_WIRINGPI_H
  arglist[CLRHOME]  = "-CH";
  arglist[DSPLMSG]  = "-dmsg";
  arglist[FSI8TST]  = "-F8T";
  arglist[INQGPIO]  = "-IG";
  arglist[PRELOAD]  = "-P";
  arglist[SCROLLR]  = "+S";
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
#ifdef HAVE_WIRINGPII2C_H
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
char eflag,igflag,iqflag,oflag,srflag,tflag,vflag,xflag;
char i2caddr[7];
char message_buffer[BUF_LEN];
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
	    case ECHO:    eflag=TRUE;       break;
	    case INQUIRE: iqflag=TRUE;      break;
	    case I2CADDR: strncpy(i2caddr, buffer2, 6); i2ca=0; break;
	    case ICISAP:  ictype=ICISAP;    break;
	    case LCD0802: lcdmodel=LCM0802; break;
	    case LCD1602: lcdmodel=LCM1602; break;
	    case LCD1604: lcdmodel=LCM1604; break;
	    case LCD2004: lcdmodel=LCM2004; break;
#ifdef HAVE_WIRINGPI_H
	    case CLRHOME: demode=CLRHOME; i2cmode=WPIMODE; break;
	    case DSPLMSG:
	      {
		demode=DSPLMSG;
		i2cmode=WPIMODE;
		strcpy(message_buffer, buffer2);
		break;
	      }
	    case FSI8TST: demode=FSI8TST; break;
	    case INQGPIO: igflag=TRUE;    iqflag=TRUE; break;
	    case PRELOAD: demode=PRELOAD; break;
	    case SCROLLR: srflag=TRUE;    break;
	    case STRBTST: demode=STRBTST; break;
	    case WDTATST: demode=WDTATST; break;
	    case WLINTST: demode=WLINTST; i2cmode=WPIMODE; break;
	    case WMSGTST: demode=WMSGTST; i2cmode=WPIMODE; break;
	    case WRGRTST: demode=WRGRTST; break;
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;     break;
	    case SYSMODE: i2cmode=SYSMODE; break;
	    case TSTMODE: tflag=TRUE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPII2C_H
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
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -dmsg 's' {Display a MeSsaGe 'like_this_one'... - implies -W},\n");
  strcat(buffer1, "         +S        {... and Scroll right},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -i2c 0x?? {set I2C device address (>0x00)},\n");
  strcat(buffer1, "                   {defaults: PCF8574P=0x26, PCF8574AP=0x3e},\n");
  strcat(buffer1, "         -AP       {use PCF8574AP default},\n");
  strcat(buffer1, "         -E    {Echo command-line arguments},\n");
  strcat(buffer1, "         -I    {Inquire about i2c devices},\n");
  strcat(buffer1, "         -0802 {LCD display type 0802},\n");
  strcat(buffer1, "         -1602 {LCD display type 1602 - default},\n");
  strcat(buffer1, "         -1604 {LCD display type 1604},\n");
  strcat(buffer1, "         -2004 {LCD display type 2004},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -CH   {Clear display & return Home - implies -W},\n");
  strcat(buffer1, "         -F8T  {Func set init 8-bit Test*},\n");
  strcat(buffer1, "         -IG   {Inquire about i2c devices & Gpio readall},\n");
  strcat(buffer1, "         -P    {Preload lcd iface},\n");
  strcat(buffer1, "         -ST   {Strobe Test*},\n");
  strcat(buffer1, "         -WDT  {Write Data Test*},\n");
  strcat(buffer1, "         -WLT  {Write Line info. Test - implies -W},\n");
  strcat(buffer1, "         -WMT  {Write Message Test - implies -W},\n");
  strcat(buffer1, "         -WRT  {Write Register Test*},\n");
  strcat(buffer1, "               {*NB: LED Test board req'd with -S},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q    {Quiet - default},\n");
  strcat(buffer1, "         -S    {use System call methods - default},\n");
  strcat(buffer1, "         -T    {using LED Test board},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -W    {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
void C2S_LcdStrobe_Test(char qx)
{
  sleep(1);
  C2S_gpio_g_write(yellow_1602.lcd_e_gpio,
		   GPIO_HIGH,
		   qx);	    
  sleep(1);
  C2S_gpio_g_write(yellow_1602.lcd_e_gpio,
		   GPIO_LOW,
		   qx);	    
  yellow_1602.lcd_data.qiop = ALL_GPIO_BITS_HIGH;
  C2S_i2cout(PI_B2,
	     yellow_1602.lcd_data.addr,
	     yellow_1602.lcd_data.qiop,
	     qx);
}

void C2S_LcdWriteRegister_Test(unsigned int reg,
			       char qx)
{
  C2S_gpio_g_write(yellow_1602.lcd_rs_gpio,
		   GPIO_LOW,
		   qx);
  yellow_1602.lcd_data.qiop = reg;
  C2S_i2cout(PI_B2,
	     yellow_1602.lcd_data.addr,
	     yellow_1602.lcd_data.qiop,
	     qx);
  C2S_LcdStrobe_Test(qx);
}

void C2S_LcdWriteData_Test(unsigned int data,
			   char qx)
{
  C2S_gpio_g_write(yellow_1602.lcd_rs_gpio,
		   GPIO_HIGH,
		   qx);
  yellow_1602.lcd_data.qiop = data;
  C2S_i2cout(PI_B2,
	     yellow_1602.lcd_data.addr,
	     yellow_1602.lcd_data.qiop,
	     qx);
  C2S_LcdStrobe_Test(qx);
}

void WP_LcdStrobe(lcd_iface_8bit *any_8bit,
		  char qx)
{
  unsigned int lwait;
  char uorm;
  uorm = TRUE;
  /* if ((*any_8bit).lcd_wait > (*any_8bit).lcd_ehmin) */
  if ((*any_8bit).lcd_wait > (*any_8bit).lcd_elmin)
    lwait = (*any_8bit).lcd_wait;
  /* else lwait = (*any_8bit).lcd_ehmin; */
  else lwait = (*any_8bit).lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  WP_digitalWrite((*any_8bit).lcd_e_gpio, GPIO_HIGH, qx);
  delayMicroseconds((*any_8bit).lcd_ehmin);
  WP_digitalWrite((*any_8bit).lcd_e_gpio, GPIO_LOW, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

void WP_LcdWriteRegister(lcd_iface_8bit *any_8bit,
			 wpi2cvb reg,
			 char qx)
{
  unsigned int old_lcd_wait;
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  lreg.reg  = reg.reg; /* Dummy - not used by WP_i2cout! */
  lreg.data = ALL_GPIO_BITS_HIGH;
  WP_digitalWrite((*any_8bit).lcd_rs_gpio, GPIO_LOW, qx);
  WP_i2cout(reg, qx);
  if (reg.data == LCD_RETURN_HOME) 
    {
      old_lcd_wait = (*any_8bit).lcd_wait;
      /* (*any_8bit).lcd_wait = 1600; */
      (*any_8bit).lcd_wait = (*any_8bit).lcd_elmax;
    }
  WP_LcdStrobe(any_8bit, qx);
  /* Reset lcd_wait? */
  if (reg.data == LCD_RETURN_HOME) (*any_8bit).lcd_wait = old_lcd_wait;
  /* WP_i2cout(lreg, qx); */
}

void WP_LcdWriteData(lcd_iface_8bit *any_8bit,
		     wpi2cvb data,
		     char qx)
{
  wpi2cvb ldata;
  ldata.did  = data.did;
  ldata.fd   = data.fd;
  ldata.reg  = data.reg; /* Dummy - not used by WP_i2cout! */
  ldata.data = ALL_GPIO_BITS_HIGH;
  WP_digitalWrite((*any_8bit).lcd_rs_gpio, GPIO_HIGH, qx);  
  WP_i2cout(data, qx);
  WP_LcdStrobe(any_8bit, qx);
  /* WP_i2cout(ldata, qx); */
}

void WP_LcdHomeClear(lcd_iface_8bit *any_8bit,
		     wpi2cvb data,
		     char qx)
{ 
  wpi2cvb ldata;
  ldata.did  = data.did;
  ldata.fd   = data.fd;
  ldata.reg  = data.reg;
  ldata.data = LCD_DISPLAY_OFF;
  WP_LcdWriteRegister(any_8bit, ldata, qx);
  ldata.data = LCD_DISPLAY_ON_CURSOR_BLINK;
  WP_LcdWriteRegister(any_8bit, ldata, qx);
  ldata.data = LCD_CLEAR_DISPLAY;
  WP_LcdWriteRegister(any_8bit, ldata, qx);	    
  ldata.data = LCD_RETURN_HOME;
  WP_LcdWriteRegister(any_8bit, ldata, qx);
  ldata.data = LCD_SET_DDRAM;
  WP_LcdWriteRegister(any_8bit, ldata, qx);
  ldata.data = LCD_CURSOR_UP_NO_SHIFT;
  WP_LcdWriteRegister(any_8bit, ldata, qx);	    
}

void WP_LcdWriteMessage(lcd_iface_8bit *any_8bit,
			wpi2cvb data,
			char qx,
			char *message)
{
  int i;
  wpi2cvb ldata;
  char lmsg[33],*lp1;
  ldata.did = data.did;
  ldata.fd  = data.fd;
  ldata.reg = data.reg;
  strncpy(lmsg, message, 32);
  lmsg[32] = NUL;
  lp1 = lmsg;
  i = 0;
  while (*lp1 != NUL)
    {
      ldata.data = toascii((int)(*lp1));
      WP_LcdWriteData(any_8bit, ldata, qx);
      lp1++;
      i++;
      if (i == (*any_8bit).lcd_nc)
	{
	  ldata.data = LCD_RETURN_HOME;
	  WP_LcdWriteRegister(any_8bit, ldata, qx);
	  ldata.data = LCD_SET_DDRAM + ((*any_8bit).lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	  WP_LcdWriteRegister(any_8bit, ldata, qx);
	}
    }
}

void WP_LcdWriteBuffer(lcd_iface_8bit any_8bit,
		       wpi2cvb data,
		       char qx,
		       char scroll)
{
  int i,j,k,ks1,ks2,ks3,ks4;
  wpi2cvb ldata;
  char lscroll,lbuf[LCD_DDRAM_SIZE+1],*lp1;
  ldata.did = data.did;
  ldata.fd  = data.fd;
  ldata.reg = data.reg;
  lscroll   = scroll;
  strncpy(lbuf, any_8bit.lcd_buffer, LCD_DDRAM_SIZE);
  lbuf[LCD_DDRAM_SIZE] = NUL;
  lp1 = lbuf;  
  i = 0;
  j = LCD_DDRAM_SIZE / 2;
  ks1 = any_8bit.lcd_nc;
  ks2 = j + any_8bit.lcd_nc;
  /* if (any_8bit.lcd_type == LCM2004) */
  if ((any_8bit.lcd_type == LCM1604) || (any_8bit.lcd_type == LCM2004)) 
    {
      ks2 = ks1 + any_8bit.lcd_nc;
      ks3 = ks2 + any_8bit.lcd_nc;
      if (any_8bit.lcd_type == LCM2004) ks4 = 20;
      else ks4 = 16;
    }
  while (*lp1 != NUL)
    {
      ldata.data = toascii((int)(*lp1));
      WP_LcdWriteData(&any_8bit, ldata, qx);
      delay(50);
      lp1++;
      i++;
      /*      if (any_8bit.lcd_type != LCM2004) */
      if ((any_8bit.lcd_type != LCM2004) && (any_8bit.lcd_type != LCM1604))
	{
	  if (lscroll && ((i == ks1) || (i == ks2)))
	    {
	      ldata.data = LCD_CURSOR_UP_SHIFTED;
	      WP_LcdWriteRegister(&any_8bit, ldata, qx);
	    }
	  if (i == j)
	    {
	      if (lscroll) delay(500);
	      ldata.data = LCD_RETURN_HOME;
	      WP_LcdWriteRegister(&any_8bit, ldata, qx);
	      if (lscroll && (i < ks2))
		{
		  ldata.data = LCD_CURSOR_UP_NO_SHIFT;
		  WP_LcdWriteRegister(&any_8bit, ldata, qx);
		}
	      ldata.data = LCD_SET_DDRAM + 
		(any_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	      WP_LcdWriteRegister(&any_8bit, ldata, qx);
	    }
	  if (lscroll && ((i > ks1) || (i > ks2))) delay(500);
	}
      /*     if (any_8bit.lcd_type == LCM2004) */
      if ((any_8bit.lcd_type == LCM1604) || (any_8bit.lcd_type == LCM2004)) 
	{
	  if (i == ks1)
	    {
	      ldata.data =
		LCD_SET_DDRAM + (any_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	      WP_LcdWriteRegister(&any_8bit, ldata, qx);
	    }
	  if (i == ks2)
	    {
	      ldata.data =
		LCD_SET_DDRAM + (any_8bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
	      WP_LcdWriteRegister(&any_8bit, ldata, qx);
	    }
	  if (i == ks3)
	    {
	      ldata.data =
		LCD_SET_DDRAM + (any_8bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
	      WP_LcdWriteRegister(&any_8bit, ldata, qx);
	      ldata.data = LCD_CURSOR_SHIFT_RIGHT;
	      /* for (k=0; k<20; k++) */
	      for (k=0; k<ks4; k++)
		{
		  delay(10);
		  WP_LcdWriteRegister(&any_8bit, ldata, qx);
		}
	    }
	}
    }
  ldata.data = LCD_RETURN_HOME;
  WP_LcdWriteRegister(&any_8bit, ldata, qx);
  ldata.data = LCD_DISPLAY_ON_NO_CURSOR;
  WP_LcdWriteRegister(&any_8bit, ldata, qx);	    
}

#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char sc,*p1,*p2;
  char dateandtimeis[DATEANDTIME_BUF_LEN],timenowis[TIMENOW_BUF_LEN];
  int i,j,k,l,mca,mcb,mcc,mcd;
  int data[16];
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
  demode   = LAST_ARG;
  i2ca     = 0x26; /* > 0, PCF8574P=0x26, PCF8574AP=0x3e */
  ictype   = LAST_ARG;
  i2cmode  = SYSMODE;
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
	    case I2CADDR: strncpy(i2caddr, p2, 6); i2ca=0; break;
	    case ICISAP:  ictype=ICISAP;    i--; break;
	    case LCD0802: lcdmodel=LCM0802; i--; break;
	    case LCD1602: lcdmodel=LCM1602; i--; break;
	    case LCD1604: lcdmodel=LCM1604; i--; break;
	    case LCD2004: lcdmodel=LCM2004; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case CLRHOME: demode=CLRHOME;  i2cmode=WPIMODE; i--; break;
	    case DSPLMSG: 
	      {
		demode=DSPLMSG;
		i2cmode=WPIMODE;
		strcpy(message_buffer, p2);
		break;
	      }
	    case FSI8TST: demode=FSI8TST;  i--; break;
	    case INQGPIO: igflag= TRUE;    iqflag=TRUE; i--; break;
	    case PRELOAD: demode=PRELOAD;  i--; break;
	    case SCROLLR: srflag=TRUE;     i--; break;
	    case STRBTST: demode=STRBTST;  i--; break;
	    case WDTATST: demode=WDTATST;  i--; break;
	    case WLINTST: demode=WLINTST;  i2cmode=WPIMODE; i--; break;
	    case WMSGTST: demode=WMSGTST;  i2cmode=WPIMODE; i--; break;
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

  if (i2ca == 0)
    {
      sscanf(i2caddr, "%x", &i2ca);
      printf("Setting I2C addr = 0x%02x\n", i2ca);
    }
  else
    {
      if (ictype == ICISAP) i2ca = 0x3e;
    }

  /* -I or -IG options lead to an early exit after the system call ... */
  if (iqflag)
    {
      if (!xflag)
	{
	  mca = init_any_iface_8bit(&blue_1602,
				    LCM1602,
				    27,
				    22,
				    i2ca,
				    "blue_1602");
	  echo_any_iface_8bit_status(&blue_1602);
	  mcb = init_any_iface_8bit(&yellow_1602,
				    LCM1602,
				    27,
				    22,
				    i2ca,
				    "yellow_1602");
	  echo_any_iface_8bit_status(&yellow_1602);
	  mcc = init_any_iface_8bit(&blue_2004,
				    LCM2004,
				    27,
				    22,
				    i2ca,
				    "blue_2004");
	  echo_any_iface_8bit_status(&blue_2004);
	}
      C2S_i2cdetect(PI_B2, xflag);
#ifdef HAVE_WIRINGPI_H
      if (igflag) C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */

  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      /* NB: All tests done in '1602' mode */
      mcb = init_any_iface_8bit(&yellow_1602,
				LCM1602,
				27,
				22,
				i2ca,
				"yellow_1602");
      echo_any_iface_8bit_status(&yellow_1602);
      switch(demode)
	{
#ifdef HAVE_WIRINGPI_H
	case FSI8TST:
	case PRELOAD:
	case STRBTST:
	case WDTATST:
	case WRGRTST:
	  {
	    yellow_1602.lcd_data.qiop = ALL_GPIO_BITS_HIGH;
	    C2S_i2cout(PI_B2,
		       yellow_1602.lcd_data.addr,
		       yellow_1602.lcd_data.qiop,
		       xflag);
	    C2S_gpio_g_mode(yellow_1602.lcd_rs_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    /* Register Select = instruction register */
	    C2S_gpio_g_write(yellow_1602.lcd_rs_gpio,
			    GPIO_LOW,
			    xflag);
	    C2S_gpio_g_mode(yellow_1602.lcd_e_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    /* Enable disabled */
	    C2S_gpio_g_write(yellow_1602.lcd_e_gpio,
			    GPIO_LOW,
			    xflag);
	    if (demode == PRELOAD) C2S_gpio("readall", xflag);
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
      switch(lcdmodel)
	{
	case LCM0802: strcpy(buffer4, "current_0802"); break;
	case LCM1604: strcpy(buffer4, "current_1604"); break;
	case LCM2004: strcpy(buffer4, "current_2004"); break;
	case LCM1602:
	default: strcpy(buffer4, "current_1602"); break;
	}
      mca = init_any_iface_8bit(&current_iface_8bit,
				lcdmodel,
				27,
				22,
				i2ca,
				buffer4);
      echo_any_iface_8bit_status(&current_iface_8bit);
      i2c_chip[0].did = (int)(current_iface_8bit.lcd_data.addr);
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
      switch(demode)
	{
	case CLRHOME:
	case DSPLMSG:
	case FSI8TST:
	case PRELOAD:
	case STRBTST:
	case WDTATST:
	case WLINTST:
	case WMSGTST:
	case WRGRTST:
	  {
	    current_iface_8bit.lcd_data.qiop = ALL_GPIO_BITS_HIGH;
	    i2c_chip[0].data = current_iface_8bit.lcd_data.qiop;
	    WP_i2cout(i2c_chip[0], xflag);	    
	    printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
	    C2S_gpio_export(27, GPIO_XOUT, xflag);
	    C2S_gpio_export(22, GPIO_XOUT, xflag);
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
	    WP_digitalWrite(current_iface_8bit.lcd_rs_gpio, GPIO_LOW, xflag);
	    /* Enable disabled */
	    WP_digitalWrite(current_iface_8bit.lcd_e_gpio, GPIO_LOW, xflag);
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
	    C2S_LcdWriteRegister_Test(LCD_FUNC_SET_8BIT_INIT, xflag);
	    sleep(3);
	    C2S_LcdWriteRegister_Test(LCD_FUNC_SET_8BIT_INIT, xflag);
	    C2S_LcdWriteRegister_Test(LCD_FUNC_SET_8BIT_INIT, xflag);
	    C2S_LcdWriteRegister_Test(yellow_1602.lcd_nfb, xflag);
	    C2S_LcdWriteRegister_Test(LCD_DISPLAY_OFF, xflag);
	    C2S_LcdWriteRegister_Test(LCD_CLEAR_DISPLAY, xflag);
	    C2S_LcdWriteRegister_Test(LCD_DISPLAY_ON_CURSOR_BLINK, xflag);
	    C2S_LcdWriteRegister_Test(LCD_CURSOR_UP_NO_SHIFT, xflag);
	    break;
	  }
	case STRBTST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_LcdStrobe_Test(xflag);
	    break;
	  }
	case WDTATST:
	  {
	    if (!tflag) xflag=FALSE;
	    for (i=0; i<16; i++)
	      {
		j = i +0x30;
		C2S_LcdWriteData_Test(j, xflag);
	      }
	    break;
	  }
	case WRGRTST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_LcdWriteRegister_Test(LCD_DISPLAY_OFF, xflag);
	    C2S_LcdWriteRegister_Test(LCD_DISPLAY_ON_CURSOR_BLINK, xflag);
	    C2S_LcdWriteRegister_Test(LCD_CLEAR_DISPLAY, xflag);
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
      if (tflag) current_iface_8bit.lcd_wait = 500000; /* Wait for 500000 micro-seconds */
      switch(demode)
	{
	case DSPLMSG: 
	  {
	    strncpy(current_iface_8bit.lcd_buffer, 
		    message_buffer,
		    current_iface_8bit.lcd_bsize);
	    current_iface_8bit.lcd_buffer[current_iface_8bit.lcd_bsize] = NUL;
	    if (!xflag || eflag || vflag)
	      printf("LCD.BUFFER = %s\n", current_iface_8bit.lcd_buffer);
	    WP_LcdHomeClear(&current_iface_8bit, i2c_chip[0], xflag);
	    sleep(1);
	    WP_LcdWriteBuffer(current_iface_8bit,
			      i2c_chip[0],
			      xflag,
			      srflag);
	    break;
	  }
	case FSI8TST:
	  {
	    i2c_chip[0].data = LCD_FUNC_SET_8BIT_INIT;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    /* if (!tflag) delayMicroseconds(500); */
	    if (!tflag) delay(5); /* >4100 us */
	    else delay(500);
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    /* Add in an extra delay ... */
	    delayMicroseconds(120); /* >100 */
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    i2c_chip[0].data = current_iface_8bit.lcd_nfb;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_OFF;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_CURSOR_BLINK;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_CLEAR_DISPLAY;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    /* Do this earlier ... */
	    /*  
	    i2c_chip[0].data = LCD_DISPLAY_ON_CURSOR_BLINK;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    */
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    break;
	  }
	case STRBTST:
	  {
	    WP_LcdStrobe(&current_iface_8bit, xflag);
	    break;
	  }
	case WDTATST:
	  {
	    WP_LcdHomeClear(&current_iface_8bit, i2c_chip[0], xflag);
	    sleep(2);
	    for (i=0; i<26; i++)
	      {
		i2c_chip[0].data = i + 65;
		WP_LcdWriteData(&current_iface_8bit, i2c_chip[0], xflag);
		sleep(1);
		if (i == current_iface_8bit.lcd_nc)
		  {
		    i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED;
		    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		  }
		if (i == 25)
		  {
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT;
		    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		  }
	      }
	    sleep(2);
	    i2c_chip[0].data = LCD_RETURN_HOME;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    i2c_chip[0].data = 
	      LCD_SET_DDRAM + (current_iface_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_CURSOR_UP_NO_SHIFT;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
	    for (i=0; i<26; i++)
	      {
		i2c_chip[0].data = i + 97;
		WP_LcdWriteData(&current_iface_8bit, i2c_chip[0], xflag);
		sleep(1);
		if (i == current_iface_8bit.lcd_nc)
		  {
		    i2c_chip[0].data = LCD_CURSOR_UP_SHIFTED;
		    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		  }
		if (i == 25)
		  {
		    i2c_chip[0].data = LCD_DISPLAY_SHIFT_LEFT;
		    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		  }
	      }
	    sleep(2);
	    i2c_chip[0].data = LCD_RETURN_HOME;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
	    break;
	  }
	case WLINTST:
	  {
	    WP_LcdHomeClear(&current_iface_8bit, i2c_chip[0], xflag);
	    sleep(1);
	    switch (lcdmodel)
	      {
	      case LCM0802:
		{
		  sprintf(message_buffer, 
			  "0x%02x #1", 
			  current_iface_8bit.lcd_ddram_idx[0]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  i2c_chip[0].data = LCD_SET_DDRAM + 
		    (current_iface_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x #2", 
			  current_iface_8bit.lcd_ddram_idx[1]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  break;
		}
	      case LCM1604:
		{
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #1", 
			  current_iface_8bit.lcd_ddram_idx[0]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  i2c_chip[0].data = LCD_SET_DDRAM + 
		    (current_iface_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #2", 
			  current_iface_8bit.lcd_ddram_idx[1]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  i2c_chip[0].data = LCD_SET_DDRAM + 
		    (current_iface_8bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x LCM1604  #3", 
			  current_iface_8bit.lcd_ddram_idx[2]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  i2c_chip[0].data = LCD_SET_DDRAM + 
		    (current_iface_8bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		  i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT;
		  for (i=0; i<16; i++)
		    {
		      delay(100);
		      WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		    }
		  sprintf(message_buffer, 
			  "0x%02x +CSRx16  #4", 
			  current_iface_8bit.lcd_ddram_idx[3]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #1", 
			  current_iface_8bit.lcd_ddram_idx[0]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  i2c_chip[0].data = LCD_SET_DDRAM + 
		    (current_iface_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #2", 
			  current_iface_8bit.lcd_ddram_idx[1]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  i2c_chip[0].data = LCD_SET_DDRAM + 
		    (current_iface_8bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #3", 
			  current_iface_8bit.lcd_ddram_idx[2]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  i2c_chip[0].data = LCD_SET_DDRAM + 
		    (current_iface_8bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		  i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT;
		  for (i=0; i<20; i++)
		    {
		      delay(100);
		      WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		    }
		  sprintf(message_buffer, 
			  "0x%02x SD +CSRx20   #4", 
			  current_iface_8bit.lcd_ddram_idx[3]);
		  WP_LcdWriteMessage(&current_iface_8bit,
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
			  current_iface_8bit.lcd_ddram_idx[0]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  i2c_chip[0].data = LCD_SET_DDRAM + 
		    (current_iface_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x LCM1602 #2", 
			  current_iface_8bit.lcd_ddram_idx[1]);
		  WP_LcdWriteMessage(&current_iface_8bit,
				     i2c_chip[0],
				     xflag,
				     message_buffer);
		  break;
		}
	      }
	    i2c_chip[0].data = LCD_RETURN_HOME;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
	    i2c_chip[0].data = LCD_DISPLAY_ON_NO_CURSOR;
	    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);	    
	    break;
	  }
	case WMSGTST:
	  {
	    WP_LcdHomeClear(&current_iface_8bit, i2c_chip[0], xflag);
	    sleep(1);

	    switch(lcdmodel)
	      {
	      case LCM0802:
		{
		  message_buffer[8] = NUL;
		  strcpy(buffer4, "LCM0802 ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM1604:
		{
		  message_buffer[16] = NUL;
		  strcpy(buffer4, "LCM1604...      ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  message_buffer[20] = NUL;
		  strcpy(buffer4, "LCM2004...          ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  message_buffer[16] = NUL;
		  strcpy(buffer4, "LCM1602...      ");
		  strcat(buffer4, message_buffer);
		  break;
		}
	      }
	    WP_LcdWriteMessage(&current_iface_8bit,
			       i2c_chip[0],
			       xflag,
			       buffer4);
	    if ((lcdmodel == LCM2004) || (lcdmodel == LCM1604))
	      {
		i2c_chip[0].data =
		  LCD_SET_DDRAM + (current_iface_8bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		get_date(buffer4);
		WP_LcdWriteMessage(&current_iface_8bit,
				   i2c_chip[0],
				   xflag,
				   buffer4);
		for (j=0; j<30; j++)
		  {
		    i2c_chip[0].data =
		      LCD_SET_DDRAM +
		      (current_iface_8bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		    WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		    if (lcdmodel == LCM2004) k = 20;
		    else k = 16;
		    i2c_chip[0].data = LCD_CURSOR_SHIFT_RIGHT;
		    for (i=0; i<k; i++)
		      {
			delay(2);
			WP_LcdWriteRegister(&current_iface_8bit, i2c_chip[0], xflag);
		      }
		    get_timenow(timenowis);
		    WP_LcdWriteMessage(&current_iface_8bit,
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
	case CLRHOME:
	case WRGRTST:
	  {
	    WP_LcdHomeClear(&current_iface_8bit, i2c_chip[0], xflag);
	    break;
	  }
	default: break;
	}	  
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

  
#ifdef HAVE_WIRINGPI_H
  /* Tidy up ... */
  if (i2cmode == WPIMODE)
    {
      C2S_gpio_unexportall(xflag);
    }
#endif /* HAVE_WIRINGPI_H */
  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: lcd_demo.c,v $
 * Revision 1.4  2018/04/04 20:11:43  pi
 * Interim commit
 *
 * Revision 1.3  2018/04/01 15:12:28  pi
 * Further (partial) -1604 implementation for -dmesg option
 *
 * Revision 1.2  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.5  2018/03/24 22:09:43  pi
 * -1604 option now complete
 *
 * Revision 1.4  2018/03/22 19:03:18  pi
 * Updated -WMT option & (partially?) implemented -1604 option
 *
 * Revision 1.3  2018/03/22 17:24:00  pi
 * Interim commit - WIP!
 *
 * Revision 1.2  2018/03/22 17:02:36  pi
 * Added -AP option
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.11  2014/11/20 20:27:47  pi
 * Use of I2CADDR moved outside of wiringPi dependencies
 *
 * Revision 1.10  2014/11/03 19:47:03  pi
 * Improved (more consistant) use of structure pointers
 * (NB. to be back-ported to other codes)
 *
 * Revision 1.9  2014/11/01 22:53:55  pi
 * Minor tidy up
 *
 * Revision 1.8  2014/10/12 17:39:49  pi
 * Revised strobe timing. PCF8574 no longer reset to 0xff after each
 * command or data byte. SCMVersion 0.05 .
 *
 * Revision 1.7  2014/09/06 21:56:02  pi
 * Strobe delay bug-fix
 *
 * Revision 1.6  2014/08/31 19:21:35  pi
 * Strobe timing improvements back-ported & minor bug-fix
 *
 * Revision 1.5  2014/08/02 13:27:02  pi
 * More minor tidy ups
 *
 * Revision 1.4  2014/08/01 21:05:00  pi
 * Minor tidy up
 *
 * Revision 1.3  2014/07/30 22:08:53  pi
 * Interim commit
 *
 * Revision 1.2  2014/07/28 22:14:16  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:13:46  pi
 * Initial revision
 *
 * Revision 1.2  2014/06/11 10:18:46  pi
 * -i2c 0x?? option added
 * SCMVersion 0.04
 *
 * Revision 1.1  2014/05/26 21:45:35  pi
 * Initial revision
 *
 * Revision 1.23  2014/05/24 22:09:41  pi
 * -WMT option now reports actual hostname
 * SCMVersion 0.02
 *
 * Revision 1.22  2014/05/23 22:24:58  pi
 * Preliminary 0802 support
 * SCMVersion 0.02
 *
 * Revision 1.21  2014/04/20 21:14:12  pi
 * Minor bug-fix
 *
 * Revision 1.20  2014/04/20 15:10:19  pi
 * SCMVersion 0.01
 *
 * Revision 1.19  2014/04/20 15:03:44  pi
 * Interim commit
 *
 * Revision 1.18  2014/04/19 15:33:41  pi
 * Interim commit
 *
 * Revision 1.17  2014/04/18 22:28:42  pi
 * Interim commit
 *
 * Revision 1.16  2014/04/17 22:41:23  pi
 * Interim commit
 *
 * Revision 1.15  2014/04/15 22:01:08  pi
 * Interim commit
 *
 * Revision 1.14  2014/04/12 21:16:10  pi
 * Interim commit
 *
 * Revision 1.13  2014/04/12 19:11:15  pi
 * Interim commit
 *
 * Revision 1.12  2014/04/07 22:24:02  pi
 * Interim commit
 *
 * Revision 1.11  2014/04/04 19:40:50  pi
 * Interim commit
 *
 * Revision 1.10  2014/04/04 19:32:09  pi
 * Interim commit
 *
 * Revision 1.9  2014/03/28 22:46:28  pi
 * Interim commit
 *
 * Revision 1.8  2014/03/27 21:20:38  pi
 * Interim commit
 *
 * Revision 1.7  2014/03/25 21:38:25  pi
 * Interim commit
 *
 * Revision 1.6  2014/03/25 20:18:40  pi
 * Interim commit
 *
 * Revision 1.5  2014/03/24 22:31:49  pi
 * Interim commit
 *
 * Revision 1.4  2014/03/24 16:52:02  pi
 * Interim commit
 *
 * Revision 1.3  2014/03/24 12:19:52  pi
 * Interim commit
 *
 * Revision 1.2  2014/03/21 21:50:27  pi
 * Interim commit
 *
 * Revision 1.1  2014/03/21 18:49:09  pi
 * Initial revision
 *
 *
 */
