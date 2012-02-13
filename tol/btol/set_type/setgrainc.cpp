/* setgra.cpp: Set grammars functions of GNU/TOL language.
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bsys.h>
#include <tol/tol_boper.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_ois.h>
#include <tol/tol_bstruct.h>

#if defined(__USE_TC__)
#  include <tol/tol_bctmsgra.h>
#endif


#ifndef OPENFLAG
#  if (defined(UNIX) || (defined(_MSC_VER) && (_MSC_VER>=1300)))
#    define OPENFLAG ios::in
#  else
#    define OPENFLAG ios::nocreate
#  endif
#endif

#define BST  BText("(BST) ") + TolPath()
#define BDC  BText("(BDC) ") + TolPath()

#define MaxLineLength 1048576

BTraceInit("setgrainc.cpp");

BArray<const BSourcePath*> BSourcePath::statusTable_(0);
const BSourcePath* BSourcePath::current_      = NIL;

BList*	    BSetFromFile::compiled_     = NIL;
BInt	      BSetFromFile::nSetFromFile_ = 0;
BTerminator BSetFromFile::terminator_   = NIL;

BDat BSetIncludeBDT::fillValue_    = 0;


//--------------------------------------------------------------------
  BText& SearchPaths() 
//--------------------------------------------------------------------
{ 
  static BText searchPaths_ = "";
  return(searchPaths_);  
}

//--------------------------------------------------------------------
  BText SearchAbsolutePath(const BText& path)
    
/*! Returns the path of file.
 */
//--------------------------------------------------------------------
{
  BText p = path;
  p.Replace('\\', '/' );
  if(!IsAbsolutePath(p))
  {
	  BText searchPaths = BDir::GetCurrent() +";"+SearchPaths();
	  BArray<BText> cur;
	  ReadAllTokens(searchPaths,cur,';');
	  BBool ok = BFALSE;
	  for(BInt i=0; !ok && (i<cur.Size()); i++)
	  {
	    cur(i).Replace('\\', '/' );
	    if(cur(i).Length() && (cur(i).Last()!='/')) { cur(i) += '/'; }
	    p = cur(i)+path;
//    Std(BText("\nBuscando el fichero ")+p);
	    if(CheckIsFile(p) || BDir::CheckIsDir(p)) { ok = BTRUE; }
	  }
  }
  return(p);
}

#ifndef NDEBUG
  static bool BSourcePath_Current_active = false;
#endif

//--------------------------------------------------------------------
  static BInt StatusTableCmp(const void* v1, const void* v2)

/*! Compair two atom receiving the pointers.
 * \param v1 first atom to compare
 * \param v2 second atom to compare
 * \return returns an integer less than,
 */
//--------------------------------------------------------------------
{
  BSourcePath* sps1 = *(BSourcePath**)v1;
  BSourcePath* sps2 = *(BSourcePath**)v2;
  size_t s1 = (size_t)sps1;
  size_t s2 = (size_t)sps2;
  return((s1<s2)?-1:(s1>s2)?1:0);
}


//--------------------------------------------------------------------
   BSourcePath::BSourcePath(const BSetFromFile* set, const BText& path)
//--------------------------------------------------------------------
: BSyntaxObject(path), set_(set), embeded_()
{
  TRACE_MEMORY_SHOW(this,"BSourcePath::BSourcePath");
  BGrammar::SymbolTable().Add(this); 
  if(statusTable_.MaxSize()==0)
  {
    statusTable_.ReallocBuffer(1024);
    statusTable_.ReallocBuffer(0);
  }
  int size = statusTable_.Size(); 
  if(statusTable_.MaxSize()==size)
  {
    statusTable_.ReallocBuffer(size*2);
    statusTable_.ReallocBuffer(size);
  }
  int pos = statusTable_.FindSorted(this, StatusTableCmp);
  assert(pos<0);
  statusTable_.AddSorted(this,StatusTableCmp);
}

//--------------------------------------------------------------------
  void BSourcePath::AddEmbed(const BText& txt)
//--------------------------------------------------------------------
{
  if(current_)
  {
    ((BSourcePath*)current_)->embeded_.Add(txt);
  }
  else
  {
  //Error("Cannot use #Embed out of a TOL file.");
  }
}

//--------------------------------------------------------------------
	BInt BSourcePath::Mode() const 
//--------------------------------------------------------------------
{ 
  return(BSOURCEPATHMODE); 
//return(BSTRUCTMODE); 
}

//--------------------------------------------------------------------
  const BSourcePath* BSourcePath::Current()
//--------------------------------------------------------------------
{
#ifndef NDEBUG
  assert(!BSourcePath_Current_active || (current_ && current_->Set()));
#endif
  return(current_);
}

  //--------------------------------------------------------------------
  const BSourcePath* BSourcePath::Find(const BText& path)	
//--------------------------------------------------------------------
{ 
  BObjClassify oc(GraAnything(),BSOURCEPATHMODE);
  BSourcePath* sp;
  sp=(BSourcePath*)BGrammar::SymbolTable().Search(oc,path);
  return(sp);
}

//--------------------------------------------------------------------
  void BSourcePath::SetDestroyed() 
//--------------------------------------------------------------------
{
  TRACE_MEMORY_SHOW(this,"BSourcePath::SetDestroyed");
  set_=NULL; 
//BGrammar::DelObject(this); 
}


//--------------------------------------------------------------------
  void BSourcePath::SetCurrent(const BSourcePath*  cp) 
//--------------------------------------------------------------------
{ 
  current_ = cp; 
#ifndef NDEBUG
  if(!current_)
  {
    BSourcePath_Current_active = false;
  }
  else
  {
    assert(current_->Set());
  }
#endif
}

//--------------------------------------------------------------------
  void BSourcePath::SetCurrent(const BSetFromFile* set)
//--------------------------------------------------------------------
{
  if(!set) 
  { 
    current_=NULL;
#ifndef NDEBUG
    BSourcePath_Current_active = false;
#endif
  }
  else
  {
    const BSourcePath* find = BSourcePath::Find(set->TolPath());
    if(!find)
    {
      current_ = new BSourcePath(set, set->TolPath());
    }
    else
    {
      current_ = find;
      ((BSourcePath*)current_)->PutSet(set);
    }
#ifndef NDEBUG
    assert(current_ && current_->Set());
    BSourcePath_Current_active = true;
#endif
  }
}

//--------------------------------------------------------------------
  bool BSourcePath::IsAlive(const BSourcePath* sp)
//--------------------------------------------------------------------
{
  if(!sp) { return(false); }
  int pos = statusTable_.FindSorted(sp, StatusTableCmp);
  assert(pos>=0);
  return(statusTable_[pos]->set_!=NULL);
}


//--------------------------------------------------------------------
BSetFromFile::BSetFromFile(BList* arg)

/*! BSetFromFile Creator
 */
//--------------------------------------------------------------------
: BSetTemporary (arg), 
  file_         (), 
  busy_         (false), 
  curSourcePath_(NULL), 
  oldSourcePath_(NULL),
  makeGlobal_(NULL)
{
  TRACE_MEMORY_SHOW(this,"BSetFromFile::BSetFromFile");
  nSetFromFile_++;
  tolPath_ = Replace(SearchAbsolutePath(Text(Arg(1))),"\\","/");
#ifndef FOR_BTOLLNK
  PutName(tolPath_);
  PutDescription(tolPath_);
#endif
  if(BGrammar::Level()==0)
  {
    compiled_ = Cons(this, compiled_);
    DecNRefs();
  }
}


//--------------------------------------------------------------------
BSetFromFile::BSetFromFile(const BText& path)

/*! BSetFromFile Creator
 */
//--------------------------------------------------------------------
: BSetTemporary (NULL), 
  file_         (), 
  busy_         (false), 
  curSourcePath_(NULL), 
  oldSourcePath_(NULL),
  makeGlobal_(NULL)
{
  TRACE_MEMORY_SHOW(this,"BSetFromFile::BSetFromFile");
  nSetFromFile_++;
  tolPath_ = path;
#ifndef FOR_BTOLLNK
  PutName(tolPath_);
  PutDescription(tolPath_);
#endif
  if(BGrammar::Level()==0)
  {
    compiled_ = Cons(this, compiled_);
    DecNRefs();
  }
  oldSourcePath_ = BSourcePath::Current();
  BSourcePath::SetCurrent(this);
  curSourcePath_ = BSourcePath::Current();
}

