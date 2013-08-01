/* tol_bgrammar.h: BGrammar (gra) class and functions header
                   of GNU/TOL language.

   Copyright (C) 2003-2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BGRAMMAR_H
#define TOL_BGRAMMAR_H 1

#include <tol/tol_bsys.h>
#include <tol/tol_bgrastk.h>
#include <tol/tol_bmethod.h>
#include <tol/tol_bdat.h>
#include <list>

#ifdef __USE_EXT_LOADER__
#include <tol/tol_bext.h>
#  include <ltdl.h>
#endif

#ifndef __USE_DYNSCOPE__
#define __USE_DYNSCOPE__
#endif

#define MAXGRAMMAR 20

//these warnings disabling macros don't work from included headers
#ifdef _MSC_VER
//base class has not dll interface
#pragma warning(disable : 4275)
//needs to have dll-interface to be used by clients of class
#pragma warning(disable : 4251)
#endif

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BOperator;
class BDerived;
class BStruct;
class BExtension;
class BSymbolTable;

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------

#ifdef __USE_EXT_LOADER__
/* Extended Buil-in Function List for Tol Extensions */
typedef struct xtd_bif_list {
    struct xtd_bif_list *next;
    char* name;
    BExtension* tolext;
//    lt_dlhandle module;
#ifdef UNIX
    void* module;
#else // Windows
    HINSTANCE module;
#endif
} xtd_bif_list;
#endif /* __USE_EXT_LOADER__ */

typedef BSyntaxObject* (*BFinderByName)(const BText&);
typedef BSyntaxObject* (*BCasting)     (BSyntaxObject*);
typedef BSyntaxObject* (*BDbiFinder)   (const BText&, const BText&);
typedef BDat           (*BCompare)     (const BSyntaxObject*,
                                        const BSyntaxObject*);
typedef BSyntaxObject* (*BCreator)     (const BText&, BSyntaxObject*);

//--------------------------------------------------------------------
   enum BGrammarId
// TOL grammar identifiers
//--------------------------------------------------------------------
{
  BGI_NotFound  = -1,
  BGI_Anything  =  0,
  BGI_Real      =  1,
  BGI_Complex   =  2,
  BGI_Matrix    =  3,
  BGI_Date      =  4,
  BGI_TimeSet   =  5,
  BGI_Serie     =  6,
  BGI_Set       =  7,
  BGI_Polyn     =  8,
  BGI_Ratio     =  9,
  BGI_Text      = 10,
  BGI_Code      = 11,
  BGI_NameBlock = 12,
  BGI_VMatrix   = 13,
  BGI_LastId    = 14
};

typedef hash_map_by_name<BGrammarId>::dense_ BGidHashByName;
typedef hash_map_by_name<BGrammar* >::dense_ BGraHashByName;
typedef hash_map_by_int <BGrammar* >::dense_ BGraHashByGid;

//--------------------------------------------------------------------
class TOL_API BGrammar: public BSyntaxObject

/*! Basic class to derive any grammar. For every type of data we
 *  want to evaluate, exists one and only one grammar that is able to
 *  recognize the variables and constants of this type, and contains
 *  a list of all the operators that returns objects of this type.
 *  Thus we can identify a evaluable type T with its corresponding
 *  grammar G. To rebound this feature, we will declare it as a nested
 *  class of T, that's also subclass of BGrammar.
 *  This class will must to have an only instance, that could be a
 *  static member of T.
 */
