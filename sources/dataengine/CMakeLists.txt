# set project name
set (SUBPROJECT plasma_engine_netctl)

# find required libaries
find_package (KDE4 REQUIRED)
include (KDE4Defaults)

add_definitions (${QT_DEFINITIONS}
                 ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${KDE4_INCLUDES}
                     ${CMAKE_CURRENT_BINARY_DIR}/../)

set (PLUGIN_NAME ${SUBPROJECT})
file (GLOB SUBPROJECT_DESKTOP *.desktop)
file (GLOB SUBPROJECT_SOURCE *.cpp)
file (GLOB SUBPROJECT_CONF *.conf)

# make
kde4_add_plugin (${PLUGIN_NAME} ${SUBPROJECT_SOURCE})
target_link_libraries (${PLUGIN_NAME} ${KDE4_KDECORE_LIBS} ${KDE4_PLASMA_LIBS})

# install
install (TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES ${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install (FILES ${SUBPROJECT_CONF} DESTINATION ${CONFIG_INSTALL_DIR})