//--------------------------------------------------------------------
BSetFromFile::~BSetFromFile()

/*! BSetFromFile Destructor
 */
//--------------------------------------------------------------------
{
  TRACE_MEMORY_SHOW(this,"BSetFromFile::~BSetFromFile");
  BinWrite();
  nSetFromFile_--;
//Contens().Delete();
  IncNRefs();
  DESTROY(makeGlobal_);
  DecNRefs();
  if(Level()==0)
  {
    IncNRefs();
    compiled_=LstRemoveAtom(compiled_,this);
  }
  if(curSourcePath_)
  {
    ((BSourcePath*)curSourcePath_)->SetDestroyed();
  }

}

//--------------------------------------------------------------------
void BSetFromFile::AddMakeGlobal(BSyntaxObject* obj) const
//--------------------------------------------------------------------
{
  BSetFromFile* T=(BSetFromFile*)this;
  T->makeGlobal_ = Cons(obj, T->makeGlobal_);
}

//--------------------------------------------------------------------
BBool BSetFromFile::Open()
//--------------------------------------------------------------------
{
  BBool isOpen = false;
  if(CheckIsFile(TolPath()))
  {
#if((defined(__GNUC__) && (__GNUC__>2)) ||(defined(_MSC_VER)))
    file_.open(TolPath().String(), std::ios::in);
#else
    file_.open(TolPath().String(), std::ios::nocreate);
#endif
    isOpen=(BBool)(file_.good());
  }
  if(!isOpen)
  {
	  Error(I2("Cannot open file ",
		 "No se puede abrir el fichero ") + TolPath());
  }
  else
  {
    oldSourcePath_ = BSourcePath::Current();
    BSourcePath::SetCurrent(this);
    curSourcePath_ = BSourcePath::Current();
	  busy_ = BTRUE;
  }
  return(isOpen);
}


//--------------------------------------------------------------------
  void BSetFromFile::Close() 
//--------------------------------------------------------------------
{ 
  file_.close(); 
  busy_ = BFALSE; 
  BSourcePath::SetCurrent(oldSourcePath_);
}

//--------------------------------------------------------------------
   const BSetFromFile* BSetFromFile::FindCompiled (const BText& name)

/*! Find a compiled file which name is the argument
 */
//--------------------------------------------------------------------
{
  const BSetFromFile* found = NIL;
  BList*	lst   = compiled_;
  BText		path  = name;
    
  if(!IsAbsolutePath(path))
  {
  	BText cur = BDir::GetCurrent();
  //Trace(cur+" es el directorio en curso ");
	  if(cur.HasName()) { path = cur+"/"+path; }
  }
  path.Replace('\\', '/' );

  const BSourcePath* pp = BSourcePath::Find(path);
  if(pp) { found = pp->Set(); }
//Std(BText("\nFindCompiled(") + name +") : " +path);
  while(lst && !found)
  {
	  BSetFromFile* car = (BSetFromFile*)Car(lst);
  //Std(BText("\n  : ") + car->TolPath());
	  if(car->TolPath()==path) { found=car; }
    lst = Cdr(lst);
  }
//if(found) { Std(" OK"); }
//else	    { Std(" FAIL"); }
  return(found);
}

//--------------------------------------------------------------------
BText FindCompiled (const BText& fileName)
//--------------------------------------------------------------------
{
  BText path;
  const BSetFromFile* found = BSetFromFile::FindCompiled(fileName);
  if(found) { path = found->TolPath(); }
  return(path);
}

//--------------------------------------------------------------------
const BSetFromFile* BSetFromFile::FindCompiled (BInt n)
    
/*! BSetFromFile finder
 */
//--------------------------------------------------------------------
{
  return((const BSetFromFile*)LstNthCar(compiled_,n));
}


//--------------------------------------------------------------------
BSyntaxObject* BSetFromFile::FindObject(const BText& tolPath)
    
/*! Takes a TolPath text constructed as
 *
 *	    ../samples/s1.tol,1,3,5,2
 *
 *	    where the first token is the path of the TOL file from the object
 *	    searched was compiled, and each next token is the successive
 *	    index of its antecedents.
 *
 *	    Note that the path of a TOL file cannot have any comma.
 */
//--------------------------------------------------------------------
{
    BSyntaxObject* result = NIL;
    BArray<BText> level;
    ReadAllTokens(tolPath,level,',');
    //Std(BText("\nBSetFromFile::FindObject(")+tolPath+")");
    //Std(BText("\nlevel[0]")+level[0]);
    const BSetFromFile* file = FindCompiled(level[0]);
    if(file)
    {
	result = (BSyntaxObject*)file;
	for(BInt n=1; result && (n<level.Size()); n++)
	{
//            Std(BText("->")+result->Identify());
//            Std(BText("\nlevel[")+n+"]"+level[n]);
	    if(result->Grammar()==OwnGrammar())
	    {
		BInt k;
		if(sscanf(level[n].String(), "%d", &k))
		{
//                    Std(BText(" = ")+k);
		    BSet& set= Set(result);
		    if((set.Card()>=k)&&(k>=1)) { result = set[k]; }
		}
	    }
	    else
	    {
		result = NIL;
	    }
	}
    }
//    if(result) { Std(BText("->")+result->Identify()); }
    return(result);
}


//--------------------------------------------------------------------
void BSetFromFile::TableInfo(const BText& tolPath, const BText& fileName)

/*! Constructs a TOL information table of the set whose TOLPath is
 *  done.
 */
//--------------------------------------------------------------------
{
    BSyntaxObject* result = FindObject(tolPath);
    if(result && (result->Grammar()==OwnGrammar()))
    {
	ofstream out;
	if(fileName!="Std") { out.open(fileName.String()); }
	BText gra, name,  uName;
	WriteFileOrStd(fileName, out,
	    (
	I2("Type~ Name~	Description~ Contents~	References~ TOLPath~\n",
	   "Tipo~ Nombre~ Descripción~ Contenido~ Referencecias~ TOLPath~\n")
		));
	BSet set = Set(result);
	for(BInt n=1; n<=set.Card(); n++)
	{
	    BSyntaxObject* obj = set[n];
	    if(obj)
	    {
		BText graP  = obj->Grammar()->Name();
		BText nameP = obj->Name();
		BText desP  = obj->Description();
		BText dumpP = obj->Dump();
		BText refP  = BText("") + obj->NRefs();
		
		desP .Replace('\t',' ');
		desP .Replace('\n',' ');
		desP .Replace('\r',' ');
		dumpP.Replace('\t',' ');
		dumpP.Replace('\n',' ');
		dumpP.Replace('\r',' ');
		desP .Compact();
		dumpP.Compact();
		nameP.Compact();
		if(!nameP.HasName()) { nameP=I2("Unnamed","SinNombre"); }
		if(!dumpP.HasName()) { dumpP="..."; }
		if(!desP .HasName()) { desP ="..."; }
		
		WriteFileOrStd(fileName, out, (graP	     + "~ " +
					       nameP	     + "~ " +
					       desP	     + "~ " +
					       dumpP	     + "~ " +
					       refP	     + "~ " +
					       tolPath+","+n + "~\n"));
	    }
	}
	if(fileName!="Std") { out.close(); }
    }
}


//--------------------------------------------------------------------
BInt BSetFromFile::BinWrite()

/*! Rewrites the corresponding obj file
 */
//--------------------------------------------------------------------
{
/*
  if(! useObj_) { return(0); }
  BInt n, numSeries=0;
  BGrammar* serGrammar = GraSerie();
  for(n=1; n<=contens_.Card(); n++)
  {
    if(contens_[n]->Grammar()==serGrammar)
    {
      BUserTimeSerie* ser = (BUserTimeSerie*)contens_[n];
      if(ser->Name().HasName() &&  ser->IsStochastic()) { numSeries++; }
    }
  }
  if(!numSeries) { return(0); }
  FILE* file = fopen(objtolPath_.String(),"wb");
  Std(BText("\nWriting to OBJ-FILE : ")+objtolPath_);
  BInt items = fwrite(&numSeries, 1, sizeof(BInt), file);
  for(n=1; n<=contens_.Card(); n++)
  {
    if(contens_[n]->Grammar()==serGrammar)
    {
      BUserTimeSerie* ser = (BUserTimeSerie*)contens_[n];
      if(ser->Name().HasName() &&  ser->IsStochastic())
      {
	Std(BText("\nOld serie of : ")+ser->Name());
	items += ser->Name().BinWrite(file);
	items += ser->BinWrite(file);
      }
    }
  }
  fclose(file);
  return(items);
*/
    return(0);
}


