/* tol_barray.h: Implements an Array of Any.
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

//! \file barray.h

#ifndef TOL_BARRAY_H
#define TOL_BARRAY_H 1

#include <tol/tol_bout.h>

#define ReallocMarginFactor 1.2

//--------------------------------------------------------------------
// BArray macros
//--------------------------------------------------------------------

#ifndef NDEBUG
#define ENSURE_DIM 1
#endif

#ifdef ENSURE_DIM
#  define BARR_ENSURE_DIM(i) EnsureDimensions(i);
#else
#  define BARR_ENSURE_DIM(i)
#endif



//--------------------------------------------------------------------
// functions
//--------------------------------------------------------------------



//--------------------------------------------------------------------
//! BArray Template. Implements an Array of virtually any class or data type.
//--------------------------------------------------------------------
template <class Any>
class  BArray

/*! Implements an Array of any basic type or class. Define methods to
 *  manipulate this classic data structure. The behavior was 
 *  controlled with three attributes. We can create an array, 
 *  delete, add elements to its buffer, sort the buffer, make a 
 *  duplicate, retrieve the values of its attributtes, and other 
 *  operations.
 */
//--------------------------------------------------------------------
{
protected:
  Any* buffer_;	   //!< Storage pointer
  BInt maxSize_;   //!< Number of items of type Any in buffer_
  BInt size_;	   //!< Number of items of type Any in buffer_

public:
  // Constructors and destructors:
  //! Create a new BArray assigning 0 to integer attributes and NIL to buffer_.
 BArray() : buffer_(NIL), maxSize_(0), size_(0) {}

  //! Create new BArray of size_ like size parameter and buffer_ like buf.
  /*! PURPOSE: Invokes method Copy(size, buf) to make a copy of each element
  //           of buf to attribute buffer_. Attributes size_ and maxSize_ 
  //           equals parameter size.
  //  \param size The size of the new BArray object
  //  \param buf A pointer to a data structure that stores elements of class
  //             Any that will be added to buffer_ attribute 
  */
  BArray(BInt size, const Any* buf=NIL)
    : buffer_(NIL), maxSize_(0), size_(0) { Copy(size, buf); }

  //! Create a new BArray
  BArray(const Any& any, BInt size);

  //! Create a new BArray invoking Copy Method
  /*! PURPOSE: Create a new BArray. Assign 0 to size_ and maxSize_ and NIL
  //           to buffer_. After that invoking Copy method with the 
  //           attributes of the BArray parameter passed to constructor. 
  */
  BArray(const BArray<Any>& arr)
    : buffer_(NIL), maxSize_(0), size_(0) 
  { Copy(arr.Size(), arr.Buffer()); }

  //! Desctructor Method - Delete the BArray object freeing memory
  /*! PURPOSE: Destructor method invokes method DeleteBuffer freeing memory
  //           used in buffer_ 
  */
  virtual ~BArray() { DeleteBuffer(); }

  //--------------------------------------------------------------------
    static Any* SafeNew(int size)
  //--------------------------------------------------------------------
  {
    Any* buffer = NULL;
    bool ok = true;
    if(size<0) 
    { 
      ok = false;
    }
    else if(size>0)
    {
      try  
      { 
        buffer = new Any[size]; 
      }
      catch ( std::bad_alloc ) 
      { 
        buffer = NULL; 
        ok = false;
      }
    }
    if(!ok)
    {
      int bytes = sizeof(Any);
      Error(I2("FATAL: Cannot allocate array of ",
               "FATAL: No se pudo alojar un arreglo de ")+
               +size+" items x "+bytes+" bytes");
    }
    return(buffer);
  };
  
  // Access & Manipulation: inline
  //! return maxSize_ attribute
  BInt MaxSize() const { return(maxSize_);  }
  //! return size_ attribute
  BInt Size() const { return(size_);  }
  //! return buffer_ attribute
  Any* GetBuffer() { return(buffer_); }
  //! return The buffer_ attribute like a const pointer
  /*! Impossible make a change to returned object */
  const Any* Buffer() const { return(buffer_); }
  //! return A duplicate of buffer_.
  Any* BufferDuplicate (BInt from, BInt until) const;
  //! return True if BArray contents something. False in other case
  BBool HasValue() const { return(buffer_ && (size_ > 0)); }
  //! return A reference to an overflow static area of type Any
  static Any& Overflow();
  //! Verify if index "i" is out of bounds of buffer_ array
  void EnsureDimensions(BInt& i) const;

  //! return The element at "i" position in buffer_
  Any&	operator[](BInt i) const;
  //! return The element at "i" position in buffer_
  Any&	operator()(BInt i) const;

  //! Replace buffer_ attribute with buf and update size_ and maxSize_
  void	PutBuffer     (BInt size, Any* buf);
  //! Alloc memory to store "size" elements in buffer_ attribute
  virtual void	AllocBuffer   (BInt size);
  //! Resize buffer_ with capacity for "size" Any elements
  virtual void	ReallocBuffer (BInt size);
  //! Delete buffer_ and initialize all attributes
  virtual void	DeleteBuffer();

  // Miscelaneus operations:
  //BText	  Name() const;
  //! Alloc memory for size elements and make a copy from buf to buffer_
  void	  Copy(BInt size, const Any* buf);
  //! Behave like the third constructor method.
  void	  Replicate(const Any& obj, BInt size=1);

  //! Add an obj Any to buffer_
  /*! Verify if there's space for an object and store it. 
   *  If there's space only for one object, then double the
   *  amount of space in buffer_ calling ReallocBuffer
   * \param obj The object of type Any to insert into buffer_
   * \sa AddSorted, AddUnique, AddUniqueSorted  
   */
  void	  Add(const Any& obj)
  {
    BInt n = size_;
    if(maxSize_<=n+1) { ReallocBuffer((int)(ReallocMarginFactor*(n+1))); }
    ReallocBuffer(n+1);
    buffer_[n] = obj;
  }

  //! Add an obj Any to buffer_ and sort its contents
  /*! Call method Add and then Sort buffer_ using method 
   *  pointed in parameter order.
   * \param obj The object of type Any to insert into buffer_
   * \param order The function used to sort buffer_ elements
   * \sa Add, AddUnique, AddUniqueSorted, BorderCriterium 
   */
  void	AddSorted(const Any& obj, BOrderCriterium order)
  {
    Add(obj); Sort(order);
  }

  //! Add a new obj to buffer_
  /*! First of all verify if obj exist in buffer_ using a 
   *  function of type BOrderCriterium and calling Find method.
   *  If obj not in buffer_ then insert it with Add method.
   * \param obj The object of type Any to insert into buffer_
   * \param order The function used to verify the equality
   * \return true if obj is unique, false if obj exist in buffer_
   * \sa Add, AddSorted, AddUniqueSorted, Find
   */
  BBool AddUnique(const Any& obj, BOrderCriterium order)
  {
    BBool unique= (Find(obj, order)<0);
    if(unique) { Add(obj); }
    return(unique);
  }

  //! Add a new obj to buffer_ and sort its contents
  /*! Behave like AddUnique method but call AddSorted method
   *  when has verified the obj isn't in buffer_
   * \param obj The object of type Any to insert into buffer_
   * \param order The function used to verify the precedence and equality
   * \return true if obj is unique, false if obj exist in buffer_
   * \sa Add, AddSorted, AddUnique, Find
   */
  BBool AddUniqueSorted(const Any& obj, BOrderCriterium order)
  {
    BBool unique = (Find(obj, order)<0);
    if(unique) { AddSorted(obj, order); }
    return(unique);
  }

  //! Append buffer_ of parameter object to buffer_
	 BArray<Any>& AutoConcat (const BArray<Any>& );
//friend BArray<Any>  Concat	 (const BArray<Any>& , const BArray<Any>& );

  //! Join buffer_ of parameters a1 and a2
  /*! Join buffer_ attributes of the two parameters object a1 and 
   *  a2 and return a new object BArray instantiated at Concat
   *  method, with the concatenated buffer_
   * \param a1 First BArray
   * \param a2 Second BArray
   * \return New BArray object created by this method
   * \sa Concat
   */
  friend BArray<Any>  operator | (const BArray<Any>& a1, const BArray<Any>& a2)
  { return(Concat(a1, a2)); }

  //! Copy BArray parameter into this BArray
  /*! Call Copy method to make a copy of BArray parameter
   * \param arr BArray parameter to copy from
   * \return Pointer to this BArray object
   * \sa Copy
   */
  BArray& operator=(const BArray& arr)
  {
    Copy(arr.Size(), arr.Buffer());
    return(*this);
  }

  //Simple-data-base functions
  //! Use qsort of stdlib to sort the contents of buffer_
  void Sort(BOrderCriterium order);
  //! Searches for the first object equal than searched
  BInt Find(const Any& searched, BOrderCriterium order) const;
  //! Searches in an ordered array the first object equal than searched
  BInt FindSorted (const Any& searched, BOrderCriterium order) const;
  //! returns the next items not equals than the n-th item
  BInt NextNotEqual(BOrderCriterium order, BInt from=0) const;
};

