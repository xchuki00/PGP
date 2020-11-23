cmake_minimum_required(VERSION 3.1.0)

PROJECT(WaterSurfaceProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)


SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_C_STANDARD 99)

list(APPEND this_project_subprojects geCore geGL geUtil geAd)


IF(CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(${APPLICATION_NAME} -W)
  add_compile_options(${APPLICATION_NAME} -Wall)
  add_compile_options(${APPLICATION_NAME} -Wconversion)
  add_compile_options(${APPLICATION_NAME} -pedantic)
ENDIF()


set(_subprojects geCore geGL geUtil geAd)
if("${as_subproject}" STREQUAL "")
  #IN_LIST is operator in CMake 3.3, CMake 3.8.1 warns 
  cmake_policy(SET CMP0057 NEW)
  list(APPEND as_subproject ${${LIB_NAME}_subprojects})
  macro(find_package name)
    if(NOT "${name}" IN_LIST as_subproject)
      _find_package(${ARGV})
    endif()
  endmacro()
else()
  list(APPEND as_subproject ${${LIB_NAME}_subprojects})
endif()

set(GECORE_DYNAMIC FALSE CACHE BOOL "")
set(GEGL_DYNAMIC FALSE CACHE BOOL "")
set(GEUTIL_DYNAMIC FALSE CACHE BOOL "")
set(GEAD_DYNAMIC FALSE CACHE BOOL "")
set(SDLWINDOW_DYNAMIC FALSE CACHE BOOL "")
add_subdirectory(geCore)
add_subdirectory(geGL)
add_subdirectory(geUtil)
add_subdirectory(geAd)

set(APP_SOURCES
  src/controler.cpp
  src/LightSource.cpp
  src/Model.cpp
  src/water.cpp
  src/WaterSurface.cpp
  )

set(APP_INCLUDES
  src/BaseShaders.h
  src/controler.h
  src/LightSource.h
  src/Model.h
  src/Models.h
  src/RRShaders.h
  src/sphere.h
  src/type.h
  src/water.h
  src/WaterShaders.h
  
  
  )
add_executable(WaterSurface ${APP_SOURCES} ${APP_INCLUDES})
set(Internal_inc
  ${WaterSurfaceProject_SOURCE_DIR}/include
  ${WaterSurfaceProject_SOURCE_DIR}
  )

target_link_libraries(WaterSurface geGL::geGL geUtil::geUtil SDLWindow::SDLWindow)
set_target_properties(WaterSurface PROPERTIES INCLUDE_DIRECTORIES "${Internal_inc}"
  )
