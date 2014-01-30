# set project name
set (SUBPROJECT netctl-gui)
# set directories
set (SUBPROJECT_BINARY_DIR bin)
set (SUBPROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
# executable path
set (EXECUTABLE_OUTPUT_PATH ${SUBPROJECT_BINARY_DIR})

# additional targets
set (TARGETS "")
set (HEADERS "")

add_subdirectory (${SUBPROJECT_SOURCE_DIR})

install (FILES ${SUBPROJECT}.desktop DESTINATION share/applications/)
install (FILES ${SUBPROJECT}-logo.png DESTINATION share/pixmaps/)
install (FILES ${SUBPROJECT}.png DESTINATION share/icons/hicolor/32x32/apps/)