//--------------------------------------------------------------------
// MEMBER FUCTIONS
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class Any>  
BArray<Any>::BArray(const Any& any, BInt size)

/*! Creates a BArray.<br>
 *  To Create a new BArray size_ and maxSize_ attributes being "size" 
 *  constructor parameter. Then reserve space for "size" elements
 *  in buffer_ attribute. Each element of buffer_ will be a copy
 *  of "any", the parameter of the constructor method.
 * \param any Object that will be copied "size" times into \a buffer_
 * \param size Create a BArray with size capacity in \a buffer_
 */
//--------------------------------------------------------------------
: buffer_(NIL), maxSize_(0), size_(0)
{      
  buffer_=SafeNew(size_ = maxSize_ = size);
  if(!buffer_) 
  { 
    maxSize_=size_=0;
  }
  else
  {
    for(BInt n = 0; n < size; n++) { buffer_[n] = any; }
  }
}


//--------------------------------------------------------------------
template <class Any> 
Any&	 BArray<Any>::Overflow()

/*! Returns a reference to an overflow static area.
 */
//--------------------------------------------------------------------
{
    static Any overflow_;
    overflow_=Any();
    return(overflow_);
}


//--------------------------------------------------------------------
template <class Any> 
void BArray<Any>::EnsureDimensions(BInt& i) const

