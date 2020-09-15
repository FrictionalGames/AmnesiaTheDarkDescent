cmake_minimum_required (VERSION 2.8.11)
project(LauncherOSX)

include_directories(
    ${HPL2_SOURCE_DIR}/include
    ${DEP_INCLUDE_DIR}
)

link_directories(
    ${DEP_LIB_DIR}
)

set(compile_defs "HPL_MINIMAL;USE_SDL2")
list(APPEND compile_defs ${HPL2_DEFINES})

FILE(GLOB HPL2_MINIMAL_SOURCES
    ${HPL2_SOURCE_DIR}/sources/impl/PlatformUnix.cpp
    ${HPL2_SOURCE_DIR}/sources/impl/PlatformSDL.cpp
    ${HPL2_SOURCE_DIR}/sources/impl/PlatformMacOSX.mm
    ${HPL2_SOURCE_DIR}/include/resources/ConfigFile.h
    ${HPL2_SOURCE_DIR}/sources/resources/ConfigFile.cpp
    ${HPL2_SOURCE_DIR}/include/resources/BinaryBuffer.h
    ${HPL2_SOURCE_DIR}/sources/resources/BinaryBuffer.cpp
    ${HPL2_SOURCE_DIR}/include/resources/LanguageFile.h
    ${HPL2_SOURCE_DIR}/sources/resources/LanguageFile.cpp
    ${HPL2_SOURCE_DIR}/include/graphics/Color.h
    ${HPL2_SOURCE_DIR}/sources/graphics/Color.cpp
    ${HPL2_SOURCE_DIR}/include/graphics/GraphicsTypes.h
    ${HPL2_SOURCE_DIR}/sources/graphics/GraphicsTypes.cpp
    ${HPL2_SOURCE_DIR}/include/math/CRC.h
    ${HPL2_SOURCE_DIR}/sources/math/CRC.cpp
    ${HPL2_SOURCE_DIR}/include/system/Platform.h
    ${HPL2_SOURCE_DIR}/sources/system/Platform.cpp
    ${HPL2_SOURCE_DIR}/include/system/String.h
    ${HPL2_SOURCE_DIR}/sources/system/String.cpp
    ${HPL2_SOURCE_DIR}/sources/impl/tinyXml/*
#    ${HPL2_SOURCE_DIR}/sources/impl/XmlDocumentTiny.cpp
    HPLMinimal.h
    HPLMinimal.mm
)

add_library(HPL2_Minimal
    ${HPL2_MINIMAL_SOURCES}
)
set_target_properties(HPL2_Minimal PROPERTIES
    COMPILE_DEFINITIONS "${compile_defs}"
)
target_link_libraries(HPL2_Minimal
    z
)

SET(RESOURCE_FILES
    MainWindow.xib
    Startup.plist
    ../game/Lux.icns
)
SET(BACKGROUND_FILE
    launcher_bg.jpg
)
add_executable(Launcher MACOSX_BUNDLE
    ../launcher/Qualitychooser.h
    ../launcher/Qualitychooser.cpp
    LauncherAppDelegate.h
    LauncherAppDelegate.mm
    TabBackground.h
    TabBackground.m
    main.m
    Launcher_Prefix.pch
    ${RESOURCE_FILES}
    ${BACKGROUND_FILE}
)
set_source_files_properties(${RESOURCE_FILES} ${BACKGROUND_FILE}
    PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
)
set_target_properties(Launcher PROPERTIES
    OUTPUT_NAME "Launcher"
    COMPILE_DEFINITIONS "${compile_defs}"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Launcher-Info.plist"
    MACOSX_BUNDLE_ICON_FILE "Lux.icns"
    MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.frictionalgames.AmnesiaLauncher"
    MACOSX_BUNDLE_BUNDLE_NAME "Amnesia Launcher"
)

FIND_LIBRARY(COCOA_LIBRARY Cocoa)
FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
FIND_LIBRARY(OPENAL_LIBRARY OpenAL)
FIND_LIBRARY(SDL2_LIBRARY SDL2
    PATHS ${DEP_LIB_DIR}
    NO_DEFAULT_PATH
)
MARK_AS_ADVANCED(COCOA_LIBRARY OPENGL_LIBRARY OPENAL_LIBRARY SDL_LIBRARY)

target_link_libraries(Launcher
    ${COCOA_LIBRARY}
    ${OPENGL_LIBRARY}
    ${OPENAL_LIBRARY}
    ${SDL2_LIBRARY}
    HPL2_Minimal
)

IF(APPLE)
	PostBuildMacBundle(Launcher "" "libSDL2.dylib")
	install(TARGETS Launcher
		RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../../redist"
		BUNDLE DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../../redist"
	)
ENDIF()

# vim: et ts=4
