/* C source file associated with a SELection of PARsing functions etc.
 * $Id: parsel.c,v 1.2 2018/03/31 21:31:30 pi Exp $
 */

#define NO_MAIN
#include "parsel.h"

/* Useful strings ... */
char blank[] = " ";
char error[] = "***ERROR*** ";
char warng[] = "**WARNING** "; 
/* Command Line Arguments and *.arg files ... */
char argfile[FN_LEN];
char argfopen=FALSE;
char argfread=FALSE;
FILE *afn;

void reset_stats(void)
{
  notab = TRUE;
  chars = 0;
  lines = 0;
}

void report_stats(char *file)
{
  if (!notab) printf("%s %s contained at least one tab character!\n",
		     warng,
		     file);
  printf("Processed %d characters in %d lines from %s.\n", 
	 chars, 
	 lines,
	 file);
}

int fetch(char line[],
	  FILE *rfn)
/* FETCH a LINE, returning its LENGTH and removing trailing blanks,
 * and carriage returns (when DOS files read via UNIX!)
 */
{
  char trunk;
  int length;
  length = 0;
  trunk = FALSE;
  do
    {
      line[length] = getc(rfn);
      if (line[length] == '\n') break;
      if (feof(rfn)) break;
      length++;
      /* Ensure no buffer overflow ... */
      if (length == (BUF_LEN - 1))
	{
	  line[length] = NUL;
	  length++;
	  trunk = TRUE;
	}	  
    }
  while ((!feof(rfn))&&(line[length-1]!=NUL));
  line[length] = NUL;
  lines++;
  if ((length == 0) || trunk) return(length);
  length--;
  while ((line[length] == ' ') || (line[length] == '\r'))
    {
      line[length] = NUL;
      length--;
    }
  length++;
  chars = chars + length;
  return(length);
}

int parse_sp(char *substr,
	     char *string)
/* PARSE SPace separated strings */
{
  char *p1,*p2;
  int l;
  l  = 0;
  p1 = string;
  p2 = substr;
  /* Skip leading blanks... */
  while (*p1 == ' ') p1++;
  /* Copy upto next blank or nul... */
  while ((*p1 != ' ') && (*p1 != NUL))
    {
      *p2++ = *p1;
      /* Overwrite copied char...*/
      *p1++ = ' ';
      l++;
    }
  *p2 = NUL;
  return(l);
}

int parse_spt(char *substr,
	      char *string)
/* PARSE SPace or Tab separated strings */
{
  char *p1,*p2;
  int l;
  l  = 0;
  p1 = string;
  p2 = substr;
  /* Skip leading blanks or tabs ... */
  while ((*p1 == ' ') || (*p1 == '\t')) 
    {
      /* 1st. check for tabs ... */
      if (notab && (*p1 == '\t')) notab = FALSE;
      p1++;
    }
  /* Copy upto next blank, tab  or nul... */
  while ((*p1 != ' ') && (*p1 != '\t') && (*p1 != NUL))
    {
      *p2++ = *p1;
      /* 2nd. check for tabs ... */
      if (notab && (*p1 == '\t')) notab = FALSE;
      /* Overwrite copied char...*/
      *p1++ = ' ';
      l++;
    }
  *p2 = NUL;
  return (l);
}

void get_date(char *date)
{
  char i,*dateandtime,digit1,digit2,*ptr;
  time_t timeis;
  ptr = date;  
  time(&timeis);
  dateandtime = ctime(&timeis);
  dateandtime = dateandtime+4;
  for (i=0; i<3; i++) *ptr++ = *dateandtime++;
  *ptr++ = '.';
  for (i=0; i<3; i++) *ptr++ = *dateandtime++;
  ptr--;
  digit2 = *ptr--;
  digit1 = *ptr++;
  ptr++;
  switch (digit2)
    {
    case '1':
    {
      if (digit1 == '1')
	{
	  *ptr++ = 't';
	  *ptr++ = 'h';
	}
      else
	{
	  *ptr++ = 's';
	  *ptr++ = 't';
	}
      break;
    }
    case '2':
    {
      if (digit1 == '1')
	{
	  *ptr++ = 't';
	  *ptr++ = 'h';
	}
      else
	{
	  *ptr++ = 'n';
	  *ptr++ = 'd';
	}
      break;
    }
    case '3':
    {
      if (digit1 == '1')
	{
	  *ptr++ = 't';
	  *ptr++ = 'h';
	}
      else
	{
	  *ptr++ = 'r';
	  *ptr++ = 'd';
	}
      break;
    }
    default:
    {
      *ptr++ = 't';
      *ptr++ = 'h';
    }
    }
  *ptr++ = '.';
  *ptr++ = ',';
  dateandtime = dateandtime+9;
  for (i=0; i<5; i++) *ptr++ = *dateandtime++;
  *ptr = NUL;
}

