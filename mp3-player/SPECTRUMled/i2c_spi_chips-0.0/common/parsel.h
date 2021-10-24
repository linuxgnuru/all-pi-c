/* Header file associated with a SELection of PARsing functions etc.
 * $Id: parsel.h,v 1.3 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef PARSEL_H
#define PARSEL_H 1

#ifdef NO_MAIN
#define TRUE       1
#define FALSE      0
#define NUL        '\0'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif /* NO_MAIN */

#include <time.h>
#include <unistd.h>

/* These #defines can be overidden by entries in the main source
 * file PRIOR to the #include of this file.
 */

#ifndef BLK_LEN
#define BLK_LEN          83  /* 80 + CR, LF & NUL */
#endif /* BLK_LEN */
#ifndef BUF_LEN
#define BUF_LEN         256
#endif /* BUF_LEN */
#ifndef FN_LEN
#define FN_LEN           64
#endif /* FN_LEN */
#ifndef CMD_LEN
#define CMD_LEN         138  /* 2 x FN_LEN + 10   */
#endif /* CMD_LEN */

#define NOT_PARSED -1

/* Ensure globals only declared once ... */
/* Relies on NO_MAIN being defined in libraries */
#ifndef NO_MAIN
/* Set up a contiguous buffer region */
static struct
{
  char A[BUF_LEN];
  char B[BUF_LEN];
  char C[BUF_LEN];
  char D[BUF_LEN];
} pad;

char *buffer1=pad.A;
char *buffer2=pad.B;
char *buffer3=pad.C;
char *buffer4=pad.D;

char notab;
int chars,lines;
#else /* !NO_MAIN */
extern char *buffer1;
extern char *buffer2;
extern char *buffer3;
extern char *buffer4;

extern char notab;
extern int chars,lines;
#endif /* !NO_MAIN */

#ifdef IS_MAIN 
/* Command Line Arguments and *.arg files ... */
extern char argfile[];
extern char argfopen;
extern char argfread;
extern FILE *afn;
#endif /* IS_MAIN */

void reset_stats(void);

void report_stats(char *file);

int fetch(char line[],
	  FILE *rfn);
/* FETCH a LINE, returning its LENGTH and removing trailing blanks,
 * and carriage returns (when DOS files read via UNIX!)
 */

int parse_sp(char *substr,
	     char *string);
/* PARSE SPace separated strings */

int parse_spt(char *substr,
	      char *string);
/* PARSE SPace or Tab separated strings */

void get_date(char *date);

#define DATEANDTIME_BUF_LEN 26
#define TIMENOW_BUF_LEN      9

void get_dateandtime(char dateandtime[DATEANDTIME_BUF_LEN]);

void get_timenow(char timenow[TIMENOW_BUF_LEN]);

char *get_stem(char filename[]);
/* GET STEM of FILENAME with form STEM.EXT */

char *get_ext(char filename[]);
/* GET EXT of FILENAME with form STEM.EXT */

void make_name(char newname[],
	       char ext[],
	       char oldname[]);
/* MAKE NEW fileNAME from OLD fileNAME and EXTention - form OLDNAME.EXT */

void remove_mule(char* string);
/* REMOVE Multiple UnderLinEs from STRING */

void remove_pars(char* string);
/* REMOVE PARentheseS from STRING */

void remove_squot(char* string);
/* REMOVE Single QUOTes from STRING */

void remove_dquot(char* string);
/* REMOVE Double QUOTes from STRING */

void make_comma_white(char* string);
/* MAKE COMMAs into WHITE space */

void make_dquot_white(char* string);
/* MAKE Double QUOTes into WHITE space */

void make_eq_white(char* string);
/* MAKE EQuals [=] into WHITE space */

void make_sc_white(char* string);
/* MAKE Semi-Colons [;] into WHITE space */

void make_col_white(char* string);
/* MAKE COLons [:] into WHITE space */

void Open_Arg_File(void);

/* Skeleton code etc. to be copied into the main program file */
/* and uncommented (and modified/added to) ...                */

