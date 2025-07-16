/*
 *  \author  Manoel McCadden
 *  \date    05-03-2025
 *  \par    manoel.mccadden@gmail.com
 *  \par    github.com/mvmccadden
 *
 *  \file    ClaPP_Engine.h
 *
 *  \brief
 *    An engine framework that handles system managment and eases the users
 *    interaction with the backend systems and libraries
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
