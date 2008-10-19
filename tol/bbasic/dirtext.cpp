/* dirtext.cpp: BDir (dir)
                GNU/TOL Language.

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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bdir.h>

BTraceInit("dirtext.cpp");

//--------------------------------------------------------------------
BInt BDir::FindFile(const BText& fileName) const

/*! finds for the name of a file
 * \param fileName name of file
 * \return returns the index of a file with the given name or -1 if not
 *         exists.
 */
//--------------------------------------------------------------------
{
  for(BInt n=0; n<numFiles_; n++)
  {
    if(fileName_[n].Name()==fileName) { return(n); }
  }
  return(-1);
}


//--------------------------------------------------------------------
BInt BDir::FindDir(const BText& dirName) const

/*! finds for the name of a file
 * \param dirName name of directory
 * \return returns the index of a subdirectory with the given name or -1
 *         if not exists.
 */
//--------------------------------------------------------------------
{
  for(BInt n=0; n<numDirs_; n++)
  {
    if(dirName_[n].Name()==dirName) { return(n); }
  }
  return(-1);
}


//--------------------------------------------------------------------
BText BDir::Text(BBool sizes, BInt columns, BBool recursive)

/*! Sample information of the directory or file.
 * \param sizes show the sizes, default false
 * \param columns number of columns, default 3
 * \param recursive shows the descending directories, default false
 * \return the names of the directories with its archives and sizes
 */
//--------------------------------------------------------------------
{
  BBool	   first=BTRUE;
  BText	   txt(Name()+": ");
  BInt	   dirCol=1, filCol=1;

  txt=txt.RightPad(15);

  if(columns<1)	     { columns=1; }
  else if(columns>4) { columns=4; }

  if(columns>3 && sizes) { dirCol=columns; filCol=3; }
  else			 { dirCol=columns; filCol=columns; }

  if(!Exist()) { txt+=Out()+I2("Don't exist","No existe")+"\n"; }
  else if(IsFile())
  {
    txt=txt+I2("Is a file","Es un fichero");
    if(sizes)
    {
      BText aux(I2(" of "," de "));
      txt+=aux.InsidePad(13,'.',Bytes())+" bytes";
    }
    txt+="\n";
  }
  else if(Empty()) { txt=txt+I2("Is empty","Esta vacio")+"\n"; }
  else
  {
    txt+="\n";
    if(NumFiles()>0)
    {
      BInt c=0;
      txt=txt+I2("  Files: ","	Archivos: ")+NumFiles();
      for(BInt f=0; f<NumFiles(); f++)
      {
    	if(c==0) { txt+="\n  "; }
    	if(! sizes)	{ txt=txt+"  "+FileName(f).RightPad(15); }
    	else { txt=txt+"  "+FileName(f).InsidePad(23,'.',FileBytes(f)); }
	    c++;
	    if(c>=filCol) { c=0; }
      }
      txt+="\n";
    }
    if(NumDirs()>0)
    {
      if(!recursive)
      {
        BInt c=0;
    	txt=txt+I2("  Directories: ","	Directorios: ")+NumDirs();
    	for(BInt d=0; d<NumDirs(); d++)
     	{
    	  if(c==0) { txt+="\n	 "; }
	      txt+=DirName(d).RightPad(15);
     	  c++;
    	  if(c>=dirCol) { c=0; }
    	}
    	txt+="\n";
      }
      else
      {
    	for(BInt d=0; d<NumDirs(); d++)
    	{
    	  BDir subDir(Name()+TSLASH+DirName(d));
    	  txt+=subDir.Text(sizes,columns,recursive);
     	}
      }
    }
  }
  return(txt);
}


//--------------------------------------------------------------------
BText SysPath(const BText& path)

/*! Gets the path in the operative system style changing the
 *  slashes '/' and '\'
 * \param path route of a directory or file
 * \return the path in the operative system style
 */
//--------------------------------------------------------------------
{
  BText p = path;
  #ifdef UNIX
  return(p.Replace('\\','/'));
  #else
  return(p.Replace('/','\\'));
  #endif
}
