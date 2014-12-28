# set project name
set (SUBPROJECT plasma_applet_netctl)
message (STATUS "Subproject ${SUBPROJECT}")
# find resources
# set (RESOURCES ${PROJECT_RESOURCE_DIR}/resources-plasmoid.qrc)
#
# # find required libaries
# find_package (KDE4 REQUIRED)
# include (KDE4Defaults)
#
# add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
# include_directories (${CMAKE_SOURCE_DIR}
#                      ${CMAKE_BINARY_DIR}
#                      ${KDE4_INCLUDES}
#                      ${CMAKE_CURRENT_BINARY_DIR}
#                      ${CMAKE_CURRENT_BINARY_DIR}/../
#                      ${PROJECT_TRDPARTY_DIR})
#
# # add_subdirectory (po)
#
# # set sources
# set (PLUGIN_NAME ${SUBPROJECT})
# file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
# file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
# file (GLOB SUBPROJECT_NOTIFY *.notifyrc)
# file (GLOB SUBPROJECT_SOURCE *.cpp)
# file (GLOB_RECURSE SUBPROJECT_UI *.ui ${PROJECT_TRDPARTY_DIR}/about/*.ui)
#
# # prepare
# configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})
#
# # make
# qt4_add_resources (QRC_SOURCES ${RESOURCES})
# kde4_add_ui_files (SUBPROJECT_SOURCE ${SUBPROJECT_UI})
# kde4_add_plugin (${PLUGIN_NAME} ${SUBPROJECT_SOURCE} ${QRC_SOURCES})
# target_link_libraries (${PLUGIN_NAME} ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})
#
# # install
# install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
# install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
# install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${DATA_INSTALL_DIR}/${PLUGIN_NAME})
# plasma_install_package(plasmoid org.kde.netctl)
find_package (Qt5 REQUIRED COMPONENTS Core Qml Widgets)
find_package (ECM 0.0.12 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
find_package(KF5 REQUIRED I18n Service WidgetsAddons KIO CoreAddons Notifications Plasma)

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings)
add_definitions (${Qt5Core_DEFINITIONS} ${Qt5Qml_DEFINITIONS})

set (Qt_INCLUDE ${Qt5Core_INCLUDE_DIRS}
                ${Qt5Qml_INCLUDE_DIRS})
set (Kf5_INCLUDE ${ConfigCore_INCLUDE_DIR}
                 ${CoreAddons_INCLUDE_DIR}
                 ${I18n_INCLUDE_DIR}
                 ${Notifications_INCLUDE_DIR}
                 ${Plasma_INCLUDE_DIR}
                 ${Service_INCLUDE_DIR})

set (PLUGIN_NAME netctlplugin)
add_subdirectory (plugin)

set(dir plasmoid)
set(component org.kde.netctl)
set(root ${ARGV2})
   set(type ${ARGV3})
   if(NOT root)
      set(root plasmoids)
   endif()
   if(NOT type)
      set(type applet)
   endif()
   install(DIRECTORY ${dir}/ DESTINATION ${PLASMA_DATA_INSTALL_DIR}/${root}/${component}
           PATTERN CMakeLists.txt EXCLUDE
           PATTERN README.txt EXCLUDE
           PATTERN Messages.sh EXCLUDE
           PATTERN dummydata EXCLUDE)

   install(FILES ${dir}/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-${type}-${component}.desktop)
