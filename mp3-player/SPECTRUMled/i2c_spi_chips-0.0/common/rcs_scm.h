/* RCS-based `Software Configuration Management' Header
 * $Id: rcs_scm.h,v 1.3 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef RCS_SCM_H
#define RCS_SCM_H 1

#define RCS_DATE_LEN   16
#define RCS_FN_LEN     64
#define RCS_STATE_LEN  16
#define RCS_STR_LEN   128
#define RCS_VSN_LEN    16
#define SCM_VSN_LEN    16

#ifndef TRUE
/* May be defined via wiringPi! */
#define TRUE       1
#define FALSE      0
#endif /* TRUE */
#define NUL        '\0'

#include <string.h>

typedef enum
{
  AUTHOR_RCS,
  AUTHOR_SCM,
  DATE,
  FILE_RCS,
  ID,
  NAME,
  REVISION,
  STATE,
  SVERSION,
  LAST_RCS,
  RCS_TYPES
} RCSnames;

RCSnames whichRCS;
static char *RCSlist[RCS_TYPES];

/* Copy these blocks into the (new) main.c file & uncomment them */
/* RCS info. blocks */
/*
 * static char RCSAuthor[]   = "$Author: pi $";
 * static char SCMAuthor[]   = "$SCMauthor: Anon. $";
 * static char RCSDate[]     = "$Date: 2018/03/31 21:31:30 $";
 * static char RCSFile[]     = "$RCSfile: rcs_scm.h,v $";
 * static char RCSId[]       = "$Id: rcs_scm.h,v 1.3 2018/03/31 21:31:30 pi Exp $";
 * static char RCSName[]     = "$Name:  $";
 * static char RCSRevision[] = "$Revision: 1.3 $";
 * static char RCSState[]    = "$State: Exp $";
 * static char SCMVersion[]  = "$SCMversion: 0.00 $";
 *
 * void setup_RCSlist(void)
 * {
 *   RCSlist[AUTHOR_RCS] = RCSAuthor;
 *   RCSlist[AUTHOR_SCM] = SCMAuthor;
 *   RCSlist[DATE]       = RCSDate;
 *   RCSlist[FILE_RCS]   = RCSFile;
 *   RCSlist[ID]         = RCSId;
 *   RCSlist[NAME]       = RCSName;
 *   RCSlist[REVISION]   = RCSRevision;
 *   RCSlist[STATE]      = RCSState;
 *   RCSlist[SVERSION]   = SCMVersion;
 *   RCSlist[LAST_RCS]   = "$$";
 * }
 */

char *getRCSAuthor(void)
{
  static char author[RCS_STR_LEN];
  static char *p1,*p2;
  strcpy(author, "Anon.");
  p1 = RCSlist[AUTHOR_RCS];
  p2 = author;
  while((*p1!=' ') && (*p1!=NUL)) p1++;
  p1++;
  while((*p1!=' ') && (*p1!=NUL)) *p2++ = *p1++;
  *p2 = NUL;
  p1  = author;
  return(p1);
}

char *getSCMAuthor(void)
{
  static char author[RCS_STR_LEN];
  static char *p1,*p2;
  strcpy(author, "Anon.");
  p1 = RCSlist[AUTHOR_SCM];
  p2 = author;
  while((*p1!=' ') && (*p1!=NUL)) p1++;
  if (*p1 == NUL) return(p2);
  p1++;
  while((*p1!=' ') && (*p1!=NUL)) *p2++ = *p1++;
  *p2 = NUL;
  p1  = author;
  return(p1);
}

char *getRCSDate(void)
{
  static char date[RCS_DATE_LEN],temp[RCS_DATE_LEN];
  static char *p1,*p2;
  int i,j;
  strcpy(temp, "??/??/200?");
  p1 = RCSlist[DATE];
  p2 = temp;
  while((*p1!=' ') && (*p1!=NUL)) p1++;
  if (*p1 == NUL) return(p2);
  p1++;
  for (i=0; i<10; i++) *p2++ = *p1++;
  temp[10] = NUL;
  /* Convert to UK format */
  for (i=0; i<4; i++)
    {
      j = i + 6;
      date[j] = temp[i];
    }
  date[0]  = temp[8];
  date[1]  = temp[9];
  date[3]  = temp[5];
  date[4]  = temp[6];
  date[2]  = '/';
  date[5]  = '/';
  date[10] = NUL;
  p1       = date;
  return(p1);
}

char *getRCSFile(char flag)
{
  static char file[RCS_FN_LEN];
  static char *p1,*p2;
  strcpy(file, "Noname");
  p1 = RCSlist[FILE_RCS];
  p2 = file;
  while((*p1!=' ') && (*p1!=NUL)) p1++;
  if (*p1 == NUL) return(p2);
  p1++;
  while(*p1!=',') *p2++ = *p1++;
  *p2 = NUL;
  p1  = file;
  if (flag)
    {
      /* Remove trailing .ext if it exists */
      p2 = file;
      while (*p2 != NUL) p2++;
      while ((*p2!= '.') && (p2!=file)) p2--;
      if (p2 != file) *p2 = NUL;
    }
  return(p1);
}

