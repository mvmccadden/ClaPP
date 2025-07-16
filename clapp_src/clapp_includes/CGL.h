/*!
 *  \author  Manoel McCadden
 *  \date    05-04-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL.h
 *
 *  \brief
 *    The interface file that wraps all CGL functionalities into a single 
 *    usable file for the generic user. Removing any unneccesary access to the
 *    inner workings and systems of the CGL 
*/
#pragma once

#include "Clarity_System.h"

namespace ClaPP
{
// Classes/Structs that are combined to make the greater CGL
class CGL_System;
struct CGL_Container;

class CGL : public Clarity_System
{
public:
  CGL();
  ~CGL();

  SYS_ERR Initialize();
  SYS_ERR Load();
  SYS_ERR Update(float deltaTime);
  SYS_ERR Render();
  SYS_ERR Unload();
  SYS_ERR Terminate();

  bool TerminateEngine();

  SYS_ERR CreateContainer(CGL *&container);
  SYS_ERR AddChildContainerToParent(CGL *&parent, CGL *&child);
  SYS_ERR PushContainer(CGL *&container);
  SYS_ERR PopContainer(CGL *&container);
  SYS_ERR DestroyContainer(CGL *&container);
private:
  CGL_System *engineGraphics = nullptr;

//  std::stack<CGL_Container> engineContainers;

  CGL(const CGL &other) = delete;
  CGL &operator=(const CGL &other) = delete;

  void UpdateObject(const uint32_t objID);
  void RenderObject(const uint32_t objID);
};
}
