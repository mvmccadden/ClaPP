/*
 *  Author:  Manoel McCadden
 *  Data:    05-03-2025
 *  Email:   manoel.mccadden@gmail.com
 *  Github:  github.com/mvmccadden
 *
 *  File:    Clarity_IO.h
 *
 *  Description:
 *    An interface for handling file io, message tracing/logging
 *    , and generic output
*/
#pragma once

// Since it is a c file we must make that clear to avoid
// unnecessary compiling errors
extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

#include <climits>
#include <fstream>
#include <functional>
#include <iostream>
#include <cstdint>
#include <mutex>
#include <string>
#include <sstream>

namespace ClaPP
{
enum FILE_ERR
{
  FILE_NO_ERR = 0
  , FILE_NOT_FOUND
  , FILE_CANNOT_BE_OPENED
  , FILE_IS_ALREADY_OPEN
  , FILE_FAILED_TO_CLOSE
  , FILE_IS_NOT_OPEN
  , FILE_FAILED_TO_WRITE
  , FILE_FAILED_TO_READ
  , FILE_UNKOWN_ERR
};

static FILE_ERR OpenFile(std::fstream &file, const char *filePath)
{
  if(file.is_open())
  {
    return FILE_IS_ALREADY_OPEN;
  }

  file.open(filePath);
  if(!file.is_open())
  {
    return FILE_CANNOT_BE_OPENED;
  }

  return FILE_NO_ERR;
}

static FILE_ERR OpenFile(std::fstream &file, const std::string &filePath)
{
  return OpenFile(file, filePath.c_str());
}

static FILE_ERR WriteToFile(std::fstream &file, const char *message)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }

  file << message << std::endl;

  return FILE_NO_ERR;
}

static FILE_ERR WriteToFile(std::fstream &file, const std::string &message)
{
  return WriteToFile(file, message.c_str());
}

static FILE_ERR ReadLineFromFile(std::fstream &file, std::string &line)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }
  if(file.eof())
  {
    return FILE_NOT_FOUND;
  }

  std::getline(file, line);
  return FILE_NO_ERR;
}

static FILE_ERR ReadWordFromFile(std::fstream &file, std::string &word)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }
  if(file.eof())
  {
    return FILE_NOT_FOUND;
  }

  word = "";
  file >> word;

  if(!file.good())
  {
    return FILE_FAILED_TO_READ;
  }

  return FILE_NO_ERR;
}

// TODO: Update this so it is like numbers only... ect (double, float, int)
template<typename T>
static FILE_ERR ReadTypeFromFile(std::fstream &file, T &type)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }
  if(file.eof())
  {
    return FILE_NOT_FOUND;
  }

  file >> type;

  if(!file.good())
  {
    return FILE_FAILED_TO_READ;
  }

  return FILE_NO_ERR;
}

static FILE_ERR ConvertFileToString(std::fstream &file
                                          , std::string &string)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }
  if(file.eof())
  {
    return FILE_NOT_FOUND;
  }

  std::stringstream stream; 
  stream << file.rdbuf();
  string = stream.str();

  if(string.empty())
  {
    return FILE_FAILED_TO_READ;
  }

  return FILE_NO_ERR;
}

static FILE_ERR CloseFile(std::fstream &file)
{
  if(!file.is_open())
  {
    return FILE_IS_NOT_OPEN;
  }

  file.close();
  return FILE_NO_ERR;
}

static void Message(const char* message)
{
  std::cout << message << std::endl;
}

static void Message(const std::string &message)
{
  Message(message.c_str());
}

// TODO: Make an error table with all error codes in this file :)
static void ErrMessage(const char *message, const uint64_t errCode)
  {
  std::cerr << "ERROR: " << errCode << " " << message << std::endl;
}

static void ErrMessage(const std::string &message, const uint64_t errCode)
{
  ErrMessage(message.c_str(), errCode);
}

/*!
 * \class EventManager
 *
 * \brief
 *  A helper class designed to manager events on a global scale.
 *
 *  operations include:
 *  - Adding an event
 *  - Reading events
 */
