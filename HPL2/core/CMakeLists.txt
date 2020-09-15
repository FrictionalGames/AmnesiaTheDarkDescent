cmake_minimum_required (VERSION 2.8.11)
project(HPL2)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(BoilerPlate)

set(HPL2_VERSION_MAJOR 2)
set(HPL2_VERSION_MINOR 0)
if(IS_APPLE_64)
    set(BIN_LIBROOT "${PLATFORM_PREFIX}64")
else()
    set(BIN_LIBROOT ${PLATFORM_PREFIX})
endif()

set(DEP_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/lib/${BIN_LIBROOT}")
set(DEP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/include")
find_path(PREBUILT_PLATFORM_ROOT .
    PATHS "${DEP_LIB_DIR}"
    NO_DEFAULT_PATH
)
set(DEP_LIB_DIR ${DEP_LIB_DIR} PARENT_SCOPE)
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} PARENT_SCOPE)
mark_as_advanced(
    PREBUILT_PLATFORM_ROOT
)

include(UtilityFunctions)

IF(APPLE)
    SET(PRIVATE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies CACHE PATH "")
    if(IS_APPLE_64)
        SET(PRIVATE_LIB_SUFFIX "lib/macosx64")
    else()
        SET(PRIVATE_LIB_SUFFIX "lib/macosx")
    endif()
ELSEIF(LINUX)
    SET(PRIVATE_LIB_SUFFIX "lib/linux")
    SET(PRIVATE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies CACHE PATH "")
ENDIF()

OPTION(USE_SDL2 "Use SDL2 instead of SDL1.2" ON)

add_subdirectory(../dependencies/OALWrapper OALWrapper)

SET(HPL2_DEFINES
    PUBLIC $<$<BOOL:${USE_SDL2}>:USE_SDL2>
)

add_definitions(
    -DUSE_OALWRAPPER
)

set(HPL2_INCLUDES
    PUBLIC include
    #Important that this is before dependencies so the correct OpenAL headers are used
    PRIVATE ${OALWRAPPER_INCLUDE_DIR}
    PUBLIC ../dependencies/include
)

IF(LINUX)
    add_library(binreloc STATIC ../dependencies/sources/binreloc/binreloc.c)
    set_target_properties(binreloc PROPERTIES COMPILE_DEFINITIONS "ENABLE_BINRELOC")

    list(APPEND HPL2_INCLUDES
        ../dependencies/sources/binreloc/
    )
ENDIF()


