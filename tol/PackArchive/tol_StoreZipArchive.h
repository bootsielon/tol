/* tol_StoreZipArchive.h: ZipArchive API

                   GNU/TOL Language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_StoreZipArchive_H
#define TOL_StoreZipArchive_H 1

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include "tol_PackArchive.h"

/*
ZipStorage.h:164:56: warning: suggest parentheses around ‘&&’ within ‘||’ [-Wparentheses]
    return !m_pFile || m_state.IsSetAny(stateAutoClose) && m_pFile->IsClosed();
 */
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"
#endif
#include <ZipArchive.h>
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#include <ZipMemFile.h>

//------------------------------------------------------------------
   class StoreZipArchive : public Store
// Abstract archive store
//------------------------------------------------------------------
{
 public:
  CZipArchive _zip;
  int _exceptions;
  StoreZipArchive();
 ~StoreZipArchive();

  //Abre el archivo para lectura o escritura
  bool Open(BText path, char openMode);
  //Cierra el archivo
  bool Close();
  //Añade un fichero al archivo
  bool FileAdd(BText originalFilePath, BText pathInZip);
  //Extrae un fichero del archivo
  bool FileExtract(BText originalFilePath, BText destinationDirPath);
  //Determina si existe o no un fichero dentro del archivo
  bool FileExist(BText originalFilePath);
  //Añade un directorio completo recursivamente
  bool DirAdd(BText originalDirPath);
  //Extrae un directorio completo recursivamente
  bool DirExtract(BText originalDirPath, BText destinationDirPath);
  //Determina si existe o no un directorio dentro del archivo
  bool DirExist(BText originalDirPath);

};

#endif // TOL_StoreZipArchive_H

