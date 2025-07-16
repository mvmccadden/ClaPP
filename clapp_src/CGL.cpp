/*!
 *  \author  Manoel McCadden
 *  \date    05-04-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL.cpp
 *
 *  \brief
 *    The implementation file that wraps all CGL functionalities into a single 
 *    usable file for the generic user. Removing any unneccesary access to the
 *    inner workings and systems of the CGL 
*/
#include "clapp_includes/pch.h"

#include "clapp_includes/CGL.h"

#include "clapp_includes/CGL_System.h"
#include "clapp_includes/Clarity_System.h"
#include "clapp_includes/Clarity_IO.h"

using namespace std;
using namespace ClaPP;

CGL::CGL()
: Clarity_System("ClarityGraphicsLibrary")
  , engineGraphics(new CGL_System("ClarityEngine")) 
{
}

CGL::~CGL() 
{
  if(engineGraphics)
  {
    delete engineGraphics;
  }
}

CGL::SYS_ERR CGL::Initialize()
{
  return engineGraphics->Initialize();
}

CGL::SYS_ERR CGL::Load()
{
  SYS_ERR err = SYS_NO_ERR;
  
  err = engineGraphics->Load();
  if(err)
  {
    return err;
  }

  return err;
}

CGL::SYS_ERR CGL::Update(float deltaTime)
{
  SYS_ERR err = SYS_NO_ERR;

  // Check for engine termination
  if(TerminateEngine())
  {
    // Returns no error for terminate engine as this is
    // most likley intentional
    return err;
  }
  
  err = engineGraphics->Update(deltaTime);
  if(err)
  {
    return err;
  }

  return err;
}

CGL::SYS_ERR CGL::Render()
{
  SYS_ERR err = SYS_NO_ERR;
  
  err = engineGraphics->Render();
  if(err)
  {
    return err;
  }

  return err;
}

CGL::SYS_ERR CGL::Unload()
{
  SYS_ERR err = SYS_NO_ERR;
  
  err = engineGraphics->Unload();
  if(err)
  {
    return err;
  }

  return err;
}

CGL::SYS_ERR CGL::Terminate()
{
  SYS_ERR err = SYS_NO_ERR;
  
  err = engineGraphics->Terminate();
  if(err)
  {
    return err;
  }

  return err;
}

bool CGL::TerminateEngine()
{
  if(engineGraphics->CheckWindowClose())
  {
    EventManager::GetInstance().AddEvent(
      EventManager::Event(EventManager::SYSTEM
                          , EventManager::ENGINE
                          , "TerminateEngine")
    );
    return true;
  }

  return false;
}
