/* Graphics LCD FONT header file creation/conversion TOOL ...
 * $Id: glcd_font_tool.c,v 1.2 2018/03/31 21:32:45 pi Exp $
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
#include "rcs_scm.h"
#include "parsel.h"
#include "glcd_fonttools.h"

/* RCS info blocks */

static char RCSAuthor[]   = "$Author: pi $";
static char SCMAuthor[]   = "$SCMauthor: F_Trevor_Gowen $";
static char RCSDate[]     = "$Date: 2018/03/31 21:32:45 $";
static char RCSFile[]     = "$RCSfile: glcd_font_tool.c,v $";
static char RCSId[]       = "$Id: glcd_font_tool.c,v 1.2 2018/03/31 21:32:45 pi Exp $";
static char RCSName[]     = "$Name:  $";
static char RCSRevision[] = "$Revision: 1.2 $";
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
  CLEAN,     /* -C                   */
  CONVRTN,   /* -conv d              */
  ECHO,      /* -E                   */
  OUTAUTO,   /* -O                   */
  OUTCOLROW, /* -OCR                 */
  OUTROWCOL, /* -ORC                 */
  OUTFILE,   /* -o filename          */
  QUIET,     /* -Q                   */
  VERBOSE,   /* -V                   */
  LAST_ARG,
  ARGTYPES
} argnames;

argnames what,which,cmode;
char *arglist[ARGTYPES];

void setupargs(void)
{
  arglist[ARGFILE]   = "-a";
  arglist[COMMENT]   = "*";
  arglist[CLEAN]     = "-C";
  arglist[CONVRTN]   = "-conv";
  arglist[ECHO]      = "-E";
  arglist[OUTAUTO]   = "-O";
  arglist[OUTCOLROW] = "-OCR";
  arglist[OUTROWCOL] = "-ORC";
  arglist[OUTFILE]   = "-o";
  arglist[QUIET]     = "-Q";
  arglist[VERBOSE]   = "-V";
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
char prefix[FN_LEN];
char eflag,oflag,vflag;
int  nconv;

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
	    case CLEAN:   cmode=CLEAN; break;
	    case CONVRTN:
	      {
		nconv = atoi(buffer2);
		if (nconv > 256) nconv = 256;
		if (nconv < 1)   nconv = 1;
		break;
	      }
	    case ECHO:    eflag=TRUE;  break;
	    case OUTAUTO: 
	      {
		oflag=TRUE;
		strcpy(prefix, get_stem(fontInfile));
		strcat(prefix, "4glcd");
		break;
	      }
	    case OUTCOLROW: cmode=OUTCOLROW; FontOutOrder=COLROWFONT; break;
	    case OUTROWCOL: cmode=OUTROWCOL; FontOutOrder=ROWCOLFONT; break;
	    case OUTFILE: oflag=TRUE; strcpy(prefix, buffer2); break;
	    case QUIET:   vflag=FALSE; break;
	    case VERBOSE: eflag=TRUE; vflag=TRUE; break;
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
  strcpy(buffer1, "[path/]fontInfile_name[.h] [options]\n");
  strcat(buffer1, "Options: -o fontOut_filename_prefix,\n");
  strcat(buffer1, "         -a arg_filename[.arg],\n");
  strcat(buffer1, "         -conv d {CONVert upto d font chars. (1 < d <= 256},\n");
  strcat(buffer1, "         -C   {Clean|Clone font file - default},\n");
  strcat(buffer1, "         -E   {Echo command-line arguments},\n");
  strcat(buffer1, "         -O   {auto-generate fontOutfile_name},\n");
  strcat(buffer1, "         -OCR {Output a Column/Row font},\n");
  strcat(buffer1, "         -ORC {Output a Row/Column font},\n");
  strcat(buffer1, "         -Q   {Quiet - default},\n");
  strcat(buffer1, "         -V   {Verbose},\n");
  print_us(getRCSFile(TRUE), buffer1);
}

