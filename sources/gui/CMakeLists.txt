# set project name
set (SUBPROJECT netctl-gui)
message (STATUS "Subproject ${SUBPROJECT}")

# set directories
set (SUBPROJECT_BINARY_DIR bin)
set (SUBPROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (SUBPROJECT_RESOURCE_DIR ${PROJECT_RESOURCE_DIR})
set (SUBPROJECT_TRANSLATION_DIR ${SUBPROJECT_RESOURCE_DIR}/translations)
# executable path
set (EXECUTABLE_OUTPUT_PATH ${SUBPROJECT_BINARY_DIR})

# additional targets
set (TARGETS "")
set (HEADERS "")

add_subdirectory (${SUBPROJECT_SOURCE_DIR})

install (FILES ${SUBPROJECT}.desktop DESTINATION share/applications/)
