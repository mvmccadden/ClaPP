/*!
 *  \author  Manoel McCadden
 *  \date    05-01-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    main.cpp
 *
 *  \brief
 *    This file is the entry point to the simple c++/glfw based 
 *    game engine known as Clarity++ or ClaPP
*/
#include "clapp_includes/pch.h"

#include "clapp_includes/Clarity_Engine.h"

using namespace std;

using namespace ClaPP;

int main()
{
  Engine engine;

  engine.Startup();
  while(engine.Run());
  engine.Exit();
  
  return 0;
}
