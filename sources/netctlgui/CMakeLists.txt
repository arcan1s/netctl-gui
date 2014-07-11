# set project name
set (SUBPROJECT ${PROJECT_LIBRARY})
message (STATUS "Subproject ${SUBPROJECT}")

# set directories
set (SUBPROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (SUBPROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# additional targets
set (TARGETS "")
set (HEADERS "")

add_subdirectory (${SUBPROJECT_SOURCE_DIR})
