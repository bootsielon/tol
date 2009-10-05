/* bdb.cpp: Bayes Data Base functions.
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

#include <tol/tol_bdb.h>
#include <tol/tol_bout.h>
#include <tol/tol_boper.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_bstruct.h>

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("bdb.cpp");
BBool ForzeLinkerBDB() { return(BTRUE); }

static BInt alias_ = 0;

#if !defined(USE_DELAY_INIT)

//--------------------------------------------------------------------
static BStruct* BDBClassifyStruct_ = NewStructSymbol
//--------------------------------------------------------------------
(
    "@BDBClassifyStruct"
    ,
    "Set:Fields,"
    "Real:FirstPosition,"
    "Real:LastPosition"
);

#else

static void * cloneBDBClassifyStruct_()
{
  return NewStructSymbol ( "@BDBClassifyStruct"
			   ,
			   "Set:Fields,"
			   "Real:FirstPosition,"
			   "Real:LastPosition" );
}

static BStruct* BDBClassifyStruct_ = (BStruct*)(__delay_init((void**)(&BDBClassifyStruct_),
							     &cloneBDBClassifyStruct_));

#endif

#if !defined(USE_DELAY_INIT)

//--------------------------------------------------------------------
static BStruct* BDBField_ = NewStructSymbol
//--------------------------------------------------------------------
(
    "@BDBField"
    ,
    "Text:Name,"
    "Real:Width,"
    "Real:Pos"
);

#else

static void * cloneBDBField_()
{
  return NewStructSymbol( "@BDBField"
			  ,
			  "Text:Name,"
			  "Real:Width,"
			  "Real:Pos" );
}

static BStruct* BDBField_ = (BStruct*)(__delay_init((void**)(&BDBField_),
						    &cloneBDBField_));

#endif

#if !defined(USE_DELAY_INIT)

//--------------------------------------------------------------------
static BStruct* BDBStructure_ = NewStructSymbol
//--------------------------------------------------------------------
(
  "@BDB"
  ,
  "Text:Alias,"
  "Text:Path,"
  "Real:Fixed,"
  "Text:Sep,"
  "Text:Def,"
  "Text:Structure,"
  "Real:Size,"
  "Real:RegWidth,"
  "Real:RegNumber,"
  "Real:FieldNumber"
//"Anything:BDBFilePointer"
);

#else

static void * cloneBDBStructure_()
{
  return NewStructSymbol( "@BDB"
			  ,
			  "Text:Alias,"
			  "Text:Path,"
			  "Real:Fixed,"
			  "Text:Sep,"
			  "Text:Def,"
			  "Text:Structure,"
			  "Real:Size,"
			  "Real:RegWidth,"
			  "Real:RegNumber,"
			  "Real:FieldNumber"
			  /*"Anything:BDBFilePointer"*/ );
}

static BStruct* BDBStructure_ = (BStruct*)(__delay_init((void**)(&BDBStructure_),
							&cloneBDBStructure_, 5));

#endif

//--------------------------------------------------------------------
BDataBase::BDataBase(const BText& alias,
		     const BText& path,
		           BBool  fixed,
		           BChar  sep,
		     const BText& def,
		           BStruct* str)
    
/*! Default BDataBase constructor.
 */
//--------------------------------------------------------------------
    : BSyntaxObject(alias), struct_(str)
{
//Std(BText("\nCreating database ")+Name());
  if(!FindDataBase(alias))
  {
    if(fixed) { file_ = new BQFixedFile(path); }
    else      { file_ = new BQSepFile  (path); }

    if(file_->IsOpen())
    {
      if(sep) { file_->AddFields(sep, def); }
      else    { file_->AddFields(def);	    }
      BGrammar::AddObject(this);
    }
    else
    {
      Error(I2("Cannot open data base file ",
	       "No se puede abrir el fichero de base de datos ") + path);
    }
  }
  else
  {
    Error(I2("It's already open data base ",
	     "Ya exixte la base de datos ") + alias);
  }
}


//--------------------------------------------------------------------
BDataBase::~BDataBase()

/*! BDataBase destructor.
 */
//--------------------------------------------------------------------
{
//Std(BText("\nDestroying database ")+Name());
    BGrammar::DelObject(this);
}