/*! Ensure if i is out of buffer_ bounds.
 * \param i index value
 */
//--------------------------------------------------------------------
{
    if((i<0)||(i>=size_))
    {
	Error(I2("Forbidden array access: ","Acceso prohibido de array: ") +
	      "0 < "+i+" < "+size_);  
    }
}

//--------------------------------------------------------------------
template <class Any> 
Any& BArray<Any>::operator[](BInt i) const

/*! Verify i value and returns object at i position of buffer_
 * \param i index value
 * \sa operator (), EnsureDimensions
 */
//--------------------------------------------------------------------
{
//BARR_ENSURE_DIM(i);
    return(((0<=i)&&(i<size_)) ? buffer_[i] : Overflow());
}


//--------------------------------------------------------------------
template <class Any> 
Any& BArray<Any>::operator()(BInt i) const

/*! Verify i with EnsureDimensions method and return object at i
 *  position of buffer_
 * \param i index value
 * \sa operator [], EnsureDimensions
 */
//--------------------------------------------------------------------
{
    BARR_ENSURE_DIM(i);
    return(buffer_[i]);
}

//--------------------------------------------------------------------
template <class Any> 
void BArray<Any>::DeleteBuffer()

/*! This method freeing memory used by buffer_ and initialize
 *  to NIL value. size_ and maxSize_ attributes set to 0. Its behave
 *  like a default constructor.
 */
