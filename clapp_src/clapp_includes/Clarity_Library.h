/*
 *  Author:  Manoel McCadden
 *  Data:    05-12-2025
 *  Email:   manoel.mccadden@gmail.com
 *  Github:  github.com/mvmccadden
 *
 *  File:    Clarity_Library.h
 *
 *  Description:
 *    A generic library system that holds different types of data for
 *    storage and easy access
*/
#pragma once

#include <map>
#include <string>
#include <memory>

#include "Clarity_IO.h"

namespace ClaPP
{
template<typename T>
class Library
{
public:
  static Library& GetInstance()
  {
    static Library instance;
    return instance;
  }
  
  /*!
   *  Clears a shelf of all it's contents freeing up any memory space it is using
  */
  void ClearShelf()
  {
    // unallocate all items in the map
    for(std::map<std::string, T*> &item : shelf)
    {
      delete item;
    }
  
    // Then clear the shelf
    shelf.clear();
  }
  
  /*!
   *  Adds an item to the library shelf. It is good practice to check that
   *  the name isn't be used with GetItem.
   *
   *  \param itemName
   *    The name that will be associated with the item
   *  \param storeItem
   *    The item being stored in the library
   *
   *  \returns
   *    A pointer to the item stored at the given itemName location.
   *    This should be the item you just placed but if there already exists 
   *    an item at the name location, then that item will returned and your
   *    item will be lost.
  */
  T *AddItem(const std::string &itemName, T *storeItem)
  {
    auto emplaced = shelf.try_emplace(itemName, storeItem);
    
    // Make sure an item doesn't already exists
    // the second is a bool checking for proper emplacement
    if(!emplaced.second)
    {
      ErrMessage("Item already allocated at key: " + itemName + ", in type: " 
                 + typeid(T).name() + ".\nReturning existing item!"
                 , LIBRARYERR);

      // Delete the pointer as it is assumed that the library would delete
      // it later
      delete storeItem;
    }
    else
    {
      Message("Allocating item: " + itemName + " in " + typeid(T).name()
              + " library.");
    }
    
    storeItem = nullptr;

    // the first item in emplaced is the iterator
    return emplaced.first->second.get();
  }
  
  /*!
   *  Returns the item associated with the given item name within 
   *  the library shelf.
   *
   *  \param itemName
   *    The name of the item being located
   *
   *  \returns
   *    A nullptr if it doesn't exists and a pointer to the item at 
   *    given itemName if one does exists.
  */
  T *GetItem(const std::string &itemName)
  {
    auto it = shelf.find(itemName);
    if(it == shelf.end())
    {
      // As this might be intentional and we are checking if the item
      // doesn't exists, this is considered to not be an error
      Message("Item not allocated at key: " + itemName + ", in " 
              + typeid(T).name() + " library." + ".\nReturning nullptr!");
      
      return nullptr;
    }

    return it->second.get();
  }

  inline static const uint64_t LIBRARYERR = 94399439;
private:
  std::map<std::string, std::unique_ptr<T>> shelf;

  Library() {}
  ~Library() {}

  Library(const Library &other) = delete;
  Library &operator=(const Library &other) = delete;
};
}
