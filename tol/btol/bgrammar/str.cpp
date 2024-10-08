/* str.cpp: Special functions of BGrammar. TOL language.

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

#ifdef NDEBUG
//#define TRACE_LEVEL 4
#endif

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_init.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_boper.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bscanner.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bnameblock.h>
#include <tol/tol_bclass.h>

static BStruct* strStructFieldInfo_  = NULL;

#ifdef CATCH_NON_STANDARD_STRUCT
ofstream& _non_standard_struct_()
{
  static ofstream* aux = NULL;
  if(!aux)
  {
    aux = new ofstream(
      BSys::TolAppData()+"syslog/_non_standard_struct_"+
      BSys::SessionInitTime().Name()+".log", ios::app);   
    (*aux) << "\n"
    "############################################################\n"<<
    "## NON STANDARD STRUCT HANDLING REPORT\n"<<
    "## TOL Version:"<<TOLVersion()<<"\n"<<
    "## Session Time: "<<DteNow().Name()<<"\n"<<
    "############################################################\n"<<
    "\n";
  }
  return(*aux);
}

static void _non_standard_struct_trace_()
{
  if(BSourcePath::Current())
  {
    _non_standard_struct_() <<  
      " IN FILE '"<<BSourcePath::Current()->Name().String()<<"'";
  }
  _non_standard_struct_() <<  "\n";
  _non_standard_struct_() <<  BUserFunction::GetCallStack();
  _non_standard_struct_() <<  "\n";
  _non_standard_struct_() <<  "\n";
  _non_standard_struct_().flush();
}

void _non_standard_struct_creating_without(const BText& name)
{
  _non_standard_struct_() << "CREATING STRUCT '"<<name.String()<<
    "' WITHOUT @";
  _non_standard_struct_trace_();
}


void _non_standard_struct_calling_without(const BText& name)
{
  _non_standard_struct_() << "CALLING STRUCT '@"<<name.String()<<
    "' WITHOUT @";
  _non_standard_struct_trace_();
}


#endif


//--------------------------------------------------------------------
// Initialization
//   Global variables (static in the class BStruct) 
//--------------------------------------------------------------------
BTraceInit("str.cpp");

BArray<BStructStatus> *BStruct::statusTable_ = NULL;

//--------------------------------------------------------------------
BField::BField()
//--------------------------------------------------------------------
: BObject(), grammar_(NIL), struct_(NIL), class_(NIL)
{
}

//--------------------------------------------------------------------
BField::BField(const BText& name, BGrammar* gra, 
               BStruct* str, BClass* cls)
//--------------------------------------------------------------------
: BObject(name), grammar_(gra), struct_(str), class_(cls)
{}

//--------------------------------------------------------------------
BField::BField(const BText& name, const BText& gra, 
               const BText& str, const BText& cls)
//--------------------------------------------------------------------
: BObject(name), grammar_(Gram(gra)), 
  struct_(FindStruct(str)), class_(FindClass(cls,-1))
{}

//--------------------------------------------------------------------
BField::~BField()
//--------------------------------------------------------------------
{}


//--------------------------------------------------------------------
BText BField::GetType() const
//--------------------------------------------------------------------
{
    if(!struct_ && grammar_) { return(grammar_->Name()); }
    else if(struct_)         { return(struct_ ->Name()); }
    else		     { return("NO FIELD TYPE DEFINED"); }
}

//--------------------------------------------------------------------
void BField::PutType(const BText& type)
//--------------------------------------------------------------------
{
    if(!(grammar_ = Gram(type)) && (struct_ = FindStruct(type)))
    {
	grammar_ = GraSet();
    }
}


//--------------------------------------------------------------------
BText BField::Dump() const
    
/*! Returns a text with all important information about this object
 */
//--------------------------------------------------------------------
{
    return(GetType()+" "+Name());
}


//--------------------------------------------------------------------
BInt FieldCmp(const void* any1, const void* any2)
    
/*! Compares two fields passed by its void pointers
 */
//--------------------------------------------------------------------
{
    BField* f1 = (BField*)any1;
    BField* f2 = (BField*)any2;
    return(StrCmp(f1->Name(), f2->Name()));
}


//--------------------------------------------------------------------
  static BInt StructStatusTableCmp(const void* v1, const void* v2)

/*! Compair two atom receiving the pointers.
 * \param v1 first atom to compare
 * \param v2 second atom to compare
 * \return returns an integer less than,
 */
