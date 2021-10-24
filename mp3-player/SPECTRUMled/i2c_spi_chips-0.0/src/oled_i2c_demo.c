/* 128 x 64 OLED via I2C DEMOnstration code
 * $Id: oled_i2c_demo.c,v 1.4 2018/03/31 21:32:45 pi Exp $
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
#include "oled128.h"
#include "oled256.h"
#include "jace128.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: oled_i2c_demo.c,v $";
static char RCSId[]       = "$Id: oled_i2c_demo.c,v 1.4 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.4 $";
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
  ECHO,      /* -E                   */
  INQUIRE,   /* -I                   */
  CLRHOME,   /* -CH                  */
  INITDSP,   /* -ID                  */
  INVERSE,   /* -inverse             */
  NORMAL,    /* -normal              */
  OLED128,   /* -oled128             */
  OLED256,   /* -oled256             */
  JACE128,   /* -jace128             */
  QUIET,     /* -Q                   */
  RSTCONT,   /* -RC                  */
  SETCON0,   /* -SC0                 */
  SETCON1,   /* -SC1                 */
  SETCON2,   /* -SC2                 */
  SETCON3,   /* -SC3                 */
  SETCON4,   /* -SC4                 */
  SETCON5,   /* -SC5                 */
  SETCON6,   /* -SC6                 */
  SETCON7,   /* -SC7                 */
  SETCON8,   /* -SC8                 */
  SYSMODE,   /* -S                   */
  TSTDISP,   /* -TD                  */
  VERBOSE,   /* -V                   */
  WFBTEST,   /* -WFT                 */
  WMSGTST,   /* -WMT                 */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  WPIMODE,   /* -W                   */
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIO_H
  PIGMODE,   /* -PG                  */
#endif /* HAVE_PIGPIO_H */
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
  arglist[INQUIRE]  = "-I";
  arglist[CLRHOME]  = "-CH";
  arglist[INITDSP]  = "-ID";
  arglist[INVERSE]  = "-inverse";
  arglist[NORMAL]   = "-normal";
  arglist[OLED128]  = "-oled128";
  arglist[OLED256]  = "-oled256";
  arglist[JACE128]  = "-jace128";
  arglist[QUIET]    = "-Q";
  arglist[RSTCONT]  = "-RC";
  arglist[SETCON0]  = "-SC0";
  arglist[SETCON1]  = "-SC1";
  arglist[SETCON2]  = "-SC2";
  arglist[SETCON3]  = "-SC3";
  arglist[SETCON4]  = "-SC4";
  arglist[SETCON5]  = "-SC5";
  arglist[SETCON6]  = "-SC6";
  arglist[SETCON7]  = "-SC7";
  arglist[SETCON8]  = "-SC8";
  arglist[SYSMODE]  = "-S";
  arglist[TSTDISP]  = "-TD";
  arglist[VERBOSE]  = "-V";
  arglist[WFBTEST]  = "-WFT";
  arglist[WMSGTST]  = "-WMT";
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  arglist[WPIMODE]  = "-W";
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
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
char eflag,iqflag,vflag,xflag;
char message_buffer[BUF_LEN];
unsigned char *oledfont;
unsigned int oledcontrast;

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
	    case INQUIRE: iqflag=TRUE;    break;
	    case CLRHOME: demode=CLRHOME;  break;
	    case INITDSP: demode=INITDSP;  break;
	    case INVERSE: demode=INVERSE;  break;
	    case NORMAL:  demode=NORMAL;   break;
	    case OLED128: fontis=OLED128; oledfont=oled128; break;
	    case OLED256: fontis=OLED256; oledfont=oled256; break;
	    case JACE128: fontis=JACE128; oledfont=jace128; break;
	    case QUIET:   vflag=FALSE;     break;
	    case RSTCONT: demode=RSTCONT; oledcontrast=OLED_C4; break;
	    case SETCON0: demode=SETCON0; oledcontrast=OLED_C0; break;
	    case SETCON1: demode=SETCON1; oledcontrast=OLED_C1; break;
	    case SETCON2: demode=SETCON2; oledcontrast=OLED_C2; break;
	    case SETCON3: demode=SETCON3; oledcontrast=OLED_C3; break;
	    case SETCON4: demode=SETCON4; oledcontrast=OLED_C4; break;
	    case SETCON5: demode=SETCON5; oledcontrast=OLED_C5; break;
	    case SETCON6: demode=SETCON6; oledcontrast=OLED_C6; break;
	    case SETCON7: demode=SETCON7; oledcontrast=OLED_C7; break;
	    case SETCON8: demode=SETCON8; oledcontrast=OLED_C8; break;
	    case SYSMODE: i2cmode=SYSMODE; break;
	    case TSTDISP: demode=TSTDISP;  break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
	    case WFBTEST: demode=WFBTEST;  break;
	    case WMSGTST: demode=WMSGTST;  break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE: i2cmode=WPIMODE; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
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
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -E   {Echo command-line arguments},\n");
  strcat(buffer1, "         -I   {Inquire about i2c devices},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -CH  {Clear display & return Home^^},\n");
  strcat(buffer1, "         -ID  {Initialise Display},\n");
  strcat(buffer1, "         -inverse {INVERSE display},\n");
  strcat(buffer1, "         -normal  {NORMAL display - default},\n");
  strcat(buffer1, "         -oled128 {use oled128 font - default},\n");
  strcat(buffer1, "         -oled256 {use oled256 font},\n");
  strcat(buffer1, "         -jace128 {use jace128 font},\n");
  strcat(buffer1, "         -Q   {Quiet - default},\n");
  strcat(buffer1, "         -RC  {Reset Contrast},\n");
  strcat(buffer1, "         -SCn {Set Contrast level n, 0<=n<=8},\n");
  strcat(buffer1, "         -S   {use System call methods - default},\n");
  strcat(buffer1, "         -TD  {Test Display},\n");
  strcat(buffer1, "         -V   {Verbose},\n");
  strcat(buffer1, "         -WFT {Write Font bytes Test^^},\n");
  strcat(buffer1, "         -WMT {Write Message Test^^},\n");
  strcat(buffer1, "              {^^ use with -S or -PG methods only},\n");
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  strcat(buffer1, "         -W   {use Wiring pi i2c methods},\n");
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_PIGPIO_H
  strcat(buffer1, "         -PG  {use PiGpio i2c methods**},\n");
  strcat(buffer1, "              {**NB: requires sudo \"prefix\"! },\n");
