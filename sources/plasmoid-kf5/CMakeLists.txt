# set project name
set (SUBPROJECT plasma_applet_netctl)
message (STATUS "Subproject ${SUBPROJECT}")

find_package (Qt5 REQUIRED COMPONENTS Core DBus Qml Widgets)
find_package (ECM 0.0.12 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
find_package(KF5 REQUIRED COMPONENTS I18n Service WidgetsAddons KIO CoreAddons Notifications Plasma)
find_package(KF5I18n CONFIG REQUIRED)

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings)
add_definitions (${Qt5Core_DEFINITIONS} ${Qt5DBus_DEFINITIONS} ${Qt5Qml_DEFINITIONS})

set (Qt_INCLUDE ${Qt5Core_INCLUDE_DIRS}
                ${Qt5DBus_INCLUDE_DIRS}
                ${Qt5Qml_INCLUDE_DIRS})
set (Kf5_INCLUDE ${ConfigCore_INCLUDE_DIR}
                 ${CoreAddons_INCLUDE_DIR}
                 ${I18n_INCLUDE_DIR}
                 ${INTERFACE_INCLUDE_DIRECTORIES}
                 ${Notifications_INCLUDE_DIR}
                 ${Plasma_INCLUDE_DIR}
                 ${Service_INCLUDE_DIR})

set (PLUGIN_NAME netctlplugin)
add_subdirectory (plugin)
plasma_install_package (plasmoid org.kde.plasma.netctl)
