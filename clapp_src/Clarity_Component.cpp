/*!
 *  \author  Manoel McCadden
 *  \date    05-14-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_Component.h
 *
 *  \brief
 *    An abstract class used to define what a component is
*/
#include "clapp_includes/pch.h"

#include "clapp_includes/Clarity_Component.h"

using namespace std;
using namespace ClaPP;

Component::Component() 
{

}

Component::~Component()
{

}

/*!
 *  Converts a component type enum to a string of it's name
 *
 *  \returns
 *    The name of the component object class
 */
const std::string &Component::ToString(const COMPONENTS &component)
{
  return componentMap[static_cast<int>(component)];
}

/*!
 *  Converts a component type to an enum based on it's name
 *
 *  \returns
 *    A component type enum
 */
Component::COMPONENTS Component::ToEnum()
{
  std::string typeName = typeid(*this).name();

  for(int i = 0; i < C_COUNT; ++i)
  {
    // Searches for a substring of the component type in the
    // given components name. As it will not be the each type name but
    // instead will have some extra characters we must search for the name
    // since it will not be an exact match as it is set at compile time.
    //
    // Find returns a position of the first occurance of the substring.
    // If it doesn't exist it will return NPOS which means no position is found
    if(typeName.find(componentMap[i]) != std::string::npos)
    {
      return static_cast<COMPONENTS>(i);
    }
  }

  return C_INVALID;
}