//--------------------------------------------------------------------
{
 protected:
    static BSymbolTable*  symbolTable_;
 private:
    static bool stopFlag_; //!< Flag to stop Evaluation Process
    //! Hashed symbol table to supply fast variable searching
    static BInt           level_;
//  static BInt           stackSize_;
    //! Local list of instances for a evaluable object??
//  static BList*         stack_;

//------------------------------
#ifndef __USE_DYNSCOPE__

    struct stack_level_t
    {
      stack_level_t(BInt s, BInt l)
        : size(s), level(l)
        {
        }

      friend ostream & operator << (ostream & out, stack_level_t & stack)
        {
          out << "size = " << stack.size << " level = " << stack.level << endl;
          return out;
        }

      BInt size;
      BInt level;
    };
    typedef std::list<stack_level_t> level_list_t;
#  if defined(_MSC_VER)
#    pragma warning (disable : 4251)
#  endif
    static level_list_t stack_level;
#  if defined(_MSC_VER)
#    pragma warning (default : 4251)
#  endif
#endif
//------------------------------

    static BDat           unparseNodes_;
    static BDbiFinder     dbiFinder_;
    //! Last used grammar
    static BText          editor_;
    //! List of all valid grammars
    static BList*         instances_;
    static BGidHashByName gidByName_;
    static BGraHashByName graByName_;
    static BGraHashByGid  graByGid_;
    //! Number of grammar
    static BInt           numGrammar_;
           BGrammarId     gid_;
    //! If true this is an autocontens type like Real or Text
    //! Else is a virtual type like TimeSet
           bool           autoContens_;
    //! List of all valid methods for this grammar
           BList*         methods_;
           BArray<BAtom*> sortedMethods_;
    //! List of all valid operators for this grammar??
           BArray<BGrammar*> castings_;
           bool           hasCasting_[BGI_LastId];
           BCasting       castingFunction_;
           BFinderByName  findConstant_;
    //! Function to create
           BCreator       newRenamed_;
           BCreator       newFunArg_;
           BCompare       compare_;
           BSyntaxObject* defect_;
 protected:
    static BGrammar*      last_;

 private:
           int            precedence_; 

 public:
#ifdef __USE_EXT_LOADER__
    /* List for Tol Extensions */
    static xtd_bif_list *extensions_;
#endif /* __USE_EXT_LOADER */
    BGrammar* Serie;

//------------------------------
#ifndef __USE_DYNSCOPE__

    static void PushStackLevel(BInt l)
      {
        stack_level.push_front(stack_level_t(BGrammar::StackSize(), l));
      }

    static void PopStackLevel()
      {
        stack_level.pop_front();
      }

 friend ostream & operator <<(ostream & out, BGrammar::level_list_t & stack)
   {
     BGrammar::level_list_t::iterator iter;

     for (iter = stack.begin(); iter != stack.end(); iter++)
       out << *iter;
     return out;
   }
#endif /* __USE_DYNSCOPE_ */
//------------------------------

public:
    // Constructors & destructors: gra.cpp
    BGrammar( 
       int precedence,
       const BText&        name,
       const BText&        description,
       bool autoContens,
                   BCasting      cast,
             BFinderByName fC,
                BCreator      newRenamed,
                BCreator      newFunArg,
             BCompare      compare);

    ~BGrammar();

    // Access & Manipulation: inline
    static bool Initialize();
    static BSymbolTable& SymbolTable()
    {
      assert(symbolTable_);
      return(*symbolTable_);
    }

    static  BInt    Level    ();
    static  BInt    PutLevel (BInt level);
    static  void    IncLevel ();
    static  void    DecLevel ();
    static  BInt    StackSize()   { return(BStackManager::CurrentEntries()); }

  //! Removes a variable from hash table.
  static void DelObject(BSyntaxObject* var);
  //! If the variable has name then adds it to hash table if
  //! is global or else to stack. Any way puts current level
  //! to object
  static void AddObject(BSyntaxObject* obj);
  static void DestroyStackUntil(BInt stackPos, BSyntaxObject* except)
  {
    BStackManager::DestroyUntil(stackPos,except);
  }

    virtual BInt  Mode() const
  { return(BGRAMMARMODE); }

    static  BDat& DoUnparseNodes()
  { return(unparseNodes_); }

    static  int UnparseNodes()
  { return((int)unparseNodes_.Value()); }

    static  BText DumpAll();
    BText Dump() const;
    void  Clean();
    BGrammar* Grammar  () const;

    static  BList*    Instances()
  { return(instances_); }

    static BGrammar* Gra(BGrammarId gid) { return(graByGid_.find(gid)->second); }
    BGrammarId Gid() const { return(gid_); }
    int Precedence() const { return( precedence_); }
    bool IsAutoContens() const { return(autoContens_); }
    bool HasCasting(const BGrammar* gra) const { return(gra?hasCasting_[gra->Gid()]:false); }
    const BArray<BGrammar*> GetCastings() const { return(castings_); }
    virtual BList*    Methods  () const;
    virtual BList*    Operators() const;
    virtual BList*    Variables() const;
    BList*    GetMethods  () const;
    BList*    GetOperators() const;
    BList*    GetVariables() const;

    static void PutDbiFinder(BDbiFinder f)
  { dbiFinder_=f; }

    static BInt NumGrammar()
  { return(numGrammar_); }

    // Query & special access: graacc.cpp
    //! Returns the last used grammar
    static BGrammar* Last    ();
    //! Sets the last used grammar
    static void     PutLast(BGrammar* gra);
    //! Finds a grammar given a name
    static BGrammarId  Gid(const BGrammar* gra) 
    { 
      if(!gra) { return(BGI_NotFound); }
      else     { return(gra->Gid()); }
    }
    static  BGrammar*        FindByGid    (BGrammarId gid, bool sendError=true);
    static  BGrammar*        FindByName   (const BText& name, bool sendError=true);
    static  bool             Exists       (const BText& name) { return(FindByName(name,false)!=NULL); }
            BMethod*         FindMethod    (const BText&, bool sendError=true) const;
    static  BSyntaxObject*   FindLocal    (const BText& name);
            BSyntaxObject*   FindVariable (const BText&) const;
            BSyntaxObject*   UnknownVar   () const;
            BSyntaxObject*   FindConstant (const BText&) const;
            BOperator*       FindFunction (const BText&) const;
            BSyntaxObject*   FindUserFunc (const BText&) const;
    virtual BSyntaxObject*   FindOperand  (const BText&, bool mayBeConstant) const;
    virtual BOperator*       FindOperator (const BText&) const;
            BGrammar*         AddCasting    (BGrammar*);
            void             AddMethod    (BMethod*);
    virtual BSyntaxObject*   New          (const BText&, BSyntaxObject*) const;
    virtual BSyntaxObject*   NewFunArg    (const BText&, BSyntaxObject*) const;
    BSyntaxObject* Casting(BSyntaxObject*);

#ifdef __USE_EXT_LOADER__
    //-----------------------------------
    // Implemented in ext_loader.cpp
    static int Ext_Load(const BText& extName);
    BOperator* Ext_FindOperator(const BText&) const;
    static int Ext_Unload(const BText& extName);
    //-----------------------------------
#endif

    //-----------------------------------
    // Implementation : graimp.cpp

 public:
    BSyntaxObject* LeftEvaluateTree(const List* tre, bool forceType=true);
    BSyntaxObject* EvaluateTree    (const List* tre, BInt from_UF=0, bool forceType=true);
    BSyntaxObject* LeftEvaluateExpr(const BText& txt, bool forceType=true);
    BSyntaxObject* EvaluateExpr    (const BText& txt, bool forceType=true);
    BDat Compare(BSyntaxObject*,BSyntaxObject*)const;

    static void  CleanTreeCache(const List* tre, bool force);
    static void  PutEditor(const BText& ed) { editor_ = ed; }
    static const BText& Editor() { return(editor_); }

    static void Turn_StopFlag_On();
    static void Turn_StopFlag_Off();
    static bool StopFlag() { return BGrammar::stopFlag_; }

    static BBool End();

    void PutDefect(BSyntaxObject* def) { defect_ = def; }
    BSyntaxObject* Defect() const { return(defect_); }
};


