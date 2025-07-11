cmake_minimum_required(VERSION 3.10)
project(ClarityPlusPLus LANGUAGES CXX C)

# Set the c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Set opengl preference
set(OpenGL_GL_PREFERENCE GLVND)

# Search for OpenGL library 
find_package(OpenGL REQUIRED)
find_package(Lua REQUIRED)

message(STATUS "Lua Library Found: " ${LUA_LIBRARY})
message(STATUS "Lua Include Directory Found: " ${LUA_INCLUDE_DIR})

# Add the glfw build as a subdirectory to be build
add_subdirectory(external/glfw)

# Create a file glob with all cpp files in the source directory
# and all sub-directories
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "clapp_src/*.cpp")

# Create a program build
add_executable(ClarityEngine 
  ${SOURCE_FILES}
  external/glad/src/glad.c
)

# Set include directory
target_include_directories(ClarityEngine PRIVATE 
  ${LUA_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/includes
  ${CMAKE_CURRENT_SOURCE_DIR}/clapp_src/clapp_includes
)

# Link the libraries
target_link_libraries(ClarityEngine PRIVATE
  lua
  glfw
  OpenGL
)
