/* SPI Graphics LCD (nokia5110-like), Humble Pi 3 schematic pinout, DEMOnstration code
 * (Clone of spi_glcdV2_demo.c with revised GPIO's)
 * $Id: spi_glcdHP3_demo.c,v 1.5 2018/03/31 21:32:45 pi Exp $
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
#include "glcd_xbmtools.h"
/* NB: include "PCD8544" header files last! */
#include "spi_PCD8544.h"
#ifndef PCD8544_FONT_H
#include "PCD8544_font.h"
#endif /* !PCD8544_FONT_H */

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: spi_glcdHP3_demo.c,v $";
static char RCSId[]       = "$Id: spi_glcdHP3_demo.c,v 1.5 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.5 $";
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
  XPMFILE,   /* -xpm filename[.xpm]  */
  COMMENT,   /* * {in argfile only!} */
  ECHO,      /* -E                   */
#ifdef HAVE_WIRINGPI_H
  INITGPIO,  /* -IG                  */
#ifdef HAVE_WIRINGPISPI_H
  CLRHOME,   /* -CH                  */
  INVERSE,   /* -inverse             */
  NORMAL,    /* -normal              */
  RESETINIT, /* -RI                  */
  SETCON0,   /* -SC0                 */
  SETCON1,   /* -SC1                 */
  SETCON2,   /* -SC2                 */
  SETCON3,   /* -SC3                 */
  SETCON4,   /* -SC4                 */
  SETCON5,   /* -SC5                 */
  SETCON6,   /* -SC6                 */
  SETCON7,   /* -SC7                 */
  SETCON8,   /* -SC8                 */
  WFDTST,    /* -WFT                 */
  WGRTST,    /* -WGT                 */
  WMSGTST,   /* -WMT                 */
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  QUIET,     /* -Q                   */
  VERBOSE,   /* -V                   */
  XECUTE,    /* -X                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,demode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]   = "-a";
  arglist[XPMFILE]   = "-xpm";
  arglist[COMMENT]   = "*";
  arglist[ECHO]      = "-E";
#ifdef HAVE_WIRINGPI_H
  arglist[INITGPIO]  = "-IG";
#ifdef HAVE_WIRINGPISPI_H
  arglist[CLRHOME]   = "-CH";
  arglist[INVERSE]   = "-inverse";
  arglist[NORMAL]    = "-normal";
  arglist[RESETINIT] = "-RI";
  arglist[SETCON0]   = "-SC0";
  arglist[SETCON1]   = "-SC1";
  arglist[SETCON2]   = "-SC2";
  arglist[SETCON3]   = "-SC3";
  arglist[SETCON4]   = "-SC4";
  arglist[SETCON5]   = "-SC5";
  arglist[SETCON6]   = "-SC6";
  arglist[SETCON7]   = "-SC7";
  arglist[SETCON8]   = "-SC8";
  arglist[WFDTST]    = "-WFT";
  arglist[WGRTST]    = "-WGT";
  arglist[WMSGTST]   = "-WMT";
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  arglist[QUIET]     = "-Q";
  arglist[VERBOSE]   = "-V";
  arglist[XECUTE]    = "-X";
  arglist[LAST_ARG]  = "";
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
unsigned int convop,dispmode;

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
	    case XPMFILE: 
	      {
		make_name(xpmfile, "xpm", buffer2);
		Open_Xpm_File("r");
		break;
	      }
	    case COMMENT: printf("%s\n", buffer4); break;
#ifdef HAVE_WIRINGPI_H
	    case INITGPIO: igflag=TRUE;     break;
#ifdef HAVE_WIRINGPISPI_H
	    case CLRHOME:   demode=CLRHOME;   break;
	    case INVERSE:
	      {
		demode=INVERSE;
		dispmode=PCD8544_DISPLAYINVERTED;
		break;
	      }
	    case NORMAL:
	      {
		demode=NORMAL;
		dispmode=PCD8544_DISPLAYNORMAL;
		break;
	      }
	    case RESETINIT: demode=RESETINIT; break;
	    case SETCON0: demode=SETCON0; convop=PCD8544_C0; break;
	    case SETCON1: demode=SETCON1; convop=PCD8544_C1; break;
	    case SETCON2: demode=SETCON2; convop=PCD8544_C2; break;
	    case SETCON3: demode=SETCON3; convop=PCD8544_C3; break;
	    case SETCON4: demode=SETCON4; convop=PCD8544_C4; break;
	    case SETCON5: demode=SETCON5; convop=PCD8544_C5; break;
	    case SETCON6: demode=SETCON6; convop=PCD8544_C6; break;
	    case SETCON7: demode=SETCON7; convop=PCD8544_C7; break;
	    case SETCON8: demode=SETCON8; convop=PCD8544_C8; break;
	    case WFDTST:  demode=WFDTST;  break;
	    case WGRTST:  demode=WGRTST;  break;
	    case WMSGTST: demode=WMSGTST; break;
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
	    case ECHO:    eflag=TRUE;       break;
	    case QUIET:   vflag=FALSE;      break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
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
  strcat(buffer1, "         -xpm xpm_filename[.xpm] (for 48x84 rotated graphic),\n");
  strcat(buffer1, "         -E   {Echo command-line arguments},\n");
#ifdef HAVE_WIRINGPI_H
  strcat(buffer1, "         -IG  {Initialise gpio's & Gpio readall},\n");
#ifdef HAVE_WIRINGPISPI_H
  strcat(buffer1, "         -CH  {Clear and return Home},\n");
  strcat(buffer1, "         -inverse {INVERSE display},\n");
  strcat(buffer1, "         -normal  {NORMAL display - default},\n");
  strcat(buffer1, "         -RI  {Reset & Initialise display},\n");
  strcat(buffer1, "         -SCn {Set Contrast level n, 0<=n<=8},\n");
  strcat(buffer1, "         -WFT {Write direct Font data Test},\n");
  strcat(buffer1, "         -WGT {Write Graphics image data Test},\n");
  strcat(buffer1, "         -WMT {Write Message Test},\n");
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
  strcat(buffer1, "         -Q  {Quiet - default},\n");
  strcat(buffer1, "         -V  {Verbose},\n");
  strcat(buffer1, "         -X  {eXecute demonstration}");
  print_us(getRCSFile(TRUE), buffer1);
}

#ifdef HAVE_WIRINGPI_H
/* Exporting the ReSeT gpio leads to spurious reset pulses so ... */
void WP_C2S_GlcdReset_Test(int rst_gpio,
			   char qx)
{
  C2S_gpio_g_write(rst_gpio,
		   GPIO_HIGH,
		   qx);	    
  delay(500);
  C2S_gpio_g_write(rst_gpio,
		   GPIO_LOW,
		   qx);	    
  delay(100);
  C2S_gpio_g_write(rst_gpio,
		   GPIO_HIGH,
		   qx);	    
}
#ifdef HAVE_WIRINGPISPI_H
/*
void WP_FS_WriteMessage(pcd8544 *this_pcd8544,
			char *this8x5font,
			char qx,
			char *message)
{
  int bite;
  char lmsg[LCDWIDTH+1],*lp1;
  strncpy(lmsg, message, LCDWIDTH);
  lmsg[LCDWIDTH] = NUL;
  lp1 = lmsg;
  while (*lp1 != NUL)
    {
      bite = toascii((int)(*lp1));
      WP_FS_WriteFontDataBytes(this_pcd8544, bite, this8x5font, qx);
      lp1++;
    }
}	
*/		
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  char spidev;
  char xpmlflag;
  int i,j,k,l;
  int mrow,nbytes,npix;
