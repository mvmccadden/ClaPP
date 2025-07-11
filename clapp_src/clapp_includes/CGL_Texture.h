/*
 * Author:  Manoel McCadden
 * Data:    06-02-2025
 * Email:   manoel.mccadden@gmail.com
 * Github:  github.com/mvmccadden
 *
 * File:    CGL_Texture.h
 *
 * Description:
 *  The interface file for the base class identity of what a texture
 *  is within the Clarity Graphics Library
 */
#include <string>
#include <stdint.h>

#include "Clarity_Component.h"

#include "../../external/glm/glm.hpp"

namespace ClaPP
{
class Texture : public Component
{
public:
  struct TextureData
  {
    unsigned int textureID;
    std::string filePath;
    glm::vec3 tintColor;
    float alpha;
  };

  // NOTE: Currently only allows for single type for each texture 
  // from file path
  Texture(const std::string &_filePath
          , const glm::vec3 &_colorTint = {1.f, 1.f, 1.f}
          , const float &_alpha = 1.f);
  ~Texture();

  void SetTextureFilePath(const std::string &_filePath);
  // TODO: Fix to allow unique tint and opacity
  void SetTextureTint(const glm::vec3 &_colorTint);
  void SetTextureOpacity(const float &_opacity);

  const TextureData &GetTextureData();

  inline static const uint64_t TEXERR = 37873787;
private:
  TextureData *textureData;

  /*!
   *  Checks if a texture exists within the library, overriding the 
   *  texture data
   *
   *  \param _filePath
   *    The filepath used for the texture
   *
   *  \returns 
   *    If the texture was found in the library
  */
  bool CheckTextureExists(const std::string &_filePath);
  void BindTexture(const std::string &_filePath);
  void UnbindTexture();
};
}
