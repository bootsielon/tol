## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright 2007 Free Software Foundation, Inc.
## Written by Daniel Rus <danirus@tol-project.org>, 2007
##
## This file is free software; the Free Software Foundation gives
## unlimited permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.#

auxdir			= @ac_aux_dir@
AUX_DIST		= $(auxdir)/config.guess \
			$(auxdir)/config.sub \
			$(auxdir)/install-sh \
			$(auxdir)/depcomp \
			$(auxdir)/ltmail.sh \
			$(auxdir)/missing \
			$(auxdir)/toldbd.m4

EXTRA_DIST		= common-build Jamfile

DEFS			= -DUNIX
DEFAULT_INCLUDES	=

pkginclude_HEADERS	= libtolbdb.h
lib_LTLIBRARIES		=

##--------------------------------------------------
## libtolodbc driver:

if odbc_ENABLED
if odbc_INC_DIR
  DEFAULT_INCLUDES	+= -I$(odbc_INC_DIR)
endif
endif

if odbc_ENABLED
lib_LTLIBRARIES 	+= tolodbc0.la
tolodbc0_la_SOURCES	= libtolodbc.c
tolodbc0_la_LDFLAGS 	= -l$(odbc_LIB) -Wl,-z,defs -module \
			-version-info $(TOLDBD_LT_VERSION) 
endif

if odbc_ENABLED
if odbc_LIB_DIR
  tolodbc0_la_LDFLAGS	+= -L$(odbc_LIB_DIR)
endif
endif

##--------------------------------------------------
## libtolmysql driver:

if mysql_ENABLED
if mysql_INC_DIR
  DEFAULT_INCLUDES	+= -I$(mysql_INC_DIR)
endif
endif

if mysql_ENABLED
lib_LTLIBRARIES 	+= tolmysql0.la
tolmysql0_la_SOURCES	= libtolmysql.c
tolmysql0_la_LDFLAGS 	= -l$(mysql_LIB) -Wl,-z,defs -module \
			-version-info $(TOLDBD_LT_VERSION) 
endif

if mysql_ENABLED
if mysql_LIB_DIR
  tolmysql0_la_LDFLAGS	+= -L$(mysql_LIB_DIR)
endif
endif

##--------------------------------------------------
## libtolpgsql driver:

if postgres_ENABLED
if postgres_INC_DIR
  DEFAULT_INCLUDES	+= -I$(postgres_INC_DIR)
endif
endif

if postgres_ENABLED
lib_LTLIBRARIES 	+= tolpgsql0.la
tolpgsql0_la_SOURCES	= libtolpgsql.c
tolpgsql0_la_LDFLAGS 	= -l$(postgres_LIB) -Wl,-z,defs -module \
			-version-info $(TOLDBD_LT_VERSION) 
endif

if postgres_ENABLED
if postgres_LIB_DIR
  tolpgsql0_la_LDFLAGS	+= -L$(postgres_LIB_DIR)
endif
endif