/*
 * 
 * typedef enum
 * {
 *   ARGFILE,   
 *   COMMENT,   
 *   ECHO,      
 *   OUTAUTO,   
 *   OUTFILE,   
 *   QUIET,     
 *   VERBOSE,   
 *   LAST_ARG,
 *   ARGTYPES
 * } argnames;
 * 
 * argnames what,which;
 * char *arglist[ARGTYPES];
 * 
 * void setupargs(void)
 * {
 *   arglist[ARGFILE]  = "-a";
 *   arglist[COMMENT]  = "*";
 *   arglist[ECHO]     = "-E";
 *   arglist[OUTAUTO]  = "-O";
 *   arglist[OUTFILE]  = "-o";
 *   arglist[QUIET]    = "-Q";
 *   arglist[VERBOSE]  = "-V";
 *   arglist[LAST_ARG] = "";  
 *   make_name(argfile, "arg", getRCSFile(FALSE));
 * }
 * 
 * argnames whicharg(char *argstr)
 * {
 *   argnames current;
 *   char flag;
 *   char *p1,*p2;
 *   flag = FALSE;
 *   for (current=ARGFILE; current<LAST_ARG; current++)
 *     {
 *       p1 = argstr;
 *       p2 = arglist[current];
 *       if (*p2 == NUL) break;
 *       while ((*p2 == *p1) && (*p1!=NUL)) 
 * 	{
 * 	  p1++;
 * 	  p2++;
 * 	}
 *       if ((*p1==NUL) && (*p2==NUL))
 * 	{
 * 	  flag = TRUE;
 * 	  break;
 * 	}
 *     }
 *   if (flag) return(current);
 *   else return(LAST_ARG);
 * }
 * 
 * void listargs(void)
 * {
 *   for (what=ARGFILE; what<LAST_ARG; what++)
 *     printf("arglist[%2d] = %s\n", what, arglist[what]);
 * }
 *
 * void Read_Arg_File(void)
 * {
 *   static int l,k;
 *   do
 *     {
 *       l = fetch(buffer1, afn);
 *       if (l > 1)
 * 	{
 * 	  strcpy(buffer3, buffer1);
 * 	  strcpy(buffer4, buffer1);
 * 	  k = parse_sp(buffer1, buffer3);
 * 	  k = parse_sp(buffer2, buffer3);
 * 	  what = whicharg(buffer1);
 * 	  switch(what)
 * 	    {
 * 	    case ARGFILE: break;
 * 	    case COMMENT: printf("%s\n", buffer4); break;
 * 	    case ECHO:    eflag=TRUE;   break;
 * 	    case OUTAUTO: oflag=TRUE;   strcpy(prefix, get_stem(infile)); break;
 * 	    case OUTFILE: oflag=TRUE;   strcpy(prefix, buffer2); break;
 * 	    case QUIET:   vflag=FALSE;  break;
 * 	    case VERBOSE: eflag=TRUE;   vflag=TRUE; break;
 * 	    case LAST_ARG: printf("* ? %s %s\n", buffer1, buffer2); break;
 * 	    default: break;
 * 	    }
 * 	}
 *     }
 *   while(!feof(afn));
 *   fclose(afn);
 *   argfopen = FALSE;
 *   argfread = TRUE;
 *   printf(" ... done.\n");
 * }
 * 
 */

#endif /* !PARSEL_H */


/* Change Log */
/*
 * $Log: parsel.h,v $
 * Revision 1.3  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.2  2017/08/19 18:05:15  pi
 * Implicit definition warnings solved and code cleaned up
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.4  2015/06/30 22:51:39  pi
 * Interim commit
 *
 * Revision 1.3  2015/06/29 18:32:14  pi
 * get_dateandtime() & get_timenow() functions added
 *
 * Revision 1.2  2014/09/08 21:50:20  pi
 * Interim commit
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.1  2014/05/26 21:44:15  pi
 * Initial revision
 *
 * Revision 1.1  2013/06/03 20:58:09  pi
 * Initial revision
 *
 * Revision 1.1  2013/05/15 07:36:16  pi
 * Initial revision
 *
 * Revision 1.1  2013/03/07 15:07:42  pi
 * Initial revision
 *
 * Revision 1.1  2013/02/17 14:53:27  pi
 * Initial revision
 *
 * Revision 1.1  2012/08/06 10:08:56  trevor
 * Initial revision
 *
 * Revision 1.1  2010/08/02 13:28:30  gowen
 * Initial revision
 *
 * Revision 1.2  2010/08/02 10:12:41  gowen
 * Now building full conveniance library
 *
 *
 *
 */
