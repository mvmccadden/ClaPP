/*
 *  \author  Manoel McCadden
 *  \date    05-02-2025
 *  \par    manoel.mccadden@gmail.com
 *  \par    github.com/mvmccadden
 *
 *  \file    Clarity_System.h
 *
 *  \brief
 *    A baseline inheritance system used to define what a clarity system
 *    should look like and contain
*/
#pragma once

#include <string>
#include <cstdint>
#include <set>

#include "Clarity_Component.h"
#include "Clarity_Entity.h"
#include "Clarity_IO.h"


namespace ClaPP
{
class ECS;
// Does this system template work for creating an inhertiable system?
class Clarity_System
{
public:
  enum SYS_ERR
  {
    SYS_NO_ERR = 0
    , SYS_FAILED_TO_CREATE
    , SYS_FAILED_TO_INITIALIZE
    , SYS_ALREADY_INITIALIZED
    , SYS_UNINITIALIZED // Check if the flag was properly set if it is used
    , SYS_FAILED_TO_LOAD
    , SYS_FAILED_TO_UPDATE
    , SYS_FAILED_TO_RENDER
    , SYS_FAILED_TO_UNLOAD
    , SYS_FAILED_TO_TERMINATE
    , SYS_FAILED_TO_DELETE
  };

  // We don't decrement the system count as we are simply attempting to track
  // the number of systems created to ease naming conventions :)
  virtual ~Clarity_System() {}

  virtual SYS_ERR Initialize() = 0;
  virtual SYS_ERR Load() = 0;
  virtual SYS_ERR Update(float deltaTime) = 0;
  virtual SYS_ERR Render() = 0;
  virtual SYS_ERR Unload() = 0;
  virtual SYS_ERR Terminate() = 0;

  const std::string &GetSysName() 
  { 
    return sysName; 
  }

  SYS_ERR SetInitalized(const bool &_isInitialized)
  {
    // Give the user a warning if they are attempting to set 
    // the initalization value to a value it is already set as.
    // This could possibly mean there is an error in their system
    // where initi
    if(isInitialized == _isInitialized)
    {
      errorMessage = "Attempting to set initialzation flag to same value";
      if (isInitialized)
      {
        return SYS_ALREADY_INITIALIZED;
      }
      else
      {
        return SYS_UNINITIALIZED;
      }
    }

    isInitialized = _isInitialized;
    return SYS_NO_ERR;
  }

  const bool &IsInitialized()
  {
    return isInitialized;
  }

  // Returns the most recently set error message to the user
  const std::string &GetErrorMessage()
  {
    return errorMessage;
  }

  bool FindEntityInSystem(const ENTITY_ID &id)
  {
    return systemEntites.find(id) != systemEntites.end();
  }

  void RemoveEntityInSystem(const ENTITY_ID &id)
  {
    if(!FindEntityInSystem(id))
    {
      // As we intentionally let the remove and add entity check in the ECS
      // we will not report this as an error as it may be intended
      Message("Failed to find entity ID: " + std::to_string(id)
                 + " in system: " + sysName, SEVERITY_WARNING);
      return;
    }
    systemEntites.erase(id);
  }

  void AddEntityInSystem(const ENTITY_ID &id)
  {
    if(FindEntityInSystem(id))
    {
      Message("Entity ID: " + std::to_string(id) + " already exists"
                + " in system: " + sysName, SEVERITY_WARNING);
      return;
    }
    systemEntites.insert(id);
  }

  ECS *&GetECSPtr()
  {
    return ecsPtr;
  }

protected:
  Clarity_System(const std::string &_sysName) 
  : sysName(_sysName) 
  { 
    ++sysCount; 
  }

  // Used to set the error message within a derived system to specify a
  // system error
  void SetErrorMessage(const std::string &_errorMessage)
  {
    errorMessage = _errorMessage;
  }


  COMPONENT_SIGNATURE systemSignature;
  std::set<ENTITY_ID> systemEntites;
private:
  // Choosing not to mutex as systems are intended to be craeted at engine
  // initalization and shouldn't need thread saftey. This is also simply
  // to help with naming conventions internal so if it is thread specific
  // this shouldn't cause any errors :)
  static inline uint16_t sysCount = 0;
  const std::string sysName;
  bool isInitialized;

  friend ECS;
  ECS *ecsPtr = nullptr;

  std::string errorMessage;
};

}