//--------------------------------------------------------------------
BInt BSetFromFile::BinRead()
    
/*! Reloads the corresponding obj file
 */
//--------------------------------------------------------------------
{
/*
  if(! useObj_) { return(0); }
  BInt n, numSeries=0;
  BGrammar* serGrammar = GraSerie();
  FILE* file = fopen(objtolPath_.String(),"rb");
  if(!file) { return(0); }

  Std(BText("\nReading from OBJ-FILE : ")+objtolPath_);
  BInt items = fread(&numSeries, 1, sizeof(BInt), file);
  for(n=0; n<numSeries; n++)
  {
    BText serieName;
    items += serieName.BinRead(file);
    BUserTimeSerie* ser = Tsr(serGrammar->FindOperand(serieName));
    if(!ser || !ser->Dating())
    {
      Error(I2("Serious OBJ-file inconsistence",
	       "Inconsistencia grave en fichero-OBJ")+ ".\n"+
	    I2("Cannot find ", "No se encuentra " ) + serieName + " Serie " +
	    I2(" in file ", " en el fichero " ) + TolPath() + ", " +
	    I2(" or it not has dating  ", " o no tiene fechado " ) + ", " +
	    I2(" but it's already exists in  ", " pero aún existe en " ) +
	    objtolPath_+
	    I2(" and this OBJ-file should be deleted",
	       " y este fichero-OBJ debería ser borrado"));
      return(0);
    }
    Std(BText("\nOld serie of : ")+serieName);
    BTsrPrimary* oldSer;
    oldSer = new BTsrPrimary("",BText("Old ")+serieName,ser->Dating());
    items += oldSer->BinRead(file);
    ser->PutOld(oldSer);
  }
  fclose(file);
  return(items);
*/

    return(0);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetIncludeBMT);
DefExtOpr(1, BSetIncludeBMT, "IncludeBMT", 1, 4, "Text Text Text Text",
	  "(Text fileName [, Text cellDelim=\";\", Text rowDelim=\"\\n\", Text skip=\"\"])", 
	  I2("Returns a set with the real matrix from a Bayes Matrix "
	     "Time Table (BMT) file.\n"
	     "Include a matrix with a row on each line and the data "
	     "separated by semicolons. Obviously, all rows must have "
	     " the same number of elements.\n\n"

	     "Example:\n\n"

	     "1023.234;	   8745.874;\n"
	     "1014.347;	   8746.565;\n"
	     "1151.925;	   8745.675;\n"
	     "1127.942;	   8747.234;\n"
	     "1003.134;	   8748.734;"
	     ,
	     "Devuelve un conjunto cuyo único elemento es la matriz de "
	     "numeros guardada en un fichero con formato Bayes Matrix "
	     "Table (BMT).\n"
	     "Incluye una matriz con una fila en cada línea y los datos "
	     "separados por punto y coma. Obviamente, todas las filas "
	     "han de tener el mismo número de elementos.\n\n"

	     "Ejemplo:\n\n"
	     "1023.234;	   8745.874;\n"
	     "1014.347;	   8746.565;\n"
	     "1151.925;	   8745.675;\n"
	     "1127.942;	   8747.234;\n"
	     "1003.134;	   8748.734;\n"),
	  BOperClassify::FileIncluding_);

//--------------------------------------------------------------------
void BSetIncludeBMT::CalcContens()
//--------------------------------------------------------------------
{
  BText fileName=Text(Arg(1));
  size_t fileSize = GetFileSize(fileName);
  BText cellDelim=";";
  BText rowDelim="\n";
  BText skip="";
  if(Arg(2)) { cellDelim = Text(Arg(2)); }
  if(Arg(3)) { rowDelim  = Text(Arg(3)); }
  if(Arg(4)) { skip      = Text(Arg(4)); }
  ifstream fd(fileName.String(), OPENFLAG);
  if (fd.fail())
  {
    Error(I2(BText("Could not open file ")+fileName,
             BText("No se pudo abrir el fichero ")+fileName));
    return;
  }
  size_t k=0;  
  int i=0,j=0;
  int cols=0;
  unsigned int rows=0;
    
  char read;
  char *buffer = new char[MaxLineLength];
  BUserMat* uMat = NULL;
  if(Arg(2)) 
  { 
    int pos = 0;
    size_t size = 0;
    BArray<BDat> x(1024);
    while(!fd.eof())
    {
      k++;
      if(pos==MaxLineLength-1)
      {
        Error(I2("Cell maximum length ","Tamaño máximo de celda ")+MaxLineLength+
              I2(" exceeded  at column "," excedido en la columna ")+j+
              I2(" iof row "," de la fila ")+i+
              I2(" of file "," del fichero ")+fileName.String());
        rows = cols = 0; break; 
      };
      fd.get(read);
      bool isRowDelim  = strchr(rowDelim .String(), read)!=NULL;
      bool isCellDelim = strchr(cellDelim.String(), read)!=NULL;
      bool skipedLastCellDelim = isRowDelim&&pos;
      if(isCellDelim||skipedLastCellDelim)
      {
        buffer[pos++] = 0;
        if(size>x.MaxSize()-1) 
        { 
          int expected_size = Minimum(100*size, Maximum(2*size, ((fileSize*(size+1))/k)));
          x.ReallocBuffer(expected_size); 
        }
        x.ReallocBuffer(size+1); 
        x[size].PutValue(buffer);
        size++;
        pos = 0;
        buffer[0] = 0;
        j++;
        if(!skipedLastCellDelim) { continue; }
      } 
      if(isRowDelim)
      {
        if(i==0) { cols = j; }
        else if(j&&(cols!=j))
        {
          Error(I2("The row number", "La fila número")+" "+(i+1)+" "+
                I2("has","tiene")+" "+j+" columns "+
                I2("but it was expected","pero se esperaban")+" "+cols+" "+
                I2(" on file "," en el fichero ")+fileName.String());
          rows = cols = 0; break;
        }
        if(j) 
        {  
          j=0;
          i++;
        }
        continue;
      } 
      if(!strchr(skip.String(), read))
      {
        buffer[pos++] = read;
      }
    }
    if(pos)
    {
      buffer[pos++] = 0;
      if(size>x.MaxSize()-1) 
      { 
        int expected_size = Minimum(100*size, Maximum(2*size, ((fileSize*(size+1))/k)));
        x.ReallocBuffer(expected_size); 
      }
      x.ReallocBuffer(size+1); 
      x[size].PutValue(buffer);
      size++;
      pos = 0;
      buffer[0] = 0;
      j++;
      if(i==0) { cols = j; }
      else if(j&&(cols!=j))
      {
        Error(I2("The row number", "La fila número")+" "+(i+1)+" "+
              I2("has","tiene")+" "+j+" columns "+
              I2("but it was expected","pero se esperaban")+" "+cols+" "+
              I2(" on file "," en el fichero ")+fileName.String());
        rows = cols = 0; 
      }
      if(j) 
      {  
        j=0;
        i++;
      }
    } 
    if(!i && size) { cols = j; rows = 1; }
    else           { rows = i; }
    BMatrix<BDat> m(rows,cols,x.GetBuffer());
    uMat = new BContensMat("", m, GetFilePrefix(TolPath()));
  }
  else
  {
    //VBR: This code is maintained in order to ensure backward compatibility
    //but it should be eliminated in next versions

    //As long as modifying matrix row number is not possible,
    //we have no choice but counting row number first and
    //then parse the file. Horrible!
    char *aux;
    //First, count columns and buffer size:
    while(fd.get(read) && (read!=rowDelim[0]))
    {
      if (read == cellDelim[0]) cols++;
    }
    
    //Count rows:
    while(fd.ignore(MaxLineLength,rowDelim[0])) rows++;

    // This MACRO is necessary for VC++ compilers. If you use a .NET 
    // compiler, comment out the next three lines.
    // See: http://support.microsoft.com/default.aspx?scid=kb;EN-US;q240015 
    // for info on this issue. 
    # if defined(_MSC_VER) && (_MSC_VER<1400)
    rows++;
    # endif
    BMatrix<BDat> m(rows,cols);
    
    fd.clear(ios::goodbit);
    fd.seekg(0,ios::beg);
    
    //Parse again the file:
    for(i=0;i<rows;i++)
    {
      fd.getline(buffer,MaxLineLength, rowDelim[0]);
      //Parse line:
      aux=&(buffer[0]);
      for(j=k=0;j<cols;j++)
      {
        char* ptr = (char*)aux;
        for(; k<MaxLineLength-1; k++)
        {
          if(aux[0] && (aux[0]!=cellDelim[0]) && !isspace((unsigned char)aux[0])) 
          { 
            ++aux; 
          }
          else
          {
            break;
          }
        }
        if(k==MaxLineLength-1)
        {
          Error(I2("Line maximum length ","Tamaño máximo de línea ")+MaxLineLength+
                I2(" exceeded  at column "," excedido en la columna ")+j+
                I2(" iof row "," de la fila ")+i+ 
                I2(" of file "," del fichero ")+fileName.String());
        };
        aux[0]='\0';
        aux++;
        m(i,j).PutValue(ptr);
      }
    }
    uMat = new BContensMat("", m, GetFilePrefix(TolPath()));
  }
  contens_.RobStruct(NCons(uMat),NIL,BSet::BMTFile);
  delete(buffer);
  fd.close();
}




