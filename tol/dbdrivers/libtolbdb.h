/* libtoldbd.h: Struct definition for db access. TOL Language.

   Copyright (C) 2003-2007, Bayes Decision, SL (Spain [EU])

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

#ifndef LIBTOLBDB_H
# define LIBTOLBDB_H 1

typedef void (*DBOutWriter)(const char *);

enum TypeDataBase 
{
  TypeDBUnknown = 0, // Unknown field-types. Will be treated as char.
  TypeDBText    = 1, // CHAR, VARCHAR, TEXT, etc.
  TypeDBDate    = 2, // ALL date-related fields.
  TypeDBReal    = 3  // All numeric fields.
};

struct dateStruct 
{
  unsigned short int year;
  unsigned short int month;
  unsigned short int day;
  unsigned short int hour;
  unsigned short int minute;
  unsigned short int second;
  unsigned short int msecond;
};

struct openInfo
{
  unsigned short int min;
  unsigned short int max;
  enum TypeDataBase *types;
};

#endif // LIBTOLBDB_H
