/*!
 *  \author  Manoel McCadden
 *  \date    05-02-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL_System.h
 *
 *  \brief
 *    The interface file for the system of the Clarity Graphics Library (CGL).
 *    This library provides a sort of wrapper over the base implementation needed
 *    to run glfw/glad.
*/
#pragma once

#include "CGL_Shader.h"
#include "Clarity_System.h"

namespace ClaPP
{
/*!
 *  \class CGL_System
 *  \paragraph
 *    A wrapper around the glfw/glad libraries used to implment base-line 
 *    graphics capabilities and handle window managment
*/
class CGL_System : public Clarity_System
{
public:
  // Used to define shaders for use in cpp file
  typedef class CGL_Program CGL_Program;
  // implemented in cpp file with glfw functinalities
  struct WindowContainer;
  enum WindowSettings
  {
    DEFAULT = 0
    , FULLSCREEN = 1
    , MAXIMIZED = 2
    , RESIZEABLE = 4
  };

  CGL_System(const std::string &_sysName, uint32_t _windowSettings = 0);
  ~CGL_System();

  // WARN: Deprecated
  bool CheckWindowClose();

  SYS_ERR Initialize();
  SYS_ERR Load();
  SYS_ERR Update(float deltaTime);
  SYS_ERR Render();
  SYS_ERR Unload();
  SYS_ERR Terminate();

  void CGL_FramebufferSizeCallback(int width, int height);
  static void CheckGLError();
  static void GraphicsError(int error, const char *details);

  // In reference to windows 3.1
private:
  bool isOpenGLInitialized = false;
  uint32_t windowSettings = 0;
  WindowContainer *windowData = nullptr;

  CGL_Program *defaultShader;

  // Remove ability to duplicate system as it could potential lead to errors
  // and is an unintended feature
  CGL_System(const CGL_System &other) = delete;
  void operator=(const CGL_System &other) = delete;

  SYS_ERR InitOpenGL();
  SYS_ERR ProcessWindowHints();
};

}

