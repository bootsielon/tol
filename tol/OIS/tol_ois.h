/* tol_ois.h: Object's Indexed Serialization.
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

#ifndef TOL_OIS_H
#define TOL_OIS_H 1


#include <tol/tol_btimer.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_oisstream.h>

//--------------------------------------------------------------------
// defines
//--------------------------------------------------------------------

//Each format change is specified by a version tag XX.YY
//When the first part doesn't change new software version is able to
//read old OIS files. This will be called a soft change.
//When the first part changes new software version is not able to
//read old OIS files. This will be called a hard change. Then a
//traslation utility will be available.
#define OIS_VERSION "02.10"

#define OIS_Load_Mode_Header          0 
#define OIS_Load_Mode_Data            1
#define OIS_Load_Mode_ShowHierarchy   2

//Maximum default size of XML strings
#define OIS_MaxXmlStrLen 1024*16

//Minimum size of strings to be compressed
#define OIS_MinStreamSizeToCompress 32

#if defined(_MSC_VER) && (_MSC_VER<1400)
#  define _LLD64_ "I64d"
#else
#  define _LLD64_ "lld"
#endif

//Enable these macroes to generate log files about OIS image contens
//It can be usefull to debug some errors
//#define TRACE_OIS_FILE
//#define TRACE_OIS_TOKEN

//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define Ensure(doit) if(!(doit)) { return(false); }


//--------------------------------------------------------------------
  class BOis
/*
  OIS: Objects Indexed Serilaization
  ==================================

  OIS is a tool for save and load TOL objects in a binary native 
  format independently of machine, operative system and compiler.

  Objects: means both TOL objects, functions and structures of 
    any type.

  Indexed: means that objects can be partially searched and 
    loaded by demand. (Delayed loading IS STILL NOT IMPLEMENTED)

  Serialization: means that all information about the object is 
    explictly specified: name, description, expression and contens.
     -Functions are saved in a binary tree so they will not be parsed.
     -TimeSet's and Serie's contens both it's virtual tree evaluation
    method and a vector with data between bounds dates. For non
    bounded ones default limit dates will be used.

  TOL user can save any set of TOL objects and reload in other TOL
  session and then use it in the same way that any just declared 
  object.

  You can create TOL libraries and modules of global functions and 
  objects or create data stores of pre-calculated objects.

  An OIS is contained in a plain directory with an XML header that
  could be used by external documentation managers and a set of
  binary files containg micro-compressed information about TOL objects
  and its hierarchy relations.

  Micro-compression means that each information item is compressed
  independently of the rest allowing partial and random reading 
  instead of full sequential reading of macro-compression. This
  feature has been implemented with standard multiplattform 
  open-source library BZIP2 (www.bzip.org)
*/
//--------------------------------------------------------------------
{
public:
  //------------------------------------------------------------------
     class BXMLParameters
  // Contains information about XML header parameters
  //------------------------------------------------------------------
  {
   public:
    int maxXmlStrLen_;  //Maximum size for XML strings
    BXMLParameters() : maxXmlStrLen_(0) {}
    ~BXMLParameters() {}
  } xmlParam_;

  //------------------------------------------------------------------
     class BControl
  // Contains information about OIS building circumstances in order
  // to control loading process
  //------------------------------------------------------------------
  {
  public:
  //Information about OIS engine 
    class OisEngine
    {
     public: 
      BText oisVersion_;         //OIS_VERSION at building time
      BDate release_;            //Building date and time
      BText curDir_;             //Current directory at building time
      OisEngine() : oisVersion_(), release_(), curDir_() {}
     ~OisEngine() {}
    } oisEngine_;

  //Information about host computer and environment where OIS was built
    class Machine
    {
     public: 
      BText hostName_;           //IP or name 
      bool  isLittleEndian_;     //Big or little endian flag
      BText osName_;             //Operative System 
      Machine() : hostName_(), isLittleEndian_(false), osName_() {}
     ~Machine() {}
    } machine_;

  //Information about TOL engine 
    class TolEngine
    {
     public: 
      BText cppCompiler_;        //C++ compiler
      BText tolVersion_;         //TOL version
      BText tolRelease_;         //TOL release date and time
      BText tolUserLang_;        //TOL user language
      TolEngine() 
      : cppCompiler_(), 
        tolVersion_(), 
        tolRelease_(), 
        tolUserLang_() 
      {}
      ~TolEngine() {}
    } tolEngine_;

  //Information about type sizes in bytes that are used in binary mode
  //and could depend on compiler options, operative system or machine
    class TypeSizes
    {
     public:     
      short sizeof_char_;        
      short sizeof_short_;       
      short sizeof_int_;
      short sizeof_BINT64_;
      short sizeof_double_;      
      short sizeof_BDat_;
      short sizeof_BCoefDeg_;
      short sizeof_BGrammarId_;

      TypeSizes() 
      : sizeof_char_       (sizeof(char)),
        sizeof_short_      (sizeof(short)),
        sizeof_int_        (sizeof(int)),
        sizeof_BINT64_     (sizeof(BINT64)),
        sizeof_double_     (sizeof(double)),
        sizeof_BDat_       (sizeof(BDat)),
        sizeof_BCoefDeg_   (sizeof(BCoefDeg)),
        sizeof_BGrammarId_ (sizeof(BGrammarId))
      {}
     ~TypeSizes() {}
    } typeSizes_;

    BControl()
    : oisEngine_(),
      machine_(),
      tolEngine_(),
      typeSizes_()
    {}
    ~BControl() {}
  } control_;


  //------------------------------------------------------------------
     class BAlias
  // Alias for replacing a value with {$alias}
  //------------------------------------------------------------------
  {
  private:
    bool CodeDecode(BText& txt, bool justIfBeginWith, const BText& a, const BText& b)
    {
      bool doReplace = true;
      if(justIfBeginWith)
      {
        doReplace = txt.BeginWith(a)==BTRUE;
        if(doReplace)
        {
          txt = b+txt.SubString(a.Length(),txt.Length());
        }
      }
      else
      {
        txt.Replace(a,b);
      }
      return(doReplace);
    }
  public:
    BText alias_;
    BText value_;

    BText Coded() const 
    { 
      return(BText("{$")+alias_+"}");
    }

    bool Code(BText& txt, bool justIfBeginWith)
    {
      return(CodeDecode(txt,justIfBeginWith,value_,Coded()));
    }

    bool Decode(BText& txt, bool justIfBeginWith)
    {
      return(CodeDecode(txt,justIfBeginWith,Coded(),value_));
    }
  };

  
//Types of micro-compressor for internal serialization
  enum BSerialEngine
  {
    BSE_DEFAULT_  = -1,
    BSE_NONE_     =  0,
    BSE_BZIP2_    =  1
  };
//Types of macro-compressor for external file manager
  enum BArchiveEngine
  {
    BAE_DEFAULT_  = -1,
    BAE_NONE_     =  0,
    BAE_ZIPARC_   =  1
  };

  //------------------------------------------------------------------
     class BOptions
  // Contains optional information about OIS configuration
  //------------------------------------------------------------------
  {
   public:
  //Information about OIS engine 
    class OisConfig
    {
     public:
      bool  buildHierarchy_;     //If false no hierarchy index will be built 
      OisConfig() : buildHierarchy_(false) {}
     ~OisConfig() {}
    } oisConfig_;

  //Information about Compression engine 
    class Compression
    {
     public:
    //Serial data must be compressed at low level in independent  
    //searchable little blocks to allow indexing
      class MicroSerialization
      {
       public:
        BSerialEngine engine_;       //Engine for compressing-decompressing 
        int           level_;        //Compression level (1..9) In BZIP is the argument blockSize100k
        int           minSizeCmprs_; //Serial blocks with length shorter than this will not be compressed
        MicroSerialization() 
        : engine_(BSE_NONE_), 
          level_(0), 
          minSizeCmprs_(0) {}
       ~MicroSerialization() {}
      } serialization_;
    //Files and directories of an OIS collection and source code files could be packed 
    //in just one ZIP-like file
      class MacroFileManager
      {
       public:
        BArchiveEngine engine_; //Engine for compressing-decompressing 
        int            level_;  //Compression level (1..9) In BZIP is the argument blockSize100k

        MacroFileManager() : engine_(BAE_NONE_), level_(0) {}
       ~MacroFileManager() {}
      } fileManager_;

      Compression() : serialization_(), fileManager_() {}
     ~Compression() {}
    } compressor_;

    //You can specify any number of alias for absolute paths of your 
    //TOL source root directories. Then each path_k at the begining of 
    //any TOL source file path by will be replaced by its correspondent
    //alias codified as {&alias_k}\n"
    BArray<BAlias> tolSourceSearchPaths_;

    BOptions()
    : oisConfig_(),
      compressor_(),
      tolSourceSearchPaths_()
    {}
    ~BOptions() {}
  } options_;

  //------------------------------------------------------------------
     class BAddress
  // Contains information about OIS address into the OIS repository
  // Address path will be built as
  //   concept/version/node
  // You can use "/" in all items to have more complex directory 
  // structure
  //------------------------------------------------------------------
  {
   public:
    BText concept_;  //Usually this will be a representative general 
                     //identifier of the contained information type
                     //and we will refer it usually as a OIS package
                     //for TOL libraries or Sessions for data stores
                     //of massive estimations or other calculations.
    BText version_;  //Version control identifer to allow evolution
                     //of the OIS package/session
    BText node_;     //The node is the internal path of the module
                     //into the OIS package or the atomic unity in
                     //data stores

    BAddress() : concept_(), version_(), node_() {}
   ~BAddress() {}
  } address_;

  //------------------------------------------------------------------
     class BDoc
  // Contains documental information
  //------------------------------------------------------------------
  {
   public:
    BText name_;         //The identifier of the OIS
    BText category_;     //Conceptual category 
    BText subject_;      //Brief description in a line
    BText keys_;         //Keywords for searching
    BText abstract_;     //Full description
    BText authors_;      //e-mail or name of authors
    BText distribution_; //Publication domain: confidential, client, public
    BText repository_;   //Repository or root path 
    BDoc() 
    : name_(),
      category_(),
      subject_(),
      keys_(),
      abstract_(),
      authors_(),
      distribution_(),
      repository_()
    {}
   ~BDoc() {}
  } doc_;

  //------------------------------------------------------------------
     class BFileStat
  // Statistics about OIS binary files
  //------------------------------------------------------------------
  { 
   public:
    int    entries_;
    BINT64 bytes_;
    BFileStat() : entries_(0), bytes_() {}
   ~BFileStat() {}
  };

  //------------------------------------------------------------------
     class BStat
  // Statistics about OIS directory
  //------------------------------------------------------------------
  { 
   public:
    double buildingTime_;
    BArray<BFileStat> fileStat_;
    BStat() : buildingTime_(0), fileStat_() {}
   ~BStat() {}
  } stat_;


  //------------------------------------------------------------------
     enum BFileId
  // OIS binary file identifiers
  //------------------------------------------------------------------
  {
    BFI_header      =  0,
    BFI_tolref      =  1,
    BFI_oisref      =  2,
    BFI_object      =  3,
    BFI_set         =  4,
    BFI_timeset     =  5,
    BFI_serie       =  6,
    BFI_matrix      =  7,
    BFI_polyn       =  8,
    BFI_ratio       =  9,
    BFI_code        = 10,
    BFI_hrchyDetail = 11,
    BFI_hrchyOffset = 12,
    BFI_hrchyOrder  = 13,
    BFI_export      = 14,
    BFI_maxnum      = 15
  };

  //------------------------------------------------------------------
     class BCoefDeg
  // Monomial structure
  //------------------------------------------------------------------
  {
   public:
    int  degree_;
    BDat coefficient_;
    BCoefDeg() : degree_(0), coefficient_() {}
   ~BCoefDeg() {}
  };

  //------------------------------------------------------------------
     class BDatOld
  // Old BDat with bool known_
  //------------------------------------------------------------------
  {
   public:
    BBool  known_;  //!< Indicates if the number is known
    BReal  value_;  //!< Store a number value
  };

  //------------------------------------------------------------------
     class BCoefDegOld
  // Old BDat with bool known_
  //------------------------------------------------------------------
  {
   public:
    int  degree_;
    BDatOld coefficient_;
    BCoefDegOld() : degree_(0), coefficient_() {}
   ~BCoefDegOld() {}
  };

  //------------------------------------------------------------------
     class BIndexElement
  // Structure of the index of a element in a set
  //------------------------------------------------------------------
  {
   public:
    BGrammarId    gid_;
    unsigned char mode_;
    BINT64        objectOffset_;
    int           hrchyEntry_;
    BIndexElement() 
    : gid_         (BGI_NotFound), 
      mode_        (0), 
      objectOffset_(0),
      hrchyEntry_  (0)
    {}
   ~BIndexElement() {}
  };

  //------------------------------------------------------------------
     class BGlossaryElement
  // Structure of the glossary of a named element in a set
  //------------------------------------------------------------------
  {
   public:
    BText name_;
    int   position_;
    BGlossaryElement() : name_(), position_(0) {}
   ~BGlossaryElement() {}
  };

  //------------------------------------------------------------------
     class BHierarchyDetail
  // Structure of deatil information about hierachy nodes
  //------------------------------------------------------------------
  {
   public:
    BGrammarId    gid_;
    unsigned char mode_;
    BINT64        objectOffset_;
    int           hrchyEntry_;
    BText         name_;
    BHierarchyDetail() 
    : gid_         (BGI_NotFound), 
      mode_        (0),
      objectOffset_(0),
      hrchyEntry_  (0),
      name_()
    {}
   ~BHierarchyDetail() {}
  };

  //------------------------------------------------------------------
     class BHierarchyIndex
  // Structure of hrchyOffset_ and hrchyOrder_ files
  //------------------------------------------------------------------
  {
   public:
    int    hrchyOrder_;
    int    hrchyEntry_;
    BINT64 hrchyOffset_;
    BHierarchyIndex() 
    : hrchyOrder_(0),
      hrchyEntry_(0),
      hrchyOffset_(0)
    {}
   ~BHierarchyIndex() {}
  };


protected:

#ifdef TRACE_OIS_FILE
  static FILE* logWrite_;
  static FILE* logRead_;
#endif

#ifdef TRACE_OIS_TOKEN
  static FILE* tokWrite_;
  static FILE* tokRead_;
#endif

//Available modes for Ois.Load function
  static BArray<BSystemDat*> loadModeOptions_;

//Current OIS_VERSION in a BText
  static BText oisCurrentVersion_;

//Current OIS default root path
  static BText oisDefRoot_;

//Default OIS archive method
  static BText oisDefArchive_;

//Default date format
  static BDateFormat dateFmt_;

//Current endian flag
  static bool isLittleEndian_;

  static BStruct*  addressStr_;
  static BStruct*  docStr_;
  static BUserSet* defDoc_;

//Enable/Disable serialization engine
  bool   enable_BSE_;

//True if the image has been closed
  bool   closed_;

//Files
  BText  connection_;
  BStreamHandler* streamHandler_;
  BStream* header_;
  BStream* tolref_;
  BStream* oisref_;
  BStream* object_;
  BStream* set_;
  BStream* serie_;
  BStream* timeset_;
  BStream* matrix_;
  BStream* polyn_;
  BStream* ratio_;
  BStream* code_;
  BStream* hrchyDetail_;
  BStream* hrchyOffset_;
  BStream* hrchyOrder_;
  BStream* export_;

  BArray<BStream*>        source_;
  BArray<BStream*>        allFiles_;
  BArray<BHierarchyIndex> hierarchyIndex_;
  BArray<int>             hierarchyOrder_;

//Time counter
  BTimer elapsed_;
  
//Functions
  void  InitBuild ();
  void  SetAllFiles();
  void  SetRoot   (const BText&  root);
  void  SetAddress(const BSet* address);
  bool  Error  (const BText& msg) const;
  bool  Warning(const BText& msg) const;
  BSyntaxObject* NullError(const BText& msg) const;

public:
  static int                  def_CLv_;
  static BSerialEngine def_BSE_;
  static BArchiveEngine       def_BAE_;

//Parameters
  
  BOis();
  virtual ~BOis();

  static BText SerialEngine(BSerialEngine e=BSE_DEFAULT_);
  static BText ArchiveEngine(BArchiveEngine a=BAE_DEFAULT_);
  static void PutDefaultSerialEngine(BOis::BSerialEngine e);
  static void PutDefaultArchiveEngine(BOis::BArchiveEngine a);
  static BText ArchiveExtension(BOis::BArchiveEngine a=BAE_DEFAULT_);
  static BSerialEngine SerialEngine(const BText& txt);
  static BArchiveEngine       ArchiveEngine      (const BText& txt);
  static const BText& CurrentVersion() { return(oisCurrentVersion_); }
  static BText AutoPath(const BText& path);
  static BText PlainPath    (const BText& path);
  static BText GetModulePath(const BText& tolFile);
  static bool  RemoveModule (const BText& tolFile);
  static BDate GetModuleTime(const BText& tolFile);

  static bool Initialize    ();
  static void SwapEndian    (void* Addr, const int Nb);
  static bool IsLittleEndian(void) { return(isLittleEndian_); }

  void  SetConnection(const BText& connection);
  void SetPath(const BText& connection, 
               const BSet*  address);

  void SetModulePath(const BText& tolFile);

  BText SubPath () const;
  BText Path    () const;
  bool  Exists  () const { return(streamHandler_!=NULL); }
  void GetTolSourceSearchPaths(const BText& fun, const BSet& options, int opt);
  bool RemoveImage ();
  virtual bool Close();
};

