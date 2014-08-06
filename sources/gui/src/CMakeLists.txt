# set files
file (GLOB SOURCES *.cpp)
file (GLOB HEADERS *.h)
file (GLOB FORMS *.ui)

file (GLOB LANGUAGES ${SUBPROJECT_TRANSLATION_DIR}/*.ts)
set (RESOURCES ${SUBPROJECT_RESOURCE_DIR}/resources.qrc)

# include_path
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../${PROJECT_LIBRARY}/include/
                     ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR})
link_directories (${PROJECT_LIBRARY}/src/lib)

if (USE_QT5)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5DBus REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5LinguistTools REQUIRED)
    add_definitions(${Qt5Core_DEFINITIONS})
    add_definitions(${Qt5DBus_DEFINITIONS})
    add_definitions(${Qt5Widgets_DEFINITIONS})
    add_definitions(${Qt5LinguistTools_DEFINITIONS})
    include_directories (${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
    set (QT_NEEDED_LIBS ${Qt5Core_LIBRARIES} ${Qt5DBus_LIBRARIES} ${Qt5Widgets_LIBRARIES})
    qt5_wrap_cpp (MOC_SOURCES ${HEADERS})
    qt5_wrap_ui (UI_HEADERS ${FORMS})
    qt5_add_resources (QRC_SOURCES ${RESOURCES})

    foreach (LANGUAGE ${LANGUAGES})
        set (TS ${LANGUAGE})
        string (REPLACE ".ts" ".qm" QM ${TS})
        set (TRANSLATIONS ${TRANSLATIONS} ${TS})
        set (TRANSLATIONS_BINARY ${TRANSLATIONS_BINARY} ${QM})
        add_custom_command (OUTPUT ${QM} COMMAND ${Qt5_LRELEASE_EXECUTABLE} ${TS} MAIN_DEPENDENCY ${TS})
    endforeach ()
    add_custom_target (translations COMMAND ${Qt5_LUPDATE_EXECUTABLE} ${HEADERS} ${SOURCES} ${UI_HEADERS} -ts ${TRANSLATIONS})
    add_custom_command (TARGET translations COMMAND ${Qt5_LRELEASE_EXECUTABLE} ${TRANSLATIONS})
else ()
    find_package (Qt4 COMPONENTS QtCore QtDBus QtGui REQUIRED)
    include (${QT_USE_FILE})
    set (QT_NEEDED_LIBS ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY})
    qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
    qt4_wrap_ui (UI_HEADERS ${FORMS})
    qt4_add_resources (QRC_SOURCES ${RESOURCES})

    foreach (LANGUAGE ${LANGUAGES})
        set (TS ${LANGUAGE})
        string (REPLACE ".ts" ".qm" QM ${TS})
        set (TRANSLATIONS ${TRANSLATIONS} ${TS})
        set (TRANSLATIONS_BINARY ${TRANSLATIONS_BINARY} ${QM})
        add_custom_command (OUTPUT ${QM} COMMAND ${QT_LRELEASE_EXECUTABLE} ${TS} MAIN_DEPENDENCY ${TS})
    endforeach ()
    add_custom_target (translations COMMAND ${QT_LUPDATE_EXECUTABLE} ${HEADERS} ${SOURCES} ${UI_HEADERS} -ts ${TRANSLATIONS})
    add_custom_command (TARGET translations COMMAND ${QT_LRELEASE_EXECUTABLE} ${TRANSLATIONS})
endif()

add_executable (${SUBPROJECT} ${UI_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${TRANSLATIONS})
target_link_libraries (${SUBPROJECT} ${PROJECT_LIBRARY} ${QT_NEEDED_LIBS})
# install properties
install (TARGETS ${SUBPROJECT} DESTINATION bin)
