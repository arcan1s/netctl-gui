# set project name
set (SUBPROJECT plasma_engine_netctl)
message (STATUS "Subproject ${SUBPROJECT}")

# find required libaries
set(QT_MIN_VERSION "5.3.0")
set(KF5_MIN_VERSION "5.0.0")
set(INSTALL_SDDM_THEME TRUE)
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Widgets Quick QuickWidgets Concurrent Test Script Network)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
                    Plasma DocTools Runner JsEmbed NotifyConfig Su NewStuff Wallet KCMUtils Baloo
                    IdleTime Declarative WebKit TextWidgets KDELibs4Support Crash)

# WARNING PlasmaQuick provides unversioned CMake config
find_package(KF5 REQUIRED COMPONENTS PlasmaQuick)
find_package(KF5 REQUIRED COMPONENTS SysGuard)
find_package(KF5 REQUIRED COMPONENTS Screen)
find_package(KF5TextEditor)
find_package(KWinDBusInterface CONFIG REQUIRED)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMPackageConfigHelpers)
include(ECMMarkNonGuiExecutable)
include(CMakePackageConfigHelpers)
include(WriteBasicConfigVersionFile)
include(CheckIncludeFiles)
include(FeatureSummary)

include_directories (/usr/include/KF5/KService
                     /usr/include/KF5/KConfigCore
                     /usr/include/KF5/KCoreAddons)
add_definitions(${Qt5Core_DEFINITIONS} ${Qt5Network_DEFINITIONS})
# add_definitions (${QT_DEFINITIONS} ${KF5_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${INTERFACE_INCLUDE_DIRECTORIES}
                     ${Plasma_INCLUDE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR}
                     ${Qt5Core_INCLUDE_DIRS}
                     ${Qt5Network_INCLUDE_DIRS})

set (PLUGIN_NAME ${SUBPROJECT})
file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file (GLOB_RECURSE SUBPROJECT_SOURCE *.cpp ${PROJECT_TRDPARTY_DIR}/task/*.cpp)
set (TASK_HEADER ${PROJECT_TRDPARTY_DIR}/task/task.h)
file (GLOB SUBPROJECT_CONF *.conf)

# prepare
configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# make
qt5_wrap_cpp (TASK_MOC_SOURCE ${TASK_HEADER})
add_library (${PLUGIN_NAME} MODULE ${SUBPROJECT_SOURCE} ${TASK_MOC_SOURCE})
target_link_libraries (${PLUGIN_NAME} KF5::Plasma ${Qt5Network_LIBRARIES})

# install
install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install (FILES ${SUBPROJECT_CONF} DESTINATION ${CONFIG_INSTALL_DIR})
