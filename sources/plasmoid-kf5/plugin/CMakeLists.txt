set (PLUGIN_NAME netctlplugin)
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.netctl\")

add_definitions (${Qt5Core_DEFINITIONS} ${Qt5DBus_DEFINITIONS} ${Qt5Qml_DEFINITIONS})
set (Qt_INCLUDE ${Qt5Core_INCLUDE_DIRS}
                ${Qt5DBus_INCLUDE_DIRS}
                ${Qt5Qml_INCLUDE_DIRS})
set (Qt_LIBRARIES ${Qt5Core_LIBRARIES}
                  ${Qt5DBus_LIBRARIES}
                  ${Qt5Qml_LIBRARIES})
set (Kf5_INCLUDE ${I18n_INCLUDE_DIR}
                 ${Notifications_INCLUDE_DIR})
set (Kf5_LIBRARIES KF5::I18n
                   KF5::Notifications)

include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR}
                     ${Qt_INCLUDE}
                     ${Kf5_INCLUDE})

file (GLOB_RECURSE SUBPROJECT_SOURCE ${PROJECT_TRDPARTY_DIR}/pdebug/*.cpp
                                     *.cpp)
file (GLOB SUBPROJECT_NOTIFY *.notifyrc)

add_library (${PLUGIN_NAME} SHARED ${SUBPROJECT_SOURCE})
target_link_libraries (${PLUGIN_NAME} ${Qt_LIBRARIES} ${Kf5_LIBRARIES})

install (TARGETS ${PLUGIN_NAME} DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/netctl)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/netctl)
install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${KNOTIFYRC_INSTALL_DIR})