/* Main body of code */
int main(int argc,
	 char **argv)
{
  char *p1,*p2;
  int i,j,k;
  int maxnchar,nchar;
  unsigned int mbit,maskIn,maskOut,pixbyte;
  unsigned int pixbyteIn[MAX_FONT_HEIGHT];
  unsigned int pixbyteOut[MAX_FONT_HEIGHT];

  setup_RCSlist();

  /* printf("getRCSVersion reports %s\n", getRCSVersion()); */

  /* Initialise argument list etc. */
  setupargs();
  setupfontkeys();

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

  /* Check first argument if it exists */
  if (argc > 1)
    {
      p1    = argv[1];
      make_name(fontInfile, "h", p1);
    }

  if (argc < 2)
    {
      usage();
      exit(EXIT_SUCCESS);
    }

  /* Set up defaults etc.*/
  eflag     = oflag = vflag = FALSE;
  cmode     = CLEAN;
  make_name(argfile, "arg", getRCSFile(FALSE));
  fontOutfn = stdout;
  strcpy(fontOutfile, "stdout");
  FontInHeight = FontInWidth = 0; /* Not yet assigned */
  FontOutOrder = CLONEFONT;
  maxnchar = 256;
  nconv    = -1; /* Undefined */
  nchar    = 0;

  /* Analyse the arguments (if any) */
  if (argc > 1)
    {
      for (i=2; i<argc; i++)
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
	    case CLEAN: cmode=CLEAN; i--; break;
	    case CONVRTN:
	      {
		nconv = atoi(p2);
		if (nconv > 256) nconv = 256;
		if (nconv < 1)   nconv = 1;
		break;
	      }
	    case ECHO:  eflag=TRUE;  i--; break;
	    case OUTAUTO:
	      {
		oflag=TRUE;
		i--;
		strcpy(prefix, get_stem(fontInfile));
		strcat(prefix, "4glcd");
		break;
	      }
	    case OUTCOLROW: cmode=OUTCOLROW; FontOutOrder=COLROWFONT; i--; break;
	    case OUTROWCOL: cmode=OUTROWCOL; FontOutOrder=ROWCOLFONT; i--; break;
	    case OUTFILE: oflag=TRUE;  strcpy(prefix, p2); break;
	    case QUIET:   vflag=FALSE; i--; break;
	    case VERBOSE: eflag=TRUE;  vflag=TRUE; i--; break;
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
      listfontkeys();
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

  /* Echo filenames */
  if (oflag) make_name(fontOutfile, "h", prefix);
  printf("Argfile: %s\nfontInfile: %s\nfontOutfile: %s\n",
	 argfile, fontInfile, fontOutfile);
  
  /* Limit conversion? ... */
  if (nconv > 0)
    {
      maxnchar = nconv;
      printf("NB: Converting/transferring only the first %d font characters!\n",
	     maxnchar);
    }

  /* Open Files ... */
  Open_Font_In_File();
  if (oflag) Open_Font_Out_File();

  fprintf(fontOutfn, 
	  "/* Converted from %s by %s %s [%s], (c)%s */\n",
	  fontInfile,
	  getRCSFile(TRUE),
	  getSCMVersion(),
	  getRCSDate(),
	  getSCMAuthor());

  reset_stats();
  printf("Cloning original header comments ...\n");
  Clone_Font_Header_Comments(vflag);
  printf("Regenerating Font Height & Width parameters ...\n");
  FontInOrder = NOTYETFONT; // Must set this before first call of Regen_Font_HW()
  Regen_Font_HW(vflag);
  Regen_Font_HW(vflag);
  report_stats(fontInfile);
  p1 = get_stem_no_path(fontOutfile);
  if (FontInOrder == ROWCOLFONT)
    {
      printf("%s contains a ROW/COLUMN font ...\n", fontInfile);
      if (FontOutOrder == COLROWFONT)
	{
	  printf("... converting to a COLUMN/ROW font in %s\n", fontOutfile);
	  fprintf(fontOutfn, "static const int %sWidth  = ", p1);
	  fprintf(fontOutfn, "%d;\n", FontInWidth);
	  fprintf(fontOutfn, "static const int %sHeight = ", p1);
	  fprintf(fontOutfn, "%d;\n", FontInHeight);
	}
     else
	{
	  fprintf(fontOutfn, "static const int %sHeight = ", p1);
	  fprintf(fontOutfn, "%d;\n", FontInHeight);
	  fprintf(fontOutfn, "static const int %sWidth  = ", p1);
	  fprintf(fontOutfn, "%d;\n", FontInWidth);
	}
    }
  if (FontInOrder == COLROWFONT)
    {
      printf("%s contains a COLUMN/ROW font ...\n", fontInfile);
      if (FontOutOrder == ROWCOLFONT)
	{
	  printf("... converting to a ROW/COLUMN font in %s\n", fontOutfile);
	  fprintf(fontOutfn, "static const int %sHeight = ", p1);	  
	  fprintf(fontOutfn, "%d;\n", FontInHeight);
	  fprintf(fontOutfn, "static const int %sWidth  = ", p1);
	  fprintf(fontOutfn, "%d;\n", FontInWidth);
	}
      else
	{
	  fprintf(fontOutfn, "static const int %sWidth  = ", p1);
	  fprintf(fontOutfn, "%d;\n", FontInWidth);
	  fprintf(fontOutfn, "static const int %sHeight = ", p1);
	  fprintf(fontOutfn, "%d;\n", FontInHeight);
	}
    }
  printf("Regenerating Font Definition Header ...\n");
  RegenFontDefnHdr(vflag);
  report_stats(fontInfile);
  printf("Regenerating \"{\" ...\n");
  RegenLBrace();
  report_stats(fontInfile);
  printf("Loading Character Block Data ...\n");
  do
    {
      qfontkey = QueryCloneCharBlockComment(vflag);
      if (qfontkey == CCOMMENT)
	{
	  for (i=0; i<FontInHeight; i++) LoadCharBlockData(pixbyteIn, i, vflag);
	  if (vflag) report_stats(fontInfile);
	  switch(cmode)
	    {
	    case OUTCOLROW:
	      {
		if (FontInOrder == COLROWFONT)
		 for (i=0; i<FontInHeight; i++) pixbyteOut[i] = pixbyteIn[i];
		else
		  {
		    j = 0;
		    k = FontInHeight - 1;
		    maskIn = 0x80;
		    for (i=0; i<FontInWidth; i++)
		      {
			pixbyte = 0x00;
			maskOut = 0x80;
			for (j=k; j>=0; j--)
			  {
			    mbit = getPixBit(pixbyteIn[j], maskIn);
			    pixbyte = pixbyte + (mbit * maskOut);
			    /*
			    printf("%1d:%1d %1d 0x%02x 0x%02x 0x%02x 0x%02x\n", 
				   i, j, mbit, pixbyteIn[j], pixbyte, maskIn, maskOut);
			    */
			      maskOut = maskOut >> 1;
			  }
			maskIn = maskIn >> 1;
			pixbyteOut[i] = pixbyte;
		      }	      
		  }
		break;
	      }
	    case OUTROWCOL:
	      {
		if (FontInOrder == ROWCOLFONT)
		 for (i=0; i<FontInHeight; i++) pixbyteOut[i] = pixbyteIn[i];
		else
		  {
		  }
		break;
	      }
	    case CLEAN:
	    default:
	      {
		for (i=0; i<FontInHeight; i++) pixbyteOut[i] = pixbyteIn[i];
		break;
	      }
	    }
	  for (i=0; i<FontInHeight; i++) WriteCharBlockData(pixbyteOut, i);
	  fprintf(fontOutfn, "\n");
	  nchar++;
	}
      else if (qfontkey == RBRACESCKEY) 
	fprintf(fontOutfn, "%s\n", fontkeylist[RBRACESCKEY]);
    }
  while ((qfontkey == CCOMMENT) && (nchar < maxnchar));
  if (!vflag) report_stats(fontInfile);
  if (nchar == maxnchar) fprintf(fontOutfn, "%s\n", fontkeylist[RBRACESCKEY]);
  Append_Empty_Change_Log(vflag);

  printf("%d font characters converted\n", nchar);

  /* Close opened files ... */
  if (fontInfopen) Close_Font_In_File();
  if (fontOutfopen) Close_Font_Out_File();

  exit(EXIT_SUCCESS);
}

/* Change Log */
/*
 * $Log: glcd_font_tool.c,v $
 * Revision 1.2  2018/03/31 21:32:45  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:47:06  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:30:20  pi
 * Initial revision
 *
 * Revision 1.11  2015/05/19 17:23:59  pi
 * Interim commit
 *
 * Revision 1.10  2015/05/19 14:52:34  pi
 * Interim commit
 *
 * Revision 1.9  2015/05/18 20:49:56  pi
 * Interim commit
 *
 * Revision 1.8  2015/05/14 19:37:01  pi
 * Interim commit
 *
 * Revision 1.7  2015/05/12 21:49:52  pi
 * Interim commit
 *
 * Revision 1.6  2015/05/11 18:09:53  pi
 * Interim commit
 *
 * Revision 1.5  2015/05/04 20:34:45  pi
 * Interim commit
 *
 * Revision 1.4  2015/05/04 19:33:29  pi
 * Interim commit
 *
 * Revision 1.3  2015/05/04 16:41:38  pi
 * Interim commit
 *
 * Revision 1.2  2015/05/03 19:57:38  pi
 * Interim commit
 *
 * Revision 1.1  2015/05/03 18:06:27  pi
 * Initial revision
 *
 *
 *
 *
 */