//--------------------------------------------------------------------
{
  BStructStatus* sps1 = (BStructStatus*)v1;
  BStructStatus* sps2 = (BStructStatus*)v2;
  size_t s1 = (size_t)sps1->str_;
  size_t s2 = (size_t)sps2->str_;
  return((s1<s2)?-1:(s1>s2)?1:0);
}


//--------------------------------------------------------------------
  BStructStatus::BStructStatus(BStruct* str, bool alive)
//--------------------------------------------------------------------
  : str_(str), name_(), alive_(alive)
{
  if(str && alive) { name_ = str->Name(); }
} 

#if(defined(TRACE_LEVEL)&&(TRACE_LEVEL>=5))
#include <tol/tol_init.h>
# define TRACE_SHOW_STATUS_TABLE(fun,status, pos) \
  if((TRACE_LEVEL>=6)||TOLHasBeenInitialized()) \
  { \
    BText txt = "\nStatusTable =\n"; \
    for (BInt i = 0; i <BStruct::statusTable_->Size(); i++) \
    { \
      const BStructStatus& st = (*BStruct::statusTable_)[i]; \
      char aux[1024]; \
      sprintf(aux,"%s: %p, %u\n", \
             st.name_.String(), st.str_, size_t(st.str_)); \
      txt << aux; \
      TRACE_SHOW_TOP(fun,txt); \
    } \
    TRACE_SHOW_MEDIUM(fun,BText("found at ")+pos); \
  } 
#else
# define TRACE_SHOW_STATUS_TABLE(fun,status,pos)
#endif


#ifdef TRACE_LEVEL
# define TRACE_Def_StructMember(member,str,name) \
  char aux[1024]; \
  sprintf(aux,"x%p",str); \
  BText fun = BText("BStruct::")+member+"\t"+aux+"\t'"; \
  if(str) { fun+=name; } 
# define TRACE_SHOW_STRUCT_STEP(fun,status,pos,step) \
  {\
    BText aux1 = BText(step)+"\t"+pos+BText("\t"); \
    if(pos>=0) { aux1 += (int)(*statusTable_)[pos].alive_; } \
    else       { aux1 += "-1"; } \
    TRACE_SHOW_MEDIUM(fun,aux1); \
    TRACE_SHOW_STATUS_TABLE(fun,status,pos); \
  }
#else
# define TRACE_Def_StructMember(member,str,name)
# define TRACE_SHOW_STRUCT_STEP(fun,status,pos,step)
#endif


//--------------------------------------------------------------------
BStruct::BStruct(const BText& name, bool addToSymbolTable)
       
/*! Default BStruct constructor. .
 */
//--------------------------------------------------------------------
: BSyntaxObject(name), field_(NULL), function_(NULL)
{
  assert(name!="");
  TRACE_Def_StructMember("BStruct",this,name); 
  TRACE_MEMORY_SHOW(this,"BStruct::BStruct");
  //VBR: Temporary behaviour until non standard struct will be obsolete
#ifdef ALLOW_NON_STANDARD_STRUCT
  if(name[0]=='@')
  {
    BText newName(name.String()+1,name.Length()-1);
    if(!BParser::DefaultParser()->Scanner()->FindSymbol(newName))
    {
      BTypeToken* tok = new BTypeToken(newName,BTypeToken::BSTRUCT);
      BParser::DefaultParser()->Scanner()->AddSymbol(tok);
    }
  }
#ifdef CATCH_NON_STANDARD_STRUCT
  if(name[0]!='@') {
    _non_standard_struct_creating_without(name); }
#endif
#endif
  if (!statusTable_) 
  {
    statusTable_ = new BArray<BStructStatus>();
  }
  BStructStatus status(this,true);
  int pos = statusTable_->FindSorted(status, StructStatusTableCmp);
  TRACE_SHOW_STRUCT_STEP(fun,status,pos,"before");
  if(pos<0)
  {
    statusTable_->AddSorted(status,StructStatusTableCmp);
    #if(!defined(NDEBUG)||(defined TRACE_LEVEL))
    pos = statusTable_->FindSorted(status, StructStatusTableCmp);
    #endif
  }
  else
  {
    assert(!(*statusTable_)[pos].alive_);
    (*statusTable_)[pos].alive_ = true;
  }
  assert(pos>=0);
  TRACE_SHOW_STRUCT_STEP(fun,status,pos,"after");
  field_ = new BArray<BField>;
  if(addToSymbolTable)
  {
    BGrammar::AddObject(this);
  }
  if(!BParser::DefaultParser()->Scanner()->FindSymbol(name.String()))
  {
    BTypeToken* tok = new BTypeToken(name,BTypeToken::BSTRUCT);
    BParser::DefaultParser()->Scanner()->AddSymbol(tok);
  }
}


