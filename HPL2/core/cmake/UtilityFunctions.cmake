### build up utility functions

## Find a prebuiltlib
## Must set a cache/global var of PREBUILT_PLATFORM_ROOT so this function knows where to find the libraries.
## the epected structure is simply to have the libs/frameworks in the directory pointed to by that variable.
## for multi-arch linux putting libs in two subdirectories of lib and lib64 will allow cmake to find the right ones
function(FindPrebuiltLibrary result_var libname)
    if(NOT PREBUILT_PLATFORM_ROOT)
        message(FATAL_ERROR "Must set PREBUILT_PLATFORM_ROOT before using this function")
    endif()

    # check prebuilt directory first
    find_library(${result_var}
        NAMES ${libname}
        PATHS ${PREBUILT_PLATFORM_ROOT}/lib ${PREBUILT_PLATFORM_ROOT}
        NO_DEFAULT_PATH)
    # Check system dir
    find_library(${result_var}
        NAMES ${libname})
    if(NOT ${result_var})
        message(FATAL_ERROR "Could not find library ${libname} in prebuilt folder ${PREBUILT_PLATFORM_ROOT}")
    endif()
endfunction()

# Helper to ensures a scope has been set for certain target properties
macro(_SetDefaultScope var_name default_scope)
    list(GET ${var_name} 0 __setdefaultscope_temp)
    if(__setdefaultscope_temp STREQUAL "PRIVATE" OR __setdefaultscope_temp STREQUAL "PUBLIC" OR __setdefaultscope_temp STREQUAL "INTERFACE")
    else()
        list(INSERT ${var_name} 0 ${default_scope})
    endif()
    unset(__setdefaultscope_temp)
endmacro()

