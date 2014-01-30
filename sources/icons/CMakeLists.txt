# set project name
set (SUBPROJECT netctl-gui-icons)
message (STATUS "Subproject ${SUBPROJECT}")

install (FILES network-idle-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME netctl-idle.png)
install (FILES network-offline-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME netctl-offline.png)
install (FILES network-idle-128-128.png DESTINATION share/pixmaps RENAME netctl-gui.png)
