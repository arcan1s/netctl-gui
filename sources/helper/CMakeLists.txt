# set project name
set (SUBPROJECT netctlgui-helper)
message (STATUS "Subproject ${SUBPROJECT}")

# set directories
set (SUBPROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# additional targets
set (TARGETS "")
set (HEADERS "")

add_subdirectory (${SUBPROJECT_SOURCE_DIR})
install (FILES org.netctlgui.helper.conf DESTINATION ${DBUS_SYSTEMCONF_PATH})
install (FILES netctlgui-helper.service DESTINATION ${SYSTEMD_SERVICE_PATH})
