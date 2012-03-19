#pragma once

// Quick and nasty reference counter
template <class T> class RefCnt
{
private:
   T* mResource;
   int* mCount;
public:
   //Default constructor
   RefCnt()
   {
      mResource = 0;
      mCount = new int;
      *mCount = 1;
   }

   //Actual constructor for use
   RefCnt(T* resource) :
      mResource(resource)
   {
      mCount = new int;
      (*mCount) = 1;
   }

   //Copy constructor
   RefCnt(const RefCnt<T>& rVal)
   {
      mResource = rVal.mResource;
      mCount = rVal.mCount;
      (*mCount)++;
   }

   //Asignment operator
   RefCnt<T>& operator=(const RefCnt<T>& rVal)
   {
      (*mCount)--;
      if(*mCount == 0)
      {
         delete mCount;
      }

      mCount = rVal.mCount;
      (*mCount)++;
      mResource = rVal.mResource;

      return *this;
   }

   ~RefCnt(void)
   {
      (*mCount)--;
      if(*mCount == 0)
      {
         delete mCount;
      }
      mCount = 0;
   }

   const T* operator->() const
   {
      return mResource;
   }

   T* operator->()
   {
      return mResource;
   }

   bool operator==(const RefCnt<T>& rhs)
   {
      return mResource == rhs.mResource;
   }

   bool operator!=(const RefCnt<T>& rhs)
   {
      return !(this == rhs);
   }

   int use_count()
   {
      return *mCount;
   }

   T* raw_pointer() const
   {
      return mResource;
   }
};