#endif /* HAVE_PIGPIO_H */
  strcat(buffer1, "         -X   {eXecute demonstration}.");
  print_us(getRCSFile(TRUE), buffer1);
}

void C2S_WriteSBCommand(i2c_ssd1306_oled *chip,
			char qx)
{
  unsigned int addr,regr,data;
  (*chip).c0dc = OLED_COMMAND;
  addr = (*chip).addr;
  regr = (*chip).c0dc;
  data = (*chip).cmds;
  C2S_i2cset(PI_B2, addr, regr, data, qx);
}

void C2S_WriteDBCommand(i2c_ssd1306_oled *chip,
			char qx)
{
  unsigned int addr,regr,data[16];
  (*chip).c0dc = OLED_COMMAND;
  addr    = (*chip).addr;
  regr    = (*chip).c0dc;
  data[0] = (*chip).cmdd[0];
  data[1] = (*chip).cmdd[1];
  C2S_i2cseti(PI_B2, addr, regr, data, 2, qx);
}

void C2S_WriteTBCommand(i2c_ssd1306_oled *chip,
			char qx)
{
  unsigned int addr,regr,data[16];
  (*chip).c0dc = OLED_COMMAND;
  addr    = (*chip).addr;
  regr    = (*chip).c0dc;
  data[0] = (*chip).cmdt[0];
  data[1] = (*chip).cmdt[1];
  data[2] = (*chip).cmdt[2];
  C2S_i2cseti(PI_B2, addr, regr, data, 3, qx);
}

void C2S_WriteFontBytes(i2c_ssd1306_oled *chip,
			unsigned int nbytes,
			char qx)
{
  unsigned int addr,regr,mbytes,data[16];
  unsigned int i;
  (*chip).c0dc = OLED_DATA;
  addr = (*chip).addr;
  regr = (*chip).c0dc;
  if (nbytes > 8) mbytes = 8;
  else mbytes = nbytes;
  for (i=0; i<mbytes; i++) data[i] = (*chip).cfont[i];
  C2S_i2cseti(PI_B2, addr, regr, data, mbytes, qx);  
}

