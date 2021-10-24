/* LCD SPI 8-bit (74HC595-based) DEMOnstration code
 * $Id: lcd_spi8b_demo.c,v 1.3 2018/03/31 21:32:45 pi Exp $
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
#ifdef HAVE_WIRINGPISPI_H
#include <wiringPiSPI.h>
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_CONFIG_H */
#include "rcs_scm.h"
#include "parsel.h"
#include "spi_chips.h"
#include "spi_wraps.h"
#include "i2c_wraps.h"
#include "lcd_disptools.h"
/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: lcd_spi8b_demo.c,v $";
static char RCSId[]       = "$Id: lcd_spi8b_demo.c,v 1.3 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.3 $";
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
  LCD2004,   /* -2004                */
#ifdef HAVE_WIRINGPI_H
  FSI8TST,   /* -F8T                 */
  INITGPIO,  /* -IG                  */
  SCROLLR,   /* +S                   */
  STRBTST,   /* -ST                  */
  WDTATST,   /* -WDT                 */
  WRGRTST,   /* -WRT                 */
#ifdef HAVE_WIRINGPISPI_H
  CLRHOME,   /* -CH                  */
  DSPLMSG,   /* -dmsg 's'            */
  WLINTST,   /* -WLT                 */
  WMSGTST,   /* -WMT                 */
#endif /* HAVE_WIRINGPISPI_H */
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

argnames what,which,demode,spimode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]  = "-a";
  arglist[COMMENT]  = "*";
  arglist[ECHO]     = "-E";
  arglist[LCD0802]  = "-0802";
  arglist[LCD1602]  = "-1602";
  arglist[LCD2004]  = "-2004";
#ifdef HAVE_WIRINGPI_H
  arglist[FSI8TST]  = "-F8T";
  arglist[INITGPIO] = "-IG";
  arglist[SCROLLR]  = "+S";
  arglist[STRBTST]  = "-ST";
  arglist[WDTATST]  = "-WDT";
  arglist[WRGRTST]  = "-WRT";
#ifdef HAVE_WIRINGPISPI_H
  arglist[CLRHOME]  = "-CH";
  arglist[DSPLMSG]  = "-dmsg";
  arglist[WLINTST]  = "-WLT";
  arglist[WMSGTST]  = "-WMT";
#endif /* HAVE_WIRINGPISPI_H */
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
char eflag,igflag,oflag,srflag,tflag,vflag,xflag;
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
	    case LCD2004: lcdmodel=LCM2004; break;
#ifdef HAVE_WIRINGPI_H
	    case FSI8TST:  demode=FSI8TST;  break;
	    case INITGPIO: igflag=TRUE;     break;
	    case SCROLLR:  srflag=TRUE;     break;
	    case STRBTST:  demode=STRBTST;  break;
	    case WDTATST:  demode=WDTATST;  break;
	    case WRGRTST:  demode=WRGRTST;  break;
#ifdef HAVE_WIRINGPISPI_H
	    case CLRHOME:  demode=CLRHOME;  spimode=WPIMODE; break;
	    case DSPLMSG:
	      {
		demode=DSPLMSG;
		spimode=WPIMODE;
		strcpy(message_buffer, buffer2);
		break;
	      }
	    case WLINTST:  demode=WLINTST;  spimode=WPIMODE; break;
	    case WMSGTST:  demode=WMSGTST;  spimode=WPIMODE; break;
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;      break;
	    case SYSMODE: spimode=SYSMODE;  break;
	    case TSTMODE: tflag=TRUE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
#ifdef HAVE_WIRINGPI_H
	    case WPIMODE: spimode=WPIMODE; break;
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
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -dmsg 's' {Display a MeSsaGe 'like_this_one'... - implies -W},\n");
  strcat(buffer1, "         +S        {... and Scroll right},\n");
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -E    {Echo command-line arguments},\n");
  strcat(buffer1, "         -0802 {LCD display type 0802},\n");
  strcat(buffer1, "         -1602 {LCD display type 1602 - default},\n");
  strcat(buffer1, "         -2004 {LCD display type 2004},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -IG   {Initialise used gpio's & Gpio readall},\n");
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -CH   {Clear display & return Home - implies -W},\n");
#endif /* HAVE_WIRINGPISPI_H */
  strcat(buffer1, "         -F8T  {Func set init 8-bit Test*},\n");
  strcat(buffer1, "         -ST   {Strobe Test*},\n");
  strcat(buffer1, "         -WDT  {Write Data Test*},\n");
  strcat(buffer1, "         -WRT  {Write Register Test*},\n");
  strcat(buffer1, "               {*NB: LED Test board req'd with -S},\n");
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -WLT  {Write Line info. Test - implies -W},\n");
  strcat(buffer1, "         -WMT  {Write Message Test - implies -W},\n");
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q    {Quiet - default},\n");
#ifndef NO_SUDO_ECHO_TO_SPI
  strcat(buffer1, "         -S    {use System call methods** - default},\n");
