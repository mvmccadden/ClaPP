/*!
 *  \author  Manoel McCadden
 *  \date    05-05-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL_Shader.h
 *
 *  \brief
 *    The interface file for handing shader data in the CGL library
*/
#pragma once

#include <cstdint>
#include <string>

namespace ClaPP
{
class CGL_Shader
{
public:
  enum SHADERTYPE 
  {
    VERTEX
    , FRAGMENT
  };

  CGL_Shader(SHADERTYPE type, const char *_shaderPath);
  CGL_Shader(const CGL_Shader &other);
  CGL_Shader &operator=(const CGL_Shader &other);
  ~CGL_Shader();

  bool CompileShader();
  void DeleteShader();
  const unsigned int &GetShaderID();

private:
  friend class CGL_Program;
  
  std::string shaderPath;
  const SHADERTYPE shaderType;
  unsigned int shaderID;
  unsigned int *sharedCount;

};

class CGL_Program
{
public:
  CGL_Program(const CGL_Shader &vertexShader
              , const CGL_Shader &fragmentShader);
  ~CGL_Program();

  bool LinkProgram();
  void DeleteProgram();
  const unsigned int &GetProgramID();

  const unsigned int &GetColorLocation();
  const unsigned int &GetPositionLocation();
  const unsigned int &GetTextureLocation();
  const unsigned int &GetObjMatrixLocation();
  const unsigned int &GetViewMatrixLocation();
  const unsigned int &GetPerspectiveMatrixLocation();

  inline static const uint64_t PROGRAMERR = 50505050;
private:
  CGL_Shader vertex, fragment;
  unsigned int programID;
  unsigned int objMatrix = 0u, inColor = 0u, inPos = 0u, inTex = 0u
  , viewMatrix = 0u, perspectiveMatrix = 0u;

  CGL_Program(const CGL_Program &other) = delete;
  CGL_Program &operator=(const CGL_Program &other) = delete;
};
}
