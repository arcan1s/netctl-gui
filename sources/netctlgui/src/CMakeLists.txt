# set files
file (GLOB SOURCES *.cpp)
file (GLOB HEADERS ${SUBPROJECT_INCLUDE_DIR}/${SUBPROJECT}/*.h)

# include_path
include_directories (${SUBPROJECT_INCLUDE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR})

if (USE_QT5)
    find_package(Qt5Core REQUIRED)
    add_definitions(${Qt5Core_DEFINITIONS})
    include_directories (${Qt5Core_INCLUDE_DIRS})
    set (QT_NEEDED_LIBS ${Qt5Core_LIBRARIES})
    qt5_wrap_cpp (MOC_SOURCES ${HEADERS})
else ()
    find_package (Qt4 REQUIRED)
    include (${QT_USE_FILE})
    set (QT_NEEDED_LIBS ${QT_QTCORE_LIBRARY})
    qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
endif()

add_library (${SUBPROJECT} SHARED ${SOURCES} ${HEADERS} ${MOC_SOURCES})
target_link_libraries (${SUBPROJECT} ${QT_NEEDED_LIBS})
# install properties
install (TARGETS ${SUBPROJECT} DESTINATION lib)