//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetIncludePRJ);
DefExtOpr(1, BSetIncludePRJ, "IncludePRJ", 1, 1, "Text",
	  I2("(Text fileName)", "(Text nombreFichero)"),
	  I2("Returns a set with the objects in a Project file (PRJ) "
	     "wich name is the argument.",
	     "Devuelve el conjunto de todos los ficheros de un fichero "
	     "de projecto (PRJ)."),
	  BOperClassify::FileIncluding_);
//--------------------------------------------------------------------
void BSetIncludePRJ::CalcContens()
//--------------------------------------------------------------------
{
    BInt n;
    BText oldDir = BDir::GetCurrent();
    if(!Open()) {
	return;
    }
    BText currentDir = GetFilePath(TolPath());
    BDir::ChangeCurrent(currentDir);
    BDir	fil(TolPath());
    BText block(fil.Bytes()+10);
    Read(File(), block);
    Close();
    
    BArray<BText> section;
    BList* result = NIL;
    BList* aux	= NIL;
    ReadAllTokens(block, section, ']');
    BArray<BText> expression;
    ReadAllTokens(section[section.Size()-1], expression, ';');
    BSyntaxObject* object;
    BGrammar* gra = NIL;
    BGrammar::PutLast(GraSet());
    for(n=0; (n<expression.Size())&&!Terminate(); n++)
    {
	Std(BText("\nProject File ") + expression[n]);
	BArray<BText> expr;
	ReadAllTokens(expression[n], expr, '=');
	if(Compact(expr[1]).HasName() &&
	   (object=::Evaluate(expr[1])))
	{
	    gra = object->Grammar();
	    BGrammar::PutLast(gra);
	    LstFastAppend(result, aux, object);
//            object->Do();
	}
    }
    contens_.RobStruct(result,NIL,BSet::PRJFile);
    contens_.PutSourcePath(curSourcePath_);
    BDir::ChangeCurrent(oldDir);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetIncludeTOL);
DefExtOpr(1, BSetIncludeTOL, "IncludeTOL", 1, 2, "Text Real",
	  I2("(Text fileName [, Real isModule=False])", 
       "(Text nombreFichero [, Real isModule=False])"),
	  I2("Returns the set of all the objects of a file wrote in "
	     "Time Object Language (TOL). wich name is the argument. "
	     "Includes valid expressions in TOL separated by semicolon "
	     "and with free format, that is to say, the tabulators, "
	     "line jumps and  characters of consecutive space are "
	     "equivalent to just one character of space. \n\n"
	     
	     "Example :\n\n"
	     
	     "	Real x = Sin(Pi/3);\n\n"
	     "	Real y = Cos(Pi/3);\n\n"
	     "	Real z = x^y;\n\n"
	     "	Real t = y/x;\n\n"
	     ,
	     "Devuelve el conjunto de todos los objetos de un fichero TOL."
	     "Incluye expresiones válidas en TOL separadas por punto y "
	     "coma y con formato libre, es decir, los tabuladores, saltos "
	     "de línea y carácteres de espacio consecutivos son "
	     "equivalentes a un único carácter de "
	     "espacio.\n\n"
	     
	     "Ejemplo :\n\n"
	     
	     "	Real x = Sin(Pi/3);\n\n"
	     "	Real y = Cos(Pi/3);\n\n"
	     "	Real z = x^y;\n\n"
	     "	Real t = y/x;\n\n"),
	  BOperClassify::FileIncluding_);
//--------------------------------------------------------------------
void BSetIncludeTOL::CalcContens()
//--------------------------------------------------------------------
{
    BText oldDir = BDir::GetCurrent();
    if(!Open())
    {
	    return;
    }
    bool isModule = false;
    if(Arg(2))
    {
      isModule = (bool)Real(Arg(2));
    }
    BText currentDir = GetFilePath(TolPath());
    BDir::ChangeCurrent(currentDir);
    BDir	fil(TolPath());
    BText block(fil.Bytes()+10);
    Read(File(), block);
    bool oldRunningUseModule = false;
    if(isModule) { oldRunningUseModule = BOis::SetRunningUseModule(true); }
    BList* result = MultyEvaluate(block);
    if(isModule) { BOis::SetRunningUseModule(oldRunningUseModule); }
    contens_.RobStruct(result,NIL,BSet::TOLFile);
    contens_.PutSourcePath(curSourcePath_);
    BDir::ChangeCurrent(oldDir);
    Close();
    BinRead();
}


//--------------------------------------------------------------------
static bool ReadHeaderToken(BText&	tok, const BText& header,
			    BInt&	pos,	   BBool& endHeader)
//--------------------------------------------------------------------
{
  if(endHeader) { return(true); }
  int numChar=0;
  BChar buf[1024*8];
  buf[0]='\0';
  sscanf(header.String()+pos, "%s%n", buf, &numChar);
  if(numChar>=64)
  {
    Error(I2("Too long Serie name in BDT header",
             "Nombre de Serie demasiado largo en cabecera de BDT")+
             ":\n'"+buf+"'");
    return(false);
  }
  if(buf[0] && (numChar) && (header.Length()+pos>numChar)) 
  {
	  pos += numChar;
  } 
  else 
  {
	  pos  = -1;      
  }
  endHeader = (pos==-1);
  if(!endHeader)
  {
	  tok = buf;
  }
  else
  {
	  tok = "";
  }
/*
    Std(BText("\nReaded BDT header token <")+tok+">");
    if(pos>=0) { Std(BText("\nRest <")+pos+">"); }
    else       { Std(BText("End of header."));   }
*/
  return(true);
}


//--------------------------------------------------------------------
DefExtOpr(1, BSetIncludeBDT, "IncludeBDT", 1, 5, "Text Text Date Date Text",
	  I2("(Text fileName [, Text separator=\";\", "
	     "Date from, Date until, Text header])",
	     "(Text nombreFichero [, Text separador=\";\", "
	     "Date desde, Date hasta, Text cabecera])"),
	  I2("Returns a set with the primary series in a Bayes Data "
	     "Table (BDT) file.\n"
	     "Include time series with the same dating and between the "
	     "same dates, in table format. In the one which the first "
	     "row contains the name of the followed dating by the names "
	     "of the series separated by semicolon.\n"
	     
	     "In the subsequent lines, appears a date in the first "
	     "column, and, in the following, the values of the "
	     "corresponding series for such date. "
	     "The dates should appear in order. If a date is not in "
	     "order, it does not exist, or it is wrong written, all "
	     "the series of the file will "
	     "contain for that date, the  value of landfill of the system "
	     "(BDTFILLVALUE), that it is initialized to zero, and the "
	     "user can change when wish writing, for example, "
	     "BDTFILLVALUE:=3.\n\n"
	     
	     "Example:\n\n"
	     
	     "Daily;		  S1;	       S2;\n"
	     "1995/01/01;   1023.234;	 8745.874;\n"
	     "1995/01/02;   1014.347;	 8746.565;\n"
	     "1995/01/03;   1151.925;	 8745.675;\n"
	     "1995/01/05;   1127.942;	 8747.234;\n"
	     "1995/01/06;   1003.134;	 8748.734;\n"
	     "1995/01/07;   1026.246;	 8741.783;\n"
	     "...\n"
	     ,
	     "Devuelve el conjunto de todas las series primarias de un "
	     "fichero con formato Bayes Data Table (BDT).\n"
	     "Incluye series temporales con el mismo fechado y entre "
	     "las mismas fechas, en formato de tabla. En la que la "
	     "primera fila contiene el nombre del fechado seguido "
	     "de los nombres de las series separadas por "
	     "punto y coma.\n"
	     
	     "En las líneas posteriores, aparece una fecha en la primera "
	     "columna, y, en  las siguientes, los valores de las series "
	     "correspondientes para dicha fecha.  Las fechas deben "
	     "aparecer en orden. Si una fecha no está en orden, no "
	     "existe, o está mal escrita, todas las series del fichero "
	     "contendrán para esa fecha, el  valor de relleno del sistema "
	     "(BDTFillValue), que está inicializado a cero, y el usuario "
	     "puede cambiar cuando desee escribiendo, por ejemplo, "
	     "BDTFillValue:=3.\n\n"
	     
	     "Ejemplo:\n\n"
	     
	     "Diario;		  S1;	       S2;\n"
	     "1995/01/01;   1023.234;	 8745.874;\n"
	     "1995/01/02;   1014.347;	 8746.565;\n"
	     "1995/01/03;   1151.925;	 8745.675;\n"
	     "1995/01/05;   1127.942;	 8747.234;\n"
	     "1995/01/06;   1003.134;	 8748.734;\n"
	     "1995/01/07;   1026.246;	 8741.783;\n"
	     "..."),
	  BOperClassify::FileIncluding_);