class EventManager
{
public:
  enum EVENT_OBJ
  {
    COMPONENT = 0
    , ENTITY
    , SYSTEM
    , ENGINE
    , USER_COMPONENT
    , USER_ENTITY
    , USER_SYSTEM

  };

  struct Event
  {
    Event(const EVENT_OBJ &_sender, const EVENT_OBJ &_reciever
          , const std::string &_eventMessage)
    : sender(_sender), reciever(_reciever), eventMessage(_eventMessage) {}


    EVENT_OBJ sender;
    EVENT_OBJ reciever;
    std::string eventMessage;
    bool eventChecked = false;
  };
  // NOTE: Initally doing this as a singleton to simplify but will adjust
  // for multi-thread or multi-engine running later
  static EventManager &GetInstance()
  {
    static EventManager instance;
    return instance;
  }

  void AddEvent(Event &&event)
  {
    events.push_back(event);
  }

  // Check all events for a specific message from a specific sender
  bool CheckEvent(const EVENT_OBJ &_desiredSender
                                , const EVENT_OBJ &_reciever
                                , const std::string &_expectedMessage)
  {
    for(Event &event : events)
    {
      if(_desiredSender == event.sender && _reciever == event.reciever
      && _expectedMessage == event.eventMessage)
      {
        event.eventChecked = true;
        return true;
      }
    }

    return false;
  }

  // Remove any events that are checked
  void Update(float deltaTime)
  {
    for(std::vector<Event>::iterator it = events.begin()
    ; it != events.end(); ++it)
    {
      if(it->eventChecked)
      {
        events.erase(it);
      }
    }
  }

private:
  EventManager()
  {

  }
  ~EventManager()
  {

  }

  EventManager(const EventManager &other) = delete;
  EventManager &operator=(const EventManager &other) = delete;

  std::vector<Event> events;

  inline static const std::string EMPTY_STRING = "";
};

/*!
 * \class LuaState
 *
 * \brief
 *  A helper class designed to execute lua files and retrieve data for the user
 *
 *  operations include:
 *  - excuting a file
 *  - getting a global
 *  - getting a field
 *  - getting a number
 *  - getting a string
 *  - getting a bool
 *  - poping the lua stack
 *
 *  Whenever you are done doing any operations you must pop the stack as any
 *  "get" operation will add to your stack
 *
 *  Lua stacks all its get operations where the stack starts at -1 and goes 
 *  down (-2, -3, -4, ...). When you pop an item the (1) value indicates the 
 *  numbers of items to pop from the top of the stack. If you get an item
 *  with and place -1 it will place it one top, -2 right after the top, ect...
*/
class LuaState
{
public:
  // NOTE: Starting with a singleton but most likly should change it to be 
  // for each thread later to ensure that all threads have their own access
  // and can simultanously access lua script data
  static LuaState &GetInstance()
  {
    static LuaState instance;
    return instance;
  }

  enum LUA_ERR
  {
    LUA_NO_ERR = 0
    , LUA_FILE_NOT_FOUND
    , LUA_INVALID_FILE
    , LUA_INVALID_GLOBAL
    , LUA_INVALID_FIELD
    , LUA_INCORRECT_TYPE
    , LUA_NULLPTR_PASSED
    , LUA_NO_FILE_IN_USE
    , LUA_FILE_STILL_IN_USE
  };

