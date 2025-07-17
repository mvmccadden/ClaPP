/*!
 *  \author  Manoel McCadden
 *  \date    06-16-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_Memory.cpp
 *
 *  \brief
 *    An implementation for handling memory allocation, deallocation
 *    , and tracking for the ClarityPlusPlus Suite 
*/

#include "clapp_includes/pch.h"

#include "clapp_includes/Clarity_Memory.h"

using namespace std;
using namespace ClaPP;

//=================//
//= CTOR and DTOR =//
//=================//

MemoryManager::MemoryManager()
{

}

MemoryManager::~MemoryManager()
{

}

//===============================//
//= Allocation Tracking Methods =//
//===============================//

const uint64_t &MemoryManager::GetTotalAllocs()
{
  return allocCount;
}

const uint64_t &MemoryManager::GetTotalDeallocs()
{
  return deallocCount;
}

uint64_t MemoryManager::GetMemoryInUse()
{
  if(deallocCount > allocCount)
  {
    ErrMessage("More memory deallocated than allocated!", MEMERRMSG);
  }

  return allocCount - deallocCount;
}