//--------------------------------------------------------------------
void BSetIncludeBDT::CalcContens()
//--------------------------------------------------------------------
{
    static BFilter     filter;
    static BDateFormat format("%Y%m%d");
    BUserTimeSet*      dating = NIL;
    BInt	       readPosition=0;
    BGrammar*	       gra;
    BBool	       endHeader = BFALSE;
    BText	       datingTxt(1025);
    BText	       serName(1025);

  //Std("\nTRACE BSetIncludeBDT::CalcContens() 01");
    if(!Open()) 
    {
	return;
    }
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 02");
    
    if(Arg(2)) {
	sep_ = Text(Arg(2)).Get(0);
    }
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 03");
    if(Arg(3)) {
	first_ = Date(Arg(3));
    }
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 04");
    if(Arg(4)) {
	last_ = Date(Arg(4));
    }
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 05");
    if(Arg(5)) {
	header_ = Text(Arg(5));
	hasHeader_ = BFALSE;
    } else {
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 06");
	do {
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 07");
	    GetLine(File(), 
		    header_, MaxLineLength, '\n'); // ASC 1024 limitacion (César)
	    header_.Replace(',',' ');
	    header_.Replace('\t',' ');
	    header_.Replace(';',' ');
	    filter.Clean(header_);
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 08");
	    header_.Compact();
//            Std(BText("\nReaded BDT line \n")+header_);
	} while(!File().eof() && !header_.HasName());
    }
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 09");

    gra = GraTimeSet();
    if(!ReadHeaderToken(datingTxt, header_, readPosition, endHeader))
    {
      return;
    };
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 10");

    if(!datingTxt.HasName())
    {
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 11");
	Error(BDT+TolPath()+I2(" has not BDT format.",
			       " no tiene formato BDT."));
	return;
    }

  //Std("\nTRACE BSetIncludeBDT::CalcContens() 12");
    if(!(dating=Tms(GraTimeSet()->EvaluateExpr(datingTxt))))
    {
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 13");
	Error(BDT+TolPath()+" "+datingTxt+ I2(" is not a valid dating.",
					      " no es un fechado válido."));
	return;
    }

  //Std("\nTRACE BSetIncludeBDT::CalcContens() 14");
//    Trace(BText("BDT Dating = ")+datingTxt);
    gra = GraSerie();
    serie_.ReallocBuffer(1000);
    BInt m = 0;

    BList* result = NIL;
    BList* aux	= NIL;
    int stackPos = BGrammar::StackSize();
    int saveLevel = BGrammar::Level();
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 15");

    if(!BTimeSerie::GlobalizeSeries().Value()) 
    { 
      BGrammar::PutLevel(saveLevel); 
    }
    else 
    {
      BGrammar::PutLevel(0);
    }
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 16");

    while(!endHeader)
    {
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.01");
	if(m==serie_.Size()-1) { serie_.ReallocBuffer(2*m); }
  if(!ReadHeaderToken(serName, header_, readPosition, endHeader))
  {
    BGrammar::PutLevel(saveLevel);
    return;
  };
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.02");
	if(!endHeader)
	{
//	      Trace(BText("BDT Serie ")+serName);
	    if(!serName.HasName() && (m==0)) {
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.03");
		serName = GetFilePrefix(TolPath()); 
	    }
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.04");

	    if(serName.HasName())
	    {
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.05");
		// search globally or locally
		if(BTimeSerie::GlobalizeSeries().Value())
		    serie_[m] = (BTsrPrimary*) gra->FindOperand(serName,false);
		else
		    serie_[m] = (BTsrPrimary*) gra->FindLocal(serName);
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.06");

		// error if previously exist
		if(serie_[m])
		{
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.07");
		    Error(I2("Serie ", "La serie ") + serName +
			  I2(" can't be created because already exists.\nThe "
			     "existing serie will be changed with the new "
			     "one\n", " no puede ser creada porque ya "
			     "existe.\nLa serie existente va a ser reemplaza "
			     "por la nueva.\n"));
		}
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.08");

		if(!serie_[m])
		    serie_[m] = new BTsrPrimary(serName, serName, dating);
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.09");
		
		LstFastAppend(result, aux, serie_[m]);
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 17.10");
		m++;
	    }
	}
    }
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 18");

    serie_.ReallocBuffer(m);
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 19");
    contens_.RobStruct(result,NIL,BSet::BDTFile);
    contens_.PutSourcePath(curSourcePath_);

  //Std("\nTRACE BSetIncludeBDT::CalcContens() 20");

    if(!BTimeSerie::GlobalizeSeries().Value())
    {
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 21");
	if(saveLevel>0)
	    BGrammar::DestroyStackUntil(stackPos, 0);
    } 
    else BGrammar::PutLevel(saveLevel);
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 22");

    Close();
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 23");
    BinRead();
  //Std("\nTRACE BSetIncludeBDT::CalcContens() 24");
    ReadData();
}



//--------------------------------------------------------------------
  void BSetIncludeBDT::ReadData()
//--------------------------------------------------------------------
{
//    InitTotalTime("BSetIncludeBDT::ReadData");
    if(!Busy())
    {
	BText header;
	if(!Open()) { return; }
	BChar ch;
	if(!Arg(2)) {
	    do File().get(ch); while((ch!='\n') && !(File().eof()));
	}

/*
        if(first_.HasValue() && last_.HasValue())
        {
	    BQSepFile q (TolPath());
	    q.AddFields(';');
	    ReadBDTQFile(TolPath(),fillValue_,q,serie_,enableWarning_,
	    first_,last_);
	}
	else
	{
	    ReadBDTFile(TolPath(),fillValue_,File(),serie_,enableWarning_);
	}
*/	

	ReadBDTFile(TolPath(),fillValue_,File(),
		    serie_,enableWarning_,first_,last_,sep_,hasHeader_);
	Close();
    }
//    SumPartialTime;
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetIncludeBSI);
DefExtOpr(1, BSetIncludeBSI, "IncludeBSI", 1, 1, "Text",
	  I2("(Text fileName)", "(Text nombreFichero)"),
	  I2("Returns a set with the primary series in a Bayes Serie Index "
	     "(BSI) File.\n"
	     ,
	     "Devuelve el conjunto de todas las series primarias de un "
	     "fichero con "
	     "formato Bayes Serie Index (BSI).\n"),
	  BOperClassify::FileIncluding_);