void get_dateandtime(char dateandtime[DATEANDTIME_BUF_LEN])
{
  char *ldateandtime,*p1;
  int i;
  time_t timeis;
  time(&timeis);
  ldateandtime = ctime(&timeis);
  p1 = dateandtime;
  for (i=0; i<DATEANDTIME_BUF_LEN; i++)
    {
      *p1++ = *ldateandtime++;
      if (*ldateandtime == NUL)
	{
	  i = DATEANDTIME_BUF_LEN;
	  *p1 = NUL;
	  break;
	}
    }
  dateandtime[DATEANDTIME_BUF_LEN-1] = NUL;
}

void get_timenow(char timenow[TIMENOW_BUF_LEN])
{
  char *ldateandtime,*p1,*p2;
  int i;
  time_t timeis;
  time(&timeis);
  ldateandtime = ctime(&timeis);
  p1 = ldateandtime + 11;
  p2 = timenow;
  for (i=0; i<TIMENOW_BUF_LEN; i++)
    {
      *p2++ = *p1++;
      if (*p1 == NUL)
	{
	  i = TIMENOW_BUF_LEN;
	  *p2 = NUL;
	  break;
	}
    }
  timenow[TIMENOW_BUF_LEN-1] = NUL;
}

char *get_stem(char filename[])
/* GET STEM of FILENAME with form STEM.EXT */
{
  static char stem[FN_LEN];
  char *p1,*p2,*p3;
  p1 = p3 = filename;
  while (*p1 != NUL) 
    {
      p1++;
      p3++;
    }
  p2 = p1;
  while ((*p1!= '.') && (p1!=filename))
    {
      p1--;
      p3--;
    }
  if (p3 == filename) p3=p2;
  p1 = filename;
  p2 = stem;
  while ((*p1!=NUL) && (p1!=p3)) *p2++ = *p1++;
  *p2 = NUL;  
  return(stem);
}

char *get_ext(char filename[])
/* GET EXT of FILENAME with form STEM.EXT */
{
  static char ext[FN_LEN];
  char *p1,*p2;
  p1 = filename;
  while (*p1 != NUL) p1++;
  p2 = p1;
  while ((*p1!= '.') && (p1!=filename)) p1--;
  p1++;
  p2 = ext;
  while (*p1 != NUL) *p2++ = *p1++;
  *p2 = NUL;  
  return(ext);
}

void make_name(char newname[],
	       char ext[],
	       char oldname[])
/* MAKE NEW fileNAME from OLD fileNAME and EXTention - form OLDNAME.EXT */
{
  char *p1,*p2,*p3,*p4;
  p1 = p3 = oldname;
  while (*p1 != NUL)
    {
      p1++;
      p3++;
    }
  p4 = p1;
  while ((*p1!= '.') && (p1!=oldname))
    {
      p1--;
      p3--;
    }
  if (p3 == oldname) p3=p4;
  p1 = oldname;
  p2 = newname;
  while ((*p1!=NUL) && (p1!=p3)) *p2++ = *p1++;
  p1 = ext;
  *p2++ = '.';
  while (*p1!=NUL) *p2++ = *p1++;
  *p2 = NUL;
}

void remove_mule(char* string)
/* REMOVE Multiple UnderLinEs from STRING */
{
  char *p1,*p2,*p3;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = p1 + 1;
  p3 = string;
  if (*p3 == NUL) return;
  while (*p1 != NUL)
    {
      if (*p1 != '_') *p3++ = *p1++;
      else
	{
	  p2 = p1 + 1;
	  if (*p2 == '_') p1 = p2;
	  *p3++ = *p1++;
	}
    }
  *p3 = NUL;
}

