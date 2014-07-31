# include files
install (FILES ${HEADERS} DESTINATION include/${SUBPROJECT})

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
install (FILES ${MAN_DIR}/Reportabug.3 DESTINATION share/man/man3 RENAME ${SUBPROJECT}_Reportabug.3)
install (FILES ${MAN_DIR}/GithubModule.3 DESTINATION share/man/man3 RENAME ${SUBPROJECT}_GithubModule.3)
install (FILES ${MAN_DIR}/GitreportModule.3 DESTINATION share/man/man3 RENAME ${SUBPROJECT}_GitreportModule.3)
# html docs
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html DESTINATION share/doc/${SUBPROJECT})