//--------------------------------------------------------------------
static BGrammar* BDB_Grammar()
//--------------------------------------------------------------------
{
 return(GraText());
}

//--------------------------------------------------------------------
BGrammar* BDataBase::Grammar() const

/*! All BDB's are already in Anything grammar.
 */
//--------------------------------------------------------------------
{
    return(BDB_Grammar());
}


//--------------------------------------------------------------------
BDataBase* FindDataBase(const BText& name)

/*! Searches an struct wich name is name and returns it.
 *  If itsn't exists returns NIL.
 */
//--------------------------------------------------------------------
{
    BSyntaxObject* s;
    BObjClassify oc(BDB_Grammar(), BDATABASEMODE);
//Std(BText("\nFinding global database ")+name);
    s=BGrammar::SymbolTable().Search(oc,name);
    if(!s)
    {
//  Std(BText("\nFinding local database ")+name);
	s = BDB_Grammar()->FindLocal(name);
	if(s && (s->Mode()!=BDATABASEMODE)) { s = NIL; }
    }
    return((BDataBase*)s);
}


//--------------------------------------------------------------------
BSyntaxObject*  BDataBase::Field (BInt reg, BInt field, BGrammar* gra)
//--------------------------------------------------------------------
{
    BSyntaxObject* result = NIL;
    if(file_ && file_->IsOpen()) {
	if(gra==GraReal()) {
	    result = new BContensDat( file_->GetDatField(reg,field));
	} else if(gra==GraDate()) {
	    result = new BContensDate(file_->GetDateField(reg,field));
	} else if(gra==GraText()) {
	    result = new BContensText(file_->GetField(reg,field));
	} else {
	    result = gra->EvaluateExpr(file_->GetField(reg,field));
	}
    }
    return(result);
}


//--------------------------------------------------------------------
BSyntaxObject*  BDataBase::Field (BInt reg, BInt field)
//--------------------------------------------------------------------
{
    BSyntaxObject* result = NIL;
    if(file_ && file_->IsOpen())
    {
	BGrammar* gra = GraText();
	if(struct_) { gra = (*struct_)[field].Grammar(); }
	if(gra) { result = Field(reg,field,gra); }
    }
    return(result);
}


//--------------------------------------------------------------------
void BDataBase::Reg(BInt reg, BSet& set)
//--------------------------------------------------------------------
{
    BList* lst = NIL;
    BList* aux = NIL;
    for(BInt i=0; i<File()->FieldNumber(); i++)
    {
	BSyntaxObject* obj = Field(reg, i);
	if(obj) { LstFastAppend(lst,aux,obj); }
    }
    set.RobStruct(lst,Struct(),BSet::Structured);
}


//--------------------------------------------------------------------
static void TestDataBase(BDataBase* bdb, const BText& name)

/*! Searches an struct wich name is name and returns it.
 *  If itsn't exists returns NIL.
 */
//--------------------------------------------------------------------
{
    if(!bdb || !bdb->File())
    {
	Error(name + I2(" is not a data base BDB",
			" no es una base de datos BDB"));
    }
}


//--------------------------------------------------------------------
static BDataBase* GetDataBase(const BText& name)

/*! Searches an struct wich name is name and returns it.
 *	    If itsn't exists returns NIL.
 */
//--------------------------------------------------------------------
{
    BDataBase* bdb = (BDataBase*)FindDataBase(name);
    TestDataBase(bdb, name);
    return(bdb);
}


//--------------------------------------------------------------------
static BDataBase* GetDataBase(BSyntaxObject* uBDB)
    
/*! Searches an struct wich name is name and returns it.
 *  If itsn't exists returns NIL.
 */
//--------------------------------------------------------------------
{
    BDataBase* bdb = NIL;
    if(uBDB->Grammar()==GraSet())
    {
	BSet& set = Set(uBDB);
	if(set.Struct()==BDBStructure_)
	{
	    BText alias = Text(set.Field("Alias"));
	    bdb = GetDataBase(alias);
	}
    }
    TestDataBase(bdb, "");
    return(bdb);
}


//--------------------------------------------------------------------
class BTsrBDB : public BTsrPrimary

/*! Serie that has been readed from a Bayes Serie Index file.
 */