void remove_pars(char* string)
/* REMOVE PARentheseS from STRING */
{
  char *p1,*p2;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = string;
  if (*p2 == NUL) return;
  while (*p1 != NUL)
    {
      if ((*p1 != '(') && (*p1 != ')')) *p2++ = *p1++;
      else p1++;
    }
  *p2 = NUL;
}

void remove_squot(char* string)
/* REMOVE Single QUOTes from STRING */
{
  char *p1,*p2;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = string;
  if (*p2 == NUL) return;
  while (*p1 != NUL)
    {
      if (*p1 != '\'') *p2++ = *p1++;
      else p1++;
    }
  *p2 = NUL;
}

void remove_dquot(char* string)
/* REMOVE Double QUOTes from STRING */
{
  char *p1,*p2;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = string;
  if (*p2 == NUL) return;
  while (*p1 != NUL)
    {
      if (*p1 != '"') *p2++ = *p1++;
      else p1++;
    }
  *p2 = NUL;
}

void make_comma_white(char* string)
/* MAKE COMMAs into WHITE space */
{
  char *p1,*p2;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = string;
  if (*p2 == NUL) return;
  while (*p1 != NUL)
    {
      if (*p1 != ',') *p2++ = *p1++;
      else 
	{
	  p1++;
	  *p2++ = ' ';
	}
    }
  *p2 = NUL;
}

void make_dquot_white(char* string)
/* MAKE Double QUOTes into WHITE space */
{
  char *p1,*p2;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = string;
  if (*p2 == NUL) return;
  while (*p1 != NUL)
    {
      if (*p1 != '"') *p2++ = *p1++;
      else 
	{
	  p1++;
	  *p2++ = ' ';
	}
    }
  *p2 = NUL;
}

void make_eq_white(char* string)
/* MAKE EQuals [=] into WHITE space */
{
  char *p1,*p2;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = string;
  if (*p2 == NUL) return;
  while (*p1 != NUL)
    {
      if (*p1 != '=') *p2++ = *p1++;
      else 
	{
	  p1++;
	  *p2++ = ' ';
	}
    }
  *p2 = NUL;
}

void make_sc_white(char* string)
/* MAKE Semi-Colons [;] into WHITE space */
{
  char *p1,*p2;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = string;
  if (*p2 == NUL) return;
  while (*p1 != NUL)
    {
      if (*p1 != ';') *p2++ = *p1++;
      else 
	{
	  p1++;
	  *p2++ = ' ';
	}
    }
  *p2 = NUL;
}

void make_col_white(char* string)
/* MAKE COLons [:] into WHITE space */
{
  char *p1,*p2;
  strncpy(buffer1, string, BUF_LEN);
  p1 = buffer1;
  p2 = string;
  if (*p2 == NUL) return;
  while (*p1 != NUL)
    {
      if (*p1 != ':') *p2++ = *p1++;
      else 
	{
	  p1++;
	  *p2++ = ' ';
	}
    }
  *p2 = NUL;
}

/* Command Line Arguments and *.arg files ... */
void Open_Arg_File(void)
{
  if ((afn=fopen(argfile, "r")) == NULL)
    {
      fprintf(stderr, "<%s>\t", argfile);
      perror("fopen");
      printf("%s not found\n", argfile);
      argfopen = FALSE;
    }
  else 
    {
      printf("Loading %s ...\n", argfile);
      argfopen = TRUE;
    }
}

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
 * 
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

/* Change Log */
/*
 * $Log: parsel.c,v $
 * Revision 1.2  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.3  2015/06/30 22:51:39  pi
 * Interim commit
 *
 * Revision 1.2  2015/06/29 18:32:14  pi
 * get_dateandtime() & get_timenow() functions added
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
 * Revision 1.1  2010/08/02 13:28:55  gowen
 * Initial revision
 *
 * Revision 1.1  2010/08/02 10:12:41  gowen
 * Initial revision
 *
 *
 *
 */