#else /* NO_SUDO_ECHO_TO_SPI */
  strcat(buffer1, "         -S    {use System call methods - default},\n");
#endif /* NO_SUDO_ECHO_TO_SPI */
  strcat(buffer1, "         -T    {using LED Test board},\n");
  strcat(buffer1, "         -V    {Verbose},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -W    {use Wiring pi spi methods},\n");
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -X    {eXecute demonstration}.\n");
#ifndef NO_SUDO_ECHO_TO_SPI
  strcat(buffer1, " ** NB: Uses \"sudo echo ...\" to the SPI device!");
#endif /* NO_SUDO_ECHO_TO_SPI */
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
void C2S_SpiLcdStrobe_Test(lcd_spi_8bit any_lcd_spi_8bit,
			   char qx)
{
  sleep(1);
  /* Enable enabled */
  C2S_gpio_g_write(any_lcd_spi_8bit.lcd_e_gpio,
		   GPIO_HIGH,
		   qx);
  sleep(1);
  /* Enable disabled */
  C2S_gpio_g_write(any_lcd_spi_8bit.lcd_e_gpio,
		   GPIO_LOW,
		   qx);
}

void C2S_SpiLcdWriteRegister_Test(lcd_spi_8bit any_lcd_spi_8bit,
				  unsigned int reg,
				  char qx)
{
  C2S_gpio_g_write(any_lcd_spi_8bit.lcd_rs_gpio,
		   GPIO_LOW,
		   qx);
  C2S_sechob2spidev(any_lcd_spi_8bit.lcd_data.spidev,
		    reg,
		    qx);
  C2S_SpiLcdStrobe_Test(any_lcd_spi_8bit, qx);
}

void C2S_SpiLcdWriteData_Test(lcd_spi_8bit any_lcd_spi_8bit,
			      unsigned int data,
			      char qx)
{
  C2S_gpio_g_write(any_lcd_spi_8bit.lcd_rs_gpio,
		   GPIO_HIGH,
		   qx);
  C2S_sechob2spidev(any_lcd_spi_8bit.lcd_data.spidev,
		    data,
		    qx);
  C2S_SpiLcdStrobe_Test(any_lcd_spi_8bit, qx);
}

void WP_SpiLcdStrobe(lcd_spi_8bit *any_lcd_spi_8bit,
		     char qx)
{
  unsigned int lwait;
  char uorm;
  uorm = TRUE;
  /* if ((*any_lcd_spi_8bit).lcd_wait > (*any_lcd_spi_8bit).lcd_ehmin) */
  if ((*any_lcd_spi_8bit).lcd_wait > (*any_lcd_spi_8bit).lcd_elmin)
    lwait = (*any_lcd_spi_8bit).lcd_wait;
  /*  else lwait = (*any_lcd_spi_8bit).lcd_ehmin; */
  else lwait = (*any_lcd_spi_8bit).lcd_elmin;
  if (lwait > 9999)
    {
      uorm = FALSE;
      lwait = lwait/1000;
    }
  WP_digitalWrite((*any_lcd_spi_8bit).lcd_e_gpio, GPIO_HIGH, qx);
  delayMicroseconds((*any_lcd_spi_8bit).lcd_ehmin);
  WP_digitalWrite((*any_lcd_spi_8bit).lcd_e_gpio, GPIO_LOW, qx);
  /* Wait for lcd to complete operation */
  if (uorm) delayMicroseconds(lwait);
  else delay(lwait);
}