void C2S_ClearHome(i2c_ssd1306_oled *chip,
		   char qx)
{
  int i;
  (*chip).cmdd[0] = OLED_SET_MEM_ADDR_MODE;
  (*chip).cmdd[1] = OLED_HORIZONTAL_ADDR_MODE;
  C2S_WriteDBCommand(chip, qx);
  (*chip).cmdt[0] = OLED_SET_COLUMN_ADDR;
  (*chip).cmdt[1] = OLED_COLUMN_START_RESET;
  (*chip).cmdt[2] = OLED_COLUMN_END_RESET;
  C2S_WriteTBCommand(chip, qx);
  (*chip).cmdt[0] = OLED_SET_PAGE_ADDR;
  (*chip).cmdt[1] = OLED_PAGE_START_RESET;
  (*chip).cmdt[2] = OLED_PAGE_END_RESET;
  C2S_WriteTBCommand(chip, qx);
  for (i=0; i<8; i++) (*chip).cfont[i] = 0x00;
  for (i=0; i<128; i++) C2S_WriteFontBytes(chip, 8, qx);
  (*chip).cmdt[0] = OLED_SET_COLUMN_ADDR;
  (*chip).cmdt[1] = OLED_COLUMN_START_RESET;
  (*chip).cmdt[2] = OLED_COLUMN_END_RESET;
  C2S_WriteTBCommand(chip, qx);
  (*chip).cmdt[0] = OLED_SET_PAGE_ADDR;
  (*chip).cmdt[1] = OLED_PAGE_START_RESET;
  (*chip).cmdt[2] = OLED_PAGE_END_RESET;
  C2S_WriteTBCommand(chip, qx);
}

void C2S_WriteThisFontBytes(i2c_ssd1306_oled *chip,
			    unsigned int ascii_code,
			    unsigned char *this8x8font,
			    char qx)
{
  unsigned char *colptr;
  char l;
  colptr = this8x8font + (ascii_code * 8);
  for (l=0; l<8; l++)
    {
      (*chip).cfont[l] = *colptr;
      colptr++;
    }
  C2S_WriteFontBytes(chip, 8, qx);
}

void C2S_WriteMessage(i2c_ssd1306_oled *chip,
		      unsigned char *this8x8font,
		      char qx,
		      char *message)
{
  int bite;
  char lmsg[OLED_CHAR_WIDTH+1],*lp1;
  strncpy(lmsg, message, OLED_CHAR_WIDTH);
  lmsg[OLED_CHAR_WIDTH] = NUL;
  lp1 = lmsg;
  while (*lp1 != NUL)
    {
      bite = toascii((int)(*lp1));
      C2S_WriteThisFontBytes(chip, bite, this8x8font, qx);
      lp1++;
    } 
}

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
void WP_WriteSBCommand(i2c_ssd1306_oled *chip,
		       wpi2cvb reg,
		       char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  (*chip).c0dc = OLED_COMMAND;
  lreg.reg  = (*chip).c0dc;
  lreg.data = (*chip).cmds;
  WP_i2cset(lreg, qx);
}

void WP_WriteDBCommand(i2c_ssd1306_oled *chip,
		       wpi2cvb reg,
		       char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  (*chip).c0dc = OLED_COMMAND;
  lreg.reg  = (*chip).c0dc;
  lreg.data = ((*chip).cmdd[1] * 0x100) + (*chip).cmdd[0];
  WP_i2cset2(lreg, qx);
}

/*
void WP_WriteTBCommand(i2c_ssd1306_oled *chip,
		       wpi2cvb reg,
		       char qx)
{
  wpi2cvb lreg;
  lreg.did  = reg.did;
  lreg.fd   = reg.fd;
  (*chip).c0dc = OLED_COMMAND;
  lreg.reg  = (*chip).c0dc;
  lreg.ibytes[0] = (*chip).cmdt[0];
  lreg.ibytes[1] = (*chip).cmdt[1];
  lreg.ibytes[2] = (*chip).cmdt[2];
  WP_i2cseti(lreg, 3, qx); // This does not work as needed 
}
*/
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_PIGPIO_H
void PG_WriteSBCommand(i2c_ssd1306_oled *chip,
		       pgi2cvb pgi2c,
		       char qx)
{
  pgi2cvb lpgi2c;
  lpgi2c.handle = pgi2c.handle;
  (*chip).c0dc  = OLED_COMMAND;
  lpgi2c.i2cReg = (*chip).c0dc;
  lpgi2c.bVal   = (*chip).cmds;
  PG_i2cset(lpgi2c, qx);
}

void PG_WriteDBCommand(i2c_ssd1306_oled *chip,
		       pgi2cvb pgi2c,
		       char qx)
{
  pgi2cvb lpgi2c;
  lpgi2c.handle = pgi2c.handle;
  (*chip).c0dc  = OLED_COMMAND;
  lpgi2c.i2cReg = (*chip).c0dc;
  lpgi2c.wVal   = ((*chip).cmdd[1] * 0x100) + (*chip).cmdd[0];
  PG_i2cset2(lpgi2c, qx);
}

