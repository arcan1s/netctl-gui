# set files
file (GLOB SOURCES *.cpp)
file (GLOB HEADERS *.h)
file (GLOB FORMS *.ui)

set (LANGUAGES english
               russian)
set (RESOURCES ${SUBPROJECT_RESOURCE_DIR}/resources.qrc)

message (STATUS "${SUBPROJECT} Sources: ${SOURCES}")
message (STATUS "${SUBPROJECT} Headers: ${HEADERS}")
message (STATUS "${SUBPROJECT} Forms: ${FORMS}")

# include_path
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})
qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_wrap_ui (UI_HEADERS ${FORMS})
qt4_add_resources (QRC_SOURCES ${RESOURCES})

foreach (LANGUAGE ${LANGUAGES})
    set (TS ${SUBPROJECT_TRANSLATION_DIR}/${LANGUAGE}.ts)
    set (QM ${SUBPROJECT_TRANSLATION_DIR}/${LANGUAGE}.qm)
    set (TRANSLATIONS ${TRANSLATIONS} ${TS})
    set (TRANSLATIONS_BINARY ${TRANSLATIONS_BINARY} ${QM})
    add_custom_command (OUTPUT ${QM} COMMAND ${QT_LRELEASE_EXECUTABLE} ${TS} MAIN_DEPENDENCY ${TS})
endforeach ()
add_custom_target (translations COMMAND ${QT_LUPDATE_EXECUTABLE} ${HEADERS} ${SOURCES} ${UI_HEADERS} -ts ${TRANSLATIONS})
add_custom_command (TARGET translations COMMAND ${QT_LRELEASE_EXECUTABLE} ${TRANSLATIONS})
message (STATUS "${SUBPROJECT} Translations: ${TRANSLATIONS_BINARY}")

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})
source_group ("Generated Files" FILES ${MOC_SOURCES})

add_executable (${SUBPROJECT} ${UI_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${TRANSLATIONS})
target_link_libraries (${SUBPROJECT} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})

# install properties
install (TARGETS ${SUBPROJECT} DESTINATION bin)
