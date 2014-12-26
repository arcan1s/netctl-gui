# set project name
set (SUBPROJECT plasma_engine_netctl)
message (STATUS "Subproject ${SUBPROJECT}")

# find required libaries
find_package (Qt5 REQUIRED COMPONENTS Widgets Network)
find_package (ECM 0.0.11 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
find_package(KF5 REQUIRED COMPONENTS Plasma Service)

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings)
add_definitions (${Qt5Core_DEFINITIONS} ${Qt5Network_DEFINITIONS})

set (Qt_INCLUDE ${Qt5Core_INCLUDE_DIRS}
                ${Qt5Network_INCLUDE_DIRS})
set (Kf5_INCLUDE ${ConfigCore_INCLUDE_DIR}
                 ${CoreAddons_INCLUDE_DIR}
                 ${Plasma_INCLUDE_DIR}
                 ${Service_INCLUDE_DIR})
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR}
                     ${Qt_INCLUDE}
                     ${Kf5_INCLUDE})

set (PLUGIN_NAME ${SUBPROJECT})
file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
message (STATUS ${SUBPROJECT_DESKTOP_IN})
file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file (GLOB_RECURSE SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/task/*.cpp)
set (TASK_HEADER ${PROJECT_TRDPARTY_DIR}/task/task.h)
file (GLOB SUBPROJECT_CONF *.conf)

# prepare
configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# make
qt5_wrap_cpp (TASK_MOC_SOURCE ${TASK_HEADER})
add_library (${PLUGIN_NAME} MODULE ${SUBPROJECT_SOURCE} ${TASK_MOC_SOURCE})
target_link_libraries (${PLUGIN_NAME} ${Plasma_LIBRARIES} ${Qt5Network_LIBRARIES})
kcoreaddons_desktop_to_json (${PLUGIN_NAME} ${SUBPROJECT_DESKTOP_IN})
# kcoreaddons_desktop_to_json (${PLUGIN_NAME} ${SUBPROJECT_DESKTOP})

# install
install (TARGETS ${PLUGIN_NAME} DESTINATION lib/qt/plugins/plasma/dataengine)
# install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/dataengine)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install (FILES ${SUBPROJECT_CONF} DESTINATION ${CONFIG_INSTALL_DIR})
