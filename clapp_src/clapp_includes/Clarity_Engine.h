/*
 * Author:  Manoel McCadden
 * Data:    05-03-2025
 * Email:   manoel.mccadden@gmail.com
 * Github:  github.com/mvmccadden
 *
 * File:    ClaPP_Engine.h
 *
 * Description:
 *  An engine framework that handles system managment and eases the users
 *  interaction with the backend systems and libraries
*/
#pragma once

#include "Clarity_ECS.h"

namespace ClaPP
{
class Engine
{
public:
  Engine();
  ~Engine();

  bool Startup();
  bool Run();
  bool Exit();

private:
  ECS ecsManager;

  bool TerminateEngine();
};
}