#ifdef HAVE_WIRINGPISPI_H
void WP_SpiLcdWriteRegister(lcd_spi_8bit *any_lcd_spi_8bit,
			    unsigned int reg,
			    char qx)
{
  unsigned int old_lcd_wait;
  /* ¬Register Select ... */
  WP_digitalWrite((*any_lcd_spi_8bit).lcd_rs_gpio, GPIO_LOW, qx);
  /* Write REGister command to 74HC595 ... */
  WP_write_byte(*(*any_lcd_spi_8bit).lcd_data.spifd, reg, qx);
  /* Enable 74HC595 outputs ... */
  WP_digitalWrite((*any_lcd_spi_8bit).lcd_data.goe_gpio,
		  GPIO_LOW,
		  qx);
  if (reg == LCD_RETURN_HOME)
    {
      old_lcd_wait = (*any_lcd_spi_8bit).lcd_wait;
      /* (*any_lcd_spi_8bit).lcd_wait = 1600; */
      (*any_lcd_spi_8bit).lcd_wait = (*any_lcd_spi_8bit).lcd_elmax;
    }
  WP_SpiLcdStrobe(any_lcd_spi_8bit, qx);
  /* Reset lcd_wait? */
  if (reg == LCD_RETURN_HOME) (*any_lcd_spi_8bit).lcd_wait = old_lcd_wait;
  /* Tr-state the 74HC595 outputs ... */
  WP_digitalWrite((*any_lcd_spi_8bit).lcd_data.goe_gpio,
		  GPIO_HIGH,
		  qx);
}

void WP_SpiLcdWriteData(lcd_spi_8bit *any_lcd_spi_8bit,
			unsigned int data,
			char qx)
{
  /* data mode ... */ 
  WP_digitalWrite((*any_lcd_spi_8bit).lcd_rs_gpio, GPIO_HIGH, qx);
  /* Write DATA to 74HC595 ... */
  WP_write_byte(*(*any_lcd_spi_8bit).lcd_data.spifd, data, qx);
  /* Enable 74HC595 outputs ... */
  WP_digitalWrite((*any_lcd_spi_8bit).lcd_data.goe_gpio,
		  GPIO_LOW,
		  qx);
  WP_SpiLcdStrobe(any_lcd_spi_8bit, qx);
  /* Tr-state the 74HC595 outputs ... */
  WP_digitalWrite((*any_lcd_spi_8bit).lcd_data.goe_gpio,
		  GPIO_HIGH,
		  qx);
}

void WP_SpiLcdHomeClear(lcd_spi_8bit any_lcd_spi_8bit,
			char qx)
{ 
  unsigned int lregr;
  lregr = LCD_DISPLAY_OFF;
  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
  lregr = LCD_DISPLAY_ON_CURSOR_BLINK;
  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
  lregr = LCD_CLEAR_DISPLAY;
  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
  lregr = LCD_RETURN_HOME;
  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
  lregr = LCD_SET_DDRAM;
  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
  lregr = LCD_CURSOR_UP_NO_SHIFT;
  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
}

