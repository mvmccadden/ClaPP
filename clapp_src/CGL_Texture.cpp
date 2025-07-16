/*!
 *  \author  Manoel McCadden
 *  \date    06-02-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL_Texture.cpp
 *
 *  \brief
 *    The implementation file for the base class identity of what a texture
 *    is within the Clarity Graphics Library
 */
#include "clapp_includes/pch.h"

#include "clapp_includes/CGL_Texture.h"

#include "clapp_includes/g_pch.h"

#include "clapp_includes/CGL_System.h"
#include "clapp_includes/Clarity_IO.h"
#include "clapp_includes/Clarity_Library.h"

using namespace std;
using namespace ClaPP;

Texture::Texture(const std::string &_filePath
                 , const glm::vec3 &_colorTint, const float &_alpha)
{
  if(!CheckTextureExists(_filePath))
  {
    textureData = new TextureData;
    textureData->tintColor = _colorTint;
    textureData->alpha = _alpha;
    BindTexture(_filePath);
  }
}

Texture::~Texture()
{
  UnbindTexture();

  delete textureData;
}

void Texture::SetTextureFilePath(const std::string &_filePath)
{
  if(CheckTextureExists(_filePath))
  {
    BindTexture(_filePath);
  }
}

void Texture::SetTextureTint(const glm::vec3 &_colorTint)
{
  textureData->tintColor = _colorTint;
}

void Texture::SetTextureOpacity(const float &_opacity)
{
  textureData->alpha = _opacity;
}

const Texture::TextureData &Texture::GetTextureData()
{
  return *textureData;
}

bool Texture::CheckTextureExists(const string &_filePath)
{
  textureData = Library<TextureData>::GetInstance().GetItem(_filePath);
  // Return the result of get item to the user
  return textureData != nullptr ? true : false;
}

void Texture::BindTexture(const string &_filePath)
{
  // Generate a new texture
  glGenTextures(1, &textureData->textureID);
  CGL_System::CheckGLError();
  glBindTexture(GL_TEXTURE_2D, textureData->textureID);
  CGL_System::CheckGLError();
  // Set the wrapping and filtering options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  CGL_System::CheckGLError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  CGL_System::CheckGLError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  CGL_System::CheckGLError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  CGL_System::CheckGLError();

  // Load and generate the texture
  int width, height, channels;
  unsigned char *imageData = stbi_load(_filePath.c_str(), &width, &height
                                       , &channels, 0);
  // Check if the image is loaded properly and generate accordanly
  if (imageData)
  {
    // PNG uses RGBA as it has an alpha
    if (channels == 4)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA 
                   , GL_UNSIGNED_BYTE, imageData);
      CGL_System::CheckGLError();
    }
    // JPG uses RGB as it doesn't have an alpha
    else if (channels == 3)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB
                   , GL_UNSIGNED_BYTE, imageData);
      CGL_System::CheckGLError();
    }
    else
    {
      ErrMessage("Invalid number of channels gotten from image", TEXERR);
      // Free data before returning 
      stbi_image_free(imageData);
      return;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    CGL_System::CheckGLError();

    textureData->filePath = _filePath;
  }
  else
  {
    ErrMessage("Failed to obtain given image from file path", TEXERR);
  }

  // Add the new texture to the library (it will check if it already exists)
  Library<TextureData>::GetInstance().AddItem(_filePath, textureData);

  // Free the data
  stbi_image_free(imageData);
}

void Texture::UnbindTexture()
{

}