//--------------------------------------------------------------------
BStruct::~BStruct()
    
/*! PURPOSE: BStruct destructor.
 */
//--------------------------------------------------------------------
{
  TRACE_Def_StructMember("~BStruct",this,Name()); 
//Std(BText("\nDestroying struct ")+Name());
  BStructStatus status(this);
  int pos = statusTable_->FindSorted(status, StructStatusTableCmp);
  TRACE_SHOW_STRUCT_STEP(fun,status,pos,"before");
  assert(pos>=0);
  (*statusTable_)[pos].alive_ = false;
  TRACE_SHOW_STRUCT_STEP(fun,status,pos,"after");
  if(function_) 
  {
    function_->DecNRefs();
    BGrammar::DelObject(function_);
    BUserFunCode* uCode = function_->GetCode();
	  DESTROY(uCode);
  }
  BGrammar::DelObject(this);
  BParser::DefaultParser()->Scanner()->DelSymbol(Name().String());
	delete field_;
  field_ = NULL;
}


//--------------------------------------------------------------------
BStruct* BStruct::Alive(BStruct* str, bool showError)
//--------------------------------------------------------------------
{
  TRACE_Def_StructMember("Alive",str,str->Name()); 
  if(!str || !statusTable_) { return(NULL); }
  BStructStatus status(str);
  int pos = statusTable_->FindSorted(status, StructStatusTableCmp);
  TRACE_SHOW_STRUCT_STEP(fun,status,pos,"searching");
  if (pos<0) 
  {
    char aux[1024]; 
    sprintf(aux,"\n%p\n",str);
    Error(BText("No se encuentra ") + str->Name()+ "," + aux);
  }
  assert(pos>=0);
  if(!(*statusTable_)[pos].alive_)
  {
	  if(showError) 
    {
      Error(I2("Attempt to access local Struct ",
               "Intento de acceso al Struct local ")+
            (*statusTable_)[pos].name_);
    }
    return(NULL);
  }
  return(str); 
}


//--------------------------------------------------------------------
void BStruct::PutFunction(BNewStruct* fun)
    
/*! Puts the constructor function for this structure.
 */
//--------------------------------------------------------------------
{
  if(!function_)
  {
    function_=fun;
    function_->IncNRefs();
  }
}


//--------------------------------------------------------------------
void BStruct::PutField(const BText& def)

/*! Puts the fields for this structure.
 */
//--------------------------------------------------------------------
{
  if(!field_->HasValue())
  {
    BArray< BArray<BText> > cell;
    ReadAllCells(def, cell, ',', ':');
    field_->ReallocBuffer(cell.Size());
//  Std("\nBStruct::PutField\n");
    for(BInt n=0; n<cell.Size(); n++)
    {
//	  Std(Compact(cell[n][0]) + " | " + Compact(cell[n][1]) + "\n");
      (*field_)[n].PutName(Compact(cell[n][1]));
      (*field_)[n].PutType(Compact(cell[n][0]));
      assert((*field_)[n].Grammar());
      assert((*field_)[n].Name().HasName());
    }
    PutDesc();
  }
}


//--------------------------------------------------------------------
void BStruct::PutField(const BArray<BField>& field)

/*! Puts the fields for this structure.
 */
//--------------------------------------------------------------------
{
  if(!field_->HasValue())
  {
   *field_ = field;
    PutDesc();
  }
}


//--------------------------------------------------------------------
void BStruct::PutDesc()
    
/*! Puts the fields for this structure.
 */
//--------------------------------------------------------------------
{
    if(field_->HasValue())
    {
	BInt n;
	BText desc;
	for(n=0;n<Size();n++)
	{
	    desc += (*this)[n].Name() + ";";
	}
	desc = Name() +";"+ desc;
	PutDescription(desc);
    }
}


//--------------------------------------------------------------------
BGrammar* BStruct::Grammar() const

/*! All structures are already in Anything grammar.
 */
//--------------------------------------------------------------------
{
    return(GraAnything());
}

//--------------------------------------------------------------------
static BStruct* FindStructInNameBlock(
  const BText& name,
  BUserNameBlock*& unb )