  LUA_ERR ExecuteFile(const std::string &luaPath)
  {
    if(!currentFile.empty())
    {
      ErrMessage("Attempting to execute a new file when another is in use"
                 , LUAERR);
      return LUA_FILE_STILL_IN_USE;
    }

    if(luaL_dofile(luaState, luaPath.c_str()) != LUA_OK)
    {
      // Lua pushes an error message to the top of its stack on 
      // failed file opening
      ErrMessage("Failed to open lua file: " + luaPath + ", " 
                 + lua_tostring(luaState, -1), LUAERR);
      // Since we need to get that message with lua_tostring we must
      // also pop it after to clear the stack
      lua_pop(luaState, 1);
      return LUA_FILE_NOT_FOUND;
    }

    currentFile = luaPath;
    currentPopLayer = 0u;

    return LUA_NO_ERR;
  }
  LUA_ERR GetGlobal(const std::string &globalName)
  {
    if(LUA_ERR err = CheckFileStatus())
    {
      return err;
    }

    lua_getglobal(luaState, globalName.c_str());

    if(!lua_istable(luaState, -1))
    {
      ErrMessage("Failed to find global: " 
                 + globalName + " in file: " + currentFile, LUAERR);
      return LUA_INVALID_GLOBAL;
    }
    
    currentGlobal = globalName;
    ++currentPopLayer;
    return LUA_NO_ERR;
  }
  LUA_ERR GetField(const std::string &fieldName)
  {
    if(LUA_ERR err = CheckFileStatus())
    {
      return err;
    }
    
    if(currentPopLayer < 1)
    {
      ErrMessage("Attempting to get field when no global has been selected"
                 , LUAERR);
      return LUA_INVALID_FIELD;
    }

    lua_getfield(luaState, -1, fieldName.c_str());
  
    if(!lua_istable(luaState, -1))
    {
      ErrMessage("Failed to find field: " + fieldName + " in global: "
                 + currentGlobal + " in file: " + currentFile, LUAERR);
    }

    currentField = fieldName;
    ++currentPopLayer;
    return LUA_NO_ERR;
  }

  LUA_ERR GetFieldLength(int &fieldSize)
  {
    if(LUA_ERR err = CheckFileStatus())
    {
      return err;
    }

    // NOTE: This is done to ensue we are within a global and a field
    // to comply with the lua standard ClaPP uses
    if(currentPopLayer < 2)
    {
      ErrMessage("Attempting to get legnth when no field has been selected"
                 , LUAERR);
      return LUA_INVALID_FIELD;
    }

    fieldSize = static_cast<int>(lua_rawlen(luaState, -1));

    return LUA_NO_ERR;
  }

  LUA_ERR GetNumber(float &ptr, const int &index)
  {
    if(LUA_ERR err = NumberChecks(index))
    {
      return err;
    }

    ptr = lua_tonumber(luaState, -1);

    // We manually pop in any get funciton after we have gotten the value
    // as we no longer need the value in the stack
    lua_pop(luaState, 1);
    return LUA_NO_ERR;
  }
  
  LUA_ERR GetNumber(double &ptr, const int &index)
  {
    if(LUA_ERR err = NumberChecks(index))
    {
      return err;
    }

    ptr = lua_tonumber(luaState, -1);

    // We manually pop in any get funciton after we have gotten the value
    // as we no longer need the value in the stack
    lua_pop(luaState, 1);
    return LUA_NO_ERR;
  }

  LUA_ERR GetNumber(int &ptr, const int &index)
  {
    if(LUA_ERR err = NumberChecks(index))
    {
      return err;
    }

    ptr = lua_tointeger(luaState, -1);

    // We manually pop in any get funciton after we have gotten the value
    // as we no longer need the value in the stack
    lua_pop(luaState, 1);
    return LUA_NO_ERR;
  }

  LUA_ERR GetNumber(unsigned int &ptr, const int &index = 0)
  {
    if(LUA_ERR err = NumberChecks(index))
    {
      return err;
    }

    ptr = static_cast<unsigned int>(lua_tointeger(luaState, -1));

    // We manually pop in any get funciton after we have gotten the value
    // as we no longer need the value in the stack
    lua_pop(luaState, 1);
    return LUA_NO_ERR;
  }

  LUA_ERR GetString(std::string &ptr, const int &index = 0)
  {
    if(LUA_ERR err = CheckFileStatus())
    {
      return err;
    }

    if(currentPopLayer < 2)
    {
      ErrMessage("Attempting to get variable when no field has been selected"
                 , LUAERR);
      return LUA_INVALID_FIELD;
    }

    lua_rawgeti(luaState, -1, index + 1);

    if(!lua_isstring(luaState, -1))
    {
      ErrMessage("No string found in field: " + currentField + " in global: "
                 + currentGlobal + " in file: " + currentFile, LUAERR);
      return LUA_INCORRECT_TYPE;
    }

    ptr = lua_tostring(luaState, -1);

    // We manually pop in any get funciton after we have gotten the value
    // as we no longer need the value in the stack
    lua_pop(luaState, 1);
    return LUA_NO_ERR;
  }

