/*
 * Author:  Manoel McCadden
 * Data:    05-02-2025
 * Email:   manoel.mccadden@gmail.com
 * Github:  github.com/mvmccadden
 *
 * File:    g_pch.h
 *
 * Description:
 *  Provides generic standard headers that don't change often and don;t
 *  need to be recompiled to simplify compilation. This version is a special
 *  addition for the graphics libraries giving opengl and glfw access
*/
#pragma once

// TODO: Fix this to make it better than a direct path 
// that goes outside of sub folders
#include "../../external/glad/include/glad/glad.h"
#include "GLFW/glfw3.h"

#include "../../external/glm/glm.hpp"
#include "../../external/glm/gtc/matrix_transform.hpp"
#include "../../external/glm/gtc/type_ptr.hpp"

#include "../../external/includes/stb_image.h"