//--------------------------------------------------------------------
{
private:
  BDataBase*   bdb_;
  BInt	       firstPosition_;
  BInt	       lastPosition_;
  BInt	       magnitude_;
  BArray<BInt> dimensions_;
  BInt	       dateColumn_;
  BDat	       fillValue_;
public:
  // Constructors and destructors
  BTsrBDB(	BText	      name,
		BText	      desc,
		BDataBase*    bdb,
		BInt	      first,
		BInt	      last,
		BInt	      magnitude,
	  const BArray<BInt>  dimensions,
		BInt	      dateColumn,
		BUserTimeSet* dating,
		BDat	      fillValue = 0);
  BDat operator[](const BDate& dte) { return(GetDat(dte)); }
  BDat GetDat(const BDate& dte);

};

//--------------------------------------------------------------------
  BTsrBDB::BTsrBDB(	 BText	       name,
			 BText	       desc,
			 BDataBase*    bdb,
			 BInt	       first,
			 BInt	       last,
			 BInt	       magnitude,
		   const BArray<BInt>  dimensions,
			 BInt	       dateColumn,
			 BUserTimeSet* dating,
			 BDat	       fillValue)
//--------------------------------------------------------------------
: BTsrPrimary(name,desc),
  bdb_(bdb),
  firstPosition_ (first),
  lastPosition_	 (last),
  magnitude_	 (magnitude),
  dimensions_	 (dimensions),
  dateColumn_	 (dateColumn),
  fillValue_	 (fillValue)
{
  PutDating(dating);
  firstDate_ = bdb_->File()->GetDateField(firstPosition_,dateColumn_);
  lastDate_  = bdb_->File()->GetDateField(lastPosition_ ,dateColumn_);
};


//--------------------------------------------------------------------
BDat BTsrBDB::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BDat x;
    if(Data().HasValue()) { x = Data()[GetIndex(dte)]; }
    else
    {
	BInt i, k=firstPosition_;
	BHash hash; Dating()->GetHashBetween(hash, firstDate_,lastDate_);
	length_ = hash.Size();
	Realloc(length_);
	BDate d = bdb_->File()->GetDateField(k,dateColumn_);
	BReal  h = d.Hash();
	for(i=0; i<=length_; i++)
	{
	    if(h==hash[i])
	    {
		PutBufDat(i,bdb_->File()->GetDatField (i,magnitude_));
		if(d==dte) { x=Data()[i]; }
		k++;
		d = bdb_->File()->GetDateField(k,dateColumn_);
		h = d.Hash();
		
	    }
	    else
	    {
		PutBufDat(i,fillValue_);
	    }
	}
    }
    return(x);
}