//--------------------------------------------------------------------
class BAnyGrammar: public BGrammar

/*! Special grammar that allows to pass an argument to a function
 *  without to known its type. This is the Anything grammar.
 */
//--------------------------------------------------------------------
{
 private:
    static BAnyGrammar* anyGrammar_;

 public:
    // Constructors & destructors: gra.cpp
    BAnyGrammar()
  : BGrammar
  (
    999999999,
    "Anything",
    I2(
    "Grammar for undefined objects. It's usefull to pass "
    "parameters which type is not known at definition time. ",
    "Gramatica de objetos de tipo indefinido. Es util para pasar "
    "parametros cuyo tipo es desconocido a priori"
    ),
    false, NIL, NIL, NIL, NIL, NIL) {}
    ~BAnyGrammar() {}

    BSyntaxObject* FindOperand (const BText&, bool mayBeConstant)    const;
    BOperator*     FindOperator(const BText&)    const;
    BSyntaxObject* New      (const BText&, BSyntaxObject*) const;
    BSyntaxObject* NewFunArg(const BText&, BSyntaxObject*) const;

    static BBool InitGrammar();
    static BAnyGrammar* AnyGrammar() { return(anyGrammar_); }
};

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
TOL_API BList*         GraList       (const BText& textList);
TOL_API BBool          InitGrammars  ();
TOL_API BSyntaxObject* Evaluate      (const BText& expr);
TOL_API BList*         MultyEvaluate (const BText& block);