//--------------------------------------------------------------------
void BSetIncludeBSI::CalcContens()
//--------------------------------------------------------------------
{
  if(!Open()) {	return; }
  contens_.RobElement(BTsrBSI::ReadFile(TolPath()));
  contens_.PutSubType(BSet::BSIFile);
  contens_.PutSourcePath(curSourcePath_);
  Close();
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetIncludeBDC);
DefExtOpr(1, BSetIncludeBDC, "IncludeBDC", 1, 1, "Text",
	  I2("(Text fileName)", "(Text nombreFichero)"),
	  I2("Returns a set with the primary series in a Bayes Data Column "
	     "(BDC) File.\n"
	     "Include time series with any dating and between dates "
	     "whatever. For "
	     "each series is written first a head-board with the name, the "
	     "description, the dating and the initial date. Subsequently "
	     "it is "
	     "written a datum in each line understanding that belong to "
	     "consecutive "
	     "dates, as of the initial, to what is long of the dating of "
	     "the series "
	     "in question.\n\n"
	     
	     "Example:\n\n"
	     
	     "#Serie1\n"
	     "UPDATE Serie1 IS A primary time Series END\n"
	     "VALUE OF (Serie1) Dating = Monthly\n"
	     "VALUE OF (Serie1) Titulo	= \"SERIES OF TEST 1, IT CAN "
	     "BE ERASED\"\n"
	     "& SUBSTITUTION DATA\n"
	     "a1992m1d1\n"
	     "19209,\n"
	     "19924,\n"
	     "29202,\n"
	     "31206,\n"
	     "19202,\n"
	     "19202,\n"
	     "19308;\n"
	     "#Serie2\n"
	     "UPDATE Serie2 IS A primary time Series END\n"
	     "VALUE OF (Serie2) Dating = Daily\n"
	     "VALUE OF (Serie2) Titulo	= \"SERIES OF TEST 2, IT CAN "
	     "BE ERASED\"\n"
	     "& SUBSTITUTION DATA\n"
	     "a1997m1d1\n"
	     "126204,\n"
	     "219456,\n"
	     "145207,\n"
	     "124209,\n"
	     "119451,\n"
	     "143920,\n"
	     "276309,\n"
	     "219456,\n"
	     "145207,\n"
	     "124209;"
	     ,
	     "Devuelve el conjunto de todas las series primarias de un "
	     "fichero con "
	     "formato Bayes Data Column (BDC).\n"
	     "Incluye series temporales con cualquier fechado y entre fechas "
	     "cualesquiera. Para cada serie se escribe primero una "
	     "cabecera con el nombre, la descripción, el fechado y la "
	     "fecha inicial. Seguidamente se escribe un dato en cada "
	     "línea entendiendo que pertenecen a fechas consecutivas, "
	     "a partir de la inicial, a lo largo del fechado de las "
	     "series en cuestión.\n\n"
	     
	     "Ejemplo:\n\n"
	     
	     "#Serie1\n"
	     "ACTUALIZACION Serie1 ES UNA Serie temporal primaria FIN\n"
	     "VALOR DE (Serie1) Fechado = Mensual\n"
	     "VALOR DE (Serie1) Titulo	= \"SERIE DE PRUEBA 1, PUEDE "
	     "BORRARSE\"\n"
	     "&DATOS SUSTITUCION\n"
	     "a1992m1d1\n"
	     "19209,\n"
	     "19924,\n"
	     "29202,\n"
	     "231206,\n"
	     "19202,\n"
	     "19202,\n"
	     "19308;\n"
	     "#Serie2\n"
	     "ACTUALIZACION Serie2 ES UNA Serie temporal primaria FIN\n"
	     "VALOR DE (Serie2) Fechado = Diario\n"
	     "VALOR DE (Serie2) Titulo	= \"SERIE DE PRUEBA 2, PUEDE "
	     "BORRARSE\"\n"
	     "&DATOS SUSTITUCION\n"
	     "a1997m1d1\n"
	     "126204,\n"
	     "219456,\n"
	     "145207,\n"
	     "124209,\n"
	     "119451,\n"
	     "143920,\n"
	     "276309,\n"
	     "219456,\n"
	     "145207,\n"
	     "124209;\n"),
	  BOperClassify::FileIncluding_);

//--------------------------------------------------------------------
void BSetIncludeBDC::CalcContens()
//--------------------------------------------------------------------
{
    static BDateFormat format("a%Ym%%md%d");
    
    if(!Open()) {
	return;
    }
    BText line;
    BInt numSer=0;
    BList* result = NIL;
    BList* aux	= NIL;
    while(!File().eof())
    {
	BInt  pos;
	BText datingName;
	BText description;
	BDate firstDate;
	BDate lastDate;
	BUserTimeSet* dating = NIL;
	
	if(GetLine(File(), line, MaxLineLength,  '#').eof() ||
	   GetLine(File(), line, MaxLineLength, '\n').eof() ||
	   !line.Compact().HasName()  ) { break; }
	BText name = line;
	GetLine(File(), line, MaxLineLength, '\n');
	GetLine(File(), line, MaxLineLength, '\n');
	while(line.Find("DATOS SUSTITUCION", 0)<0)
	{
	    if(line.Find("Fechado", 0)>=0)
	    {
		pos = line.Find('=', 0);
		datingName.Copy(line, pos+1, line.Length()-1);
		datingName.Compact();
	    }
	    else if(line.Find("Titulo", 0)>=0)
	    {
		pos = line.Find('=', 0);
		description.Copy(line, pos+1, line.Length()-1);
		description.Replace('\"', ' ');
		description.Compact();
	    }
	    GetLine(File(), line, MaxLineLength, '\n');
	}
	GetLine(File(), line, MaxLineLength, '\n');
	firstDate = format.TextToDate(line.Compact());
	if(dating=Tms(GraTimeSet()->EvaluateExpr(datingName)))
	{
	    if(!firstDate.HasValue() || !dating->Contain(firstDate))
	    {
		Error(BDC + line +
		      I2(" is not a date in "," no es una fecha de ") +
		      dating->Name());
		DESTROY(result);
		return;
	    }
	    numSer++;
	    BTsrPrimary* ser = new BTsrPrimary(name, description, dating);
//	    if(numSer%100==0) { Std(BText("\nReading ")+numSer+"-th serie "+name); }
	    ser->Realloc(100);
	    BInt numData=0;
	    BDate dte = firstDate;
	    BChar last;
	    do
	    {
		if(numData==ser->Data().Size()-1)
		{
		    ser->Realloc((numData+1)*2);
		}
		BDat  x;
		BChar dataLine[256];
		File() >> dataLine;
//		Std(BText("\n")+dataLine);
		if(dataLine[0]!='?')
		{
		    BReal v;
		    sscanf(dataLine, "%lg%c", &v, &last);
		    x = v;
		}
		else
		{
		    BChar ch;
		    sscanf(dataLine, "%c%c", &ch, &last);
		}
//		Std(BText(" dte = ")+dte+" val = "+x);
		ser->PutBufDat(numData,x);
		numData++;
		if(last!=';') { dte+=dating; }
	    }
	    while(last!=';');
	    lastDate = dte;
	    ser->Realloc(numData);
	    ser->PutFirstDate(firstDate);
	    ser->PutLastDate (lastDate);
	    LstFastAppend(result, aux, ser);
	}
    }
    contens_.RobElement(result);
    contens_.PutSubType(BSet::BDCFile);
    contens_.PutSourcePath(curSourcePath_);
    Close();
    BinRead();
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetIncludeBST);
DefExtOpr(1, BSetIncludeBST, "IncludeBST", 1, 1, "Text",
	  I2("(Text fileName)", "(Text nombreFichero)"),
	  I2("Include a matrix with a row in each line and the data "
	     "separated by semicolon. Obviously, all the rows have of "
	     "having the same number of elements.\n\n"
	     "If we have defined previously the structure\n\n"
	     "Struct PerData\n"
	     "(\n"
	     "	Text Name,\n"
	     "	Date Birth,\n"
	     "	Real Weight\n"
	     ");\n\n"
	     "We can include a file BST as this\n\n"
	     "PerData; Name;    Birth;      Weight;\n"
	     "	     ; \"John\"  ;a1963m11d01;67;\n"
	     "       ; \"Joseph\";a1965m02d03;72;\n"
	     "       ; \"Juan\"  ;a1966m05d11;78;\n"
	     ,
	     "Devuelve el conjunto de todas los conjuntos estruturados de "
	     "un fichero con formato Bayes Structured Table (BST)."
	     "Incluye una matriz con una fila en cada línea y los datos "
	     "separados por punto y coma. Obviamente, todas las filas han "
	     "de tener el mismo número de elementos.\n\n"
	     "Si tenemos definida anteriormente la estructura\n\n"
	     "Struct PerData\n"
	     "(\n"
	     "	Text Name,\n"
	     "	Date Birth,\n"
	     "	Real Weight\n"
	     ");\n\n"
	     "Podemos incluir un fichero BST como éste\n\n"
	     "PerData; Name;    Birth;      Weight;\n"
	     "	     ; \"John\"  ;a1963m11d01;67;\n"
	     "       ; \"Joseph\";a1965m02d03;72;\n"
	     "       ; \"Juan\"  ;a1966m05d11;78;\n\n"),
	  BOperClassify::FileIncluding_);
