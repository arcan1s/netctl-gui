# set files
file (GLOB SOURCES *.cpp)
file (GLOB HEADERS *.h)

# include_path
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_LIBRARY}/include/
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR})
link_directories (${PROJECT_LIBRARY}/src/lib)

if (USE_QT5)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5DBus REQUIRED)
    add_definitions(${Qt5Core_DEFINITIONS})
    add_definitions(${Qt5DBus_DEFINITIONS})
    include_directories (${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
    set (QT_NEEDED_LIBS ${Qt5Core_LIBRARIES} ${Qt5DBus_LIBRARIES})
    qt5_wrap_cpp (MOC_SOURCES ${HEADERS})
else ()
    find_package (Qt4 COMPONENTS QtCore QtDBus REQUIRED)
    include (${QT_USE_FILE})
    set (QT_NEEDED_LIBS ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY})
    qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
endif()

add_executable (${SUBPROJECT} ${SOURCES} ${HEADERS} ${MOC_SOURCES})
target_link_libraries (${SUBPROJECT} ${PROJECT_LIBRARY} ${QT_NEEDED_LIBS})
# install properties
install (TARGETS ${SUBPROJECT} DESTINATION bin
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID)
