/* oiscompress.cpp: main and init functions of GNU/TOL language.

   Copyright (C) 2005-2007, Bayes Decision, SL (Spain [EU])

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
#  include <win_tolinc.h>
#endif

#include <tol/tol_oiscreator.h>
#include <tol/tol_oisloader.h>
#include <bzlib.h>


BTraceInit("oiscompress.cpp");

static size_t _MaxBlockLength_ = 1024*1024;

//--------------------------------------------------------------------
// OIS compression coding functions
//--------------------------------------------------------------------
//--------------------------------------------------------------------
  bool BOisCreator::WriteBlock(const void* v, size_t size, size_t count, BStream* stream)
//--------------------------------------------------------------------
{
  unsigned int sourceLen, destLen;
  sourceLen = size*count;
  destLen   = sourceLen;
  size_t w, c;
  if(!enable_BSE_ || (options_.compressor_.serialization_.engine_!=BSE_BZIP2_) ||
    (sourceLen<options_.compressor_.serialization_.minSizeCmprs_))
  {
    w = stream->Write(v,size,c=count);
  }
  else //BZip2 Compress method
  {
    char* dest = new char[destLen];
    int cmprs = BZ2_bzBuffToBuffCompress 
    ( 
      dest, 
      &destLen,
      (char*)v, 
      sourceLen,
      options_.compressor_.serialization_.level_, //blockSize100k: compression factor (1..9)
      0, //verbosity: 0..3
      30  //workFactor: sorting effort(1-255) 0 is default value 30
    );
    if(cmprs==BZ_OUTBUFF_FULL)
    {
      destLen=0;
      EWrite(destLen, stream);
      w = stream->Write(v,size,c=count);
    }
    else
    {
    //Std(BText("\nBOisCreator::Write local compress from ")<< (int)sourceLen << " to " << (int)destLen <<" bytes");
      EWrite(destLen, stream);
      w = stream->Write(dest,1,c=destLen);
    }
  }
  if(w!=c) 
  { 
    return(Error(I2("Cannot write on file ",
                    "No se pudo escribir en el fichero ")+stream->Name()+" "+
                 (int)count+ I2(" items of " ," elementos de ")+
                 (int)size+ "bytes")); 
  }
#ifdef TRACE_OIS_FILE
  if(&stream==&tolref_) { return(true); }
  if(&stream==&oisref_) { return(true); }
  if(&stream==&code_)   { return(true); }
  fprintf(logWrite_, "\n%s\tw\t%ld\t%ld", stream->Name().String(), size, count);
  fflush(logWrite_);
#endif
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::Write(const void* v_, size_t size, size_t count, BStream* stream)
//--------------------------------------------------------------------
{
  int maxCount = 1 + _MaxBlockLength_ / size;
  int c = count;
  char* v = (char*)v_;
  bool ok = true;
  while(ok && (c>maxCount))
  {
    ok=WriteBlock(v,size,maxCount,stream);
    v += size*maxCount;
    c -= maxCount;
  }
  if(ok && c)  
  {
    ok=WriteBlock(v,size,c,stream);
  }
  return(ok);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const BINT64& x, BStream* stream)
//--------------------------------------------------------------------
{
  union un { BINT64 x_; char s_[8]; } u;
  u.x_=x;
  char i;
  if(control_.machine_.isLittleEndian_)
  {
    for(i=7; i>=0; i--)
    {
      if(u.s_[i]) { break; }
    }
    Ensure(Write(i, stream));
    for(; i>=0; i--)
    {
      Ensure(Write(u.s_[i], stream));
    }
  }
  else
  {
    for(i=0; i<8; i++)
    {
      if(u.s_[i]) { break; }
    }
    Ensure(Write(i, stream));
    for(; i<8; i++)
    {
      Ensure(Write(u.s_[i], stream));
    }
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const unsigned BINT64& x, BStream* stream)
//--------------------------------------------------------------------
{
  union un { unsigned BINT64 x_; char s_[8]; } u;
  u.x_=x;
  char i;
  if(control_.machine_.isLittleEndian_)
  {
    for(i=7; i>=0; i--)
    {
      if(u.s_[i]) { break; }
    }
    Ensure(Write(i, stream));
    for(; i>=0; i--)
    {
      Ensure(Write(u.s_[i], stream));
    }
  }
  else
  {
    for(i=0; i<8; i++)
    {
      if(u.s_[i]) { break; }
    }
    Ensure(Write(i, stream));
    for(; i<8; i++)
    {
      Ensure(Write(u.s_[i], stream));
    }
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const int& x, BStream* stream)
//--------------------------------------------------------------------
{
  union un { int x_; char s_[4]; } u;
  u.x_=x;
  char i;
  if(control_.machine_.isLittleEndian_)
  {
    for(i=3; i>=0; i--)
    {
      if(u.s_[i]) { break; }
    }
    Ensure(Write(i, stream));
    for(; i>=0; i--)
    {
      Ensure(Write(u.s_[i], stream));
    }
  }
  else
  {
    for(i=0; i<4; i++)
    {
      if(u.s_[i]) { break; }
    }
    Ensure(Write(i, stream));
    for(i; i<4; i++)
    {
      Ensure(Write(u.s_[i], stream));
    }
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const unsigned int& x, BStream* stream)
//--------------------------------------------------------------------
{
  union un { unsigned int x_; char s_[4]; } u;
  u.x_=x;
  char i;
  if(control_.machine_.isLittleEndian_)
  {
    for(i=3; i>=0; i--)
    {
      if(u.s_[i]) { break; }
    }
    Ensure(Write(i, stream));
    for(; i>=0; i--)
    {
      Ensure(Write(u.s_[i], stream));
    }
  }
  else
  {
    for(i=0; i<4; i++)
    {
      if(u.s_[i]) { break; }
    }
    Ensure(Write(i, stream));
    for(i; i<4; i++)
    {
      Ensure(Write(u.s_[i], stream));
    }
  }    return(true);
};

//--------------------------------------------------------------------
  bool BOisCreator::Write(const float&   x, BStream* stream) 
//--------------------------------------------------------------------
{ 
//VBR: Hay que hacer lo mismo que con los otros tipos numéricos para 
//     hacerlo independiente del endianness
  return(Write((const void*)&x,4,1,stream)); 
}

//--------------------------------------------------------------------
  bool BOisCreator::Write(const double&  x, BStream* stream) 
//--------------------------------------------------------------------
{ 
//VBR: Hay que hacer lo mismo que con los otros tipos numéricos para 
//     hacerlo independiente del endianness
  return(Write((const void*)&x,8,1,stream)); 
}


//--------------------------------------------------------------------
// OIS compression decoding functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  bool BOisLoader::ReadBlock(void* v, size_t size, size_t count, BStream* stream)
//--------------------------------------------------------------------
{
  unsigned int sourceLen, destLen=count*size;
  size_t r, c;
  if(!enable_BSE_ || (options_.compressor_.serialization_.engine_!=BSE_BZIP2_) ||
     (destLen<unsigned(options_.compressor_.serialization_.minSizeCmprs_)))
  {
    r = stream->Read(v,size,c=count);
  }
  else //BZip2 Decompress method
  {
    ERead(sourceLen, stream);
    if(sourceLen==0)
    {
      r = stream->Read(v,size,c=count);
    }
    else
    {
      char* source = new char[sourceLen];
      if(!source)
      {
        return(Error(BText("Wrong string format or not enougth memory in ")+stream->Name()));
      }
      r = stream->Read(source,1,c=sourceLen);
      int dcmprs = BZ2_bzBuffToBuffDecompress 
      ( 
        (char*)v, 
        &destLen,
        source,
        sourceLen,
        0, 
        0 
      );
    }
  }
  if(r!=c) 
  { 
    return(Error(I2("Cannot read from file ",
                    "No se pudo leer del fichero ")+stream->Name()+" "+
                 (int)count+ I2(" items of " ," elementos de ")+
                 (int)size+ "bytes"));
  }
#ifdef TRACE_OIS_FILE
  if(&stream==&tolref_)  { return(true); }
  if(&stream==&oisref_)  { return(true); }
  if(&stream==&code_)    { return(true); }
  fprintf(logRead_, "\n%s\tr\t%ld\t%ld", stream->Name().String(), size, count);
  fflush(logRead_);
#endif
  return(true);
}

//--------------------------------------------------------------------
  bool BOisLoader::Read(void* v_, size_t size, size_t count, BStream* stream)
//--------------------------------------------------------------------
{
  bool ok = true;
  if(control_.oisEngine_.oisVersion_<"02.12")
  {
    ok = ReadBlock(v_,size,count,stream);
  }
  else
  {
    int maxCount = 1 + _MaxBlockLength_ / size;
    int c = count;
    char* v = (char*)v_;
    while(ok&&(c>maxCount))
    {
      ok=ReadBlock(v,size,maxCount,stream);
      v += size*maxCount;
      c -= maxCount;
    }
    if(ok&&c)  
    {
      ok=ReadBlock(v,size,c,stream);
    }
  }
  return(ok);
};

//--------------------------------------------------------------------
  bool BOisLoader::Read(BINT64& x, BStream* stream)
//--------------------------------------------------------------------
{
  if(control_.oisEngine_.oisVersion_<"01.04")
  {
    Read((void*)&x,8,1,stream);
  }
  else
  {
    union un { BINT64 x_; char s_[8]; } u;
    char i,j;
    u.x_=0;
    Ensure(Read(i, stream));
    if((i<-1)||(i>=8)) 
    {
      return(Error(BText("Wrong int64 format in ")+stream->Name()));
    }
    if(control_.machine_.isLittleEndian_)
    {
      for(; i>=0; i--)
      {
        j = isLittleEndian_?i:7-i;
        Ensure(Read(u.s_[j], stream));
      }
    }
    else
    {
      if((i<0)||(i>8)) 
      {
        return(Error(BText("Wrong int64 format in ")+stream->Name()));
      }      
      for(i; i<8; i++)
      {
        j = isLittleEndian_?i:7-i;
        Ensure(Read(u.s_[j], stream));
      }
    }
    x=u.x_;
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisLoader::Read(unsigned BINT64& x, BStream* stream)
//--------------------------------------------------------------------
{
  if(control_.oisEngine_.oisVersion_<"01.04")
  {
    Read((void*)&x,8,1,stream);
  }
  else
  {
    union un { unsigned BINT64 x_; char s_[8]; } u;
    char i,j;
    u.x_=0;
    Ensure(Read(i, stream));
    if(control_.machine_.isLittleEndian_)
    {
      if((i<-1)||(i>=8)) 
      {
        return(Error(BText("Wrong unsigned int64 format in ")+stream->Name()));
      }
      for(; i>=0; i--)
      {
        j = isLittleEndian_?i:7-i;
        Ensure(Read(u.s_[j], stream));
      }
    }
    else
    {
      if((i<0)||(i>8)) 
      {
        return(Error(BText("Wrong unsigned int64 format in ")+stream->Name()));
      }
      for(; i<8; i++)
      {
        j = isLittleEndian_?i:7-i;
        Ensure(Read(u.s_[j], stream));
      }
    }
    x=u.x_;
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisLoader::Read(int& x, BStream* stream)
//--------------------------------------------------------------------
{
  if(control_.oisEngine_.oisVersion_<"01.04")
  {
    Read((void*)&x,4,1,stream);
  }
  else
  {
    union un { int x_; char s_[4]; } u;
    char i, j;
    u.x_=0;
    Ensure(Read(i, stream));
    if(control_.machine_.isLittleEndian_)
    {
      if((i<-1)||(i>=4)) 
      {
        return(Error(BText("Wrong int format in ")+stream->Name()));
      }
      for(; i>=0; i--)
      {
        j = isLittleEndian_?i:3-i;
        Ensure(Read(u.s_[j], stream));
      }
    }
    else
    {
      if((i<0)||(i>4)) 
      {
        return(Error(BText("Wrong int format in ")+stream->Name()));
      }
      for(; i<4; i++)
      {
        j = isLittleEndian_?i:3-i;
        Ensure(Read(u.s_[j], stream));
      }
    }
    x=u.x_;
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisLoader::Read(unsigned int& x, BStream* stream)
//--------------------------------------------------------------------
{
  if(control_.oisEngine_.oisVersion_<"01.04")
  {
    Read((void*)&x,4,1,stream);
  }
  else
  {
    union un { unsigned int x_; char s_[4]; } u;
    char i, j;
    u.x_=0;
    Ensure(Read(i, stream));
    if(control_.machine_.isLittleEndian_)
    {
      if((i<-1)||(i>=4)) 
      {
        return(Error(BText("Wrong unsigned int format in ")+stream->Name()));
      }
      for(; i>=0; i--)
      {
        j = isLittleEndian_?i:3-i;
        Ensure(Read(u.s_[j], stream));
      }
    }
    else
    {
      if((i<0)||(i>4)) 
      {
        return(Error(BText("Wrong unsigned int format in ")+stream->Name()));
      }
      for(i; i<4; i++)
      {
        j = isLittleEndian_?i:3-i;
        Ensure(Read(u.s_[j], stream));
      }
    }
    x=u.x_;
  }
  return(true);
};

//--------------------------------------------------------------------
  bool BOisLoader::Read(float&   x, BStream* stream) 
//--------------------------------------------------------------------
{
//VBR: Hay que hacer lo mismo que con los otros tipos numéricos para 
//     hacerlo independiente del endianness
  return(Read((void*)&x,4,1,stream)); 
}

//--------------------------------------------------------------------
  bool BOisLoader::Read(double&  x, BStream* stream) 
//--------------------------------------------------------------------
{ 
//VBR: Hay que hacer lo mismo que con los otros tipos numéricos para 
//     hacerlo independiente del endianness
  return(Read((void*)&x,8,1,stream)); 
}


