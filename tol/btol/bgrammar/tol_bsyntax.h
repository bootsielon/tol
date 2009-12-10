/* tol_bsyntax.h: Defines BSyntaxObject class for
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
 
/*! \file bsyntax.h
//  \brief Defines BSyntaxObject and BSyntaxImage classes.
*/
/*! \class BSyntaxObject
//  \brief Defining a basic class to derive evaluable types.
//  \brief Funcionality has been implemented in syn.cpp
*/ 
/*! \class BSyntaxImage
//  \brief Defining images of existent objects.
//  \brief Funcionality has been implemented in syn.cpp
*/ 

#ifndef TOL_BSYNTAX_H
#define TOL_BSYNTAX_H 1

#include <tol/tol_batom.h>
#include <tol/tol_bout.h>
#include <tol/tol_hash_map.h>

//these warning disabling macros don't work from included headers
#ifdef _MSC_VER
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#endif

#ifdef _DEBUG
//#define TRACE_MEMORY
#else
//#define TRACE_MEMORY
#endif

#ifdef  TRACE_MEMORY
  #define TRACE_MEMORY_SHOW(OBJ, MESSAGE) OBJ->TraceMemory(MESSAGE)
#else
  #define TRACE_MEMORY_SHOW(NAME, MESSAGE)
#endif

//--------------------------------------------------------------------
// Forward References
//--------------------------------------------------------------------
class BGrammar;
class BSourcePath;
class BSetFromFile;
class List;
class BSet;
class BSyntaxObject;
class BNameBlock;

TOL_API BText AliveObjInfo();

BEGIN_DECLS
TOL_API const char* GetModeName(int mode);
TOL_API int TOLHasBeenInitialized();
TOL_API int TOLHasBeenEnded();
END_DECLS


//--------------------------------------------------------------------
// Enable this define for DEBUG all creating and deleting calls of 
// BSyntaxObject
//--------------------------------------------------------------------

#define BNOMODE	         0 
#define BIMAGEMODE       1 
#define BOBJECTMODE      2
#define BBUILTINFUNMODE  3
#define BGRAMMARMODE     4 
#define BFIELDMODE       5 
#define BSTRUCTMODE      6
#define BMETHODMODE      7 
#define BCLASSMODE       8 
#define BDATABASEMODE    9
#define BUSERFUNMODE    10
#define BSOURCEPATHMODE 11
#define BLASTMODE       12

//#define TRACE_OPTINFO

#define GetOpt(MEMBER) ((optInfo_)?optInfo_->MEMBER:BSyntaxObject::CBTextNullRef())


//--------------------------------------------------------------------
    class TOL_API BSyntaxObject: public BAtom

/*! Basic class to derive evaluable objects.
 */
