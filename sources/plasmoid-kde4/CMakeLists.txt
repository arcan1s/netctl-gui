# set project name
set (SUBPROJECT plasma_applet_netctl)
message (STATUS "Subproject ${SUBPROJECT}")
# find resources
set (RESOURCES ${PROJECT_RESOURCE_DIR}/resources-plasmoid.qrc)

# find required libaries
find_package (KDE4 REQUIRED)
include (KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${KDE4_INCLUDES}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR})

# set sources
set (PLUGIN_NAME ${SUBPROJECT})
file (GLOB SUBPROJECT_DESKTOP_IN *.desktop)
file (RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file (GLOB SUBPROJECT_NOTIFY *.notifyrc)
file (GLOB SUBPROJECT_SOURCE *.cpp)
file (GLOB_RECURSE SUBPROJECT_UI *.ui ${PROJECT_TRDPARTY_DIR}/about/*.ui)

# prepare
configure_file (${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# make
qt4_add_resources (QRC_SOURCES ${RESOURCES})
kde4_add_ui_files (SUBPROJECT_SOURCE ${SUBPROJECT_UI})
kde4_add_plugin (${PLUGIN_NAME} ${SUBPROJECT_SOURCE} ${QRC_SOURCES})
target_link_libraries (${PLUGIN_NAME} ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

# install
install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${DATA_INSTALL_DIR}/${PLUGIN_NAME})