//--------------------------------------------------------------------
{
#ifdef TRACE_MEMORY
    Trace(Out() << "Deleting array of size " << size_ ); 
#endif
    if(buffer_) { delete [] buffer_; }
    maxSize_ = size_ = 0;
    buffer_ = NIL;
}


//--------------------------------------------------------------------
template <class Any> 
Any* BArray<Any>::BufferDuplicate(BInt from, BInt until)  const

/*! Returns a duplicate of buffer_.
 */
//--------------------------------------------------------------------
{
  if(from<0)	   { from=0; }
  if(until>=size_) { until=size_; }
  if(from <= until)
  {
	  BInt copySize = until - from +1;
  	Any* dup=SafeNew(copySize);
	  for(BInt i=0; i<copySize; i++)
	  { dup[i]=(*this)[from+i]; }
	  return(dup);
  }
  else
  {
	  return(NULL);
  }
}


//--------------------------------------------------------------------
template <class Any> 
void BArray<Any>::PutBuffer(BInt size, Any* buf)

/*! Overwrites the contents of the array.
 */
//--------------------------------------------------------------------
{
    DeleteBuffer();
    if((size > 0) && (buf != NIL))
    {
	maxSize_ = size_ = size;
	buffer_ = buf;
    }
}


//--------------------------------------------------------------------
template <class Any> 
void BArray<Any>::AllocBuffer(BInt size)

/*! Alloc memory for the Array buffer with capacity for size elements
 *  of type Any. If the array is not empty the actual buffer is
 *  deleted.
 */
//--------------------------------------------------------------------
{
  if (size < 0) { size = 0; }
  if(maxSize_ < size)
  {
    DeleteBuffer();
	  buffer_ = SafeNew(maxSize_ = size_ = size);
    if(size && !buffer_) 
    { 
      maxSize_=size_=0;
    }
  }
  else
  { 
    size_ = size; 
  }
}


//--------------------------------------------------------------------
template <class Any> 
void BArray<Any>::ReallocBuffer(BInt size)

/*! Realloc memory for the Array buffer with capacity for size
 *  elements of type Any.
 */
//--------------------------------------------------------------------
{
  if (size < 0) { size = 0; }
  if (maxSize_ < size)
  {
    Any* oldBuf = buffer_;
    buffer_ = SafeNew(maxSize_ = size);
    if(size && !buffer_) 
    { 
      maxSize_=size_=0;
      return;
    }	
    if(size_ && oldBuf)
    {
      for(BInt i=0; i < size_; i++)
      { buffer_[i] = oldBuf[i]; }
      delete [] oldBuf;
    }
  }
  size_ = size;
}


//--------------------------------------------------------------------
template <class Any> 
void BArray<Any>::Replicate(const Any& obj, BInt size)

/*! Replicates the item obj size times in the buffer.
 *  Behave like the third constructor method.
 */
//--------------------------------------------------------------------
{
    AllocBuffer(size);
    for(BInt n=0; n<size_; n++) { buffer_[n]=obj; }
}


//--------------------------------------------------------------------
template <class Any> 
void BArray<Any>::Copy(BInt size, const Any*	 buf)

/*! Reserve memory for size elements of type Any, then copy
 *  each element of buf parameter from 0 to size in buffer_
 *  attribute, updating size_ and maxSize_
 * \param size Number of elements to copy
 * \param buf buffer where \a size elements (0 to \a size-1) will be stored
 */
//--------------------------------------------------------------------
{
    if (size < 0) { size = 0; }
    
    AllocBuffer(size);
    
    if(buf != NIL)
    {
	for(BInt n=0; n<size_; n++) { buffer_[n]=buf[n]; }
    }
}


//--------------------------------------------------------------------
template <class Any> 
BArray<Any>& BArray<Any>::AutoConcat(const BArray<Any>& arr)

