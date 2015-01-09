find_package(Gettext REQUIRED)
if (NOT GETTEXT_MSGFMT_EXECUTABLE)
   message(FATAL_ERROR "Please install the msgfmt binary")
endif (NOT GETTEXT_MSGFMT_EXECUTABLE)

if (BUILD_KDE4)
  find_package(KDE4 REQUIRED)
  set (MO_NAME plasma_applet_netctl.mo)
else ()
  find_package (ECM 0.0.12 REQUIRED NO_MODULE)
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
  find_package(KF5 REQUIRED COMPONENTS I18n)
  include (KDEInstallDirs)
  set (MO_NAME plasma_applet_org.kde.plasma.netctl.mo)
endif ()

file (GLOB _po_files *.po)
set (_gmoFiles)

foreach (_current_PO_FILE ${_po_files})
  get_filename_component (_lang ${_current_PO_FILE} NAME_WE)
   set (_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)
   add_custom_command (OUTPUT ${_gmoFile}
         COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
         DEPENDS ${_current_PO_FILE}
      )

   install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo DESTINATION ${LOCALE_INSTALL_DIR}/${_lang}/LC_MESSAGES/ RENAME ${MO_NAME})
   list (APPEND _gmoFiles ${_gmoFile})
endforeach (_current_PO_FILE)
add_custom_target (pofiles ALL DEPENDS ${_gmoFiles})