void PG_WriteTBCommand(i2c_ssd1306_oled *chip,
		       pgi2cvb pgi2c,
		       char qx)
{
  pgi2cvb lpgi2c;

  lpgi2c.handle = pgi2c.handle;
  (*chip).c0dc  = OLED_COMMAND;
  lpgi2c.i2cReg = (*chip).c0dc;
  lpgi2c.block_buf[0] = (*chip).cmdt[0];
  lpgi2c.block_buf[1] = (*chip).cmdt[1];
  lpgi2c.block_buf[2] = (*chip).cmdt[2];
  PG_i2cseti(lpgi2c, 3, qx);
}

void PG_WriteFontBytes(i2c_ssd1306_oled *chip,
		       pgi2cvb pgi2c,
		       unsigned int nbytes,
		       char qx)
{
  unsigned int addr,regr,mbytes,data[16];
  unsigned int i;
  pgi2cvb lpgi2c;
  (*chip).c0dc = OLED_DATA;
  addr    = (*chip).addr;
  regr    = (*chip).c0dc;
  if (nbytes > 8) mbytes = 8;
  else mbytes = nbytes;
  lpgi2c.handle = pgi2c.handle;
  lpgi2c.i2cReg = (*chip).c0dc;
  for (i=0; i<mbytes; i++) lpgi2c.block_buf[i] = (*chip).cfont[i];
  PG_i2cseti(lpgi2c, mbytes, qx);
}

void PG_ClearHome(i2c_ssd1306_oled *chip,
		  pgi2cvb pgi2c,
		  char qx)
{
  int i;
  (*chip).cmdd[0] = OLED_SET_MEM_ADDR_MODE;
  (*chip).cmdd[1] = OLED_HORIZONTAL_ADDR_MODE;
  PG_WriteDBCommand(chip, pgi2c, qx);
  (*chip).cmdt[0] = OLED_SET_COLUMN_ADDR;
  (*chip).cmdt[1] = OLED_COLUMN_START_RESET;
  (*chip).cmdt[2] = OLED_COLUMN_END_RESET;
  PG_WriteTBCommand(chip, pgi2c, qx);
  (*chip).cmdt[0] = OLED_SET_PAGE_ADDR;
  (*chip).cmdt[1] = OLED_PAGE_START_RESET;
  (*chip).cmdt[2] = OLED_PAGE_END_RESET;
  PG_WriteTBCommand(chip, pgi2c, qx);
  for (i=0; i<8; i++) (*chip).cfont[i] = 0x00;
  for (i=0; i<128; i++) PG_WriteFontBytes(chip, pgi2c, 8, qx);
  (*chip).cmdt[0] = OLED_SET_COLUMN_ADDR;
  (*chip).cmdt[1] = OLED_COLUMN_START_RESET;
  (*chip).cmdt[2] = OLED_COLUMN_END_RESET;
  PG_WriteTBCommand(chip, pgi2c, qx);
  (*chip).cmdt[0] = OLED_SET_PAGE_ADDR;
  (*chip).cmdt[1] = OLED_PAGE_START_RESET;
  (*chip).cmdt[2] = OLED_PAGE_END_RESET;
  PG_WriteTBCommand(chip, pgi2c, qx);
}

void PG_WriteThisFontBytes(i2c_ssd1306_oled *chip,
			   pgi2cvb pgi2c,
			   unsigned int ascii_code,
			   unsigned char *this8x8font,
			   char qx)
{
  unsigned char *colptr;
  char l;
  colptr = this8x8font + (ascii_code * 8);
  for (l=0; l<8; l++)
    {
      (*chip).cfont[l] = *colptr;
      colptr++;
    }
  PG_WriteFontBytes(chip, pgi2c, 8, qx);
}

void PG_WriteMessage(i2c_ssd1306_oled *chip,
		     pgi2cvb pgi2c,
		     unsigned char *this8x8font,
		     char qx,
		     char *message)
{
  int bite;
  char lmsg[OLED_CHAR_WIDTH+1],*lp1;
  strncpy(lmsg, message, OLED_CHAR_WIDTH);
  lmsg[OLED_CHAR_WIDTH] = NUL;
  lp1 = lmsg;
  while (*lp1 != NUL)
    {
      bite = toascii((int)(*lp1));
      PG_WriteThisFontBytes(chip, pgi2c, bite, this8x8font, qx);
      lp1++;
    } 
}
#endif /* HAVE_PIGPIO_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  int i,j,k,l,mca,mcb;
  int bite,data[16];
  i2c_ssd1306_oled current_oled;
#ifdef HAVE_WIRINGPI_H
  int wPSS;
