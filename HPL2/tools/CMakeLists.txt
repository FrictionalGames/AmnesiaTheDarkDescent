cmake_minimum_required (VERSION 2.8.11)
project(Tools)

find_file(TOOLINFO_PLIST Tool-Info.plist
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}
    NO_DEFAULT_PATH
)
mark_as_advanced(TOOLINFO_PLIST)

function(AddToolTarget target)
    add_executable(${target} MACOSX_BUNDLE
        ${ARGN}
    )
    target_link_libraries(${target} HPL2)

    set_target_properties(${target} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${TOOLINFO_PLIST}"
        MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.frictionalgames.HPL2.${target}"
        MACOSX_BUNDLE_DISPLAY_NAME "${target}"
        MACOSX_BUNDLE_BUNDLE_NAME "HPL2: ${target}"
    )

    if(APPLE)
        PostBuildMacBundle(${target} "${HPL2_FRAMEWORKS}" "${HPL2_LIBRARIES}")
    endif()

endfunction()

if(USERDIR_RESOURCES)
    add_definitions(
        -DUSERDIR_RESOURCES
    )
endif()

add_subdirectory(editors)

file(GLOB common_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ../tests/Common/*.*
)

### MapViewer

AddToolTarget(MapViewer
    mapview/MapView.cpp
    ${common_sources}
)

### ModelViewer

AddToolTarget(ModelViewer
    modelview/ModelView.cpp
    ${common_sources}
)

### ParticleViewer

#AddToolTarget(ParticleViewer
#    particleview/ParticleView.cpp
#    ${common_sources}
#)

##  Mesh Converter

add_executable(MshConverter
    mshconverter/MshConverter.cpp
)
target_link_libraries(MshConverter HPL2)