# magic function to handle the power functions below
function(_BuildDynamicTarget name type)
    set(_mode "dirs")
    foreach(dir ${ARGN})
        if(dir STREQUAL "EXCLUDE")
            set(_mode "excl")
        elseif(dir STREQUAL "DIRS")
            set(_mode "dirs")
        elseif(dir STREQUAL "FILES")
            set(_mode "files")
        elseif(dir STREQUAL "INCLUDES")
            set(_mode "incl")
        elseif(dir STREQUAL "DEFINES")
            set(_mode "define")
        elseif(dir STREQUAL "FLAGS")
            set(_mode "flags")
        elseif(dir STREQUAL "LINK")
            set(_mode "link")
        else()
            if(_mode STREQUAL "excl")
                file(GLOB _files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
                    ${dir}
                )
                if(_files)
                    list(REMOVE_ITEM _source_files
                        ${_files}
                    )
                endif()
            elseif(_mode STREQUAL "files")
                list(APPEND _source_files
                    ${dir}
                )
            elseif(_mode STREQUAL "incl")
                list(APPEND _include_dirs
                    ${dir}
                )
            elseif(_mode STREQUAL "define")
                list(APPEND _definitions
                    ${dir}
                )
            elseif(_mode STREQUAL "flags")
                list(APPEND _flags
                    ${dir}
                )
            elseif(_mode STREQUAL "link")
                list(APPEND _link_libs
                    ${dir}
                )
            elseif(_mode STREQUAL "dirs")
                file(GLOB _files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
                    ${dir}/*.c
                    ${dir}/*.cpp
                    ${dir}/*.h
                    ${dir}/*.hpp
                    ${dir}/*.inl
                )
                if(_files)
                    list(APPEND _source_files
                        ${_files}
                    )
                endif()
            else()
                message(FATAL_ERROR "Unknown Mode ${_mode}")
            endif()
        endif()
    endforeach()
    if (NOT _source_files)
        message(FATAL_ERROR "Could not find any sources for ${name}")
    endif()
    if(type STREQUAL "lib")
        add_library(${name} EXCLUDE_FROM_ALL
            ${_source_files}
        )
    else()
        add_executable(${name} MACOSX_BUNDLE WIN32
            ${_source_files}
        )
    endif()
    if(_include_dirs)
        _SetDefaultScope(_include_dirs PRIVATE)
        target_include_directories(${name} ${_include_dirs})
    endif()
    if(_definitions)
        _SetDefaultScope(_definitions PRIVATE)
        target_compile_definitions(${name} ${_definitions})
    endif()
    if(_link_libs)
        target_link_libraries(${name} ${_link_libs})
    endif()
    if(_flags)
        set_target_properties(${name} PROPERTIES
            COMPILE_FLAGS "${_flags}"
        )
    endif()
endfunction()

## These two power functions build up library and program targets
## 
## the parameters are simply the target name followed by a list of directories or other parameters
## parameters that can be specified
## DIRS     followed by a list of directories ..  will glob in *.c, *.cpp, *.h, *.hpp, *.inl
## EXCLUDE  followed by a list of files/globs to exclude
## FILES    followed by a list of explicit files to add (or generated files)
## INCLUDES followed by a list of include directories. These use Generator expressions (see CMAKE documentation) default is PRIVATE scoped
## DEFINES  followed by a list of compiler defines.  These use Generator expressions (see CMAKE documentation) default is PRIVATE scoped
## FLAGS    followed by a list of compiler flags
## LINK     followed by a list of link targets.  Can use Generator expressions (see CMAKE documentation)

function(CreateLibrary name)
    _BuildDynamicTarget(${name} lib ${ARGN})
endfunction()

function(CreateProgram name)
    _BuildDynamicTarget(${name} exe ${ARGN})
endfunction()


## Helper functions to make development easier by handling mac OS X bundle preparations
if(APPLE)
    ## TODO make more versitile to handle frameworks with a version other than A
    ## TODO make handle "Mac App Store" required symlink fun
    function(PostBuildMacBundle target framework_list lib_list)
        INCLUDE(BundleUtilities)
        GET_TARGET_PROPERTY(_BIN_NAME ${target} LOCATION)
        GET_DOTAPP_DIR(${_BIN_NAME} _BUNDLE_DIR)

        set(_SCRIPT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${target}_prep.cmake")
        file(WRITE ${_SCRIPT_FILE}
            "# Generated Script file\n"
            "include(BundleUtilities)\n"
            "get_bundle_and_executable(\"\${BUNDLE_APP}\" bundle executable valid)\n"
            "if(valid)\n"
            "  set(framework_dest \"\${bundle}/Contents/Frameworks\")\n"
            "  foreach(framework_path ${framework_list})\n"
            "    get_filename_component(framework_name \${framework_path} NAME_WE)\n"
            "    file(MAKE_DIRECTORY \"\${framework_dest}/\${framework_name}.framework/Versions/A/\")\n"
            "    copy_resolved_framework_into_bundle(\${framework_path}/Versions/A/\${framework_name} \${framework_dest}/\${framework_name}.framework/Versions/A/\${framework_name})\n"
            "  endforeach()\n"
            "  foreach(lib ${lib_list})\n"
            "    get_filename_component(lib_file \${lib} NAME)\n"
            "    copy_resolved_item_into_bundle(\${lib} \${framework_dest}/\${lib_file})\n"
            "  endforeach()\n"
            "else()\n"
            "  message(ERROR \"App Not found? \${BUNDLE_APP}\")\n"
            "endif()\n"
            "#fixup_bundle(\"\${BUNDLE_APP}\" \"\" \"\${DEP_LIB_DIR}\")\n"
        )

        ADD_CUSTOM_COMMAND(TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -DBUNDLE_APP="${_BUNDLE_DIR}" -P "${_SCRIPT_FILE}"
        )
    endfunction()
    function(PostBuildCopyMacResourceDir target dir)
        if(ARGV2)
            set(subdir "/${ARGV2}")
        else()
            set(subdir "")
        endif()
        INCLUDE(BundleUtilities)
        GET_TARGET_PROPERTY(_BIN_NAME ${target} LOCATION)
        GET_DOTAPP_DIR("${_BIN_NAME}" _BUNDLE_DIR)
        SET(resource_dir "${_BUNDLE_DIR}/Contents/Resources")

        add_custom_command(TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${resource_dir}
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${dir} ${resource_dir}${subdir}
        )
    endfunction()
    function(PostBuildCopyMacResourceFile target file)
        if(ARGV2)
            set(subdir "/${ARGV2}")
        else()
            set(subdir "")
        endif()
        INCLUDE(BundleUtilities)
        GET_TARGET_PROPERTY(_BIN_NAME ${target} LOCATION)
        GET_DOTAPP_DIR("${_BIN_NAME}" _BUNDLE_DIR)
        SET(resource_dir "${_BUNDLE_DIR}/Contents/Resources")

        add_custom_command(TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${resource_dir}
            COMMAND ${CMAKE_COMMAND} -E copy ${file} ${resource_dir}${subdir}
        )
    endfunction()
endif()