//--------------------------------------------------------------------
{
  BStruct* str = NULL;
  BSyntaxObject* obj;
  int pos = name.Find("::",0);
  if(pos>=0)
  {
    BText prefix;
    BText suffix;
    prefix = name.SubString(0,pos-1);
    suffix  = name.SubString(pos+2,name.Length());
    if(!unb)
    {
      obj = GraNameBlock()->FindOperand(prefix,false);
      if(obj && obj->Grammar()==GraNameBlock())
      {
        unb = (BUserNameBlock*)obj;
      }
    }
    else
    {
      BNameBlock& nb = unb->Contens();
      obj = nb.Member(prefix);
      if(obj && obj->Grammar()==GraNameBlock())
      {
        unb = (BUserNameBlock*)obj;
      }
      else
      {
        unb = NULL;
      }
    }
    if(unb) 
    {
      str = FindStructInNameBlock(suffix, unb);         
    }
  }
  else if(unb)
  {
    BNameBlock& nb = unb->Contens();
    obj = nb.Member(name);
    if(obj && obj->Mode()==BSTRUCTMODE)
    {
      str = (BStruct*)obj;
    }
  }
  return(str); 
}


//--------------------------------------------------------------------
BStruct* FindStruct(const BText& name,
                    bool addArroba,
                    bool removeArroba)

/*! Searches an struct wich name is name and returns it.
 *  If itsn't exists returns NIL.
 */
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BUserNameBlock* unb = NULL;
  BStruct* bstr = FindStructInNameBlock(name, unb);
  if(!bstr && BNameBlock::Current())
  {
    result = BNameBlock::Current()->Member(name);
    if(result && (result->Mode()==BSTRUCTMODE))
    {
      bstr = (BStruct*)result;
    }
  }
  if(!bstr)
  {
    bstr = BStackManager::FindStruct(name);
  }
  if(!bstr)
  {
    const BNameBlock* cns = BNameBlock::Current();
    while(cns && !result)
    {
      result = cns->Member(name);
      if(result && (result->Mode()==BSTRUCTMODE))
      {
        bstr = (BStruct*)result;
      }
      if(!result)
      {
      //cns = cns->Father(); 
        cns = NULL;
      }
    }
  }
  if(!bstr) 
  {
    BObjClassify oc(GraAnything(),BSTRUCTMODE);
    bstr= (BStruct*)BGrammar::SymbolTable().Search(oc, name);
  }
  if(!bstr) 
  {
    result = BNameBlock::UsingMember(name);
    if(result && (result->Mode()==BSTRUCTMODE))
    {
      bstr = (BStruct*)result;
    }
  }
#ifdef ALLOW_NON_STANDARD_STRUCT
  if(addArroba && !bstr && (name[0]=='@'))
  {
    BText newName(name.String()+1,name.Length()-1);
    bstr = FindStruct(newName, false, false);
  }
  if(removeArroba && !bstr && (name[0]!='@'))
  {
    BText newName = BText("@")+name;
    bstr = FindStruct(newName, false, false);
#ifdef CATCH_NON_STANDARD_STRUCT
    if(bstr) {
      _non_standard_struct_calling_without(name); }
#endif
  }
#endif
  return(bstr);
}


//--------------------------------------------------------------------
BStruct* NewStruct  (const BText& name, const BText& def)
//--------------------------------------------------------------------
{
  BStruct* str = FindStruct(name);
  if(!str)
  {
    str = new BStruct(name);
    str->PutField(def);
    str->PutFunction(new BNewStruct(*str));
  }
  return(str);
}


//--------------------------------------------------------------------
BStruct* NewStructSymbol  (const BText& name, const BText& def)
//--------------------------------------------------------------------
{
  return(NewStruct(name,def));
}


//--------------------------------------------------------------------
BInt BStruct::FindField(const BText& name) const

/*! Searches a field of this struct wich name is name and returns
 *  its index.
 *  If itsn't exists returns a negative number.
 */
//--------------------------------------------------------------------
{
    BField field(name, NIL);
    BInt found = field_->Find(field, FieldCmp);
    if(found<0)
    { Error(name+I2(" is not a valid field for structure ",
                    " no es un campo v�lido para la estructura "+
            Name())); }
    return(found);
}


//--------------------------------------------------------------------
BText BStruct::Dump() const
    
/*! Returns a text with all important information about this object
 */
//--------------------------------------------------------------------
{
    BText dump = "Struct ";
    dump += FullName() + " {\n  ";
    if(field_->HasValue())
    {
	BInt n;
	BText arg = "";
	for(n=0; n<field_->Size(); n++)
	{
	    if(arg.HasName()) { arg += ", \n"; }
	    arg += (*field_)[n].Dump() ;
	}
	dump += arg;
    }
    dump += "\n};";
    return(dump);
}


