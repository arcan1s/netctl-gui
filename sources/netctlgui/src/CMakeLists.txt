# set files
file (GLOB SOURCES *.cpp)
file (GLOB HEADERS ${SUBPROJECT_INCLUDE_DIR}/${SUBPROJECT}/*.h)

message (STATUS "${SUBPROJECT} Sources: ${SOURCES}")
message (STATUS "${SUBPROJECT} Headers: ${HEADERS}")

# include_path
include_directories (${SUBPROJECT_INCLUDE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR})

if (USE_QT5)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    add_definitions(${Qt5Core_DEFINITIONS})
    add_definitions(${Qt5Widgets_DEFINITIONS})
    qt5_wrap_cpp (MOC_SOURCES ${HEADERS})

    source_group ("Header Files" FILES ${HEADERS})
    source_group ("Source Files" FILES ${SOURCES})
    source_group ("Generated Files" FILES ${MOC_SOURCES})

    include_directories (${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
    add_library (${SUBPROJECT} SHARED ${SOURCES} ${HEADERS} ${MOC_SOURCES})
    target_link_libraries (${SUBPROJECT} ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES})
else ()
    find_package (Qt4 REQUIRED)
    include (${QT_USE_FILE})
    qt4_wrap_cpp (MOC_SOURCES ${HEADERS})

    source_group ("Header Files" FILES ${HEADERS})
    source_group ("Source Files" FILES ${SOURCES})
    source_group ("Generated Files" FILES ${MOC_SOURCES})

    add_library (${SUBPROJECT} SHARED ${SOURCES} ${HEADERS} ${MOC_SOURCES})
    target_link_libraries (${SUBPROJECT} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})
endif()

# install properties
install (TARGETS ${SUBPROJECT} DESTINATION lib)
install (DIRECTORY ${SUBPROJECT_INCLUDE_DIR}/ DESTINATION include/)
