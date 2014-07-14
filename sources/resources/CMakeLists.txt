# set project name
set (SUBPROJECT netctl-gui-resources)
message (STATUS "Subproject ${SUBPROJECT}")

if (BUILD_GUI)
  install (FILES icon.png DESTINATION share/pixmaps RENAME netctl-gui.png)
endif ()
if (BUILD_DATAENGINE OR BUILD_PLASMOID)
  install (FILES icon-plasmoid.png DESTINATION share/pixmaps RENAME netctl-gui-widget.png)
  install (FILES network-idle-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME netctl-idle.png)
  install (FILES network-offline-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME netctl-offline.png)
endif ()