//--------------------------------------------------------------------
// inline functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  inline int CompareAddress(const void* v1, const void* v2)

/*! Compair two atom receiving the double pointers.
 * \param v1 first atom to caompare
 * \param v2 second atom to compare
 * \return returns an integer less than, equal to, or greather then zero,
 *         if the name of \a v1is found, respectively, to be less then, to
 *         match, or be greater then name of \a v2
 */
//--------------------------------------------------------------------
{
  BSyntaxObject* obj1 = *((BSyntaxObject**)v1);
  BSyntaxObject* obj2 = *((BSyntaxObject**)v2);
  size_t s1 = (size_t)obj1;
  size_t s2 = (size_t)obj2;
  return((s1<s2)?-1:(s1>s2)?1:0);
}

//--------------------------------------------------------------------
   inline int CompareGlossaryElement(const void* v1, const void* v2)
// Compares the name of two received pointers to BGlossaryElement
//--------------------------------------------------------------------
{
  BOis::BGlossaryElement* obj1 = (BOis::BGlossaryElement*)v1;
  BOis::BGlossaryElement* obj2 = (BOis::BGlossaryElement*)v2;
	return( BText::Compare(&obj1->name_, &obj2->name_));
};


//--------------------------------------------------------------------
   inline int CompareHrchyEntry(const void* v1, const void* v2)
// Compares the entry number of two received pointers to 
// BHierarchyIndex
//--------------------------------------------------------------------
{
  BOis::BHierarchyIndex* obj1 = (BOis::BHierarchyIndex*)v1;
  BOis::BHierarchyIndex* obj2 = (BOis::BHierarchyIndex*)v2;
  size_t s1 = (size_t)obj1->hrchyEntry_;
  size_t s2 = (size_t)obj2->hrchyEntry_;
  return((s1<s2)?-1:(s1>s2)?1:0);
}

//--------------------------------------------------------------------
   inline int CompareAliasValue(const void* v1, const void* v2)
// Compares the entry number of two received pointers to 
// BAlias
//--------------------------------------------------------------------
{
  BOis::BAlias* obj1 = (BOis::BAlias*)v1;
  BOis::BAlias* obj2 = (BOis::BAlias*)v2;
	return( BText::Compare(&obj1->value_, &obj2->value_));
}

#endif // TOL_OIS_H