//--------------------------------------------------------------------
BSyntaxObject* BStruct::Create(BList* lst, const BText& desc)
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NIL;
  if(function_ && lst)
  {
	  result = function_->Evaluator(lst);
    if(result) { result->PutDescription(desc); }
  }
  return(result);
}

//--------------------------------------------------------------------
  bool BStruct::Match(const BSet& set)
//--------------------------------------------------------------------
{
  bool ok = true;
  BSyntaxObject* obj;
  BField* fld;
  int n;
  if(set.Card()==field_->Size())
  {
    for(n=0; ok && (n<field_->Size()); n++)
    {
      obj = set[n+1];
      fld = &(*field_)[n];
      ok = obj && ((fld->Grammar()==GraAnything()) || 
                   (fld->Grammar()==obj->Grammar()));
      if(ok)
      {
        if(fld->Struct() && (fld->Struct()!=Set(obj).Struct()))
        {
          Error(I2("Struct field with structure ","Se esperaba campo de estructura ")+
              fld->Struct()->Name()+" "+Name()+"->"+fld->Name()+" "+
              I2("was expected when was found object of type",
                 " cuando se encontro el objeto de tipo")+" "+
              obj->Grammar()->Name()+" "+
              I2("without structure or with another one",
                 "sin estructura o con otra distinta")+" "+
              I2("at element number ", "en el elemento n�mero ")+(n+1));
          ok = false;
        }
        if(fld->Class())
        {
          BUserNameBlock* unb = (BUserNameBlock*)obj;
          if((fld->Class()!=unb->Contens().Class()))
          {
            Error(I2("Struct field instance of class","Se esperaba campo instancia de clase ")+
                fld->Class()->Name()+" "+Name()+"->"+fld->Name()+" "+
                I2("was expected when was found object of type",
                   " cuando se encontro el objeto de tipo")+" "+
                obj->Grammar()->Name()+" "+
                I2("instancing other class or no one",
                   "instancia de otra clase distinta o de ninguna")+" "+
                I2("at element number ", "en el el elemento n�mero ")+(n+1));
            ok = false;
          }
        }
      }
      else
      {
        Error(I2("Struct field ","Se esperaba campo de estructura ")+
              fld->Grammar()->Name()+" "+Name()+"->"+fld->Name()+" "+
              I2("was expected when was found object of type",
                 " cuando se encontro el objeto de tipo")+" "+
              obj->Grammar()->Name()+" "+
              I2("at element number ", "en el el elemento n�mero ")+(n+1));
      }
    }
  }
  else
  {
    Error(I2("Cannot apply structure", "No se puede aplicar la estructura ")+
          Name()+ I2(" having ", " que tiene ")+field_->Size()+
          I2(" fields to a set with ", " campos a un conjunto con ")+
          set.Card()+" "+I2("elements","elementos"));
    ok = false;
  }
  return(ok);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BStructFields);
DefExtOpr(1, BStructFields, "StructFields", 1, 1, "Text",
 "(Text className)",
 I2("Returns a set with a register of information about each field "
    " on a structure. The information is returned using structure\n",
    "Devuelve un conjunto con un registro de informaci�n acerca "
    "de cada uno de los campos de una estructura. La "
    "informaci�n se devuelve usando la estructura:\n")+
    "  @StructFieldInfo { \n"
    "    Text Type,\n"
    "    Text Name,\n"
    "    Text Description }\n",
 BOperClassify::System_);
//--------------------------------------------------------------------
void BStructFields::CalcContens()
//--------------------------------------------------------------------
{
  static BStruct* strStructFieldInfo_ = NULL;
  if(!strStructFieldInfo_)
  {
    strStructFieldInfo_ = FindStruct("@StructFieldInfo");
  }
  assert(strStructFieldInfo_);
  const BText& strName = Text(Arg(1));
  BStruct* str = FindStruct(strName);
  if(!str) { return; }
  int i;
  contens_.PrepareStore(str->Size());
  for(i=0; i<str->Size(); i++)
  {
    BField& fld = (*str)[i];
    BSet reg;
    reg.PrepareStore(3);
    reg.AddElement(new BContensText("",fld.GetType(),""));
    reg.AddElement(new BContensText("",fld.Name(),""));
    reg.AddElement(new BContensText("","",""));
    reg.PutStruct(strStructFieldInfo_);
    contens_.AddElement(new BSetTuple("", reg));
  }
}