#ifdef HAVE_WIRINGPI_H
  int wPS,wPSS;
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPISPI_H
  int *myspifd;
  int wP_SPIfd[2];
#endif /* HAVE_WIRINGPISPI_H */
  pcd8544 nokia5110HP3;

  setup_RCSlist();

  /* printf("getRCSVersion reports %s\n", getRCSVersion()); */

  /* Initialise argument list etc. */
  setupargs();
  setupxbmkeys();
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
  eflag    = igflag = tflag = vflag = xflag = FALSE;
  convop   = PCD8544_C3;
  dispmode = PCD8544_DISPLAYNORMAL;
  demode   = INITGPIO; /* Dummy mode */
  spidev   = SPI_DEV00;
#ifdef HAVE_WIRINGPISPI_H
  myspifd = &wP_SPIfd[0]; /* == SPI_DEV00 */
  wP_SPIfd[0] = wP_SPIfd[1] = -1; /* ie. as yet undefined */
#endif /* HAVE_WIRINGPISPI_H */
  init_colpix_store(&nokia5110HP3_buffer);
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
	    case XPMFILE: 
	      {
		make_name(xpmfile, "xpm", p2);
		Open_Xpm_File("r");
		break;
	      }
#ifdef HAVE_WIRINGPI_H
	    case INITGPIO: igflag=TRUE;     i--; break;