# common shared source
file(GLOB common_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    sources/ai/*
    sources/engine/*
    sources/generate/*
    sources/graphics/*
    sources/gui/*
    sources/haptic/*
    sources/input/*
    sources/math/*
    sources/physics/*
    sources/resources/*
    sources/scene/*
    sources/sound/*
    sources/system/*

    include/ai/*
    include/engine/*
    include/generate/*
    include/graphics/*
    include/gui/*
    include/haptic/*
    include/input/*
    include/math/*
    include/physics/*
    include/resources/*
    include/scene/*
    include/sound/*
    include/system/*
    include/*.*
)
list(APPEND common_sources include/BuildID_HPL2_0.h)
list(REMOVE_DUPLICATES common_sources)

#setup various IMPL sources
file(GLOB impl_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    include/impl/tinyXML/*
    include/impl/*.*

    # tinyXML
    sources/impl/tinyXml/*
    sources/impl/XmlDocumentTiny.cpp
    # scripting
    sources/impl/SqScript.cpp
    sources/impl/scriptarray.cpp
    sources/impl/scripthelper.cpp
    sources/impl/scriptstring.cpp
    sources/impl/scriptstring_utils.cpp
    # Image loading
    sources/impl/BitmapLoader*
    # theora video
    sources/impl/VideoStreamTheora.cpp
    # Newton
    sources/impl/*Newton.cpp
    # GL
    sources/impl/FrameBufferGL.cpp
    sources/impl/GLSL*
    sources/impl/OcclusionQueryOGL.cpp
    sources/impl/VertexBufferOGL_Array.cpp
    sources/impl/VertexBufferOGL_VBO.cpp
    sources/impl/VertexBufferOpenGL.cpp
    # SDL
    sources/impl/GamepadSDL.cpp
    sources/impl/GamepadSDL2.cpp
    sources/impl/KeyboardSDL.cpp
    sources/impl/MouseSDL.cpp
    sources/impl/MutexSDL.cpp
    sources/impl/ThreadSDL.cpp
    sources/impl/TimerSDL.cpp
    sources/impl/LowLevelGraphicsSDL.cpp
    sources/impl/LowLevelInputSDL.cpp
    sources/impl/LowLevelResourcesSDL.cpp
    sources/impl/LowLevelSystemSDL.cpp
    sources/impl/SDLEngineSetup.cpp
    sources/impl/SDLFontData.cpp
    sources/impl/SDLTexture.cpp
    # OpenAL
    sources/impl/LowLevelSoundOpenAL.cpp
    sources/impl/OpenAL*
    # mesh loader
    sources/impl/MeshLoaderCollada.cpp
    sources/impl/MeshLoaderColladaHelpers.cpp
    sources/impl/MeshLoaderColladaLoader.cpp
    sources/impl/MeshLoaderMSH.cpp
    sources/impl/MeshLoaderFBX.cpp
)

IF(APPLE)
    LIST(APPEND impl_sources
        sources/impl/PlatformMacOSX.mm
        sources/impl/PlatformUnix.cpp
        sources/impl/PlatformSDL.cpp
    )
    IF(NOT USE_SDL2)
        LIST(APPEND impl_sources
            SDLMain.m
            SDLMain.h
        )
    ENDIF()
ELSEIF(LINUX)
    LIST(APPEND impl_sources
        sources/impl/PlatformUnix.cpp
        sources/impl/PlatformSDL.cpp
    )
ENDIF()

# Setup buildID source

IF(APPLE)
    SET(BUILDID_SUFFIX MacOSX)
ELSEIF(LINUX)
    SET(BUILDID_SUFFIX Linux)
ELSE()
    message(FATAL_ERROR "Unknown platform")
ENDIF()
SET(BUILDID_SUFFIX ${BUILDID_SUFFIX} CACHE STRING "Build ID Suffix")
find_file(BUILDID_BIN buildcounter.pl
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}
    NO_DEFAULT_PATH
)
mark_as_advanced(BUILDID_SUFFIX BUILDID_BIN)

function(GenerateBuildID outvar idname includedir sourcedir)
    add_custom_command(
        OUTPUT
            ${sourcedir}/BuildID_${idname}_${BUILDID_SUFFIX}.cpp
            ${includedir}/BuildID_${idname}.h
        COMMAND
            ${BUILDID_BIN}
        ARGS
            ${idname} ${includedir} ${sourcedir}
    )
    set(${outvar} ${sourcedir}/BuildID_${idname}_${BUILDID_SUFFIX}.cpp
            ${includedir}/BuildID_${idname}.h PARENT_SCOPE)
endfunction()

GenerateBuildID(version_source
    HPL2_0
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/sources
)

# setup source groups
source_group("AI" REGULAR_EXPRESSION "\\/ai\\/.+")
source_group("Engine" REGULAR_EXPRESSION "\\/engine\\/.+")
source_group("Generate" REGULAR_EXPRESSION "\\/generate\\/.+")
source_group("Graphics" REGULAR_EXPRESSION "\\/graphics\\/.+")
source_group("GUI" REGULAR_EXPRESSION "\\/gui\\/.+")
source_group("Input" REGULAR_EXPRESSION "\\/input\\/.+")
source_group("Math" REGULAR_EXPRESSION "\\/math\\/.+")
source_group("Physics" REGULAR_EXPRESSION "\\/physics\\/.+")
source_group("Resources" REGULAR_EXPRESSION "\\/resources\\/.+")
source_group("Scene" REGULAR_EXPRESSION "\\/scene\\/.+")
source_group("Sound" REGULAR_EXPRESSION "\\/sound\\/.+")
source_group("System" REGULAR_EXPRESSION "\\/system\\/.+")

source_group("Implementation Specific" REGULAR_EXPRESSION "\\/impl\\/.+")
source_group("TinyXML" REGULAR_EXPRESSION "\\/impl\\/tiny.+")
source_group("SDL Specific" REGULAR_EXPRESSION "\\/impl\\/.*SDL.+")
source_group("OpenGL Specific" REGULAR_EXPRESSION "\\/impl\\/.*GL.+")
source_group("HaptX Specific" REGULAR_EXPRESSION "\\/impl\\/.+HaptX.+")
source_group("Newton Specific" REGULAR_EXPRESSION "\\/impl\\/.+Newton.+")
source_group("Mesh Loaders" REGULAR_EXPRESSION "\\/impl\\/Mesh.+")
source_group("OpenAL Specific" REGULAR_EXPRESSION "\\/impl\\/.*OpenAL.+")
source_group("Win32" REGULAR_EXPRESSION "\\/impl\\/.+Win32.+")
source_group("Platform" REGULAR_EXPRESSION "\\/impl\\/Platform.+")


# setup libs

FindPrebuiltLibrary(NEWTON_LIBRARY Newton)
FindPrebuiltLibrary(ANGELSCRIPT_LIBRARY angelscript)
FindPrebuiltLibrary(FBX_LIBRARY fbxsdk-2012.2-static)
FindPrebuiltLibrary(GLEW_LIBRARY GLEW)
FindPrebuiltLibrary(THEORA_LIBRARY theora)
FindPrebuiltLibrary(DEVIL_LIBRARY IL)

find_package(OpenGL)
mark_as_advanced(NEWTON_LIBRARY ANGELSCRIPT_LIBRARY FBX_LIBRARY GLEW_LIBRARY THEORA_LIBRARY DEVIL_LIBRARY)

# setup HPL2 compile target

add_library(HPL2 STATIC
    ${common_sources}
    ${version_sources}
    ${impl_sources}
)

target_include_directories(HPL2
    ${HPL2_INCLUDES}
)

target_compile_definitions(HPL2
    ${HPL2_DEFINES}
)

if(LINUX)
    set(PLATFORM_LIBS
        binreloc
        pthread
        dl
    )
elseif(APPLE)
    FIND_LIBRARY(SC_LIBRARY SystemConfiguration)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    mark_as_advanced(COCOA_LIBRARY THEORA_LIBRARY SC_LIBRARY)

    set(PLATFORM_LIBS
        iconv ${SC_LIBRARY}
        ${COCOA_LIBRARY}
    )
endif()

target_link_libraries(HPL2
    OALWrapper # also pulls in ogg,vorbis,SDL
    ${NEWTON_LIBRARY}
    ${ANGELSCRIPT_LIBRARY}
    ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY}
    ${GLEW_LIBRARY}
    ${THEORA_LIBRARY}
    ${DEVIL_LIBRARY}
    z
    ${FBX_LIBRARY}
    ${PLATFORM_LIBS}
)


if(APPLE)   
    # helper function to copy the needed frameworks in at build time
    SET(HPL2_FRAMEWORKS ${HPL2_FRAMEWORKS} CACHE STRING "HPL2 copy frameworks")
    SET(HPL2_LIBRARIES libSDL2.dylib libIL.1.dylib) 
    SET(HPL2_LIBRARIES ${HPL2_LIBRARIES}  CACHE STRING "HPL2 copy libs")
    mark_as_advanced(HPL2_FRAMEWORKS HPL2_LIBRARIES)

    FUNCTION(PostBuildMacBundle APP_TARGET framework_list lib_list)
        INCLUDE(BundleUtilities)
        GET_TARGET_PROPERTY(_BIN_NAME ${APP_TARGET} LOCATION)
        GET_DOTAPP_DIR(${_BIN_NAME} _BUNDLE_DIR)

        set(_SCRIPT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${APP_TARGET}_prep.cmake)
        file(WRITE ${_SCRIPT_FILE}
            "# Generated Script file\n"
            "include(BundleUtilities)\n"
            "get_bundle_and_executable(\"\${BUNDLE_APP}\" bundle executable valid)\n"
            "if(valid)\n"
            "  set(framework_path \"\${bundle}/Contents/Frameworks\")\n"
            "  foreach(framework ${framework_list})\n"
            "    file(MAKE_DIRECTORY \"\${framework_path}/\${framework}.framework/Versions/A/\")\n"
            "    copy_resolved_framework_into_bundle(\${DEP_LIB_DIR}/\${framework}.framework/Versions/A/\${framework} \${framework_path}/\${framework}.framework/Versions/A/\${framework})\n"
            "  endforeach()\n"
            "  foreach(lib ${lib_list})\n"
            "    copy_resolved_item_into_bundle(\${DEP_LIB_DIR}/\${lib} \${framework_path}/\${lib})\n"
            "  endforeach()\n"
            "else()\n"
            "  message(ERROR \"App Not found? \${BUNDLE_APP}\")\n"
            "endif()\n"
            "#fixup_bundle(\"\${BUNDLE_APP}\" \"\" \"\${DEP_LIB_DIR}\")\n"
        )

        ADD_CUSTOM_COMMAND(TARGET ${APP_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -DBUNDLE_APP="${_BUNDLE_DIR}" -DDEP_LIB_DIR="${DEP_LIB_DIR}" -P "${_SCRIPT_FILE}"
        )
    ENDFUNCTION()
ENDIF()

FUNCTION(AddTestTarget target_name)
    add_executable(${target_name} EXCLUDE_FROM_ALL
        ${ARGN}
    )
    target_link_libraries(${target_name} HPL2)
    set_target_properties(${target_name} PROPERTIES
        XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "${DEP_LIB_DIR}"
        INSTALL_RPATH "${DEP_LIB_DIR}"
    )
ENDFUNCTION()

add_subdirectory(../tests tests)
add_subdirectory(../tools tools)


