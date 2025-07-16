/*
 * Author:  Manoel McCadden
 * Data:    05-01-2025
 * Email:   manoel.mccadden@gmail.com
 * Github:  github.com/mvmccadden
 *
 * File:    main.cpp
 *
 * Description:
 *  This file is the entry point to the simple c++/glfw based 
 *  game engine known as Clarity++ or ClaPP
*/
#include "clapp_includes/pch.h"

#include "clapp_includes/Clarity_Engine.h"

// NOTE: CHECKED FILES
// ENGINE 
//
// TODO: CURRENTLY WORKING ON FILES
// ECS
//
// NOTE: Partly Done
// COMPONENT
// 

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
