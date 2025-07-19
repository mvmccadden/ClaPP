/*
 *  \author  Manoel McCadden
 *  \date    07-18-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_LUA.h
 *
 *  \brief
 *    An interface for handling LUA functionality
*/
#pragma once

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

#include "Clarity_IO.h"

namespace ClaPP
{
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
  static LuaState &GetInstance();

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

  struct EnumPair
  {
    std::string luaName;
    int enumValue;
  };

  LUA_ERR ExecuteFile(const std::string &luaPath);
  LUA_ERR GetGlobal(const std::string &globalName);
  LUA_ERR GetField(const std::string &fieldName);
  LUA_ERR GetFieldLength(int &fieldSize);
  LUA_ERR GetNumber(float &ptr, const int &index);
  LUA_ERR GetNumber(double &ptr, const int &index);
  LUA_ERR GetNumber(int &ptr, const int &index);
  LUA_ERR GetNumber(unsigned int &ptr, const int &index = 0);
  LUA_ERR GetString(std::string &ptr, const int &index = 0);
  LUA_ERR GetBool(bool &ptr, const int &index = 0);
  LUA_ERR CreateEnum(std::string luaEnum
                     , const std::vector<EnumPair>  &enumPairs);
  LUA_ERR Pop();
private:
  LuaState();
  ~LuaState();
  LUA_ERR CheckFileStatus();
  LUA_ERR NumberChecks(const int &index);

  lua_State *luaState;
  std::string currentFile;
  std::string currentGlobal;
  std::string currentField;
  uint8_t currentPopLayer;
  std::mutex luaMutex;
};
}
