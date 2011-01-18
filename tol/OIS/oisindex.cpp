/* oisindex.cpp: main and init functions of GNU/TOL language.

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
#include <tol/tol_btxtgra.h>


BTraceInit("oisindex.cpp");


//--------------------------------------------------------------------
// OIS writing index hierarchy functions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  bool BOisCreator::WriteIndexElement(const BOis::BIndexElement& x, 
                                      BArray<char>& streamBuf,
                                      unsigned int& pos)
//--------------------------------------------------------------------
{
  bool isSet = x.gid_==BGI_Set;
  int needed = 
    pos+ 
    sizeof(BGrammarId)+
    sizeof(x.mode_)+
    sizeof(BINT64)+
    ((isSet)?sizeof(int):0);
  if(streamBuf.MaxSize()<=needed+10)
  {
    streamBuf.ReallocBuffer(needed*2);
  }
  memcpy(streamBuf.GetBuffer()+pos,&x.gid_,          sizeof(BGrammarId));
  pos+=sizeof(BGrammarId);
  memcpy(streamBuf.GetBuffer()+pos,&x.mode_,         sizeof(x.mode_));
  pos+=sizeof(x.mode_);
  memcpy(streamBuf.GetBuffer()+pos,&x.objectOffset_, sizeof(BINT64));
  pos+=sizeof(BINT64);
  if(isSet) {
  memcpy(streamBuf.GetBuffer()+pos,&x.hrchyEntry_,   sizeof(int));
  pos+=sizeof(int); }
  return(true);
}


//--------------------------------------------------------------------
  bool BOisCreator::WriteGlossaryElement(const BOis::BGlossaryElement& x, 
                                         BArray<char>& streamBuf,
                                         unsigned int& pos)
//--------------------------------------------------------------------
{
  unsigned short len = x.name_.Length();
  int needed = 
    pos+ 
    sizeof(unsigned short)+
    len +
    sizeof(int);
  if(streamBuf.MaxSize()<=needed+10)
  {
    streamBuf.ReallocBuffer(needed*2);
  }
  memcpy(streamBuf.GetBuffer()+pos,&len,sizeof(unsigned short));
  pos+=sizeof(unsigned short);
  memcpy(streamBuf.GetBuffer()+pos,x.name_.String(),len);
  pos+=len;
  memcpy(streamBuf.GetBuffer()+pos,&x.position_,sizeof(int));
  pos+=sizeof(int);
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::WriteIndexSet(const BArray<BOis::BIndexElement>& x, 
                                  BArray<char>& streamBuf,
                                  unsigned int& pos)
//--------------------------------------------------------------------
{
  int i;
  int card = x.Size();
  int needed = 
    pos+ 
    sizeof(int)+
    card;
  if(streamBuf.MaxSize()<=needed+10)
  {
    streamBuf.ReallocBuffer(needed*2);
  }
  memcpy(streamBuf.GetBuffer()+pos,&card, sizeof(int));
  pos+=sizeof(int);
  for(i=0; i<card; i++)
  {
    Ensure(WriteIndexElement(x[i],streamBuf,pos));
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::WriteGlossarySet(const BArray<BOis::BGlossaryElement>& x, 
                                     BArray<char>& streamBuf,
                                     unsigned int& pos)
//--------------------------------------------------------------------
{
  int i;
  int card = x.Size();
  int needed = 
    pos+ 
    sizeof(int)+
    card;
  if(streamBuf.MaxSize()<=needed+10)
  {
    streamBuf.ReallocBuffer(needed*2);
  }
  memcpy(streamBuf.GetBuffer()+pos,&card, sizeof(int));
  pos+=sizeof(int);
  for(i=0; i<card; i++)
  {
    Ensure(WriteGlossaryElement(x[i],streamBuf,pos));
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::WriteHierarchy(const BArray<BOis::BIndexElement   >& idx, 
                                   const BArray<BOis::BGlossaryElement>& glo, 
                                   BStream* stream)
//--------------------------------------------------------------------
{
  BArray<char> streamBuf(1024*32);
  unsigned int pos = 0;
  WriteIndexSet   (idx,streamBuf,pos);
  WriteGlossarySet(glo,streamBuf,pos);
  EWrite(pos,stream);
  Write(streamBuf.Buffer(),1,pos,stream);
  return(true);
}

//--------------------------------------------------------------------
  bool BOisCreator::SortHierarchy()
//--------------------------------------------------------------------
{
  if(!options_.oisConfig_.buildHierarchy_) { return(true); }
  int n;
  hrchyOrder_->PutEntries(hierarchyIndex_.Size());
  hierarchyIndex_.Sort(CompareHrchyEntry);
  hierarchyOrder_.AllocBuffer(hrchyOrder_->Entries());
  for(n=0; n<hierarchyIndex_.Size(); n++)
  {
    hierarchyOrder_[n] = hierarchyIndex_[n].hrchyOrder_;
#   ifdef TRACE_OIS_HIERARCHY
    fprintf(logHrchyWrite_,"\nhrchyOrder[%ld,%ld,%"_LLD64_"]",
      hierarchyIndex_[n].hrchyOrder_, 
      hierarchyIndex_[n].hrchyEntry_, 
      hierarchyIndex_[n].hrchyOffset_);
    fflush(logHrchyWrite_);
#   endif
  }
  Ensure(Write(hierarchyOrder_.Buffer(),sizeof(int),hrchyOrder_->Entries(),hrchyOrder_));
  return(true);
}


//--------------------------------------------------------------------
// OIS reading index hierarchy functions 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  bool BOisLoader::ReadIndexElement(BOis::BIndexElement& x, 
                                    BArray<char>& streamBuf,
                                    unsigned int& pos)
//--------------------------------------------------------------------
{
  memcpy(&x.gid_,streamBuf.Buffer()+pos,sizeof(BGrammarId));
  pos+=sizeof(BGrammarId);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&x.gid_,sizeof(BGrammarId));
  }
  memcpy(&x.mode_,streamBuf.Buffer()+pos,sizeof(x.mode_));
  pos+=sizeof(x.mode_);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&x.mode_,sizeof(x.mode_));
  }

  memcpy(&x.objectOffset_,streamBuf.Buffer()+pos,sizeof(BINT64));
  pos+=sizeof(BINT64);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&x.objectOffset_,sizeof(BINT64));
  }

  if(x.gid_==BGI_Set)
  {
    memcpy(&x.hrchyEntry_,streamBuf.Buffer()+pos,sizeof(int));
    pos+=sizeof(int);
    if(control_.machine_.isLittleEndian_!=isLittleEndian_)
    {
      SwapEndian(&x.hrchyEntry_,sizeof(int));
    }
  }
  else
  {
    x.hrchyEntry_ = -1;
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisLoader::ReadGlossaryElement(BOis::BGlossaryElement& x, 
                                       BArray<char>& streamBuf,
                                       unsigned int& pos)
//--------------------------------------------------------------------
{
  unsigned short len;
  memcpy(&len,streamBuf.Buffer()+pos,sizeof(unsigned short));
  pos+=sizeof(unsigned short);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&len,sizeof(unsigned short));
  }

  BText name(len+1);
  memcpy(name.Buffer(),streamBuf.Buffer()+pos,len);
  x.name_ = name;
  pos+=len;

  memcpy(&x.position_,streamBuf.Buffer()+pos,sizeof(int));
  pos+=sizeof(int);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&x.position_,sizeof(int));
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisLoader::ReadIndexSet(BArray<BOis::BIndexElement>& x, 
                                BArray<char>& streamBuf,
                                unsigned int& pos)
//--------------------------------------------------------------------
{
  int i, card;
  memcpy(&card,streamBuf.Buffer()+pos,sizeof(int));
  pos+=sizeof(int);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&card,sizeof(int));
  }

  x.ReallocBuffer(card);
  for(i=0; i<card; i++)
  {
    Ensure(ReadIndexElement(x[i],streamBuf,pos));
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisLoader::ReadGlossarySet(BArray<BOis::BGlossaryElement>& x, 
                                   BArray<char>& streamBuf,
                                   unsigned int& pos)
//--------------------------------------------------------------------
{
  int i, card;
  memcpy(&card,streamBuf.Buffer()+pos,sizeof(int));
  pos+=sizeof(int);
  if(control_.machine_.isLittleEndian_!=isLittleEndian_)
  {
    SwapEndian(&card,sizeof(int));
  }

  x.ReallocBuffer(card);
  for(i=0; i<card; i++)
  {
    Ensure(ReadGlossaryElement(x[i],streamBuf,pos));
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BOisLoader::ReadHierarchy(BArray<BOis::BIndexElement   >& idx, 
                                 BArray<BOis::BGlossaryElement>& glo, 
                                 BStream* stream)
//--------------------------------------------------------------------
{
  if(control_.oisEngine_.oisVersion_<"01.07")
  {
    return(false);
  }
  else
  {
    unsigned int pos;
    ERead(pos,stream);
    BArray<char> streamBuf(pos);
    Ensure(Read(streamBuf.GetBuffer(),1,pos,stream));
    Ensure(ReadIndexSet   (idx,streamBuf,pos));
    Ensure(ReadGlossarySet(glo,streamBuf,pos));
    return(true);
  }
}

//--------------------------------------------------------------------
  bool BOisLoader::ReadHierarchyIndex()
//--------------------------------------------------------------------
{
  int n, m, sz = stat_.fileStat_[BFI_hrchyOrder].entries_;
  BArray<int> invOrder(sz);
  hierarchyOrder_.AllocBuffer(sz);
  hierarchyIndex_.AllocBuffer(sz);
  Ensure(Read(invOrder.GetBuffer(),sizeof(int),sz,hrchyOrder_));
  for(m=0; m<sz; m++)
  {
    n = invOrder[m];
    hierarchyOrder_[n] = m;
  }
  for(n=0; n<sz; n++)
  {
    m = hierarchyOrder_[n];
          hierarchyIndex_[m].hrchyOrder_ = n;
    ERead(hierarchyIndex_[m].hrchyEntry_,  hrchyOffset_);
    ERead(hierarchyIndex_[m].hrchyOffset_, hrchyOffset_);
# ifdef TRACE_OIS_HIERARCHY
    fprintf(logHrchyRead_,"\nhrchyEntry[%ld,%ld,%"_LLD64_"]",
      n, 
      hierarchyIndex_[m].hrchyEntry_, 
      hierarchyIndex_[m].hrchyOffset_);
    fflush(logHrchyRead_);
#   endif
  }
# ifdef TRACE_OIS_HIERARCHY
  for(m=0; m<sz; m++)
  {
    fprintf(logHrchyRead_,"\nhrchyOrder[%ld,%ld,%"_LLD64_"]",
      hierarchyIndex_[m].hrchyOrder_, 
      hierarchyIndex_[m].hrchyEntry_, 
      hierarchyIndex_[m].hrchyOffset_);
    fflush(logHrchyRead_);
  }
#   endif

  return(true);
}



//--------------------------------------------------------------------
  bool BOisLoader::SearchOffsetInHierarchy(      int     hrchyPos, 
                                                 BINT64 objectOffset,  
                                           const BSet*   partial_,
                                                 int     depth)
//--------------------------------------------------------------------
{
  int found;
  BText prefix__= curHrchyPartialPath_.HasName()?curHrchyPartialPath_+",":curHrchyPartialPath_;
  BArray<BOis::BIndexElement   > idx;
  BArray<BOis::BGlossaryElement> glo;  
  hrchyDetail_->SetPos(hierarchyIndex_[hrchyPos].hrchyOffset_);
  ReadHierarchy(idx,glo,hrchyDetail_);
  const BSet& partial = *partial_;
  if(partial[depth]->Grammar()==GraReal())
  {
    int pos = (int)Real(partial[depth])-1;
    found = (pos>idx.Size())?-1:pos;
    if(found<0)
    {
      Error(I2("Hierarchy path ","El camino de la jerarquía ")+prefix__+(pos+1)+
            I2("doesn't exist"," no existe"));
      return(false);
    }
    else { prefix__ += (found+1); }
  }
  else if(partial[depth]->Grammar()==GraText())
  {
    static BGlossaryElement ge_;
    ge_.name_ = Text(partial[depth]);
    BText tok = BText("\"")+ge_.name_+"\"";
    found = glo.FindSorted(ge_, CompareGlossaryElement);
    if(found<0)
    {
      Error(I2("Hierarchy path ","El camino de la jerarquía ")+prefix__+tok+
            I2("doesn't exist"," no existe"));
      return(false);
    }
    else { prefix__ += tok; }
  }
  else
  {
    Error(I2("Hierarchy path must have just Real or Text elements",
             "El camino de la jerarquía sólo puede contener objetos de los tipos Real y Text"));
    return(false); 
  }
  int idxPos = glo[found].position_;
  BINT64 of = idx[idxPos].objectOffset_;
  int hp = idx[idxPos].hrchyEntry_;
  if(depth==partial.Card()) 
  {
    object_->SetPos(of);
    hrchyDetail_->SetPos(hierarchyIndex_[hp].hrchyOffset_);
    return(true); 
  }
  else if(idx[idxPos].gid_!=BGI_Set)
  {
    Error(I2("Partial hierarchy path ","El camino parcial de la jerarquía ")+prefix__+
          I2("isn't a set."," no corresponde con un conjunto."));
    return(false);
  }
  curHrchyPartialPath_ = prefix__;
  return(SearchOffsetInHierarchy(hp, of, partial_, depth+1));
}

//--------------------------------------------------------------------
  bool BOisLoader::SearchOffsetInHierarchy(const BSet* partial)
//--------------------------------------------------------------------
{
  if(!partial) { return(true); }
  const BSet& p = *partial;
  BInt c = p.Card();
  if(!c) { return(true); }
  curHrchyPartialPath_="";
  bool ok = true;
  for(int n=1; n<=c; n++)
  {
    if(p[n]->Grammar()==GraReal())
    {
      curHrchyPartialPath_<<((int)Real(p[n]));
    }
    else if(p[n]->Grammar()==GraText())
    {
      curHrchyPartialPath_<<Text(p[n]);
    }
    else
    {
      Error(I2("Hierarchy path must have just Real or Text elements",
               "El camino de la jerarquía sólo puede contener objetos de los tipos Real y Text"));
    }
    if(n<c) { curHrchyPartialPath_ << ","; }
  }
  return(SearchOffsetInHierarchy(0,object_->Offset(),partial,1));
}

//--------------------------------------------------------------------
  void BOisLoader::ShowHierarchy(const BText& margin, 
                                       int    hrchyPos, 
                                       int    depth, 
                                       int    maxDepth, 
                                       int    maxChilds,
                                       FILE*  tmp)
//--------------------------------------------------------------------
{
  if((maxDepth>0) && (depth>=maxDepth)) { return; }
  int k;
  BArray<BOis::BIndexElement   > idx;
  BArray<BOis::BGlossaryElement> glo;  
  BArray<BOis::BHierarchyDetail> dtl;  
  if(hrchyPos>=0)
  {
    hrchyDetail_->SetPos(hierarchyIndex_[hrchyPos].hrchyOffset_);
  }
  ReadHierarchy(idx,glo,hrchyDetail_);
  dtl.AllocBuffer(idx.Size());
  for(k=0; k<idx.Size(); k++)
  {
    dtl[k].gid_          = idx[k].gid_;
    dtl[k].mode_         = idx[k].mode_;
    dtl[k].objectOffset_ = idx[k].objectOffset_;
    dtl[k].hrchyEntry_   = idx[k].hrchyEntry_;
    dtl[k].name_         = "";
  }
  for(k=0; k<glo.Size(); k++)
  {
    dtl[glo[k].position_].name_ = glo[k].name_;
  }
  fprintf(tmp,"%s",(margin+"[[\n").String());
  BText prefix__ = curHrchyPartialPath_.HasName()?curHrchyPartialPath_+",":curHrchyPartialPath_;
  BText margin__ = margin+"  ";
  int childs = (maxChilds<0)?dtl.Size():Minimum(dtl.Size(),maxChilds);
  for(k=0; k<childs; k++)
  {
    curHrchyPartialPath_ = prefix__+(k+1);
    BGrammar* gra = BGrammar::FindByGid(dtl[k].gid_,true);
    fprintf(tmp,"%s",(margin__+"{"+prefix__+(k+1)+"}"+gra->Name()+" "+ dtl[k].name_).String());
    if(dtl[k].gid_==BGI_Set)
    {
      if((maxDepth<0) || (depth<maxDepth-1)) 
      {
        fprintf(tmp,"%s",(margin+" =\n").String());
        ShowHierarchy(margin__,dtl[k].hrchyEntry_,depth+1,maxDepth,maxChilds, tmp);
      }
    }
    if(k!=dtl.Size()-1) { fprintf(tmp,","); }
    fprintf(tmp,"\n");
  }
  if(childs<dtl.Size())
  {
    fprintf(tmp,"%s",(margin__+"...,\n").String());
    k=dtl.Size();
    curHrchyPartialPath_ = prefix__+(k+1);
    BGrammar* gra = BGrammar::FindByGid(dtl[k].gid_,true);
    fprintf(tmp,"%s",(margin__+"{"+prefix__+(k+1)+"}"+gra->Name()+" "+ dtl[k].name_).String());
    if(dtl[k].gid_==BGI_Set)
    {
      if((maxDepth<0) || (depth<maxDepth-1)) 
      {
        fprintf(tmp,"%s",(margin+" =\n").String());
        ShowHierarchy(margin__,dtl[k].hrchyEntry_,depth+1,maxDepth,maxChilds, tmp);
      }
    }
    fprintf(tmp,"\n");
  }
  fprintf(tmp,"%s",(margin+"]]").String());
}

//--------------------------------------------------------------------
  void BOisLoader::ShowHierarchy()
//--------------------------------------------------------------------
{
  BText tmpNam = BSys::TempNam("","OIS.Hrchy.",".txt",20);
  FILE* tmp = fopen(tmpNam.String(),"w");

  fprintf(tmp,"%s",(BText("\nOIS: Hierarchy of ")+Path()+" {"+curHrchyPartialPath_+"}:\n").String());
  curHrchyPartialPath_ = "";
  ShowHierarchy("",0,0,-1,-1,tmp);
  fclose(tmp);
  BSys::System(BText("\"")+tmpNam+"\"");
  BSys::Unlink(tmpNam);
}

/*
//--------------------------------------------------------------------
  void BOisLoader::ShowPartialHierarchy(const BText& branch)
//--------------------------------------------------------------------
{
  BText tmpNam = BSys::TempNam("","OIS.Hrchy.",".txt",20);
  FILE* tmp = fopen(tmpNam.String(),"w");

  fprintf(tmp,"%s",(BText("\nOIS: Partial hierarchy of ")+
          Path()+"{"+branch+"}"+":\n").String());
  ShowHierarchy("","",-1,0,-1,-1,tmp);
  fclose(tmp);
  BSys::System(BText("\"")+tmpNam+"\"");
  BSys::Unlink(tmpNam);
}
*/