void WP_SpiLcdWriteMessage(lcd_spi_8bit any_lcd_spi_8bit,
			   char qx,
			   char *message)
{
  int i;
  unsigned int ldata,lregr;
  char lmsg[33],*lp1;
  strncpy(lmsg, message, 32);
  lmsg[32] = NUL;
  lp1 = lmsg;
  i = 0;
  while (*lp1 != NUL)
    {
      ldata = toascii((int)(*lp1));
      WP_SpiLcdWriteData(&any_lcd_spi_8bit, ldata, qx);
      lp1++;
      i++;
      if (i == any_lcd_spi_8bit.lcd_nc)
	{
	  lregr = LCD_RETURN_HOME;
	  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
	  lregr = 
	    LCD_SET_DDRAM + (any_lcd_spi_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
	}
    }
}

void WP_SpiLcdWriteBuffer(lcd_spi_8bit any_lcd_spi_8bit,
			  char qx,
			  char scroll)
{
  int i,j,k,ks1,ks2,ks3;
  unsigned int ldata,lregr;
  char lscroll,lbuf[LCD_DDRAM_SIZE+1],*lp1;
  lscroll   = scroll;
  strncpy(lbuf, any_lcd_spi_8bit.lcd_buffer, LCD_DDRAM_SIZE);
  lbuf[LCD_DDRAM_SIZE] = NUL;
  lp1 = lbuf;  
  i = 0;
  j = LCD_DDRAM_SIZE / 2;
  ks1 = any_lcd_spi_8bit.lcd_nc;
  ks2 = j + any_lcd_spi_8bit.lcd_nc;
  if (any_lcd_spi_8bit.lcd_type == LCM2004)
    {
      ks2 = ks1 + any_lcd_spi_8bit.lcd_nc;
      ks3 = ks2 + any_lcd_spi_8bit.lcd_nc;
    }
  while (*lp1 != NUL)
    {
      ldata = toascii((int)(*lp1));
      WP_SpiLcdWriteData(&any_lcd_spi_8bit, ldata, qx);
      lp1++;
      i++;
      if (any_lcd_spi_8bit.lcd_type != LCM2004)
	{
	  if (lscroll && ((i == ks1) || (i == ks2)))
	    {
	      lregr = LCD_CURSOR_UP_SHIFTED;
	      WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
	    }
	  if (i == j)
	    {
	      if (lscroll) delay(500);
	      lregr = LCD_RETURN_HOME;
	      WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
	      if (lscroll && (i < ks2))
		{
		  lregr = LCD_CURSOR_UP_NO_SHIFT;
		  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
		}
	      lregr = 
		LCD_SET_DDRAM + (any_lcd_spi_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	      WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
	    }
	  if (lscroll && ((i > ks1) || (i > ks2))) delay(500);	  
	}
      if (any_lcd_spi_8bit.lcd_type == LCM2004) 
	{
	  if (i == ks1)
	    {
	      lregr = 
		LCD_SET_DDRAM + (any_lcd_spi_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	      WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);	      
	    }
	  if (i == ks2)
	    {
	      lregr = 
		LCD_SET_DDRAM + (any_lcd_spi_8bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
	      WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);	      
	    }
	  if (i == ks3)
	    {
	      lregr = 
		LCD_SET_DDRAM + (any_lcd_spi_8bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
	      WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);	      
	      lregr = LCD_CURSOR_SHIFT_RIGHT;
	      for (k=0; k<20; k++)
		{
		  delay(10);
		  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
		}
	    }
	}
    }
  lregr = LCD_RETURN_HOME;
  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);
  lregr = LCD_DISPLAY_ON_NO_CURSOR;
  WP_SpiLcdWriteRegister(&any_lcd_spi_8bit, lregr, qx);  
}

#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char spidev;
  int i,j,k,l;
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPISPI_H
  int *myspifd;
  int wP_SPIfd[2];
  unsigned int spidata,spiregr;
#endif /* HAVE_WIRINGPISPI_H */

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
  eflag    = igflag = srflag = tflag = vflag = xflag = FALSE;
  demode   = INITGPIO; /* Dummy mode */
  lcdmodel = LCM1602;
  spidev   = SPI_DEV00;
  spimode  = SYSMODE;
#ifdef HAVE_WIRINGPISPI_H
  myspifd = &wP_SPIfd[0]; /* == SPI_DEV00 */
  wP_SPIfd[0] = wP_SPIfd[1] = -1; /* ie. as yet undefined */
#endif /* HAVE_WIRINGPISPI_H */
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
	    case LCD2004: lcdmodel=LCM2004; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case FSI8TST:  demode=FSI8TST;  i--; break;
	    case INITGPIO: igflag=TRUE;     i--; break;
	    case SCROLLR:  srflag=TRUE;     i--; break;
	    case STRBTST:  demode=STRBTST;  i--; break;
	    case WDTATST:  demode=WDTATST;  i--; break;
	    case WRGRTST:  demode=WRGRTST;  i--; break;
#ifdef HAVE_WIRINGPISPI_H
	    case CLRHOME:  demode=CLRHOME;  spimode=WPIMODE; i--; break;
	    case DSPLMSG: 
	      {
		demode=DSPLMSG;
		spimode=WPIMODE;
		strcpy(message_buffer, p2);
		break;
	      }
	    case WLINTST:  demode=WLINTST;  spimode=WPIMODE; i--; break;
	    case WMSGTST:  demode=WMSGTST;  spimode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
	    case QUIET:   vflag=FALSE;      i--; break;
	    case SYSMODE: spimode=SYSMODE;  i--; break;
	    case TSTMODE: tflag=TRUE; i--;  break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
#ifdef HAVE_WIRINGPI_H
	    case WPIMODE: spimode=WPIMODE; i--; break;
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
	strcpy(buffer4, "yellow_0802");
	break;
      }
    case LCM2004:
      {
	strcpy(buffer4, "blue_2004");
	break;
      }
    case LCM1602:
    default:
      {
	strcpy(buffer4, "yellow_1602");
	break;
      }
    }

  /* -IG option leads to an early exit after the system call ... */
  if (igflag)
    {
      init_current_spi_8bit(&current_spi_8bit,
			    lcdmodel,
			    22,
			    27,
			    17,
			    18,
			    spidev,
			    buffer4);
      echo_any_spi_8bit_status(&current_spi_8bit);
#ifdef HAVE_WIRINGPI_H
      C2S_gpio_g_mode(current_spi_8bit.lcd_data.goe_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_spi_8bit.lcd_data.scl_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_spi_8bit.lcd_rs_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      C2S_gpio_g_mode(current_spi_8bit.lcd_e_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* Tri-state the 74HC595 outputs ... */
      C2S_gpio_g_write(current_spi_8bit.lcd_data.goe_gpio,
		       GPIO_HIGH,
		       xflag);
      /* 74HC595 Shift register CL inactive ... */
      C2S_gpio_g_write(current_spi_8bit.lcd_data.scl_gpio,
		       GPIO_HIGH,
		       xflag);
      /* Register Select = instruction register */
      C2S_gpio_g_write(current_spi_8bit.lcd_rs_gpio,
		       GPIO_LOW,
		       xflag);
      /* Enable disabled */
      C2S_gpio_g_write(current_spi_8bit.lcd_e_gpio,
		       GPIO_LOW,
		       xflag);
      C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
    }
			
  /* Execute other options */
  /* Initialisation ... */
  init_current_spi_8bit(&current_spi_8bit,
			lcdmodel,
			22,
			27,
			17,
			18,
			spidev,
			buffer4);
  echo_any_spi_8bit_status(&current_spi_8bit);

  if (spimode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      switch(demode)
	{
#ifdef HAVE_WIRINGPI_H
	case FSI8TST:
	case STRBTST:
	case WDTATST:
	case WRGRTST:
	  {
	    if (!tflag) xflag=FALSE;
	    C2S_gpio_g_mode(current_spi_8bit.lcd_data.goe_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_spi_8bit.lcd_data.scl_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_spi_8bit.lcd_rs_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    C2S_gpio_g_mode(current_spi_8bit.lcd_e_gpio,
			    GPIO_MODE_OUT,
			    xflag);
	    /* Enable the 74HC595 outputs ... */
	    C2S_gpio_g_write(current_spi_8bit.lcd_data.goe_gpio,
			     GPIO_LOW,
			     xflag);
	    /* 74HC595 Shift register CL inactive ... */
	    C2S_gpio_g_write(current_spi_8bit.lcd_data.scl_gpio,
			     GPIO_HIGH,
			     xflag);
	    /* Register Select = instruction register */
	    C2S_gpio_g_write(current_spi_8bit.lcd_rs_gpio,
			     GPIO_LOW,
			     xflag);
	    /* Enable disabled */
	    C2S_gpio_g_write(current_spi_8bit.lcd_e_gpio,
			     GPIO_LOW,
			     xflag);
	    break;
	  }
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
  if (spimode == WPIMODE)
    {  
      switch(demode)
	{
	case CLRHOME:
	case DSPLMSG:
	case FSI8TST:
	case STRBTST:
	case WDTATST:
	case WLINTST:
	case WMSGTST:
	case WRGRTST:
	  {
	    printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
	    C2S_gpio_export(current_spi_8bit.lcd_rs_gpio,
			    GPIO_XOUT,
			    xflag);
	    C2S_gpio_export(current_spi_8bit.lcd_e_gpio,
			    GPIO_XOUT,
			    xflag);
	    if (current_spi_8bit.lcd_data.goe_gpio != GPIO_UNASSIGNED)
	      C2S_gpio_export(current_spi_8bit.lcd_data.goe_gpio,
			      GPIO_XOUT,
			      xflag);
	    if (current_spi_8bit.lcd_data.scl_gpio != GPIO_UNASSIGNED)
	      C2S_gpio_export(current_spi_8bit.lcd_data.scl_gpio,
			      GPIO_XOUT,
			      xflag);	      
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
	    C2S_gpio("exports", xflag);
	    if (eflag || vflag) 
	      {
		printf("ls -laF /sys/class/gpio/\n");
		if (xflag) system("ls -laF /sys/class/gpio/");
	      }
	    /* Enable the 74HC595 outputs ... */
	    WP_digitalWrite(current_spi_8bit.lcd_data.goe_gpio,
			    GPIO_LOW,
			    xflag);
	    /* 74HC595 Shift register CL inactive ... */
	    WP_digitalWrite(current_spi_8bit.lcd_data.scl_gpio,
			    GPIO_HIGH,
			    xflag);
	    /* Register Select = instruction register */
	    WP_digitalWrite(current_spi_8bit.lcd_rs_gpio,
			    GPIO_LOW,
			    xflag);
	    /* Enable disabled */
	    WP_digitalWrite(current_spi_8bit.lcd_e_gpio,
			    GPIO_LOW,
			    xflag);
	    break;
	  }
	default: break;
	}
    }
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
      printf("Using wiringPiSPI methods: ...\n");
      if ((wP_SPIfd[0]=wiringPiSPISetup(0, 500000)) < 0)
	{
	  fprintf(stderr, "Could not setup wP_SPIfd[0]!\n");
	  perror("wiringPiSPISetup");
	  exit(EXIT_FAILURE);
	}
      else printf("wP_SPIfd[0] = %d\n", wP_SPIfd[0]);
      if ((wP_SPIfd[1]=wiringPiSPISetup(1, 500000)) < 0)
	{
	  fprintf(stderr, "Could not setup wP_SPIfd[1]!\n");
	  perror("wiringPiSPISetup");
	  exit(EXIT_FAILURE);
	}
      else printf("wP_SPIfd[1] = %d\n", wP_SPIfd[1]);
      if (*myspifd > 0)
	{
	  current_spi_8bit.lcd_data.spifd = myspifd;
	  printf("*current_spi_8bit.lcd_data.spifd = %d\n",
		 *current_spi_8bit.lcd_data.spifd);
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Demonstration ... */
  if (spimode == SYSMODE)
    {
      switch(demode)
	{
#ifdef HAVE_WIRINGPI_H
	case FSI8TST:
	  {
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_FUNC_SET_8BIT_INIT,
					 xflag);
	    sleep(3);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_FUNC_SET_8BIT_INIT,
					 xflag);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_FUNC_SET_8BIT_INIT,
					 xflag);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 current_spi_8bit.lcd_nfb,
					 xflag);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_DISPLAY_OFF,
					 xflag);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_CLEAR_DISPLAY,
					 xflag);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_DISPLAY_ON_CURSOR_BLINK,
					 xflag);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_CURSOR_UP_NO_SHIFT,
					 xflag);
	    break;
	  }
	case STRBTST:
	  {
	    C2S_SpiLcdStrobe_Test(current_spi_8bit, xflag);
	    break;
	  }
	case WDTATST:
	  {
	    for (i=0; i<16; i++)
	      {
		j = i +0x30;
		C2S_SpiLcdWriteData_Test(current_spi_8bit, j, xflag);
	      }
	    break;
	  }
	case WRGRTST:
	  {
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_DISPLAY_OFF,
					 xflag);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_DISPLAY_ON_CURSOR_BLINK,
					 xflag);
	    C2S_SpiLcdWriteRegister_Test(current_spi_8bit,
					 LCD_CLEAR_DISPLAY,
					 xflag);
	    break;
	  }    
