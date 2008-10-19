/* TolFindExecutable: Unix version for GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

/* The following code is taken and adapted from unix/tclUnixFile.c at
   Tcl source distribution */

#include <tol/tol_init.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <ctype.h>

#define UCHAR(c) ((unsigned char) (c)) 

/*
 * Make sure that MAXPATHLEN and MAXNAMLEN are defined.
 */

#ifndef MAXPATHLEN
#   ifdef PATH_MAX
#       define MAXPATHLEN PATH_MAX
#   else
#       define MAXPATHLEN 2048
#   endif
#endif

#ifndef MAXNAMLEN
#   ifdef NAME_MAX
#       define MAXNAMLEN NAME_MAX
#   else
#       define MAXNAMLEN 255
#   endif
#endif

/*
 * Define access mode constants if they aren't already defined.
 */

#ifndef F_OK
#    define F_OK 00
#endif
#ifndef X_OK
#    define X_OK 01
#endif
#ifndef W_OK
#    define W_OK 02
#endif
#ifndef R_OK
#    define R_OK 04
#endif

#ifndef S_ISREG
#   ifdef S_IFREG
#       define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#   else
#       define S_ISREG(m) 0
#   endif
#endif /* !S_ISREG */

/*
 *---------------------------------------------------------------------------
 *
 * TolFindExecutable --
 *
 *	This procedure computes the absolute path name of the current
 *	application, given its argv[0] value.
 *
 * Results:
 *	the path to the executable.
 *
 * Side effects:
 *	The variable tolNativeExecutableName gets filled in with the file
 *	name for the application, if we figured it out.  If we couldn't
 *	figure it out, tolNativeExecutableName is set to NULL.
 *
 *---------------------------------------------------------------------------
 */

static char*tolNativeExecutableName = NULL;

char *
TolFindExecutable(const char *argv0)
/* The value of the application's argv[0] (native). */
{
  const char *name, *p;
  struct stat statBuf;
  int length;
  char _buffer[MAXPATHLEN], *buffer;

  if (argv0 == NULL) {
    return NULL;
  }
  if (tolNativeExecutableName != NULL) {
    return tolNativeExecutableName;
  }
  
  name = argv0;
  for (p = name; *p != '\0'; p++) {
    if (*p == '/') {
      /*
       * The name contains a slash, so use the name directly
       * without doing a path search.
       */
      
      goto gotName;
    }
  }
  
  p = getenv("PATH");					/* INTL: Native. */
  if (p == NULL) {
    /*
     * There's no PATH environment variable; use the default that
     * is used by sh.
     */
    
    p = ":/bin:/usr/bin";
  } else if (*p == '\0') {
    /*
     * An empty path is equivalent to ".".
     */
    
    p = "./";
  }

  /*
   * Search through all the directories named in the PATH variable
   * to see if argv[0] is in one of them.  If so, use that file
   * name.
   */
  
  while (1) {
    while (isspace(UCHAR(*p))) {		/* INTL: BUG */
      p++;
    }
    name = p;
    while ((*p != ':') && (*p != 0)) {
      p++;
    }
    buffer = &_buffer[0];
    if (p != name) {
      strncat(buffer, name, p - name);
      //Tcl_DStringAppend(&buffer, name, p - name);
      if (p[-1] != '/') {
        strcat(buffer, "/");
        //Tcl_DStringAppend(&buffer, "/", 1);
      }
    }
    name = strcat(buffer, argv0);
    //name = Tcl_DStringAppend(&buffer, argv0, -1);
        
    if ((access(name, X_OK) == 0)	/* INTL: Native. */
        && (stat(name, &statBuf) == 0)	/* INTL: Native. */
        && S_ISREG(statBuf.st_mode)) {
      goto gotName;
    }
    if (*p == '\0') {
      break;
    } else if (*(p+1) == 0) {
      p = "./";
    } else {
      p++;
    }
  }
  goto done;
  
  /*
   * If the name starts with "/" then just copy it to tolExecutableName.
   */
  
 gotName:
  if (name[0] == '/')  {
    /*Tcl_ExternalToUtfDString(NULL, name, -1, &nameString);
    tclNativeExecutableName = (char *)
      ckalloc((unsigned) (Tcl_DStringLength(&nameString) + 1));
    strcpy(tclNativeExecutableName, Tcl_DStringValue(&nameString));
    Tcl_DStringFree(&nameString);*/
    tolNativeExecutableName = strdup(name);
    goto done;
  }
  
  /*
   * The name is relative to the current working directory.  First
   * strip off a leading "./", if any, then add the full path name of
   * the current working directory.
   */
  
  if ((name[0] == '.') && (name[1] == '/')) {
    name += 2;
  }
  
  /*Tcl_ExternalToUtfDString(NULL, name, -1, &nameString);
  
  Tcl_DStringFree(&buffer);
  TclpGetCwd(NULL, &buffer);*/
  buffer = getcwd(_buffer, MAXPATHLEN);
  
  /*length = Tcl_DStringLength(&buffer) + Tcl_DStringLength(&nameString) + 2;
    tclNativeExecutableName = (char *) ckalloc((unsigned) length);
    strcpy(tclNativeExecutableName, Tcl_DStringValue(&buffer));
    tclNativeExecutableName[Tcl_DStringLength(&buffer)] = '/';
    strcpy(tclNativeExecutableName + Tcl_DStringLength(&buffer) + 1,
    Tcl_DStringValue(&nameString));
    Tcl_DStringFree(&nameString);*/
  
  length = strlen(buffer) + strlen(name) + 2;
  tolNativeExecutableName = (char *)malloc(length);
  strcpy(tolNativeExecutableName, buffer);
  tolNativeExecutableName[strlen(buffer)] = '/';
  strcpy(tolNativeExecutableName + strlen(buffer) + 1, name);
  
 done:
  /*Tcl_DStringFree(&buffer);*/
  return tolNativeExecutableName;
}
