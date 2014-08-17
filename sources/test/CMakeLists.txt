# set project name
set (SUBPROJECT netctlgui-test)
message (STATUS "Subproject ${SUBPROJECT}")

# set directories
set (SUBPROJECT_BINARY_DIR bin)

# additional targets
set (TARGETS "")

# set files
# netctl
set (NETCTL_HEADERS testnetctl.h)
set (NETCTL_SOURCES testnetctl.cpp)
# netctl-auto
set (NETCTLAUTO_HEADERS testnetctlauto.h)
set (NETCTLAUTO_SOURCES testnetctlauto.cpp)
# netctl profile
set (PROFILE_HEADERS testnetctlprofile.h)
set (PROFILE_SOURCES testnetctlprofile.cpp)

# include_path
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_LIBRARY}/include/
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
    qt5_wrap_cpp (NETCTL_MOC_SOURCES ${NETCTL_HEADERS})
    qt5_wrap_cpp (NETCTLAUTO_MOC_SOURCES ${NETCTLAUTO_HEADERS})
    qt5_wrap_cpp (PROFILE_MOC_SOURCES ${PROFILE_HEADERS})
else ()
    find_package (Qt4 COMPONENTS QtCore QtTest REQUIRED)
    include (${QT_USE_FILE})
    set (QT_NEEDED_LIBS ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
    qt4_wrap_cpp (NETCTL_MOC_SOURCES ${NETCTL_HEADERS})
    qt4_wrap_cpp (NETCTLAUTO_MOC_SOURCES ${NETCTLAUTO_HEADERS})
    qt4_wrap_cpp (PROFILE_MOC_SOURCES ${PROFILE_HEADERS})
endif()

# netctl
add_executable (${SUBPROJECT}-netctl ${NETCTL_HEADERS} ${NETCTL_SOURCES} ${NETCTL_MOC_SOURCES})
target_link_libraries (${SUBPROJECT}-netctl ${PROJECT_LIBRARY} ${QT_NEEDED_LIBS})
# netctl-auto
add_executable (${SUBPROJECT}-netctlauto ${NETCTLAUTO_HEADERS} ${NETCTLAUTO_SOURCES} ${NETCTLAUTO_MOC_SOURCES})
target_link_libraries (${SUBPROJECT}-netctlauto ${PROJECT_LIBRARY} ${QT_NEEDED_LIBS})
# netctl profile
add_executable (${SUBPROJECT}-profile ${PROFILE_HEADERS} ${PROFILE_SOURCES} ${PROFILE_MOC_SOURCES})
target_link_libraries (${SUBPROJECT}-profile ${PROJECT_LIBRARY} ${QT_NEEDED_LIBS})

# install properties
add_test (NAME Netctl COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-netctl
          "-o" "../Testing/output-netctl.log")
add_test (NAME NetctlAuto COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-netctlauto
          "-o" "../Testing/output-netctlauto.log")
add_test (NAME Profile COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT}-profile
          "-o" "../Testing/output-profile.log")

