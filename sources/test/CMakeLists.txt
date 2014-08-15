enable_testing ()

# set project name
set (SUBPROJECT netctlgui-test)
message (STATUS "Subproject ${SUBPROJECT}")

# set directories
set (SUBPROJECT_BINARY_DIR bin)

# additional targets
set (TARGETS "")
set (HEADERS "")

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
    find_package(Qt5Test REQUIRED)
    add_definitions(${Qt5Core_DEFINITIONS})
    add_definitions(${Qt5Test_DEFINITIONS})
    include_directories (${Qt5Core_INCLUDE_DIRS} ${Qt5Test_INCLUDE_DIRS})
    set (QT_NEEDED_LIBS ${Qt5Core_LIBRARIES} ${Qt5Test_LIBRARIES})
    qt5_wrap_cpp (MOC_SOURCES ${HEADERS})
else ()
    find_package (Qt4 COMPONENTS QtCore QtTest REQUIRED)
    include (${QT_USE_FILE})
    set (QT_NEEDED_LIBS ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
    qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
endif()

add_executable (${SUBPROJECT} ${HEADERS} ${SOURCES} ${MOC_SOURCES})
target_link_libraries (${SUBPROJECT} ${PROJECT_LIBRARY} ${QT_NEEDED_LIBS})
# install properties
add_test (BaseTest ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT})

