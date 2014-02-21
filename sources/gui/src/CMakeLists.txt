# set files
file (GLOB SOURCES *.cpp)
file (GLOB HEADERS *.h)
file (GLOB FORMS *.ui)

message (STATUS "${SUBPROJECT} Sources: ${SOURCES}")
message (STATUS "${SUBPROJECT} Headers: ${HEADERS}")
message (STATUS "${SUBPROJECT} Forms: ${FORMS}")

# include_path
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})
qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_wrap_ui (UI_HEADERS ${FORMS})

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})
source_group ("Generated Files" FILES ${MOC_SOURCES})

add_executable (${SUBPROJECT} ${UI_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES})
target_link_libraries (${SUBPROJECT} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})

# install properties
install (TARGETS ${SUBPROJECT} DESTINATION bin)
