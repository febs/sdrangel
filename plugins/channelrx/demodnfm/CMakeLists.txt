project(nfm)

set(nfm_SOURCES
	nfmdemod.cpp
    nfmdemodsettings.cpp
    nfmdemodwebapiadapter.cpp
	nfmplugin.cpp
)

set(nfm_HEADERS
	nfmdemod.h
    nfmdemodsettings.h
    nfmdemodwebapiadapter.h
	nfmplugin.h
)

include_directories(
        ${CMAKE_SOURCE_DIR}/swagger/sdrangel/code/qt5/client
)

if(NOT SERVER_MODE)
    set(nfm_SOURCES
        ${nfm_SOURCES}
        nfmdemodgui.cpp

	nfmdemodgui.ui
    )
    set(nfm_HEADERS
        ${nfm_HEADERS}
        nfmdemodgui.h
    )

    set(TARGET_NAME demodnfm)
    set(TARGET_LIB "Qt5::Widgets")
    set(TARGET_LIB_GUI "sdrgui")
    set(INSTALL_FOLDER ${INSTALL_PLUGINS_DIR})
else()
    set(TARGET_NAME demodnfmsrv)
    set(TARGET_LIB "")
    set(TARGET_LIB_GUI "")
    set(INSTALL_FOLDER ${INSTALL_PLUGINSSRV_DIR})
endif()

add_library(${TARGET_NAME} SHARED
	${nfm_SOURCES}
)

target_link_libraries(${TARGET_NAME}
        Qt5::Core
        ${TARGET_LIB}
	sdrbase
	${TARGET_LIB_GUI}
        swagger
)

install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_FOLDER})