#ifdef HAVE_WIRINGPII2C_H
  wpi2cvb i2c_chip;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
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
      exit(EXIT_SUCCESS);
    }

  /* Set up defaults */
  eflag        = vflag = xflag = FALSE;
  demode       = LAST_ARG;
  i2cmode      = SYSMODE;
  fontis       = OLED128; 
  oledfont     = oled128;
  oledcontrast = OLED_C4;
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
	    case CLRHOME: demode=CLRHOME;  i--; break;
	    case INITDSP: demode=INITDSP;  i--; break;
	    case INVERSE: demode=INVERSE;  i--; break;
	    case NORMAL:  demode=NORMAL;   i--; break;
	    case OLED128: fontis=OLED128; oledfont=oled128; i--; break;
	    case OLED256: fontis=OLED256; oledfont=oled256; i--; break;
	    case JACE128: fontis=JACE128; oledfont=jace128; i--; break;
	    case QUIET:   vflag=FALSE;     i--; break;
	    case RSTCONT: demode=RSTCONT; oledcontrast=OLED_C4; i--; break;
	    case SETCON0: demode=SETCON0; oledcontrast=OLED_C0; i--; break;
	    case SETCON1: demode=SETCON1; oledcontrast=OLED_C1; i--; break;
	    case SETCON2: demode=SETCON2; oledcontrast=OLED_C2; i--; break;
	    case SETCON3: demode=SETCON3; oledcontrast=OLED_C3; i--; break;
	    case SETCON4: demode=SETCON4; oledcontrast=OLED_C4; i--; break;
	    case SETCON5: demode=SETCON5; oledcontrast=OLED_C5; i--; break;
	    case SETCON6: demode=SETCON6; oledcontrast=OLED_C6; i--; break;
	    case SETCON7: demode=SETCON7; oledcontrast=OLED_C7; i--; break;
	    case SETCON8: demode=SETCON8; oledcontrast=OLED_C8; i--; break;
	    case SYSMODE: i2cmode=SYSMODE; i--; break;
	    case TSTDISP: demode=TSTDISP;  i--; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
	    case WFBTEST: demode=WFBTEST;  i--; break;
	    case WMSGTST: demode=WMSGTST;  i--; break;
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
	    case WPIMODE: i2cmode=WPIMODE; i--; break;
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */
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
	  init_i2c_ssd1306_oled(&current_oled,
				0x3c,
				"KEYES");
	  echo_i2c_ssd1306_oled_status(current_oled);
	}
      C2S_i2cdetect(PI_B2, xflag);
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */

  if (i2cmode == SYSMODE)
    {
      printf("Using 'C' system call-based methods: ...\n");
      init_i2c_ssd1306_oled(&current_oled,
			    0x3c,
			    "KEYES");
      echo_i2c_ssd1306_oled_status(current_oled);
      switch(demode)
	{
	default: break;
	}
    }

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPII2C_H
  if (i2cmode == WPIMODE)
    {
      printf("Using wiringPiI2C methods: ...\n");
      init_i2c_ssd1306_oled(&current_oled,
			    0x3c,
			    "KEYES");
      echo_i2c_ssd1306_oled_status(current_oled);
      i2c_chip.did = (int)(current_oled.addr);
      if ((i2c_chip.fd=wiringPiI2CSetup(i2c_chip.did)) < 0)
	{
	  fprintf(stderr,
		  "Could not setup i2c_chip.did %d!",
		  i2c_chip.did);
	  perror("wiringPiI2CSetup");
	  exit(EXIT_FAILURE);
	}
      else 
	{
	  printf("I2C Device 0x%02x has i2c_chip.fd of %d\n",
		 i2c_chip.did, i2c_chip.fd);
	}
      switch(demode)
	{
	default: break;
	}
    }
#endif /* HAVE_WIRINGPII2C_H */
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_PIGPIO_H
  if (i2cmode == PIGMODE)
    {
      printf("Using pigpio I2C methods:\n");
      init_i2c_ssd1306_oled(&current_oled,
			    0x3c,
			    "KEYES");
      echo_i2c_ssd1306_oled_status(current_oled);
      if ((pigpiovsn = gpioInitialise()) < 0)
	{
	  fprintf(stderr, "Could not initialise pigpio!");
	  perror("gpioInitialise");
	  exit(EXIT_FAILURE);
	}
      else printf("pigpio Version %d initialised ...\n", pigpiovsn);
      pgi2c.i2cBus   = PI_B2;
      pgi2c.i2cAddr  = current_oled.addr;
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
    }
