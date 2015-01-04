include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${PROJECT_TRDPARTY_DIR}
                     ${Qt_INCLUDE}
                     ${Kf5_INCLUDE})

file (GLOB SUBPROJECT_SOURCE *.cpp)
file (GLOB SUBPROJECT_NOTIFY *.notifyrc)

add_library (${PLUGIN_NAME} SHARED ${SUBPROJECT_SOURCE})
target_link_libraries (${PLUGIN_NAME} ${Qt5Core_LIBRARIES} ${Qt5DBus_LIBRARIES} ${Qt5Qml_LIBRARIES} ${I18n_LIBRARIES} ${Plasma_LIBRARIES}
                                      KF5::I18n KF5::Notifications)

install (TARGETS ${PLUGIN_NAME} DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/netctl)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/netctl)
install (FILES ${SUBPROJECT_NOTIFY} DESTINATION ${KNOTIFYRC_INSTALL_DIR})
