cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(deus VERSION 0.1.0 LANGUAGES CXX)

# Common
include(third_party/compat/compat.cmake)

# Packages
find_package(glew REQUIRED PATHS third_party/glew)
find_package(glfw REQUIRED PATHS third_party/glfw)
find_package(glm  REQUIRED PATHS third_party/glm)
find_package(gsl  REQUIRED PATHS third_party/gsl)
find_package(jpeg REQUIRED PATHS third_party/jpeg)
find_package(png  REQUIRED PATHS third_party/png)
find_package(utf8 REQUIRED PATHS third_party/utf8)

# Configuration
set(COMPANY_TEXT "Xiphos")
set(PROJECT_TEXT "DEUS")
set(COPYING_TEXT "2016 Alexej Harm")

# Soureces
file(GLOB include src)
file(GLOB_RECURSE sources src/*.h src/*.cpp res/*.js res/html/*)

# Resources
file(GLOB_RECURSE resources res/data/*)

if(WIN32)
  list(APPEND include res)
  foreach(file ${resources})
    file(RELATIVE_PATH name ${CMAKE_CURRENT_SOURCE_DIR}/res/data ${file})
    set(DATA_RESOURCES "${DATA_RESOURCES}${name} RCDATA \"${file}\"\n")
  endforeach()
  configure_file(res/resource.rc.in ${CMAKE_CURRENT_BINARY_DIR}/resource.rc)
  list(APPEND sources ${resources} res/resource.h res/resource.manifest)
  set(resources ${CMAKE_CURRENT_BINARY_DIR}/resource.rc)
  source_group(res FILES ${resources})
endif()

# Application
assign_source_group(${sources})
add_executable(${PROJECT_NAME} WIN32 ${sources} ${resources})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME $<LOWER_CASE:${PROJECT_NAME}>)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${include})
target_link_libraries(${PROJECT_NAME} PRIVATE glew glfw glm gsl jpeg png utf8)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Visual Studio
if(MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()