//--------------------------------------------------------------------
void BSetIncludeBST::CalcContens()
//--------------------------------------------------------------------
{
//InitTotalTime("BSetIncludeBST::CalcContens");
  static BFilter filter;
  static BDateFormat format("a%Ym%%md%d");
  BBool enableWarning = BTRUE;
  BList* result = NIL;
  BList* aux = NIL;
  BBool error = BFALSE;
  BArray<BText> line;
  BStruct* str;
  BInt n;
  BInt numLine=0;
  BBool lastComma = BFALSE;
  BText errMsg=BST;
  BText txt(MaxLineLength,' ');

  if(!Open())
  {
    Error(BST+TolPath()+I2(" can't open"," no se pudo abrir"));
    return;
  }
  if(Arg(2)) { enableWarning = (BBool)Real(Arg(2)); }
  GetLine(File(), txt, MaxLineLength, ';');
  txt.Replace("\"","");
  txt.Compact();
  if(!(str = FindStruct(txt)))
  {
    Error(errMsg+", "+txt+I2(" isn't an struct"," no es una estructura"));
    return;
  }
  GetLine(File(), txt, MaxLineLength, '\n');
  txt.Replace('\"',' ');
  txt.Replace('\r',' ');
  txt.Replace('\t',' ');
  txt.Compact();
  while(txt.Last()==' ') { txt.PutLength(txt.Length()-1); }
  ReadAllTokens(txt, line, ';', '\"','\\');
  for(n = 0; (n<line.Size())&&(n<str->Size()); n++)
  {
    if(line[n].Compact()!=(*str)[n].Name())
    {
      Error(errMsg+", "+line[n]+I2(" breaks structure's order.",
                                   " rompe el orden de la estructura."));
      error = BTRUE;
    }
  }
  if(n<str->Size()-1)
  {
    Error(errMsg+", "+I2(" there are not enoutgh columns with field names",
                         " no hay suficientes columnas con nombres de campos"));
    return;
  }
  if((n>str->Size()+1)||((n==str->Size()+1)&&line[n].Compact().HasName()))
  {
    Error(errMsg+", "+I2(" there are too columns with field names",
                         " hay demasiadas columnas con nombres de campos"));
    return;
  }
  while(!File().eof() && !error)
  {
    numLine++;
    BArray<BText> fieldText;
    BText name;
    GetLine(File(), name, MaxLineLength, ';');
    GetLine(File(), txt,  MaxLineLength, '\n');
  //txt.Compact();
    if(txt.HasName())
    {
      ReadAllTokens(txt, fieldText, ';', '\"','\\');
      name.Compact();
      filter.Transform(name);
      name.Replace(' ','_');
      if(name.HasName() && !isalpha(name.Get(0)))
      {
        Error(errMsg+", "+name+I2(" isn't a valid name.",
                                  " no es un nombre válido."));
        error = BTRUE;
        DESTROY(result);
      }
      else
      {
        BList* fieldList = NIL;
        BList* fieldAux   = NIL;
        BText ftxt;
        for(n = 0; (n<str->Size() && !error); n++)
        {
          BSyntaxObject* fieldValue = NIL;
          bool ok = true;   
          ftxt = fieldText[n]; 
        //ftxt.Compact();
          if(!ftxt.HasName())
          {
            BField&   field = (*str)[n];
            Error(errMsg+I2(", line ", ", línea ")+ numLine+" "+
                         I2(" empty field ", " campo vacío ")+field.Name());
            error = BTRUE;
          }
          else
          {
            ftxt.Replace('\'','\"');
            BField&   field = (*str)[n];
            BGrammar* gra   = field.Grammar();
          //Std(BText("\n(")+numLine+","+n+") : <"+ftxt+">");
            if(gra==GraDate())
            {
              BText expr = Compact(ftxt);
              BDate dte;
              if((expr=="") | (expr=="?") | (expr=="Unknown") | (expr=="UnknownDate"))
              {
                ok = true; 
              }
              else
              { 
                dte = ConstantDate(expr);
                ok = (dte.HasValue()!=0);
              }
              fieldValue = new BContensDate("",dte,"");
            }
            else if(gra==GraReal())
            {
              char * pEnd;
              BText expr = Compact(ftxt);
              double x = BDat::Nan();
              // check for inf because strtod from gcc consider it as
              // a valid number
              if((expr=="") || (expr=="?")
#ifdef __GNUC__
                 ||
                 !strcasecmp(expr.String(),"inf")  ||
                 !strcasecmp(expr.String(),"-inf") ||
                 !strcasecmp(expr.String(),"+inf")
#endif
                 )
              {
                ok = true; 
              }
              else
              { 
                x = strtod(expr.String(),&pEnd);
                if(!pEnd || pEnd[0]) { x = BDat::Nan(); }
                ok = !IS_NAN(x);
              }
              fieldValue = new BContensDat("",x,"");
            }
            else if(gra==GraSet())
            {
              BText expr = Compact(ftxt);
              if((expr=="[[ ]]"))
              {
                ok = true; 
                BSet empty;
                fieldValue = new BContensSet("",empty,"");
              }
            }
            if(!fieldValue) 
            {
              fieldValue = gra->EvaluateExpr(ftxt);
            }
            bool error = !fieldValue;
            if(error || !ok)
            {
              BText msg = errMsg+I2(", line ", ", linea ")+numLine+
                    " '"+ftxt+"' "+
                    I2("is not a valid value for field ",
                       "no es un valor válido para el campo ")+
                    field.Grammar()->Name()+ " " +field.Name(); 
              if(error) { Error(msg); }
              else      { Warning(msg+"\nSe asumirá el valor desconocido"); }
            }
            if(!error)
            {
            //Std(BText("\n")+field.Name()+" = "
            //+fieldValue->Dump());
              LstFastAppend(fieldList, fieldAux, fieldValue);
            }
          }
        }
        if(error)
        {
          DESTROY(result);
          DESTROY(fieldList);
        }
        else
        {
          LstFastAppend(result,aux,
          NewSet(name,
                 "",
                 fieldList,
                 str,
                 BSet::Structured));
        }
      }
    }
  }
  contens_.RobStruct(result,NIL,BSet::BSTFile);
  contens_.PutSourcePath(curSourcePath_);
  Close();
//SumPartialTime;
}

static BText validExtensions = "prj in tol bst bdt bdc bsi bmt";

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetIncludeDIR);
DefExtOpr(1, BSetIncludeDIR, "IncludeDIR", 1, 1, "Text",
	  I2("(Text path)", "(Text camino)"),
	  I2("Returns a set with all the recognozible files in a directory.",
	     "Devuelve el conjunto de todos los ficheros de un directorio "
	     "que TOL pueda reconocer."),
	  BOperClassify::FileIncluding_);

//--------------------------------------------------------------------
void BSetIncludeDIR::CalcContens()
//--------------------------------------------------------------------
{
    BDir dir(TolPath());
    BList* result = NIL;
    BList* aux	= NIL;
    
    BText oldDir = BDir::GetCurrent();
    BDir::ChangeCurrent(TolPath());
    
    for(BInt f = 0; (f<dir.NumFiles())&&!Terminate(); f++)
    {
	BText extension = GetFileExtension(dir.FileName(f));
	if(validExtensions.Find(ToLower(extension))>=0)
	{
	    BSyntaxObject* fileObj =IncludeFile(dir.FileName(f));
	    if(fileObj) { LstFastAppend(result, aux, fileObj); }
	}
    }
    for(BInt d = 0; (d<dir.NumDirs())&&!Terminate(); d++)
    {
	BSyntaxObject* dirObj = IncludeFile(dir.DirName(d));
	if(dirObj) { LstFastAppend(result, aux, dirObj); }
    }
    contens_.RobStruct(result,NIL,BSet::DIRFile);
    contens_.PutSourcePath(curSourcePath_);
    BDir::ChangeCurrent(oldDir);
}


//--------------------------------------------------------------------
BUserSet* IncludeFile(const BText& fileName)
    
/*! Includes a file of a recognozible type and searches recursively
 *  if fileName is a directory.
 */