  LUA_ERR GetBool(bool &ptr, const int &index = 0)
  {
    if(LUA_ERR err = CheckFileStatus())
    {
      return err;
    }

    if(currentPopLayer < 2)
    {
      ErrMessage("Attempting to get variable when no field has been selected"
                 , LUAERR);
      return LUA_INVALID_FIELD;
    }

    lua_rawgeti(luaState, -1, index + 1);

    if(!lua_isboolean(luaState, -1))
    {
      ErrMessage("No bool found in field: " + currentField + " in global: "
                 + currentGlobal + " in file: " + currentFile, LUAERR);
      return LUA_INCORRECT_TYPE;
    }

    // NOTE: Changed 2nd param from index to -1 so that it is 
    // the top of the stack
    ptr = lua_toboolean(luaState, -1);

    // We manually pop in any get funciton after we have gotten the value
    // as we no longer need the value in the stack
    lua_pop(luaState, 1);
    return LUA_NO_ERR;
  }

  struct EnumPair
  {
    std::string luaName;
    int enumValue;
  };
  
  LUA_ERR CreateEnum(std::string luaEnum
                     , const std::vector<EnumPair>  &enumPairs)
  {
    // Create a new table
    lua_newtable(luaState);

    // Push all the new enums to the table
    for(EnumPair pair : enumPairs)
    {
      // Push a new enum with a string to access it from the table
      lua_pushstring(luaState, pair.luaName.c_str());
      lua_pushinteger(luaState, pair.enumValue);
      // Set the table value then go back up the chain
      lua_settable(luaState, -3);
    }

    // Set the global table name
    lua_setglobal(luaState, luaEnum.c_str());

    return LUA_NO_ERR;
  }

  LUA_ERR Pop()
  {
    if(currentPopLayer == 0u && !currentFile.empty())
    {
      currentFile.clear();
      // Done just to ensure that they are always cleared when a file is done
      currentGlobal.clear();
      currentField.clear();
    }
    else if(currentFile.empty())
    {
      ErrMessage("Attempting to lua pop when no file is in use", LUAERR);
      return LUA_NO_FILE_IN_USE;
    }
    else 
    {
      lua_pop(luaState, 1);
      --currentPopLayer;
      
      if(currentPopLayer == 1)
      {
        currentField.clear();
      }
      else if(currentPopLayer == 0)
      {
        currentGlobal.clear();
      }
    }

    return LUA_NO_ERR;
  }

  inline static const uint64_t LUAERR = 43434343;
private:
  LuaState() 
  : currentFile(), currentGlobal(), currentField()
    , currentPopLayer(0u), luaMutex()
  {
    // Create a new state
    luaState = luaL_newstate();

    if(!luaState)
    {
      ErrMessage("Lua State not properly created", LUAERR);
      return;
    }

    // Open the library on creation
    luaL_openlibs(luaState);
  }
  ~LuaState()
  {
    if(!luaState)
    {
      ErrMessage("Lua State not properly created", LUAERR);
      return;
    }

    lua_close(luaState);
  }

  LUA_ERR CheckFileStatus()
  {
    if(currentFile.empty())
    {
      ErrMessage("Attempting to get a global when no file is selected", LUAERR);
      return LUA_NO_FILE_IN_USE;
    }

    return LUA_NO_ERR;
  }

  LUA_ERR NumberChecks(const int &index)
  {
    if(LUA_ERR err = CheckFileStatus())
    {
      return err;
    }

    if(currentPopLayer < 2)
    {
      ErrMessage("Attempting to get variable when no field has been selected"
                 , LUAERR);
      return LUA_INVALID_FIELD;
    }

    lua_rawgeti(luaState, -1, index + 1);

    if(!lua_isnumber(luaState, -1))
    {
      ErrMessage("No number found in field: " + currentField + " in global: "
                 + currentGlobal + " in file: " + currentFile, LUAERR);
      return LUA_INCORRECT_TYPE;
    }

    return LUA_NO_ERR;
  }

  lua_State *luaState;
  std::string currentFile;
  std::string currentGlobal;
  std::string currentField;
  uint8_t currentPopLayer;
  std::mutex luaMutex;
};

}
