/*
 *  \author  Manoel McCadden
 *  \date    07-18-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_LUA.cpp
 *
 *  \brief
 *    An implementation for handling LUA functionality
*/

#include "clapp_includes/Clarity_LUA.h"
#include "clapp_includes/Clarity_IO.h"

namespace ClaPP
{
// NOTE: Starting with a singleton but most likly should change it to be 
// for each thread later to ensure that all threads have their own access
// and can simultanously access lua script data
LuaState &LuaState::GetInstance()
{
  static LuaState instance;
  return instance;
}

LuaState::LUA_ERR LuaState::ExecuteFile(const std::string &luaPath)
{
  if(!currentFile.empty())
  {
    ErrMessage("Attempting to execute a new file when another is in use"
                , EC_LUA);
    return LUA_FILE_STILL_IN_USE;
  }
  if(luaL_dofile(luaState, luaPath.c_str()) != LUA_OK)
  {
    // Lua pushes an error message to the top of its stack on 
    // failed file opening
    ErrMessage("Failed to open lua file: " + luaPath + ", " 
                + lua_tostring(luaState, -1), EC_LUA);
    // Since we need to get that message with lua_tostring we must
    // also pop it after to clear the stack
    lua_pop(luaState, 1);
    return LUA_FILE_NOT_FOUND;
  }
  currentFile = luaPath;
  currentPopLayer = 0u;
  return LUA_NO_ERR;
}

LuaState::LUA_ERR LuaState::GetGlobal(const std::string &globalName)
{
  if(LUA_ERR err = CheckFileStatus())
  {
    return err;
  }
  lua_getglobal(luaState, globalName.c_str());
  if(!lua_istable(luaState, -1))
  {
    ErrMessage("Failed to find global: " 
                + globalName + " in file: " + currentFile, EC_LUA);
    return LUA_INVALID_GLOBAL;
  }
  
  currentGlobal = globalName;
  ++currentPopLayer;
  return LUA_NO_ERR;
}

LuaState::LUA_ERR LuaState::GetField(const std::string &fieldName)
{
  if(LUA_ERR err = CheckFileStatus())
  {
    return err;
  }
  
  if(currentPopLayer < 1)
  {
    ErrMessage("Attempting to get field when no global has been selected"
                , EC_LUA);
    return LUA_INVALID_FIELD;
  }
  lua_getfield(luaState, -1, fieldName.c_str());

  if(!lua_istable(luaState, -1))
  {
    ErrMessage("Failed to find field: " + fieldName + " in global: "
                + currentGlobal + " in file: " + currentFile, EC_LUA);
  }
  currentField = fieldName;
  ++currentPopLayer;
  return LUA_NO_ERR;
}

LuaState::LUA_ERR LuaState::GetFieldLength(int &fieldSize)
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
                , EC_LUA);
    return LUA_INVALID_FIELD;
  }
  fieldSize = static_cast<int>(lua_rawlen(luaState, -1));
  return LUA_NO_ERR;
}

LuaState::LUA_ERR LuaState::GetNumber(float &ptr, const int &index)
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

LuaState::LUA_ERR LuaState::GetNumber(double &ptr, const int &index)
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

LuaState::LUA_ERR LuaState::GetNumber(int &ptr, const int &index)
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

LuaState::LUA_ERR LuaState::GetNumber(unsigned int &ptr, const int &index)
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

LuaState::LUA_ERR LuaState::GetString(std::string &ptr, const int &index)
{
  if(LUA_ERR err = CheckFileStatus())
  {
    return err;
  }
  if(currentPopLayer < 2)
  {
    ErrMessage("Attempting to get variable when no field has been selected"
                , EC_LUA);
    return LUA_INVALID_FIELD;
  }
  lua_rawgeti(luaState, -1, index + 1);
  if(!lua_isstring(luaState, -1))
  {
    ErrMessage("No string found in field: " + currentField + " in global: "
                + currentGlobal + " in file: " + currentFile, EC_LUA);
    return LUA_INCORRECT_TYPE;
  }
  ptr = lua_tostring(luaState, -1);
  // We manually pop in any get funciton after we have gotten the value
  // as we no longer need the value in the stack
  lua_pop(luaState, 1);
  return LUA_NO_ERR;
}

LuaState::LUA_ERR LuaState::GetBool(bool &ptr, const int &index)
{
  if(LUA_ERR err = CheckFileStatus())
  {
    return err;
  }
  if(currentPopLayer < 2)
  {
    ErrMessage("Attempting to get variable when no field has been selected"
                , EC_LUA);
    return LUA_INVALID_FIELD;
  }
  lua_rawgeti(luaState, -1, index + 1);
  if(!lua_isboolean(luaState, -1))
  {
    ErrMessage("No bool found in field: " + currentField + " in global: "
                + currentGlobal + " in file: " + currentFile, EC_LUA);
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

LuaState::LUA_ERR LuaState::CreateEnum(std::string luaEnum
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

LuaState::LUA_ERR LuaState::Pop()
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
    ErrMessage("Attempting to lua pop when no file is in use", EC_LUA);
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

LuaState::LuaState() 
: currentFile(), currentGlobal(), currentField()
  , currentPopLayer(0u), luaMutex()
{
  // Create a new state
  luaState = luaL_newstate();
  if(!luaState)
  {
    ErrMessage("Lua State not properly created", EC_LUA);
    return;
  }
  // Open the library on creation
  luaL_openlibs(luaState);
}

LuaState::~LuaState()
{
  if(!luaState)
  {
    ErrMessage("Lua State not properly created", EC_LUA);
    return;
  }
  lua_close(luaState);
}

LuaState::LUA_ERR LuaState::CheckFileStatus()
{
  if(currentFile.empty())
  {
    ErrMessage("Attempting to get a global when no file is selected", EC_LUA);
    return LUA_NO_FILE_IN_USE;
  }
  return LUA_NO_ERR;
}

LuaState::LUA_ERR LuaState::NumberChecks(const int &index)
{
  if(LUA_ERR err = CheckFileStatus())
  {
    return err;
  }
  if(currentPopLayer < 2)
  {
    ErrMessage("Attempting to get variable when no field has been selected"
                , EC_LUA);
    return LUA_INVALID_FIELD;
  }
  lua_rawgeti(luaState, -1, index + 1);
  if(!lua_isnumber(luaState, -1))
  {
    ErrMessage("No number found in field: " + currentField + " in global: "
                + currentGlobal + " in file: " + currentFile, EC_LUA);
    return LUA_INCORRECT_TYPE;
  }
  return LUA_NO_ERR;
}
}
