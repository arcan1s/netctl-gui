# doxygen documentation
find_package (Doxygen)
if (NOT DOXYGEN_FOUND)
    message (STATUS "WARNING: Doxygen not found - Reference manual will not be created")
    return ()
endif ()

configure_file (doxygen.conf.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf)
add_custom_target (oxygen-docs ALL COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf)

# man pages
# workaround for man pages
set (MAN_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/man/man3)
install (FILES ${MAN_DIR}/Netctl.3 DESTINATION share/man/man3 RENAME ${SUBPROJECT}_Netctl.3)
install (FILES ${MAN_DIR}/NetctlProfile.3 DESTINATION share/man/man3 RENAME ${SUBPROJECT}_NetctlProfile.3)
install (FILES ${MAN_DIR}/WpaSup.3 DESTINATION share/man/man3 RENAME ${SUBPROJECT}_WpaSup.3)
# html docs
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html DESTINATION share/doc/${PROJECT_NAME})
