cmake_minimum_required (VERSION 2.8)

cmake_policy (SET CMP0003 OLD)
cmake_policy (SET CMP0002 OLD)
cmake_policy (SET CMP0011 NEW)
cmake_policy (SET CMP0015 NEW)

project (netctl-gui)
set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 0)
set (PROJECT_VERSION_PATCH 5)
set (PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
configure_file (${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

message (STATUS "Version: ${PROJECT_VERSION}")

# install options
option (USE_QT5 "Use Qt5 instead of Qt4" ON)
option (BUILD_GUI "Build GUI" ON)
option (BUILD_DATAENGINE "Build data engine" ON)
option (BUILD_PLASMOID "Build plasmoid" ON)

# flags
if (CMAKE_COMPILER_IS_GNUCXX)
  set (ADD_CXX_FLAGS  "-Wall")
  set (CMAKE_CXX_FLAGS "-O0 ${ADD_CXX_FLAGS}")
  set (CMAKE_CXX_FLAGS_DEBUG "-g -O0")
  set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
else ()
  message (STATUS "Unknown compiler")
endif ()

add_subdirectory (icons)

if (BUILD_PLASMOID)
  set (BUILD_DATAENGINE ON)
endif ()

if (BUILD_GUI)
  add_subdirectory (gui)
endif ()
if (BUILD_DATAENGINE)
  add_subdirectory (dataengine)
endif ()
if (BUILD_PLASMOID)
  add_subdirectory (plasmoid)
endif ()
