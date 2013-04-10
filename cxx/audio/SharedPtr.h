#ifndef SHARED_PTR_H
#define SHARED_PTR_H

/** A reference-counted shared pointer implementation. */
template <typename T> class SharedPtr
{
public:

  SharedPtr()
    :ptr(0),refCount(0)
  {}
  //---------------------------------------------------------------------------

  explicit SharedPtr(T* p)
    :ptr(0),refCount(0)
  {
    bind(p);
  }
  //---------------------------------------------------------------------------

  ~SharedPtr()
  {
    release();
  }
  //---------------------------------------------------------------------------

  /** copy constructor (templated so polymorphism works) */
  SharedPtr(const SharedPtr<T>& rhs)
    :ptr(0),refCount(0)
  {
    ptr = rhs.getPointer();
    refCount = rhs.getRefCountPtr();
    if(refCount)
      ++(*refCount);
  }
  //---------------------------------------------------------------------------

  /** copy constructor (templated so polymorphism works) */
  template <typename Y> SharedPtr(const SharedPtr<Y>& rhs)
    :ptr(0),refCount(0)
  {
    ptr = rhs.getPointer();
    refCount = rhs.getRefCountPtr();
    if(refCount)
      ++(*refCount);
  }
  //---------------------------------------------------------------------------

  /** Plain ol' assignment op */
  SharedPtr& operator=(const SharedPtr& rhs)
  {
    if(ptr == rhs.ptr)
      return *this;
    SharedPtr<T> temp(rhs);
    swap(temp);
    return *this;
  }
  //---------------------------------------------------------------------------

  /** Templated assignment op, so it can store derived types */
  template <typename Y> SharedPtr& operator=(const SharedPtr<Y>& rhs)
  {
    if(ptr == rhs.getPointer())
      return *this;
    SharedPtr<T> temp(rhs);
    swap(temp);
    return *this;
  }
  //---------------------------------------------------------------------------

  void bind(T* p)
  {
    ptr = p;
    refCount = new unsigned int(1);
  }
  //---------------------------------------------------------------------------

  void release()
  {
    if(!ptr)
      return;
    --(*refCount);
    if((*refCount) == 0)
    {
      delete ptr;
      delete refCount;
      ptr = 0;
      refCount = 0;
    }
  }
  //---------------------------------------------------------------------------

  inline T* getPointer() const
  {
    return ptr;
  }
  //---------------------------------------------------------------------------

  inline unsigned int* getRefCountPtr() const
  {
    return refCount;
  }
  //---------------------------------------------------------------------------

  inline unsigned int getRefCount() const
  {
    return *refCount;
  }
  //---------------------------------------------------------------------------
  
  inline bool isNull()
  {
    return !ptr;
  }
  //---------------------------------------------------------------------------

  inline void setNull()
  {
    release();
    ptr = 0;
    refCount = 0;
  }
  //---------------------------------------------------------------------------

  inline T& operator*() const {return *ptr;}
  inline T* operator->() const {return ptr;}

protected:

  void swap(SharedPtr<T>& other)
  {
    std::swap(ptr, other.ptr);
    std::swap(refCount, other.refCount);
  }
  //---------------------------------------------------------------------------

  unsigned int* refCount;
  T* ptr;
  
};

#endif