//--------------------------------------------------------------------
{
    BText path = Replace(fileName,'\\', '/' );
    BText name = GetFilePrefix(path);
//    BText expression = BText("Set ")+name+" = Include(\""+path+"\")";
    BText expression = BText("Include(\"")+path+"\")";
    BSetFromFile* set= (BSetFromFile*)(GraSet()->EvaluateExpr(expression));
    
    return(set);
}


//--------------------------------------------------------------------
BSyntaxObject* IncludeEvaluator(BList* arg)
//--------------------------------------------------------------------
{
  static BSyntaxObject* show_msg_including_so = NULL;
  static bool show_msg_including = true;
  static BSyntaxObject* show_msg_included_so = NULL;
  static bool show_msg_included = true;
  if(!show_msg_including_so)
  {
    show_msg_including_so = BNameBlock::FindPublicMember(GraReal(),
     "TolConfigManager::Config::Various::Verbose::IncludingFile");
  }
  if(!show_msg_included_so)
  {
    show_msg_included_so = BNameBlock::FindPublicMember(GraReal(),
     "TolConfigManager::Config::Various::Verbose::IncludedFile");
  }
  if(show_msg_including_so)
  {
    show_msg_including = Real(show_msg_including_so)!=0.0;
  }
  if(show_msg_included_so)
  {
    show_msg_included = Real(show_msg_included_so)!=0.0;
  }

  BTimer tm;
  BSyntaxObject* result = NIL;
  BText name = Text((BSyntaxObject*)Car(arg));
  if(show_msg_including)
  {
    Std(I2("Including ","Incluyendo ")+GetStandardAbsolutePath(name)+"\n");
  }
  BText extension = ToLower(GetFileExtension(name));
  BText autoPath = BOis::AutoPath(name);
  BBool isOis = false;
  if(extension=="")
  {
    isOis = CheckIsFile(autoPath+"/header.xml");
  }
  else if(extension=="oza")
  {
    isOis = CheckIsFile(autoPath);
  }
  if(isOis) 
  { 
    result=GraSet()->EvaluateExpr(BText("Ois.Load(\"")+name+"\")[1]"); 
  }
  if(!result && (BGrammar::Level()==0)) 
  { 
    result = (BSyntaxObject*)BSetFromFile::FindCompiled(name); 
    if(result && (result->Level()>0)) { result = NULL; }
  }
  if( result) { DESTROY(arg); }
  else
  {
   	     if(extension=="prj") { result=new BSetIncludePRJ(arg); }
	  else if(extension=="in")  { result=new BSetIncludeTOL(arg); }
	  else if(extension=="tol") { result=new BSetIncludeTOL(arg); }
	  else if(extension=="bst") { result=new BSetIncludeBST(arg); }
	  else if(extension=="bdt") { result=new BSetIncludeBDT(arg); }
	  else if(extension=="bdc") { result=new BSetIncludeBDC(arg); }
	  else if(extension=="bsi") { result=new BSetIncludeBSI(arg); }
	  else if(extension=="bmt") { result=new BSetIncludeBMT(arg); }
	  else if(BDir(name).IsDir())	   
    { 
      result=new BSetIncludeDIR(arg);
    }
	  else			           
    { 
      Error(I2("Unknown file type ","Tipo de fichero desconocido ")+"\""+name+"\"");
      DESTROY(arg); 
    }
    BText p;
    BText ok = I2("Included","Ha sido incluido");
	  if(result)
	  {
      double tm1 = (double)time(0);
	    p = result->TolPath();
	    Set(result);
      if(!CheckIsFile(p) && !BDir::CheckIsDir (p)) 
      {
        ok = I2("NOT included","No ha sido incluido");
      }
	  }
    else
    {
      p = name;
      ok = I2("NOT included","No ha sido incluido");
    }
    if(show_msg_included)
    {
      Std(ok+I2(" file ", " el fichero ")+p+" ["+BDat((double)tm.MSec()/1000.0).Format("%.3lf")+" sec.]\n");
    }
  }
  return(result);
}
    

#if !defined(USE_DELAY_INIT)
    
static BExternalOperator include_ (
    "Include",
    GraSet(),
    "Text",
    IncludeEvaluator,
    1,
    1,
    I2("(Text path)", "(Text camino)"),
    I2("Calls to the corresponding include function for the given path.",
       "Llama al inclusor adecuado para el camino dado."),
    BOperClassify::FileIncluding_
    );

#else

static void * clone_include_() {
    return new
	BExternalOperator (
	    "Include",
	    GraSet(),
	    "Text",
	    IncludeEvaluator,
	    1,
	    1,
	    I2("(Text path)", "(Text camino)"),
	    I2("Calls to the corresponding include function for the "
	       "given path.",
	       "Llama al inclusor adecuado para el camino dado."),
	    BOperClassify::FileIncluding_);
}

static BExternalOperator * include_ = 
  (BExternalOperator*)(__delay_init((void**)&include_,
				    &clone_include_));

#endif




#if defined(__USE_TC__) && defined(__USE_KPI__)
//--------------------------------------------------------------------
DefExtOpr(1, BSetWriteSet, "WriteSet", 2, 2, "Set Text",
	  I2("(Set set, Text fileName)",
	     "(Set cto, Text nombreFichero)"),
	  I2("",
	     "Escribe un conjunto, en el fichero dado, en formato binario."),
	  BOperClassify::General_);
//--------------------------------------------------------------------
void BSetWriteSet::CalcContens()
{
    TolType toltype;
    int intTolType;
    BText fileName = Text(Arg(2));
    FILE* fil = fopen(fileName.String(),"wb");
    if(!fil)
    {
	Error(I2("Cannot open for write file ",
		 "No se pudo abrir para escritura el fichero ") + fileName);
	return;
    }

    contens_ = Set(Arg(1));
    int setsize = contens_.Card();
    BList* element = contens_.Element();
    fwrite(&setsize, 1, sizeof(int), fil);
    
    while(element)
    {
	BSyntaxObject* obj = static_cast<BSyntaxObject*>(Car(element));

	if     (obj->Grammar()==GraCTimeSet()) toltype = TT_CTIMESET;
	else if(obj->Grammar()==GraCTime())    toltype = TT_CTIME;

	intTolType = static_cast<int>(toltype);
	fwrite(&intTolType, 1, sizeof(int), fil);

	switch(toltype)
	{
	    case TT_CTIME:
		BRenContens<BCTime>* renCTime = 
		    static_cast<BRenContens<BCTime>*>(obj);
		BUserCTime* uctmi = renCTime->GetResult();
		uctmi->BinWrite(fil);
/*
		if(!uctmi->HasName()) 
		{
		    BText guion = "-";
		    guion.BinWrite(fil);
		} 
		else (uctmi->Name()).BinWrite(fil);
*/
		break;

	    case TT_CTIMESET:
		BUserCTimeSet* uctms = CTms(obj);
		uctms->BinWrite(fil);
		break;
	}
	    
	element = Cdr(element);
    }
    fclose(fil);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetFromFile, BSetReadSet);
DefExtOpr(1, BSetReadSet, "ReadSet", 1, 1, "Text",
	  I2("(Text fileName)", "(Text nombreFichero)"),
	  I2("Creates a Set reading its content from the given binary file\n",
	     "Crea un conjunto leyendo su contenido de un fichero binario.\n"),
	  BOperClassify::FileIncluding_);
//--------------------------------------------------------------------
void BSetReadSet::CalcContens()
{
    TolType toltype;
    int intTolType;
    BList* objList = 0;
    BList* auxList = 0;
    int setsize = 0;
//    BText name;

    BText fileName = Text(Arg(1));
    FILE* fil = fopen(fileName.String(),"rb");
    if(!fil)
    {
	Error(I2("Cannot read file ",
		 "No se pudo abrir para lectura el fichero ") + fileName);
    }

    fread(&setsize, 1, sizeof(int), fil);
    for(int i=0; i<setsize; i++)
    {
	BSyntaxObject* obj = 0;
	fread(&intTolType, 1, sizeof(int), fil);
	toltype = static_cast<TolType>(intTolType);

	switch(toltype)
	{
	    case TT_CTIME:
		BSyntaxObject* uctmi = BUserCTime::New(fil);
		obj = BRenContens<BCTime>::New("",uctmi);
		obj->PutName(uctmi->Name());
		break;
	    case TT_CTIMESET:
		obj = BCTmsRenamed::New(fil);
		break;
	}
	if(obj) LstFastAppend(objList, auxList, obj);
    }

    contens_ = objList;
}
#endif // __USE_TC__ && __USE_KPI__