#endif /* HAVE_PIGPIO_H */
  
  switch(fontis)
    {
    case JACE128: oledfont=jace128; break;
    case OLED256: oledfont=oled256; break;
    case OLED128:
    default:
      {
	oledfont = oled128;
	break;
      }
    }

  /* Demonstration/Test ... */
  if (i2cmode == SYSMODE)
    {
      switch(demode)
	{
	case CLRHOME:
	  {
	    C2S_ClearHome(&current_oled, xflag);
	    break;
	  }
	case INITDSP:
	  {
	    current_oled.cmds = OLED_DISPLAY_ON;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    current_oled.cmds = OLED_DISPLAY_NORMAL;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    current_oled.cmds = OLED_SET_SEG0_127;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    current_oled.cmds = OLED_SET_COMO_REMAP;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = OLED_CONTRAST_RESET;
	    C2S_WriteDBCommand(&current_oled, xflag);
	    current_oled.cmdd[0] = OLED_SET_MEM_ADDR_MODE;
	    current_oled.cmdd[1] = OLED_HORIZONTAL_ADDR_MODE;
	    C2S_WriteDBCommand(&current_oled, xflag);
	    current_oled.cmdt[0] = OLED_SET_COLUMN_ADDR;
	    current_oled.cmdt[1] = OLED_COLUMN_START_RESET;
	    current_oled.cmdt[2] = OLED_COLUMN_END_RESET;
	    C2S_WriteTBCommand(&current_oled, xflag);
	    current_oled.cmdt[0] = OLED_SET_PAGE_ADDR;
	    current_oled.cmdt[1] = OLED_PAGE_START_RESET;
	    current_oled.cmdt[2] = OLED_PAGE_END_RESET;
	    C2S_WriteTBCommand(&current_oled, xflag);
	    break;
	  }
	case INVERSE:
	  {
	    current_oled.cmds = OLED_DISPLAY_INVERSE;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    break;
	  }
	case NORMAL:
	  {
	    current_oled.cmds = OLED_DISPLAY_NORMAL;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    break;
	  }
	case RSTCONT:
	  {
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = OLED_CONTRAST_RESET;
	    C2S_WriteDBCommand(&current_oled, xflag);
	    break;
	  }
	case SETCON0:
	case SETCON1:
	case SETCON2:
	case SETCON3:
	case SETCON4:
	case SETCON5:
	case SETCON6:
	case SETCON7:
	case SETCON8:
	  {
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = oledcontrast;
	    C2S_WriteDBCommand(&current_oled, xflag);
	    break;
	  }
	case TSTDISP:
	  {
	    current_oled.cmds = OLED_DISPLAY_ON;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_ON_NO_RAM;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_ON_WITH_RAM;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_OFF;
	    C2S_WriteSBCommand(&current_oled, xflag);
	    break;
	  }
	case WFBTEST:
	  {
	    C2S_ClearHome(&current_oled, xflag);
	    switch(fontis)
	      {
	      case OLED256:
		{
		  for (i=0; i<128; i++)
		    C2S_WriteThisFontBytes(&current_oled, i, oledfont, xflag);
		  sleep(10);
		  C2S_ClearHome(&current_oled, xflag);
		  for (i=128; i<256; i++)
		    C2S_WriteThisFontBytes(&current_oled, i, oledfont, xflag);
		  break;
		}
	      case JACE128:
	      case OLED128:
	      default:
		{
		  for (i=0; i<128; i++)
		    C2S_WriteThisFontBytes(&current_oled, i, oledfont, xflag);
		  break;
		}
	      }
	    break;
	  }
	case WMSGTST:
	  {
	    C2S_ClearHome(&current_oled, xflag);
	    sleep(1);
	    strcpy(buffer4, current_oled.name);
	    strcat(buffer4, "           ");
	    l = strlen(buffer4);
	    message_buffer[OLED_CHAR_WIDTH - l] = NUL;
	    strcat(buffer4, message_buffer);
	    /* strcat(buffer4, "  "); */
	    i = strlen(buffer4) % 16; /* 16 8x8 pixel chars. per line */
	    while(i > 0)
	      {
		strcat(buffer4, " ");
		i = strlen(buffer4) % 16;
	      }
	    strcat(buffer4, getRCSFile(TRUE));
	    strcat(buffer4, "   Vsn");
	    strcat(buffer4, getSCMVersion());
	    strcat(buffer4, "         ");
	    strcat(buffer4, getRCSDate());
	    printf("%s\n", buffer4);
	    C2S_WriteMessage(&current_oled,
			     oledfont,
			     xflag,
			     buffer4);
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
	case INITDSP:
	  {
	    current_oled.cmds = OLED_DISPLAY_ON;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    current_oled.cmds = OLED_DISPLAY_NORMAL;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    current_oled.cmds = OLED_SET_SEG0_127;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    current_oled.cmds = OLED_SET_COMO_REMAP;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = OLED_CONTRAST_RESET;
	    WP_WriteDBCommand(&current_oled, i2c_chip, xflag);
	    current_oled.cmdd[0] = OLED_SET_MEM_ADDR_MODE;
	    current_oled.cmdd[1] = OLED_HORIZONTAL_ADDR_MODE;
	    WP_WriteDBCommand(&current_oled, i2c_chip, xflag);
	    current_oled.cmdt[0] = OLED_SET_COLUMN_ADDR;
	    current_oled.cmdt[1] = OLED_COLUMN_START_RESET;
	    current_oled.cmdt[2] = OLED_COLUMN_END_RESET;
	    C2S_WriteTBCommand(&current_oled, xflag);
	    current_oled.cmdt[0] = OLED_SET_PAGE_ADDR;
	    current_oled.cmdt[1] = OLED_PAGE_START_RESET;
	    current_oled.cmdt[2] = OLED_PAGE_END_RESET;
	    C2S_WriteTBCommand(&current_oled, xflag);
	    break;
	  }
	case INVERSE:
	  {
	    current_oled.cmds = OLED_DISPLAY_INVERSE;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    break;
	  }
	case NORMAL:
	  {
	    current_oled.cmds = OLED_DISPLAY_NORMAL;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    break;
	  }
	case RSTCONT:
	  {
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = OLED_CONTRAST_RESET;
	    WP_WriteDBCommand(&current_oled, i2c_chip, xflag);
	    break;
	  }
	case SETCON0:
	case SETCON1:
	case SETCON2:
	case SETCON3:
	case SETCON4:
	case SETCON5:
	case SETCON6:
	case SETCON7:
	case SETCON8:
	  {
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = oledcontrast;
	    WP_WriteDBCommand(&current_oled, i2c_chip, xflag);
	    break;
	  }
	case TSTDISP:
	  {
	    current_oled.cmds = OLED_DISPLAY_ON;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_ON_NO_RAM;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_ON_WITH_RAM;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_OFF;
	    WP_WriteSBCommand(&current_oled, i2c_chip, xflag);
	    break;
	  }
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
	case CLRHOME:
	  {
	    PG_ClearHome(&current_oled, pgi2c, xflag);
	    break;
	  }
	case INITDSP:
	  {
	    current_oled.cmds = OLED_DISPLAY_ON;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    current_oled.cmds = OLED_DISPLAY_NORMAL;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    current_oled.cmds = OLED_SET_SEG0_127;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    current_oled.cmds = OLED_SET_COMO_REMAP;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = OLED_CONTRAST_RESET;
	    PG_WriteDBCommand(&current_oled, pgi2c, xflag);
	    current_oled.cmdd[0] = OLED_SET_MEM_ADDR_MODE;
	    current_oled.cmdd[1] = OLED_HORIZONTAL_ADDR_MODE;
	    PG_WriteDBCommand(&current_oled, pgi2c, xflag);
	    current_oled.cmdt[0] = OLED_SET_COLUMN_ADDR;
	    current_oled.cmdt[1] = OLED_COLUMN_START_RESET;
	    current_oled.cmdt[2] = OLED_COLUMN_END_RESET;
	    PG_WriteTBCommand(&current_oled, pgi2c, xflag);
	    current_oled.cmdt[0] = OLED_SET_PAGE_ADDR;
	    current_oled.cmdt[1] = OLED_PAGE_START_RESET;
	    current_oled.cmdt[2] = OLED_PAGE_END_RESET;
	    PG_WriteTBCommand(&current_oled, pgi2c, xflag);
	    break;
	  }
	case INVERSE:
	  {
	    current_oled.cmds = OLED_DISPLAY_INVERSE;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    break;
	  }
	case NORMAL:
	  {
	    current_oled.cmds = OLED_DISPLAY_NORMAL;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    break;
	  }
	case RSTCONT:
	  {
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = OLED_CONTRAST_RESET;
	    PG_WriteDBCommand(&current_oled, pgi2c, xflag);
	    break;
	  }
	case SETCON0:
	case SETCON1:
	case SETCON2:
	case SETCON3:
	case SETCON4:
	case SETCON5:
	case SETCON6:
	case SETCON7:
	case SETCON8:
	  {
	    current_oled.cmdd[0] = OLED_SET_CONTRAST;
	    current_oled.cmdd[1] = oledcontrast;
	    PG_WriteDBCommand(&current_oled, pgi2c, xflag);
	    break;
	  }
	case TSTDISP:
	  {
	    current_oled.cmds = OLED_DISPLAY_ON;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_ON_NO_RAM;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_ON_WITH_RAM;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    sleep(2);
	    current_oled.cmds = OLED_DISPLAY_OFF;
	    PG_WriteSBCommand(&current_oled, pgi2c, xflag);
	    break;
	  }
	case WFBTEST:
	  {
	    PG_ClearHome(&current_oled, pgi2c, xflag);
	    switch(fontis)
	      {
	      case OLED256:
		{
		  for (i=0; i<128; i++)
		    PG_WriteThisFontBytes(&current_oled, pgi2c, i, oledfont, xflag);
		  sleep(8);
		  PG_ClearHome(&current_oled, pgi2c, xflag);
		  for (i=128; i<256; i++)
		    PG_WriteThisFontBytes(&current_oled, pgi2c, i, oledfont, xflag);
		  break;
		}
	      case JACE128:
	      case OLED128:
	      default:
		{
		  for (i=0; i<128; i++)
		    PG_WriteThisFontBytes(&current_oled, pgi2c, i, oledfont, xflag);
		  break;
		}
	      }
	    break;
	  }
	case WMSGTST:
	  {
	    PG_ClearHome(&current_oled, pgi2c, xflag);
	    delay(500);
	    strcpy(buffer4, current_oled.name);
	    strcat(buffer4, "           ");
	    l = strlen(buffer4);
	    message_buffer[OLED_CHAR_WIDTH - l] = NUL;
	    strcat(buffer4, message_buffer);
	    /* strcat(buffer4, "  "); */
	    i = strlen(buffer4) % 16; /* 16 8x8 pixel chars. per line */
	    while(i > 0)
	      {
		strcat(buffer4, " ");
		i = strlen(buffer4) % 16;
	      }
	    strcat(buffer4, getRCSFile(TRUE));
	    strcat(buffer4, "   Vsn");
	    strcat(buffer4, getSCMVersion());
	    strcat(buffer4, "         ");
	    strcat(buffer4, getRCSDate());
	    printf("%s\n", buffer4);
	    PG_WriteMessage(&current_oled,
			    pgi2c,
			    oledfont,
			    xflag,
			    buffer4);
	    break;
	  }
	default: break;
	}
    }
#endif /* HAVE_PIGPIO_H */

  /* Tidy up ... */
  if (i2cmode == SYSMODE)
    {
      switch(demode)
	{
	case INITDSP:
	case RSTCONT:
	case TSTDISP:
	case WFBTEST:
	  {
	    echo_i2c_ssd1306_oled_status(current_oled);
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
	case INITDSP:
	case RSTCONT:
	case TSTDISP:
	  {
	    echo_i2c_ssd1306_oled_status(current_oled);
	    break;
	  }
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
	case INITDSP:
	case RSTCONT:
	case TSTDISP:
	case WFBTEST:
	  {
	    echo_i2c_ssd1306_oled_status(current_oled);
	    break;
	  }
	default: break;
	}
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
 * $Log: oled_i2c_demo.c,v $
 * Revision 1.4  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.3  2017/12/21 22:55:08  pi
 * Minor tidying up
 *
 * Revision 1.2  2016/05/13 16:15:18  pi
 * updated here
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.16  2015/08/26 20:03:14  pi
 * Added -SCn options
 *
 * Revision 1.15  2015/07/04 16:12:04  pi
 * Unused variable removed
 *
 * Revision 1.14  2015/05/26 20:49:46  pi
 * -WMT option added
 *
 * Revision 1.13  2015/05/25 19:27:53  pi
 * jace128 font added
 *
 * Revision 1.12  2015/05/22 20:22:41  pi
 * -CH, -ID, -inverse, -normal, -oled128, -oled256, -RC, -TD
 * and -WFT options working (some only for -S & -PG methods)
 * SCMVersion 0.01
 *
 * Revision 1.11  2015/05/22 18:15:52  pi
 * Interim commit
 *
 * Revision 1.10  2015/05/19 17:23:59  pi
 * Interim commit
 *
 * Revision 1.9  2015/04/29 20:12:42  pi
 * Interim commit
 *
 * Revision 1.8  2015/04/28 20:48:57  pi
 * Interim commit
 *
 * Revision 1.7  2015/04/28 19:25:08  pi
 * Interim commit
 *
 * Revision 1.6  2015/04/27 21:21:30  pi
 * Interim commit
 *
 * Revision 1.5  2015/04/19 18:59:09  pi
 * Interim commit
 *
 * Revision 1.4  2015/04/19 11:58:41  pi
 * Interim commit
 *
 * Revision 1.3  2015/04/18 20:00:54  pi
 * Interim commit
 *
 * Revision 1.2  2015/04/17 21:11:17  pi
 * Interim commit
 *
 * Revision 1.1  2015/04/16 21:28:11  pi
 * Initial revision
 *
 *
 *
 */
