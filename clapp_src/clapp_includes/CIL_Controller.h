/*
 *  Author:  Manoel McCadden
 *  Data:    06-14-2025
 *  Email:   manoel.mccadden@gmail.com
 *  Github:  github.com/mvmccadden
 *
 *  File:    CIL_Controller.h
 *
 *  Description:
 *    The interface file for handling player input for entities
 *    and their controllers
 */

#include "Clarity_Component.h"

namespace ClaPP
{
class Controller : public Component
{
public:
  struct ControllerData
  {
    
  };

  Controller() 
  {

  }
  ~Controller()
  {

  }

  ControllerData &GetControllerData()
  {
    return controllerData;
  }

private:
  ControllerData controllerData;
};
}
