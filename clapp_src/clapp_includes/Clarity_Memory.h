/*!
 *  \author  Manoel McCadden
 *  \date    05-02-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_Memory.h
 *
 *  \brief
 *    A interface for handling memory allocation, deallocation, and tracking
 *    for the ClarityPlusPlus Suite 
*/
#pragma once

#include <mutex>
#include <utility>
#include <cstdint>
#include <new>
#include <typeinfo>

#include "Clarity_IO.h"


namespace ClaPP
{
/*!
 *  \class MemoryManager
 *
 *  \brief
 *    A simple class that handles memory managment for the ClaPP suite. This
 *    allows for memory tracking and allocation saftey.
 */
class MemoryManager
{
public:
  /*!
   *  \brief
   *    Default CTOR for the memory manager.
   *
   *  The intent is that each ECS and possible other systems will have
   *  their own memory manager to minimize overlap in memory and allow
   *  for more efficent memory allocation, deallocation, and retrival
   */
  MemoryManager();
  /*!
   *  \brief
   *    Default DTOR for the memory manager.
   */
  ~MemoryManager();

  /*!
   *  \returns
   *    The number of bytes allocated by the memory manager
   */
  const uint64_t &GetTotalAllocs();

  /*!
   *  \returns
   *    The number of bytes deallocation made by the memory manager
   */
  const uint64_t &GetTotalDeallocs();

  /*!
   *  \returns
   *    The size of memory currently in use by the memory manager
   */
  uint64_t GetMemoryInUse();

  /*!
   *  \brief
   *    Allocates and constructs a new object of type T into the heap
   *
   *  \param args
   *    A recursive set of arguments of any type that are used to construct
   *    a new object of type T. The arguments must be in order with the
   *    constructor and must contain all argumetns required to construct 
   *    the object.
   *
   *  \returns
   *    A pointer to the newly allocated object in the heap
   *
   *  TODO: Will update to use memory arenas/pages to better allocate data
   */
  template<typename T, typename... Args>
  T *Alloc(Args&&... args)
  {
    // Update allocation counts then attempt to construct a new object of T
    
    // Refer to Dealloc for why we are scoping allocCount with mutex
    {
      std::lock_guard<std::mutex> lock(countMutex);
      allocCount += sizeof(T);
    }

    try
    {
      // Must add ... after the forward to let the compiler know that this is a
      // pack of parameters and should be passed as individual parameters not as
      // a single argument
      return new T(std::forward<Args>(args)...);
    }
    // Catch any allocation errors and report an error
    catch(const std::bad_alloc &e)
    {
      // Must manually add the type id after 
      std::string str = "Failed to allocate new object of type: ";
      str += typeid(T).name();
      str += ", with error: ";
      str += e.what();

      ErrMessage(str, MEMERRMSG);

      return nullptr;
    }
  }

  /*!
   *  \brief
   *    Deallocates a given object of type T, freeing it's heap memory and 
   *    setting the pointer to a nullptr
   *
   *  \param obj
   *    The object being deleted from the heap and set to a nullptr
   */
  template<typename T>
  void Dealloc(T *&obj)
  {
    // We place the lock guard into a scope to ensure it only affects
    // the count as that is the only part that requires thread saftey.
    // As new and delete are thread safe we don't need to worry about them
    {
      std::lock_guard<std::mutex> lock(countMutex);
      deallocCount += sizeof(T);
    }

    delete obj;
    obj = nullptr;
  }

private:
  //! A count for the number of bytes that have been allocated
  uint64_t allocCount = 0;
  //! A count for the number of bytes that have been deallocated
  uint64_t deallocCount = 0;

  /*! 
   *  mutex for allocation and deallocation count for the memory managers
   *  to ensure thread saftey.
   */
  std::mutex countMutex;

  //! The memory manager's error value
  inline static const uint64_t MEMERRMSG = 73737373;
};
}

