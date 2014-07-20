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
# build man
file (GLOB SUBPROJECT_MAN_IN *.1)
file (RELATIVE_PATH SUBPROJECT_MAN ${CMAKE_SOURCE_DIR} ${SUBPROJECT_MAN_IN})
configure_file (${SUBPROJECT_MAN_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_MAN})

install (FILES ${SUBPROJECT}.desktop DESTINATION share/applications/)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_MAN} DESTINATION share/man/man1/)
install (FILES bash-completions DESTINATION share/bash-completion/completions/ RENAME ${SUBPROJECT})
install (FILES zsh-completions DESTINATION share/zsh/site-functions/ RENAME _${SUBPROJECT})
