cmake_minimum_required (VERSION 2.8)

cmake_policy (SET CMP0003 OLD)
cmake_policy (SET CMP0002 OLD)
cmake_policy (SET CMP0011 NEW)
cmake_policy (SET CMP0015 NEW)

project (netctl-gui)
set (PROJECT_AUTHOR "Evgeniy Alekseev")
set (PROJECT_CONTACT "esalexeev@gmail.com")
set (PROJECT_LICENSE "GPLv3")
set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 2)
set (PROJECT_VERSION_PATCH 0)
set (PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
string (TIMESTAMP CURRENT_DATE "%Y-%m-%d %H:%M" UTC)
string (TIMESTAMP CURRENT_YEAR "%Y")

message (STATUS "Project: ${PROJECT_NAME}")
message (STATUS "Version: ${PROJECT_VERSION}")
message (STATUS "Build date: ${CURRENT_DATE}")

# install options
option (USE_QT5 "Use Qt5 instead of Qt4" ON)
# components
option (BUILD_GUI "Build GUI" ON)
option (BUILD_LIBRARY "Build library" ON)
option (BUILD_DATAENGINE "Build data engine" ON)
option (BUILD_PLASMOID "Build plasmoid" ON)
if (BUILD_GUI)
    set (BUILD_LIBRARY ON)
endif ()
if (BUILD_PLASMOID)
    set (BUILD_DATAENGINE ON)
endif ()
# documentation
option (BUILD_DOCS "Build documentation and install headers" ON)
if (BUILD_DOCS)
    set (BUILD_LIBRARY ON)
endif ()

# flags
if (CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_CXX_FLAGS "-Wall -std=c++11")
    set (CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
else ()
    message (STATUS "Unknown compiler")
endif ()

configure_file (${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# resources
set (PROJECT_LIBRARY netctlgui)
set (PROJECT_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
add_subdirectory (${PROJECT_RESOURCE_DIR})

# components
if (BUILD_LIBRARY)
    add_subdirectory (${PROJECT_LIBRARY})
endif()
if (BUILD_GUI)
    add_subdirectory (gui)
endif ()
if (BUILD_DATAENGINE)
    add_subdirectory (dataengine)
endif ()
if (BUILD_PLASMOID)
    add_subdirectory (plasmoid)
endif ()
