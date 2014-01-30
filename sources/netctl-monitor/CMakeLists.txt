# set project name
set (SUBPROJECT plasma_applet_netctl-monitor)

# find required libaries
find_package (KDE4 REQUIRED)
include (KDE4Defaults)

add_definitions (${QT_DEFINITIONS}
                 ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${KDE4_INCLUDES}
                     ${CMAKE_CURRENT_BINARY_DIR}/../)

# set sources
set (PLUGIN_NAME ${SUBPROJECT})
file (GLOB SUBPROJECT_DESKTOP *.desktop)
file (GLOB SUBPROJECT_ICON *.png)
file (GLOB SUBPROJECT_NOTIFY *.notifyrc)
file (GLOB SUBPROJECT_SOURCE *.cpp)
file (GLOB SUBPROJECT_UI *.ui)

# make
kde4_add_ui_files (SUBPROJECT_SOURCE ${SUBPROJECT_UI})
kde4_add_plugin (${PLUGIN_NAME} ${SUBPROJECT_SOURCE})
target_link_libraries (${PLUGIN_NAME} ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})

# install
install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES ${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install (FILES ${SUBPROJECT_ICON} DESTINATION ${ICON_INSTALL_DIR})
install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${DATA_INSTALL_DIR}/${PLUGIN_NAME})