#endif /* HAVE_WIRINGPI_H */
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
      if (tflag) current_spi_8bit.lcd_wait = 500000; /* Wait for 500000 micro-seconds */
      switch(demode)
	{
	case CLRHOME:
	  {
	    WP_SpiLcdHomeClear(current_spi_8bit, xflag);
	    break;
	  }
	case DSPLMSG: 
	  {
	    strncpy(current_spi_8bit.lcd_buffer, 
		    message_buffer,
		    current_spi_8bit.lcd_bsize);
	    current_spi_8bit.lcd_buffer[current_spi_8bit.lcd_bsize] = NUL;
	    if (!xflag || eflag || vflag)
	      printf("LCD.BUFFER = %s\n", current_spi_8bit.lcd_buffer);
	    WP_SpiLcdHomeClear(current_spi_8bit, xflag);
	    sleep(1);
	    WP_SpiLcdWriteBuffer(current_spi_8bit, xflag, srflag);
	    break;
	  }
	case FSI8TST:
	  {
	    spiregr = LCD_FUNC_SET_8BIT_INIT;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    if (!tflag) delayMicroseconds(500);
	    else delay(500);	    
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    spiregr = current_spi_8bit.lcd_nfb;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    spiregr = LCD_DISPLAY_OFF;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    spiregr = LCD_CLEAR_DISPLAY;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    spiregr = LCD_DISPLAY_ON_CURSOR_BLINK;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    spiregr = LCD_CURSOR_UP_NO_SHIFT;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    break;
	  }
	case STRBTST:
	  {
	    WP_SpiLcdStrobe(&current_spi_8bit, xflag);
	    break;
	  }
	case WDTATST:
	  {
	    WP_SpiLcdHomeClear(current_spi_8bit, xflag);
	    sleep(2);
	    for (i=0; i<26; i++)
	      {
		spidata = i + 65;
		WP_SpiLcdWriteData(&current_spi_8bit, spidata, xflag);
		sleep(1);
		if (i == current_spi_8bit.lcd_nc)
		  {
		    spiregr = LCD_CURSOR_UP_SHIFTED;
		    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
		  }
		if (i == 25)
		  {
		    spiregr = LCD_DISPLAY_SHIFT_LEFT;
		    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
		  }
	      }
	    sleep(2);
	    spiregr = LCD_RETURN_HOME;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    spiregr = 
	      LCD_SET_DDRAM + (current_spi_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    spiregr = LCD_CURSOR_UP_NO_SHIFT;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    for (i=0; i<26; i++)
	      {
		spidata = i + 97;
		WP_SpiLcdWriteData(&current_spi_8bit, spidata, xflag);
		sleep(1);
		if (i == current_spi_8bit.lcd_nc)
		  {
		    spiregr = LCD_CURSOR_UP_SHIFTED;
		    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
		  }
		if (i == 25)
		  {
		    spiregr = LCD_DISPLAY_SHIFT_LEFT;
		    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
		  }
	      }
	    sleep(2);
	    spiregr = LCD_RETURN_HOME;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
	    spiregr = LCD_DISPLAY_ON_NO_CURSOR;
	    WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);	    
	    break;
	  }
	case WLINTST:
	  {
	    WP_SpiLcdHomeClear(current_spi_8bit, xflag);
	    sleep(1);
	    switch (lcdmodel)
	      {
	      case LCM0802:
		{
		  sprintf(message_buffer, 
			  "0x%02x #1", 
			  current_spi_8bit.lcd_ddram_idx[0]);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					message_buffer);
		  spiregr = LCD_SET_DDRAM + 
		    (current_spi_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x #2", 
			  current_spi_8bit.lcd_ddram_idx[1]);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					message_buffer);
		  break;
		}
	      case LCM2004:
		{
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #1", 
			  current_spi_8bit.lcd_ddram_idx[0]);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					message_buffer);
		  spiregr = LCD_SET_DDRAM + 
		    (current_spi_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #2", 
			  current_spi_8bit.lcd_ddram_idx[1]);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					message_buffer);
		  spiregr = LCD_SET_DDRAM + 
		    (current_spi_8bit.lcd_ddram_idx[2] & LCD_DDRAM_ADDR_MASK);
		  WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x SD LCM2004   #3", 
			  current_spi_8bit.lcd_ddram_idx[2]);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					message_buffer);
		  spiregr = LCD_SET_DDRAM + 
		    (current_spi_8bit.lcd_ddram_idx[3] & LCD_DDRAM_ADDR_MASK);
		  WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);	    
		  spiregr = LCD_CURSOR_SHIFT_RIGHT;
		  for (i=0; i<20; i++)
		    {
		      delay(100);
		      WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);
		    }
		  sprintf(message_buffer, 
			  "0x%02x SD +CSRx20   #4", 
			  current_spi_8bit.lcd_ddram_idx[3]);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					message_buffer);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  sprintf(message_buffer, 
			  "0x%02x LCM1602 #1", 
			  current_spi_8bit.lcd_ddram_idx[0]);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					message_buffer);
		  spiregr = LCD_SET_DDRAM + 
		    (current_spi_8bit.lcd_ddram_idx[1] & LCD_DDRAM_ADDR_MASK);
		  WP_SpiLcdWriteRegister(&current_spi_8bit, spiregr, xflag);	    
		  sprintf(message_buffer, 
			  "0x%02x LCM1602 #2", 
			  current_spi_8bit.lcd_ddram_idx[1]);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					message_buffer);
		  break;
		}
	      }
	    break;
	  }
	case WMSGTST:
	  {
	    WP_SpiLcdHomeClear(current_spi_8bit, xflag);
	    sleep(1);
	    switch(lcdmodel)
	      {
	      case LCM0802:
		{
		  message_buffer[8] = NUL;
		  strcpy(buffer4, "LCM0802 ");
		  strcat(buffer4, message_buffer);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					buffer4);
		  break;
		}
	      case LCM2004:
		{
		  message_buffer[20] = NUL;
		  strcpy(buffer4, "LCM2004...          ");
		  strcat(buffer4, message_buffer);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					buffer4);
		  break;
		}
	      case LCM1602:
	      default:
		{
		  message_buffer[16] = NUL;
		  strcpy(buffer4, "LCM1602...      ");
		  strcat(buffer4, message_buffer);
		  WP_SpiLcdWriteMessage(current_spi_8bit,
					xflag,
					buffer4);
		  break;
		}
	      }
	    break;
	  }
	case WRGRTST:
	  {
	    WP_SpiLcdWriteRegister(&current_spi_8bit,
				   LCD_DISPLAY_OFF,
				   xflag);
	    WP_SpiLcdWriteRegister(&current_spi_8bit,
				   LCD_DISPLAY_ON_CURSOR_BLINK,
				   xflag);
	    WP_SpiLcdWriteRegister(&current_spi_8bit,
				   LCD_CLEAR_DISPLAY,
				   xflag);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */

  if (spimode == SYSMODE)
    {
#ifdef HAVE_WIRINGPI_H
      /* Tri-state the 74HC595 outputs ... */
      C2S_gpio_g_write(current_spi_8bit.lcd_data.goe_gpio,
		       GPIO_HIGH,
		       xflag);
      /* 74HC595 Shift register CL active ... */
      C2S_gpio_g_write(current_spi_8bit.lcd_data.scl_gpio,
		       GPIO_LOW,
		       xflag);
      /* Register Select = instruction register */
      C2S_gpio_g_write(current_spi_8bit.lcd_rs_gpio,
		       GPIO_LOW,
		       xflag);
      /* Enable disabled */
      C2S_gpio_g_write(current_spi_8bit.lcd_e_gpio,
		       GPIO_LOW,
		       xflag);
      if (eflag || vflag) C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  if (spimode == WPIMODE)
    {
      /* Tr-state the 74HC595 outputs ... */
      WP_digitalWrite(current_spi_8bit.lcd_data.goe_gpio,
		      GPIO_HIGH,
		      xflag);
      /* 74HC595 Shift register CL active ... */
      WP_digitalWrite(current_spi_8bit.lcd_data.scl_gpio,
		      GPIO_LOW,
		      xflag);
      /* Register Select = instruction register */
      WP_digitalWrite(current_spi_8bit.lcd_rs_gpio,
		      GPIO_LOW,
		      xflag);
      /* Enable disabled */
      WP_digitalWrite(current_spi_8bit.lcd_e_gpio,
		      GPIO_LOW,
		      xflag);
      if (eflag || vflag) 
	{
	  printf("ls -laF /sys/class/gpio/\n");
	  if (xflag) system("ls -laF /sys/class/gpio/");
	}
      C2S_gpio_unexportall(xflag);
      if (wP_SPIfd[0] > 0)
	{
	  if (close(wP_SPIfd[0]) < 0) perror("close");
	}
      if (wP_SPIfd[1] > 0)
	{
	  if (close(wP_SPIfd[1]) < 0) perror("close");
	}
      if (eflag || vflag) 
	{
	  printf("ls -laF /sys/class/gpio/\n");
	  if (xflag) system("ls -laF /sys/class/gpio/");
	  C2S_gpio("readall", xflag);
	}
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: lcd_spi8b_demo.c,v $
 * Revision 1.3  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.2  2017/08/14 19:46:32  pi
 * sudo echo --> printf "octal" workaround now in force (see ../common/spi_wraps.c)
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.19  2014/10/11 21:52:29  pi
 * Revised strobe timings & mechanism
 *
 * Revision 1.18  2014/09/06 21:56:02  pi
 * Strobe delay bug-fix
 *
 * Revision 1.17  2014/08/28 21:56:26  pi
 * SCMVersion 0.02
 *
 * Revision 1.16  2014/08/28 20:03:14  pi
 * Interim commit. Strobe/WriteRegister timing logic/mechanism improved/fixed
 *
 * Revision 1.15  2014/08/27 18:44:50  pi
 * Strobe delay bug-fixes
 *
 * Revision 1.14  2014/08/20 19:59:44  pi
 * Interim commit
 *
 * Revision 1.13  2014/08/19 22:23:49  pi
 * Interim commit
 *
 * Revision 1.12  2014/08/19 18:54:13  pi
 * Some tidying up of conditionally compiled sections
 *
 * Revision 1.11  2014/08/02 16:31:45  pi
 * Minor message bug-fix
 *
 * Revision 1.10  2014/08/02 13:27:02  pi
 * All main options working
 * SCMVersion 0.01
 *
 * Revision 1.9  2014/08/01 21:05:00  pi
 * Interim commit. Most of the major options now working
 *
 * Revision 1.8  2014/07/30 22:08:53  pi
 * Interim commit
 *
 * Revision 1.7  2014/07/30 21:27:08  pi
 * Interim commit
 *
 * Revision 1.6  2014/07/28 22:14:16  pi
 * Interim commit
 *
 * Revision 1.5  2014/07/27 18:40:50  pi
 * Interim Commit
 *
 * Revision 1.4  2014/07/27 18:12:55  pi
 * Interim Commit
 *
 * Revision 1.3  2014/07/23 21:53:24  pi
 * Interim commit
 *
 * Revision 1.2  2014/07/22 21:54:05  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:13:46  pi
 * Initial revision
 *
 * Revision 1.4  2014/07/14 21:26:51  pi
 * Interim commit
 *
 * Revision 1.3  2014/07/13 21:42:52  pi
 * Interim commit
 *
 * Revision 1.2  2014/07/13 20:42:44  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/13 20:19:26  pi
 * Initial revision
 *
 *
 *
 */
