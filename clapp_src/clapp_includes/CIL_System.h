/*
 *  Author:  Manoel McCadden
 *  Data:    05-20-2025
 *  Email:   manoel.mccadden@gmail.com
 *  Github:  github.com/mvmccadden
 *
 *  File:    CIL_System.h
 *
 *  Description:
 *    The interface file for interacting with the clarity input system
 */
#pragma once

#include <string>
#include <cstdint>

#include "Clarity_System.h"

namespace ClaPP
{
class Input_System : public Clarity_System
{
public:
  Input_System(const std::string &_sysName);
  ~Input_System();

  SYS_ERR Initialize();
  SYS_ERR Load();
  SYS_ERR Update(float deltaTime);
  SYS_ERR Render();
  SYS_ERR Unload();
  SYS_ERR Terminate();

  inline static const uint64_t INPUTERR = 49593190;
private:
};
}
