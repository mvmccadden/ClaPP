/*
 *  \author  Manoel McCadden
 *  \date    06-06-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CIL_PlayerControllerSystem.h
 *
 *  \brief
 *    The interface file for handling player input for entities
 *    and their controllers
 */
#pragma once

#include <string>

#include "Clarity_System.h"

namespace ClaPP
{
class PlayerControllerSystem : public Clarity_System
{
public:
  PlayerControllerSystem(const std::string &_sysName = "P_ControlSystem");
  ~PlayerControllerSystem();

  SYS_ERR Initialize();
  SYS_ERR Load();
  SYS_ERR Update(float deltaTime);
  SYS_ERR Render();
  SYS_ERR Unload();
  SYS_ERR Terminate();

private:

};
}
