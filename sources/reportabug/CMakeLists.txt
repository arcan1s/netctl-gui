cmake_minimum_required (VERSION 2.8)

cmake_policy (SET CMP0003 OLD)
cmake_policy (SET CMP0002 OLD)
cmake_policy (SET CMP0011 NEW)
cmake_policy (SET CMP0015 NEW)
cmake_policy (SET CMP0048 OLD)

project (reportabug)

set (SUBPROJECT reportabug)
set (SUBPROJECT_AUTHOR "Evgeniy Alekseev")
set (SUBPROJECT_CONTACT "esalexeev@gmail.com")
set (SUBPROJECT_LICENSE "LGPLv3")
set (SUBPROJECT_VERSION_MAJOR 1)
set (SUBPROJECT_VERSION_MINOR 0)
set (SUBPROJECT_VERSION_PATCH 2)
set (SUBPROJECT_VERSION ${SUBPROJECT_VERSION_MAJOR}.${SUBPROJECT_VERSION_MINOR}.${SUBPROJECT_VERSION_PATCH})
string (TIMESTAMP CURRENT_DATE "%Y-%m-%d %H:%M" UTC)
string (TIMESTAMP CURRENT_YEAR "%Y")

message (STATUS "Subproject: ${SUBPROJECT}")
message (STATUS "Version: ${SUBPROJECT_VERSION}")
message (STATUS "Build date: ${CURRENT_DATE}")

# install options
set (BUILD_AS_LIBRARY ON CACHE BOOL "Build the application as a shared library")
set (BUILD_DOCS OFF CACHE BOOL "Build developers documentation")
set (ENABLE_GITHUB ON CACHE BOOL "Enable GitHub module")
set (ENABLE_GITREPORT ON CACHE BOOL "Enable GitReport module")
set (USE_QT5 ON CACHE BOOL "Use Qt5 instead of Qt4")
set (OWN_GITHUB_TOKEN "" CACHE STRING "Use own GitHub token instead of GitReport")
# set flags
if (OWN_GITHUB_TOKEN STREQUAL "")
    set (USE_OWN_TOKEN OFF)
else ()
    set (USE_OWN_TOKEN ON)
endif ()
if (USE_OWN_TOKEN)
    set (ENABLE_GITREPORT OFF)
endif ()

# flags
if (CMAKE_COMPILER_IS_GNUCXX)
    set (ADD_CXX_FLAGS  "-Wall")
    set (CMAKE_CXX_FLAGS "-O0 ${ADD_CXX_FLAGS}")
    set (CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
else ()
    message (STATUS "Unknown compiler")
endif ()

configure_file (${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# build
# set directories
set (SUBPROJECT_BINARY_DIR bin)
set (SUBPROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
# executable path
set (EXECUTABLE_OUTPUT_PATH ${SUBPROJECT_BINARY_DIR})

# additional targets
set (TARGETS "")
set (HEADERS "")

add_subdirectory (${SUBPROJECT_SOURCE_DIR})

if (BUILD_DOCS)
    include (docs.cmake)
endif ()
if (NOT BUILD_AS_LIBRARY)
    install (FILES ${SUBPROJECT}.desktop DESTINATION share/applications/)
endif ()