//--------------------------------------------------------------------
// TOL Functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetBDBOpen);
DefExtOpr(1, BSetBDBOpen, "BDBOpen", 5, 5, "Text Real Text Text Text",
  I2("(Text filePath, Real fixed, Text sep, Text def, Text structure)",
     "(Text camino, Real fijo, Text separador, "
      "Text definicion, Text estructura)"),
  I2("",
     "Abre una nueva Base de Datos Bayes con el fichero de texto indicado. "
     "El argumento <fijo> indica si el fichero tiene un número fijo de "
     "caracteres para cada campo si es cierto, o bien los campos están "
     "identificados exclusivamente por el carácter separador indicado por "
     "el argumento <separador>. Nótese que un fichero de tipo fijo puede a "
     "su vez tener caracter separador. Si no lo tiene entonces hay que "
     "especificar el nombre y la longitud de los campos en el argumento "
     "<definicion>, por ejemplo definicion=\"Nombre:25,Edad:3\". "
     "El parámetro <estructura> es el nombre de una Struct de TOL y sirve "
     "para dotar de tipos TOL a los campos de los registros del fichero. "
     "Si no se quiere usar alguno de los tres últimos parametros se pasa "
     "la cadena vacía."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetBDBOpen::CalcContens()
//--------------------------------------------------------------------
{
  alias_ ++;
  BText	   alias = BText("_BDB_")+alias_+"_";

  BText	   path	 = Text(Arg(1));
  BBool	   fixed = BTRUE;
  BChar	   sep	 = ';';
  BText	   def	 = "";
  BStruct* str	 = NIL;

  if(Arg(2)) { fixed = (BBool)Real(Arg(2)); }
  if(Arg(3)) { sep   = Text(Arg(3)).Get(0); }
  if(Arg(4)) { def   = Text(Arg(4)); }
  if(Arg(5)) 
  { 
    BText& strName = Text(Arg(5));
    if(strName.HasName())
    {
      str = FindStruct(strName); 
    }
  }

  BDataBase* bdb = new BDataBase(alias, path, fixed, sep, def, str);
  
  if(!(bdb->File()) || !(bdb->File()->IsOpen()))
  {
    Error(I2("Cannot create data base BDB ",
	     "No se puede crear la base de datos BDB ") + alias);
    DESTROY(bdb);
  }
  else
  {
//  bdb->IncNRefs();
    if(str)
    {
      if(str->Size() != bdb->File()->FieldNumber())
      {
	Error(I2("BDB : Given structure has a different number of fields "
		 "than table.",
		 "BDB : La estructura dada no tiene el mismo número de "
		 "campos que la tabla."));
      }
      for(BInt n=0; n<str->Size(); n++)
      {
	bdb->File()->PutFieldName(n, (*str)[n].Name());
/*
	Std(BText("\nStruct Field ")+n+" : "+bdb->File()->FieldName(n)+
				       " : "+(*str)[n].Name());
*/
      }
    }

    BList* lst = NIL;
    BList* aux = NIL;

    LstFastAppend(lst, aux, new BContensText(alias));
    LstFastAppend(lst, aux, Arg(1));
    LstFastAppend(lst, aux, Arg(2));
    LstFastAppend(lst, aux, Arg(3));
    LstFastAppend(lst, aux, Arg(4));
    LstFastAppend(lst, aux, Arg(5));
    LstFastAppend(lst, aux, new BContensDat(bdb->File()->Size()));
    LstFastAppend(lst, aux, new BContensDat(bdb->File()->RegWidth()));
    LstFastAppend(lst, aux, new BContensDat(bdb->File()->RegNumber()));
    LstFastAppend(lst, aux, new BContensDat(bdb->File()->FieldNumber()));
//  LstFastAppend(lst, aux, bdb);
    args_ = LstAppend(args_,bdb);
    card_++;

    contens_.RobStruct(lst,BDBStructure_,BSet::Structured);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatBDBClose);
DefExtOpr(1, BDatBDBClose, "BDBClose", 1, 1, "@BDB",
  "(@BDB bdb)",
  I2("",""),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BDatBDBClose::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
    bdb->File()->Close();
//bdb->DecNRefs();
//DESTROY(bdb);
    contens_=BTRUE;
}



//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtBDBFieldName);
DefExtOpr(1, BTxtBDBFieldName, "BDBFieldName", 2, 2, "@BDB Real",
  I2("(@BDB bdb, Real pos)",
     "(@BDB bdb, Real pos)"),
  I2("",
     "Devuelve el nombre del campo del fichero <bdb> que ocupa la "
     "posición dada por <pos>. Si no existe tal campo devuelve la "
     "cadena vacía."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BTxtBDBFieldName::CalcContens()
//--------------------------------------------------------------------
{
  BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
  contens_ = bdb->File()->FieldName((BInt)Real(Arg(2))-1);
}


//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatBDBFieldPos);
DefExtOpr(1, BDatBDBFieldPos, "BDBFieldPos", 2, 2, "@BDB Text",
  I2("(@BDB bdb, Text name)",
     "(@BDB bdb, Text nombre)"),
  I2("",
     "Devuelve la posición del campo <nombre> en el fichero <bdb>."
     "Si no existe tal campo devuelve cero."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BDatBDBFieldPos::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
    contens_ = 1+bdb->File()->Field(Text(Arg(2)));
}




//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtBDBReg);
DefExtOpr(1, BTxtBDBReg, "BDBLine", 2, 2, "@BDB Real",
  "(@BDB bdb, Real num)",
  I2("",
     "Devuelve la línea número <num> del fichero <bdb>."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BTxtBDBReg::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
    contens_ = bdb->File()->GetRegister((BInt)Real(Arg(2))-1);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetBDBReg);
DefExtOpr(1, BSetBDBReg, "BDBReg", 2, 2, "@BDB Real",
  "(@BDB bdb, Real num)",
  I2("",
     "Devuelve un conjunto con los elementos del registro número <num> del "
     "fichero <bdb>. Si éste tiene estructura el conjunto resultado "
     "la adopta y de lo contrario se crea un conjunto de textos."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetBDBReg::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
    bdb->Reg((BInt)Real(Arg(2))-1, contens_);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetBDBTable);
DefExtOpr(1, BSetBDBTable, "BDBTable", 1, 1, "@BDB",
  "(@BDB bdb)",
  I2("",
     "Devuelve un conjunto con todos los registros del fichero <bdb> "
     "creados según se explica en la función BDBReg."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetBDBTable::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
    BInt reg = bdb->File()->RegNumber();
    contens_.PrepareStore(reg);
    for(BInt n=0; n<reg; n++)
    {
//  if(!(n%100)) { Std(BText("\nBDBTable register ")+n); }
	BUserSet* uSet = new BContensSet;
	bdb->Reg(n, uSet->Contens());
  contens_.AddElement(uSet);
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtBDBRead);
DefExtOpr(1, BTxtBDBRead, "BDBRead", 3, 3, "@BDB Real Real",
  I2("(@BDB bdb, Real from, Real length)",
     "(@BDB bdb, Real desde, Real longitud)"),
  I2("",
     "Lee del fichero <bdb> el texto de los <longitud> caracteres a partir "
     "de la posición <desde>"),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BTxtBDBRead::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
    
    BInt	 from	= (BInt)Real(Arg(2))-1;
    BInt	 len	= (BInt)Real(Arg(3));
    contens_	= bdb->File()->GetText(from, len);
}


//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtBDBRegField);
DefExtOpr(1, BTxtBDBRegField, "BDBCell", 3, 3, "@BDB Real Real",
  I2("(@BDB bdb, Real regNum, Real fieldNum)",
     "(@BDB bdb, Real numReg, Real numCampo)"),
  I2("",
     "Devuelve el texto del campo <numCampo> del registro <numReg> del "
     "fichero <bdb>."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BTxtBDBRegField::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
    
    BInt	 reg   = (BInt)Real(Arg(2))-1;
    BInt	 field = (BInt)Real(Arg(3))-1;
    contens_     = bdb->File()->GetField(reg,field);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetBDBSort);
DefExtOpr(1, BSetBDBSort, "BDBSort", 1, 2, "@BDB Set",
  I2("(@BDB bdb [, Set selField = Empty])",
     "(@BDB bdb [, Set selCampo = Empty])"),
  I2("",
     "Ordena los registros por los campos cuyos indices son los elementos "
     "del parámetro selCampo."
     "El conjunto vacío indica la selección de todos los campos."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetBDBSort::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }
    BSet	     field;
    if(Arg(2)) { field = Set(Arg(2)); }
    BInt	     M	    = field.Card();
    BArray<BInt> c,r;
    BInt j;
    if(M>0)
    {
	c.ReallocBuffer(M);
	for(j=0; j<M; j++) { c[j] = (BInt)Real(field[j+1])-1; }
    }
    else
    {
	M = bdb->File()->FieldNumber();
	c.ReallocBuffer(M);
	for(j=0; j<M; j++) { c[j] = j; }
    }
    bdb->File()->Sort(c,r);
    
    BList* lst = NIL;
    BList* aux = NIL;
    for(j=0; j<r.Size(); j++)
    {
	BContensDat* uDat = BContensDat::New(r[j]);
	LstFastAppend(lst,aux,uDat);
    }
    contens_.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BTxt, BTxtTemporary, BTxtBDBSortAndSave);
DefExtOpr(1, BTxtBDBSortAndSave, "BDBSortAndSave", 2, 3, "@BDB Text Set",
  I2("(@BDB bdb, Text path [, Set selField = Empty])",
     "(@BDB bdb, Text camino [, Set selCampo = Empty])"),
  I2("",
     "Ordena los registros por los campos seleccionados y guarda el "
     "resulatdo en el fichero indicado."
     "El conjunto vacío indica la selección de todos los campos."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BTxtBDBSortAndSave::CalcContens()
//--------------------------------------------------------------------
{
    BDataBase* bdb     = GetDataBase(Arg(1)); if(!bdb) { return; }
    BText	     path    = Text(Arg(2));
    ofstream   output(path.String());
    BSet	     field;
    if(Arg(3)) { field = Set(Arg(3)); }
    BInt	     M	     = field.Card();
    BInt	     N	     = bdb->File()->RegNumber();
    BArray<BInt> c,r;
    BInt i,j,n;
    if(M>0)
    {
	c.ReallocBuffer(M);
	for(j=0; j<M; j++) { c[j] = (BInt)Real(field[j+1])-1; }
    }
    else
    {
	M = bdb->File()->FieldNumber();
	c.ReallocBuffer(M);
	for(j=0; j<N; j++) { c[j] = j; }
    }
    bdb->File()->Sort(c,r);
    for(i=0; i<N; i++)
    {
	n = r[i];
	for(j=0; j<bdb->File()->FieldNumber(); j++)
	{
	    BText txt;
	    txt = bdb->File()->GetField(n,j);
	    output << txt << bdb->File()->Separator();
	}
	output << "\n";
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetBDBClassify);
DefExtOpr(1, BSetBDBClassify, "BDBClassify", 2, 2, "@BDB Set",
  I2("(@BDB bdb, Set selField)",
     "(@BDB bdb, Set selCampo)"),
  I2("",
     "Extrae todas las combinaciones sin repetición de los campos "
     "seleccionados que aparecen en el fichero BDB dado. "
     "Es necesario que el fichero BDB esté previamente ordenado por "
     "dichos campos."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetBDBClassify::CalcContens()
//--------------------------------------------------------------------
{
  BDataBase* bdb   = GetDataBase(Arg(1)); if(!bdb) { return; }
  BSet	     field = Set(Arg(2));
  BInt	     M	   = field.Card();
  BArray<BQClassifyStruct> classes;
  BArray<BInt> c;
  BInt i,j;

  c.ReallocBuffer(M);
  for(j=0; j<M; j++) { c[j] = (BInt)Real(field[j+1])-1; }
  bdb->File()->Classify(c,classes);

  BList* lst1 = NIL;
  BList* aux1 = NIL;
  for(i=0; i<classes.Size(); i++)
  {
    BSet fields;
    BList* lst2 = NIL;
    BList* aux2 = NIL;
    BList* lst3 = NIL;
    BList* aux3 = NIL;
    BText name;
    for(BInt j=0; j<classes[i].Fields.Size(); j++)
    {
      name += classes[i].Fields[j];
      LstFastAppend(lst3,aux3,BContensText::New(classes[i].Fields[j]));
    }
    fields.RobElement(lst3);
    BSet clas;
    LstFastAppend(lst2,aux2,BContensSet::New(fields));
    LstFastAppend(lst2,aux2,BContensDat::New(classes[i].FirstPosition));
    LstFastAppend(lst2,aux2,BContensDat::New(classes[i].LastPosition));
    clas.RobStruct(lst2,BDBClassifyStruct_,BSet::Structured);
    clas[1]->PutName(name);
    LstFastAppend(lst1,aux1,BContensSet::New("",clas, ""));
  }
  contens_.RobElement(lst1);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetBDBSeries);
DefExtOpr(1, BSetBDBSeries, "BDBSeries", 5, 7,
	    "@BDB Set Set Real TimeSet Set Real",
  I2("(@BDB bdb, Set dimensions, Set magnitudes, Real dateColumn, "
     "TimeSet dating [, Set classify=EMPTY, Real defaultValue=0])",
     "(@BDB bdb, Set dimensiones, Set magnitudes, Real dateColumn, "
     "TimeSet fechado [, Set clasificacion=EMPTY, Real valorPorDefecto=0])"),
  I2("",
     "Crea series temporales a partir de un BDT. "
     "Es necesario que el fichero BDB esté previamente ordenado por "
     "dichos campos."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetBDBSeries::CalcContens()
//--------------------------------------------------------------------
{
  BDataBase*	bdb	 = GetDataBase(Arg(1)); if(!bdb) { return; }
  BSet		dim	 = Set(Arg(2));
  BSet		mag	 = Set(Arg(3));
  BInt		dateCol	 = (BInt)Real(Arg(4))-1;
  BUserTimeSet* dating	 = Tms(Arg(5));
  BDat		fillVal	 = 0;
  BInt D=dim.Card();
  BInt M=mag.Card();
  BArray<BInt> d,m;
  BInt i,j;
  BSet classify;
  BArray<BQClassifyStruct> classes;

  d.ReallocBuffer(D);
  for(j=0; j<D; j++) { d[j] = (BInt)Real(dim[j+1])-1; }
  m.ReallocBuffer(M);
  for(j=0; j<M; j++) { m[j] = (BInt)Real(mag[j+1])-1; }
  if(Arg(6)) { classify = Set(Arg(6)); }
  if(Arg(7)) { fillVal	= Dat(Arg(7)); }
  if(classify.Card())
  {
    classes.ReallocBuffer(classify.Card());
    for(i=0;i<classes.Size();i++)
    {
      BSet theClass = Set(classify[i+1]);
      BSet fields   = Set(theClass[1]);
      classes[i].Fields.ReallocBuffer(fields.Card());
      for(j=1;j<fields.Card();j++)
      {
	classes[i].Fields[j] = Text(fields[j+1]);
      }
      classes[i].FirstPosition = (BInt)Real(theClass[2]);
      classes[i].LastPosition  = (BInt)Real(theClass[3]);
    }
  }
  else
  {
    bdb->File()->Classify(d,classes);
  }


  BList* lst = NIL;
  BList* aux = NIL;
  for(i=0; i<classes.Size(); i++)
  {
    BList* lst2 = NIL;
    BList* aux2 = NIL;
    BText name, desc;
    for(j=0; j<D; j++)
    {
      BText f = bdb->File()->GetField(classes[i].FirstPosition,d[j]);
      name += f;
      desc += bdb->File()->FieldName(d[i]) + ":" + f + ";";
    }
    for(j=0; j<M; j++)
    {
      BText mag = bdb->File()->FieldName(m[j]);
      BTsrBDB* ser = new BTsrBDB
      (
	name + mag,
	desc + BText("Magnitud:") + mag + ";",
	bdb,
	classes[i].FirstPosition,
	classes[i].LastPosition,
	m[j],
	d,
	dateCol,
	dating,
	fillVal
      );
//    Std(BText("\nBDBSeries: Ha siso creada la serie ") + ser->Name());
      LstFastAppend(lst2,aux2,ser);
    }
    BUserSet* series = NewSet("", desc, lst2, NIL, BSet::Generic);
    series->PutName(name);
    LstFastAppend(lst,aux,series);
  }
  contens_.RobElement(lst);
}

//--------------------------------------------------------------------
DeclareContensClass(BTxt, BTxtTemporary, BTxtBDBSaveAs);
DefExtOpr(1, BTxtBDBSaveAs, "BDBSaveAs", 4, 5, "@BDB Text Set Set Text",
  I2("(@BDB bdb, Text path, Set selReg, Set selField [, Text separator])",
     "(@BDB bdb, Text camino, Set selReg, Set selCam [, Text separador])"),
  I2("",
     "Guarda un conjunto de registros y campos seleccionados en el fichero "
     "indcado por el parámetro 'camino'. Los argumentos <selReg> y <selCam> "
     "son los conjuntos seleccionados de índices de registros y campos "
     "respectivamente. En ambos casos el conjunto vacío indica la selección "
     "de todos los índices."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BTxtBDBSaveAs::CalcContens()
//--------------------------------------------------------------------
{
  BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }

  BText	       path   = Text(Arg(2));
  BSet	       reg    = Set(Arg(3));
  BSet	       field  = Set(Arg(4));
  BChar	       sep    = bdb->File()->Separator();
  ofstream     output(path.String());
  BInt	       N      = reg.Card();
  BInt	       M      = field.Card();
  BArray<BInt> r,c;
  BInt	       i,j,n,m;
  BInt	       R = bdb->File()->RegNumber();
  BInt	       F = bdb->File()->FieldNumber();

  if(Arg(5)) { sep = Text(Arg(5)).Get(0); }
  if(!output.good())
  {
    Warning(I2("Cannot open for writing file ",
	       "No se puede abrir para escritura el fichero ") + path);
    return;
  }
  contens_ = path;
  if(N>0)
  {
    r.ReallocBuffer(N);
    for(i=0; i<N; i++) { r[i] = (BInt)Real(reg	[i+1])-1; }
  }
  else
  {
    N = R;
    r.ReallocBuffer(N);
    for(i=0; i<N; i++) { r[i] = i; }
  }
  if(M>0)
  {
    c.ReallocBuffer(M);
    for(j=0; j<M; j++) { c[j] = (BInt)Real(field[j+1])-1; }
  }
  else
  {
    M = F;
    c.ReallocBuffer(M);
    for(j=0; j<M; j++) { c[j] = j; }
  }

  for(i=0; i<N; i++)
  {
    n = r[i];
    if((n>=0)&&(n<R))
    {
      for(j=0; j<M; j++)
      {
	BText txt;
	m = c[j];
	if((m>=0)&&(m<F))
	{
	  txt = bdb->File()->GetField(n,m);
	  output << txt << sep;
	}
      }
      output << "\n";
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSext, BSetTemporary, BSetBDBExtract);
DefExtOpr(1, BSetBDBExtract, "BDBExtract", 3, 3, "@BDB Set Set",
  I2("(@BDB bdb, Set selReg, Set selField)",
     "(@BDB bdb, Set selReg, Set selCam)"),
  I2("",
     "Escribe en un fichero con nombre <camino> los registros y campos de "
     "<bdb> seleccionados. Los argumentos <selReg> y <selCam> son los "
     "conjuntos seleccionados de índices de registros y campos "
     "respectivamente. En ambos casos el conjunto vacío indica la selección "
     "de todos los índices."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetBDBExtract::CalcContens()
//--------------------------------------------------------------------
{
  BDataBase* bdb = GetDataBase(Arg(1)); if(!bdb) { return; }

  BSet	       reg    = Set(Arg(2));
  BSet	       field  = Set(Arg(3));
  BInt	       N      = reg.Card();
  BInt	       M      = field.Card();
  BArray<BInt> r,c;
  BInt	       i,j,n,m;
  BInt	       R = bdb->File()->RegNumber();
  BInt	       F = bdb->File()->FieldNumber();

  if(N>0)
  {
    r.ReallocBuffer(N);
    for(i=0; i<N; i++) { r[i] = (BInt)Real(reg	[i+1])-1; }
  }
  else
  {
    N = R;
    r.ReallocBuffer(N);
    for(i=0; i<N; i++) { r[i] = i; }
  }
  if(M>0)
  {
    c.ReallocBuffer(M);
    for(j=0; j<M; j++) { c[j] = (BInt)Real(field[j+1])-1; }
  }
  else
  {
    M = F;
    c.ReallocBuffer(M);
    for(j=0; j<M; j++) { c[j] = j; }
  }

  BList* lst1 = NIL;
  BList* aux1 = NIL;
  for(i=0; i<N; i++)
  {
    n = r[i];
    BList* lst2 = NIL;
    BList* aux2 = NIL;
    if((n>=0)&&(n<R))
    {
      for(j=0; j<M; j++)
      {
	BText txt;
	m = c[j];
	if((m>=0)&&(m<F))
	{
	  LstFastAppend(lst2,aux2,bdb->Field(n,m));
	}
      }
      LstFastAppend(lst1,aux1,NewSet("", "", lst2, NIL, BSet::Generic));
    }
  }
  contens_.RobElement(lst1);
}

/*
//--------------------------------------------------------------------
  DeclareContensClass(BTxt, BTxtTemporary, BTxtBDBSaveAs);
  DefExtOpr(1, BTxtBDBSaveAs, "BDBSaveSeries", 2, 4, "Text Set Text",
  I2("(Text path, Set dimSer "
     "[, Text separator=\";\", Text format=\"%12.4lg\" ])",
     "(Text camino, Set serDim "
     "[, Text separador=\";\", Text formato=\"%12.4lg\" ])"),
  I2("",
     "Salva un conjunto de series."),
  BOperClassify::BayesDataBase_);
  void BTxtBDBSaveAs::CalcContens()
//--------------------------------------------------------------------
{
  BText	       path   = Text(Arg(1));
  BSet	       dimSer = Set(Arg(2));
  BText	       format = "%12.4lg";
  BChar	       sep    = ';';
  ofstream     output(path.String());

  If(Arg(3)) { sep = Text(Arg(3))[1]; }
  If(Arg(4)) { format = Text(Arg(4)); }
  if(!output.good())
  {
    Warning(I2("Cannot open for writing file ",
	       "No se puede abrir para escritura el fichero ") + path);
    return;
  }
  contens_ = path;
}
*/