//--------------------------------------------------------------------
{
protected:
  struct 
  {
    bool system_      :1; //!< Indicates if object is a system object
    bool isHashed_    :1; //!< Indicates if object has an entry in the language hash table
    bool released_    :1; //!< Indicates if object is released
    bool calculated_  :1; //!< Indicates if object is already calculated
    bool calculating_ :1; //!< Indicates if object is still calculating
    bool isDeadObj_   :1; //!< Indicates if object is a false or dead object
    bool isConst_     :1; //!< Indicates if object value is constant or variable
    bool isMethod_    :1; //!< Indicates if object is a method of a class
  } flags_;

public:
  //--------------------------------------------------------------------
    class TOL_API BSynObjOptInfo
  /*! Stores optional information about a BSyntaxObject 
    */
  //--------------------------------------------------------------------
  {
  public:
    #ifdef TRACE_OPTINFO
    static int    count_;
    static int    countName_;
    static int    countExpression_;
    static int    countDescription_;
    static double kBytes_;
    #endif
  public:
    BText             name_;        //!< Name of BSyntaxObject object
    BText             expression_;  //!< Expression of BSyntaxObject object
    BText             description_; //!< Description of BSyntaxObject object
    const BNameBlock* nameBlock_;   //!< Address of name space or NULL
    //! BUserObject constructor without name
    BSynObjOptInfo()
    : name_        (""),
      expression_  (""),
      description_ (""),
      nameBlock_   (NULL)
    {
      #ifdef TRACE_OPTINFO
      count_++;
      kBytes_ += double(sizeof(BSynObjOptInfo))/1024.0;
      #endif
    }
   ~BSynObjOptInfo() 
    {
      #ifdef TRACE_OPTINFO
      count_--;
      kBytes_ -= double(sizeof(BSynObjOptInfo))/1024.0;
      kBytes_ -= double(name_.Size()+description_.Size()+expression_.Size())/1024.0;
      #endif
    }
    DeclareClassNewDelete(BSynObjOptInfo);
  };

private:
  BSynObjOptInfo* optInfo_; 
  static BInt nSyntaxObject_; //!< BSyntaxObject objects counter
  short level_;   //!< Level of BSyntaxObject object (0 = global)
  const BSourcePath*  sourcePath_;  //!< Source path is the TOL-native file where object was created

  int ems_; //!< Time elapsed during object evaluation in miliseconds

  void InitSynObj(bool isDeadObj=false);

public:
  //! Returns number of BSyntaxObject objects
  static BInt NSyntaxObject() { return(nSyntaxObject_); }
    
  //! BSyntaxObject constructor
  BSyntaxObject(bool isDeadObj=false);
  BSyntaxObject(const BText& name, bool isDeadObj=false);
  BSyntaxObject(const BText& name, const BText& desc, bool isDeadObj=false);
  //! BSyntaxObject destructor
 ~BSyntaxObject();
  
  BInt IncNRefs();
  BInt DecNRefs();

  // Access & Manipulation: inline
  void           TraceMemory (const char* msg) const;
  const char*    ModeName    () const;
  const BText&   Identify    () const;
  const BText&   SourcePath  () const;
  bool           HasOptInfo  () const { return(OptInfoC()!=NULL); } 
  BBool          IsGlobal    () const { return(level_==0); }
  BInt	         Level	     () const { return(level_);    }
  bool		       System	     () const { return(flags_.system_); }
  bool		       IsHashed    () const { return(flags_.isHashed_); }
  bool           Released    () const { return(flags_.released_); }
  bool		       IsDeadObj   () const { return(flags_.isDeadObj_); }
  bool		       IsConstant  () const { return(flags_.isConst_); }
  bool		       IsMethod    () const { return(flags_.isMethod_); }
  int            Ems         () const { return(ems_); }

  const BSetFromFile*  Source () const;
  const BSetFromFile*  GlobalSource () const;


  void PutLevel	  (BInt level) { level_ = level; }
  void PutGlobal	()			     { level_ = 0; }
  void PutSystem	(bool sys)	 { flags_.system_ = sys; }
  void PutHashed  (bool isHas) { flags_.isHashed_ = isHas; }
  void PutReleased()           { flags_.released_ = true; }
  void PutConstant()           { flags_.isConst_  = true; }
  void PutMethod()             { flags_.isMethod_  = true; }
  void PutEms     (int ems)    { ems_ = ems; }

  virtual BInt                 Mode       () const { return(BNOMODE); }
  virtual BGrammar*	           Grammar	  () const { return(NIL); }
  virtual const BText&         TolPath    () const { return(BSyntaxObject::CBTextNullRef()); }
  virtual const BText&         LocalName  () const { return(BSyntaxObject::CBTextNullRef()); }
  virtual const BChar*         String     () const { return(GetOpt(name_).String()); }
  virtual const BText&         Name       () const { return(GetOpt(name_)); }
  virtual const BText&         Description() const;
  virtual const BText&         Expression () const { return(GetOpt(expression_)); }
  virtual const BNameBlock*    NameBlock  () const { return((optInfo_)?optInfo_->nameBlock_:NULL); }
  virtual BText                FullName   () const;
  virtual BText		             Dump	      () const { return(Identify()); }
  virtual BText		             Info	      () const { return(Dump()); }
          BText                LexInfo    () const;
  virtual BSyntaxObject*       CopyContens()       { return(this); }
  virtual void		             ReCalc	    ()       {}
  virtual void		             Do	        ()       {}

  virtual void PutLocalName (const BText&   loc)
  {
    Error(BText("Cannot put local name '")<<loc+"' to object" << Identify());
  }

  virtual void PutName      (const BChar*   name  ) 
  { 
#ifdef TRACE_OPTINFO
    int oldSize = (optInfo_)?optInfo_->name_.Size():0;
#endif
    if(name && name[0] && !optInfo_) 
    { 
      optInfo_ = new BSynObjOptInfo; 
    }
    if(optInfo_) 
    { 
      optInfo_->name_ = name; 
      #ifdef TRACE_OPTINFO
      BSynObjOptInfo::kBytes_ += double(optInfo_->name_.Size()-oldSize)/1024.0;
      BSynObjOptInfo::countName_ += (oldSize==0);
      #endif
    }
  }
  virtual void PutName        (const BText&   name  )
  { 
#ifdef TRACE_OPTINFO
    int oldSize = (optInfo_)?optInfo_->name_.Size():0;
#endif
    if(name.HasName() && !optInfo_) 
    { 
      optInfo_ = new BSynObjOptInfo; 
    }
    if(optInfo_) 
    { 
      optInfo_->name_ = name; 
      #ifdef TRACE_OPTINFO
      BSynObjOptInfo::kBytes_ += double(optInfo_->name_.Size()-oldSize)/1024.0;
      BSynObjOptInfo::countName_ += (oldSize==0);
      #endif
    }
  }
  virtual void PutDescription (const BText&   desc  )
  { 
#ifdef TRACE_OPTINFO
    int oldSize = (optInfo_)?optInfo_->description_.Size():0;
#endif
    if(desc.HasName() && !optInfo_) 
    { 
      optInfo_ = new BSynObjOptInfo; 
    }
    if(optInfo_) 
    { 
      optInfo_->description_ = desc;
      #ifdef TRACE_OPTINFO
      BSynObjOptInfo::kBytes_ += double(optInfo_->description_.Size()-oldSize)/1024.0;
      BSynObjOptInfo::countDescription_ += (oldSize==0);
      #endif
    }
  }
  virtual void PutExpression (const BText&   expr  )
  { 
#ifdef TRACE_OPTINFO
    int oldSize = (optInfo_)?optInfo_->expression_.Size():0;
#endif
    if(expr.HasName() && !optInfo_) 
    { 
      optInfo_ = new BSynObjOptInfo; 
    }
    if(optInfo_) 
    { 
      optInfo_->expression_ = expr;
      #ifdef TRACE_OPTINFO
      BSynObjOptInfo::kBytes_ += double(optInfo_->expression_.Size()-oldSize)/1024.0;
      BSynObjOptInfo::countExpression_ += (oldSize==0);
      #endif
    }
  }

  virtual void PutNameBlock(const BNameBlock* nameBlock);

  #ifdef TRACE_OPTINFO
  static int    OptInfoCount            () { return(BSynObjOptInfo::count_); }
  static int    OptInfoCountName        () { return(BSynObjOptInfo::countName_); }
  static int    OptInfoCountExpression  () { return(BSynObjOptInfo::countExpression_); }
  static int    OptInfoCountDescription () { return(BSynObjOptInfo::countDescription_); }
  static double OptInfoKBytes           () { return(BSynObjOptInfo::kBytes_); }
  #endif

  static BSyntaxObject* GetObjectFromAddress(const BText& address);
  BText GetAddressFromObject() const;

#ifndef FOR_BTOLLNK
    virtual BBool IsFile() const
	{ return(BFALSE); }
#endif

protected:
  virtual       BSynObjOptInfo* OptInfo ()       { return(optInfo_); }
  virtual const BSynObjOptInfo* OptInfoC() const { return(optInfo_); }

  static const BText& CBTextNullRef();
private:
  BINT64 oisOffset_;
public:
  const BSourcePath*  GetSourcePath   () const;
  BINT64              OisOffset       () const { return(oisOffset_); }
  virtual const List* GetOisTree      () const { return(NULL); }
  void                PutOisOffset    (BINT64 offset)        { oisOffset_=offset; }
  virtual void        PutOisTree      (const List* tree)      {}
};


typedef hash_map_by_name<BSyntaxObject*>::dense_ BObjByNameHash;

#endif // BSYNTAXOBJECT