#ifdef HAVE_WIRINGPISPI_H
	    case CLRHOME:   demode=CLRHOME;   i--; break;
	    case INVERSE:
	      {
		demode=INVERSE;
		dispmode=PCD8544_DISPLAYINVERTED;
		i--;
		break;
	      }
	    case NORMAL:
	      {
		demode=NORMAL;
		dispmode=PCD8544_DISPLAYNORMAL;
		i--;
		break;
	      }
	    case RESETINIT: demode=RESETINIT; i--; break;
	    case SETCON0: demode=SETCON0; convop=PCD8544_C0; i--; break;
	    case SETCON1: demode=SETCON1; convop=PCD8544_C1; i--; break;
	    case SETCON2: demode=SETCON2; convop=PCD8544_C2; i--; break;
	    case SETCON3: demode=SETCON3; convop=PCD8544_C3; i--; break;
	    case SETCON4: demode=SETCON4; convop=PCD8544_C4; i--; break;
	    case SETCON5: demode=SETCON5; convop=PCD8544_C5; i--; break;
	    case SETCON6: demode=SETCON6; convop=PCD8544_C6; i--; break;
	    case SETCON7: demode=SETCON7; convop=PCD8544_C7; i--; break;
	    case SETCON8: demode=SETCON8; convop=PCD8544_C8; i--; break;
	    case WFDTST:  demode=WFDTST;  i--; break;
	    case WGRTST:  demode=WGRTST;  i--; break;
	    case WMSGTST: demode=WMSGTST; i--; break;
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
	    case ECHO:    eflag=TRUE;       i--; break;
	    case QUIET:   vflag=FALSE;      i--; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; i--; break;
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
      listxbmkeys();
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

  /* Load an XPM file if declared and opened ... */
  if (xpmfopen)
    {
      Read_Xpm_Header(vflag);
      Read_Xpm_Header_Data(vflag);
      xpmtype = Check_Xpm_Header_Type(XPMCOLPIX8);
      printf("XPM file type = %s\n", xbmkeylist[xpmtype]);
      if (xpmtype == XPMCOLPIX8)
	npix = Read_Xpm_Colpix8_Data(&nokia5110HP3_buffer, vflag);
      if (npix > 0) xpmlflag = TRUE;
      Close_Xpm_File();
    }

  /* -IG option leads to an early exit after the system call ... */
  if (igflag)
    {
      init_pcd8544(&nokia5110HP3,
		   24,
		   25,
		   GPIO_UNASSIGNED,
		   spidev, 
		   "Nokia5110HP3");
      echo_pcd8544_status(nokia5110HP3);
      echo_colpix_store_status(&nokia5110HP3_buffer);
#ifdef HAVE_WIRINGPI_H
      C2S_gpio_g_mode(nokia5110HP3.rst_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* ReSeT inactive */
      C2S_gpio_g_write(nokia5110HP3.rst_gpio,
		       GPIO_HIGH,
		       xflag);
      C2S_gpio_g_mode(nokia5110HP3.dc_gpio,
		      GPIO_MODE_OUT,
		      xflag);
      /* Data/¬Command = Command */
      C2S_gpio_g_write(nokia5110HP3.dc_gpio,
		       GPIO_LOW,
		       xflag);
      C2S_gpio("readall", xflag);
#endif /* HAVE_WIRINGPI_H */
      exit(EXIT_SUCCESS);
    }

  /* Execute other options */
  /* Initialisation ... */
#ifdef HAVE_WIRINGPI_H
  init_pcd8544(&nokia5110HP3,
	       24,
	       25,
	       GPIO_UNASSIGNED,
	       spidev, 
	       "Nokia5110HP3");
  echo_pcd8544_status(nokia5110HP3);
  echo_colpix_store_status(&nokia5110HP3_buffer);
  printf("Using 'C' system calls to 'gpio' and wiringPi methods: ...\n");
  /* Exporting the ReSeT gpio leads to spurious reset pulses so ... */
  /* ... use SYSTEM method instead! */
  C2S_gpio_g_mode(nokia5110HP3.rst_gpio, GPIO_MODE_OUT, xflag);
  /* O.K. to export Data/¬Command gpio ... */
  C2S_gpio_export(nokia5110HP3.dc_gpio, GPIO_XOUT, xflag);
  /* An exported the BackLight gpio is not saved on exit by kernel 3.18... */
  /* ... so use SYSTEM method instead! */
  /* if (nokia5110HP3.bl_gpio != GPIO_UNASSIGNED)
     C2S_gpio_export(nokia5110HP3.bl_gpio, GPIO_XOUT, xflag); */
  if (nokia5110HP3.bl_gpio != GPIO_UNASSIGNED)
    C2S_gpio_g_mode(nokia5110HP3.bl_gpio, GPIO_MODE_OUT, xflag);
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
  /* ReSeT disabled */
  /* Exporting the ReSeT gpio leads to spurious reset pulses so ... */
  /* ... use SYSTEM method instead! */
  C2S_gpio_g_write(nokia5110HP3.rst_gpio, GPIO_HIGH, xflag);
  /* Data/¬Command = Command mode */
  WP_digitalWrite(nokia5110HP3.dc_gpio, GPIO_LOW, xflag);
  C2S_gpio("exports", xflag);
  if (eflag || vflag) 
    {
      printf("ls -laF /sys/class/gpio/\n");
      if (xflag) system("ls -laF /sys/class/gpio/");
    }
#ifdef HAVE_WIRINGPISPI_H
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
      nokia5110HP3.spifd = myspifd;
      printf("*nokia5110HP3.spifd = %d\n",
	     *nokia5110HP3.spifd);
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Demonstration ... */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  switch(demode)
    {
    case CLRHOME:
      {
	WP_FS_ClearHome(&nokia5110HP3, xflag);
	break;
      }
    case INVERSE:
    case NORMAL:
      {
	nokia5110HP3.dcval = PCD8544_FUNCTIONSET;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	nokia5110HP3.dcval = PCD8544_DISPLAYCONTROL | dispmode;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	break;
      }
    case RESETINIT:
      {
	/* Based upon LCDInit from PCD8544,c */
	WP_C2S_GlcdReset_Test(nokia5110HP3.rst_gpio, xflag);
	nokia5110HP3.dcval = PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	nokia5110HP3.dcval = PCD8544_SETBIAS | 0x04;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	nokia5110HP3.dcval = PCD8544_SETVOP | convop;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	nokia5110HP3.dcval = PCD8544_FUNCTIONSET;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	nokia5110HP3.dcval = PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
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
	nokia5110HP3.dcval = PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	nokia5110HP3.dcval = PCD8544_SETVOP | convop;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	nokia5110HP3.dcval = PCD8544_FUNCTIONSET;
	WP_FS_WriteCommandByte(&nokia5110HP3, xflag);
	break;
      }
    case WFDTST:
      {
	WP_FS_ClearHome(&nokia5110HP3, xflag);
	for (i=0; i<64; i++)
	  WP_FS_WriteFontDataBytes(&nokia5110HP3, i, font8x5, xflag);
	sleep(10);
	WP_FS_ClearHome(&nokia5110HP3, xflag);
	for (i=64; i<128; i++)
	  WP_FS_WriteFontDataBytes(&nokia5110HP3, i, font8x5, xflag);
	sleep(10);
	WP_FS_ClearHome(&nokia5110HP3, xflag);
	for (i=128; i<192; i++)
	  WP_FS_WriteFontDataBytes(&nokia5110HP3, i, font8x5, xflag);
	sleep(10);
	WP_FS_ClearHome(&nokia5110HP3, xflag);
	for (i=192; i<256; i++)
	  WP_FS_WriteFontDataBytes(&nokia5110HP3, i, font8x5, xflag);
	break;
      }
    case WGRTST:
      {
	WP_FS_ClearHome(&nokia5110HP3, xflag);
	WP_FS_WriteGraphicsDataBytes(&nokia5110HP3, &nokia5110HP3_buffer, xflag);
	break;
      }
    case WMSGTST:
      {
	WP_FS_ClearHome(&nokia5110HP3, xflag);
	sleep(1);
	strcpy(buffer4, nokia5110HP3.name);
	strcat(buffer4, "  ");
	l = strlen(buffer4);
	message_buffer[LCDWIDTH - l] = NUL;
	strcat(buffer4, message_buffer);
	strcat(buffer4, " ");
	strcat(buffer4, getRCSFile(TRUE));
	strcat(buffer4, " Version ");
	strcat(buffer4, getSCMVersion());
	strcat(buffer4, " ");
	strcat(buffer4, getRCSDate());
	printf("%s\n", buffer4);
	WP_FS_WriteMessage(&nokia5110HP3,
			   font8x5,
			   xflag,
			   buffer4);
	break;
      }
    default: break;
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  /* Tidy up ... */
#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
  /* Data/¬Command = Command mode */
  WP_digitalWrite(nokia5110HP3.dc_gpio, GPIO_LOW, xflag);
  if (eflag || vflag) 
    {
      printf("ls -laF /sys/class/gpio/\n");
      if (xflag) system("ls -laF /sys/class/gpio/");
    }
  C2S_gpio_unexportall(xflag);
  if (wP_SPIfd[0] > 0) close(wP_SPIfd[0]);
  if (wP_SPIfd[1] > 0) close(wP_SPIfd[1]);
  if (eflag || vflag) 
    {
      printf("ls -laF /sys/class/gpio/\n");
      if (xflag) system("ls -laF /sys/class/gpio/");
      C2S_gpio("readall", xflag);
    }
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: spi_glcdHP3_demo.c,v $
 * Revision 1.5  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.4  2017/08/21 18:56:48  pi
 * SCMVersion 0.01
 *
 * Revision 1.3  2017/08/21 18:45:32  pi
 * Self-help corrected/updated
 *
 * Revision 1.2  2017/08/21 18:41:22  pi
 * Now working
 *
 * Revision 1.1  2017/08/21 17:17:19  pi
 * Initial revision
 *
 *
 *
 */