//--------------------------------------------------------------------
// inline functions
//--------------------------------------------------------------------

inline BGrammar* Gram(const BText& name) { return(BGrammar::FindByName(name,false)); }
inline void ApplyMethod(const BText& name,
      const BText& met,
            BList* lst,
      const BText& fileName = BSys::TempNam())
{ Gram(name)->FindMethod(met)->Apply(lst,fileName); }

//--------------------------------------------------------------------
// classes for Tol symbols table using hash_map
//--------------------------------------------------------------------

//--------------------------------------------------------------------
class TOL_API BObjClassify: public BUniKey
//Tol object classificator by type and mode used for hash_map key
//--------------------------------------------------------------------
{
 public:
  static BObjClassify null_;
  const BGrammar* grammar_;
  int             mode_;
  BObjClassify(const BSyntaxObject* obj=NULL)
  : grammar_ (NULL), 
    mode_    (BNOMODE) 
  {
    if(!obj) { return; }
    Set(obj->Grammar(),obj->Mode());
  }
  BObjClassify(const BGrammar* gra, int mode)
  {
    Set(gra,mode);
  }
  void Set(const BGrammar* gra, int mode)
  {
    grammar_ = gra; 
    mode_    = mode; 
    if(mode_==BBUILTINFUNMODE)
    {
      mode_=BUSERFUNMODE;
    }
  }
  //! Overloads type operator const BChar*
  size_t UniqueKey() const
  {
    size_t k = (BGrammar::Gid(grammar_)+1)*BLASTMODE+mode_;
    return(k);
  }
  static const BObjClassify& EmptyKey() 
  {
    static BObjClassify empty_(NULL,0);
    return(empty_);
  }
  static const BObjClassify& DeletedKey() 
  {
    static BObjClassify deleted_(NULL,1);
    return(deleted_);
  }
};

#if (__USE_HASH_MAP__==__HASH_MAP_MSVC__)

template<>
inline size_t stdext::hash_value(const BObjClassify& k)
{
  return (k.UniqueKey());
}
template<> 
struct less<BObjClassify>
{
  bool operator()(const BObjClassify & x, const BObjClassify & y) const 
  {
    if ( x.UniqueKey() < y.UniqueKey() ) { return(true); }
    return(false);
  }
};
#endif

//--------------------------------------------------------------------
//hash_map class for Tol objects classify
//--------------------------------------------------------------------
typedef 
  hash_map_by_unikey<BObjClassify,BObjByNameHash*>::dense_ 
  BObjByClassNameHash;




#endif // TOL_BGRAMMAR_H