/*! Concats this with arr, puts the result in this and returns this.
 */
//--------------------------------------------------------------------
{
    if(!arr.HasValue()) { return (*this); } // It's done
    
    BInt i=Size(), j;
    
    ReallocBuffer(Size() + arr.Size());
    
    for(j=0; (j < arr.Size()) && (i < Size()); j++, i++)
    { buffer_[i]=arr(j); }
    
    return(*this);
}

//! Function that returns the result of concat arr1 with arr2.
//--------------------------------------------------------------------
template <class Any> 
BArray<Any> Concat(const BArray<Any>& arr1, const BArray<Any>& arr2)

/*! Function that returns the result of concat arr1 with arr2.
 * \param arr1 BArray<Any> parameter allocated first in attribute \a buffer_
 *        of the new BArray returned.
 * \param arr2 Second BArray<Any> parameter allocated in \a buffer_ after 
 *        parameter \a arr1 in the new BArray returned.
 */
//--------------------------------------------------------------------
{
    BArray<Any> newArray;
    
    newArray.AllocBuffer(arr1.Size()+arr2.Size());
    
    BInt i=0,j=0;
    
    for(i=0; i<arr1.Size(); i++)
    { newArray(i)=arr1(i); }
    
    for(j=0; (j < arr2.Size()) && (i < newArray.Size()); j++, i++)
    { newArray(i)=arr2(j); }
    
    return(newArray);
}


//--------------------------------------------------------------------
template<class Any> 
void BArray<Any>::Sort(BOrderCriterium order)

/*! Sorts this buffer_ with passing the order criterium, defined
 *  by *order function, as a parameter tu qsort of C++ stdlib
 * \param order Of class BOrderCriterium, a user defined type (in bystype.h),
 *        that represents a pointer to function.
 * \sa BOrderCriterium 
 */
//--------------------------------------------------------------------
{
    if(HasValue() && size_ >= 2)
    { qsort(buffer_, size_, sizeof(Any), order); }
}


//--------------------------------------------------------------------
template<class Any> 
BInt BArray<Any>::Find(const Any& searched,
		       BOrderCriterium order) const

/*! Searches for the first object equal than searched with a
 *  determinate equivalence relation *order function parameter.
 * \param searched The object that will be searched
 * \param order BOrderCriterium function
 * \return -1 or index of the object found.
 */
//--------------------------------------------------------------------
{
    BInt found=-1;
    
    for(BInt n=0; (n<Size()) && (found==-1); n++)
    {
	if(!(*order)(&buffer_[n], &searched)) { found = n; }
    }
    return(found);
}


//--------------------------------------------------------------------
template<class Any> 
BInt BArray<Any>::FindSorted(const Any& searched,
			     BOrderCriterium order) const

/*! Searches in an ordered array with the order relation order for
 *  the first object equal than searched with a determinate
 *  equivalence relation *order.
 * \param searched The object that will be searched
 * \param order BOrderCriterium function
 * \return -1: if no object has been found. Else returns the index of
 *             the found object.
 */
//--------------------------------------------------------------------
{
    void* vFound = bsearch(&searched, buffer_, size_, sizeof(Any), order);
    Any* found = (Any*)vFound;
    return((found) ? found - buffer_ : -1);
}


//--------------------------------------------------------------------
template<class Any> 
BInt BArray<Any>::NextNotEqual(BOrderCriterium order,
			       BInt from) const

/*! Begining from n+1, returns the next items not equals than the n-th
 *  item with the criterium function *order.
 * \param order BOrderCriterium function
 * \param from Index used to begining the search
 * \return -1 if no object has been found, else return the index of the
 *            found object
 */
//--------------------------------------------------------------------
{
    BInt found=-1;
    
    for(BInt i=1; (from+i < Size()) && (found == -1); i++)
    {
	if(!(*order)(&buffer_[from+i], &buffer_[from])) { found = from + i; }
    }
    
    return (found);
}



#endif // TOL_BARRAY_H
