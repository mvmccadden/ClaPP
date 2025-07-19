/*
 *  \author  Manoel McCadden
 *  \date    05-20-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CIL_System.h
 *
 *  \brief
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

private:
};
}