char *getRCSState(void)
{
  static char state[RCS_STATE_LEN];
  static char *p1,*p2;
  strcpy(state, "Unknown");
  p1 = RCSlist[STATE];
  p2 = state;
  while((*p1!=' ') && (*p1!=NUL)) p1++;
  if (*p1 == NUL) return(p2);
  p1++;
  while((*p1!=' ') && (*p1!=NUL)) *p2++ = *p1++;
  *p2 = NUL;
  p1  = state;
  return(p1);
}

char *getRCSVersion(void)
{
  static char version[RCS_VSN_LEN];
  static char *p1,*p2;
  strcpy(version, "1.0");
  p1 = RCSlist[REVISION];
  p2 = version;
  while((*p1!=' ') && (*p1!=NUL)) p1++;
  if (*p1 == NUL) return(p2);
  p1++;
  while((*p1!=' ') && (*p1!=NUL)) *p2++ = *p1++;
  *p2 = NUL;
  p1  = version;
  return(p1);
}

char *getSCMVersion(void)
{
  static char version[SCM_VSN_LEN];
  static char *p1,*p2;
  strcpy(version, "0.00");
  p1 = RCSlist[SVERSION];
  p2 = version;
  while((*p1!=' ') && (*p1!=NUL)) p1++;
  if (*p1 == NUL) return(p2);
  p1++;
  while((*p1!=' ') && (*p1!=NUL)) *p2++ = *p1++;
  *p2 = NUL;
  p1  = version;
  return(p1);
}

void listRCSnames(void)
{
  for (whichRCS=AUTHOR_RCS; whichRCS<LAST_RCS; whichRCS++)
    printf("RCSlist[%2d] = %s\n", whichRCS, RCSlist[whichRCS]);
}

void print_scm(char * prog,
	       char * auth,
	       char * vsn,
	       char * state,
	       char * last)
{
/* Print program name, version, last modification date and author */
/* Call thus: print_scm(getRCSFile(FALSE),
 *                      getSCMAuthor(),
 *                      getSCMVersion(),
 *                      getRCSState(),
 *                      getRCSDate());
 *        or: print_scm(getRCSFile(FALSE),
 *                      getRCSAuthor(),
 *                      getRCSVersion(),
 *                      getRCSState(),
 *                      getRCSDate());
 *        or: print_scm(getRCSFile(FALSE),
 *                      getSCMAuthor(),
 *                      getRCSVersion(),
 *                      getRCSState(),
 *                      getRCSDate());
 *        or: print_scm(getRCSFile(FALSE),
 *                      getRCSAuthor(),
 *                      getSCMVersion(),
 *                      getRCSState(),
 *                      getRCSDate());
 * Note: Change FALSE to TRUE if you DON'T want the filename tag
 */
  printf(prog);
  printf(" Version ");
  printf(vsn);
  printf(" {");
  printf(state);
  printf("},");
  printf(" [");
  printf(last);
  printf("], (c) ");
  printf(auth);
  putchar('\n');
}

void print_us(char * prog,
	      char * usage)
{
/* Use it in this manner:
 * ...
 * if (argc < 2)
 *   {
 *     print_us(getRCSFile(TRUE), "[path/]filenam1[.tg1] ...");
 *     ...
 *     exit(1);
 *   }
 * ...
 */
  printf("Usage ");
  printf(prog);
  putchar(' ');
  puts(usage);
}

/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>

#define PKG_VSN_LEN    SCM_VSN_LEN
#define PKG_NAM_LEN    RCS_FN_LEN

char *getPKGVersion(void)
{
  static char version[PKG_VSN_LEN];
  static char *p;
  p  = version;
  strncpy(version, PACKAGE_VERSION, (PKG_VSN_LEN-1));
  version[PKG_VSN_LEN-1] = NUL;
  return(p);
}

char *getPKGName(void)
{
  static char name[PKG_NAM_LEN];
  static char *p;
  p  = name;
  strncpy(name, PACKAGE_NAME, (PKG_NAM_LEN-1));
  name[PKG_NAM_LEN-1] = NUL;
  return(p);
}

void print_pkg_info(char * package,
		    char * vsn)
{
/* PRINT PacKaGe info. ... */
  printf("\t\t<--| Package %s_%s |-->\n", package, vsn);
}
#endif /* HAVE_CONFIG_H */

#endif /* !RCS_SCM_H */

/* Change Log */
/*
 * $Log: rcs_scm.h,v $
 * Revision 1.3  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.2  2016/05/13 16:14:41  pi
 * updated here
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
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
 * Revision 1.1  2010/08/02 13:28:04  gowen
 * Initial revision
 *
 * Revision 1.1  2010/07/27 10:57:46  gowen
 * Initial revision
 *
 * Revision 1.1  2010/03/31 08:27:58  gowen
 * Initial revision
 *
 *
 *
 */

