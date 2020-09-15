### setup options
option(FULL_WARNINGS "Enable full warnings" OFF)
option(FORCE32 "Force a 32bit compile on 64bit" OFF)

# this adds the build directory to the include path automatically
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if("${CMAKE_SYSTEM}" MATCHES "Linux")
    set(LINUX ON)
endif()

if(FORCE32)
    if(LINUX)
        set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    elseif(APPLE)
        set(CMAKE_OSX_ARCHITECTURES "i386")
    endif()
endif()

if(FULL_WARNINGS)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
endif()

if(LINUX)
    set(PLATFORM_PREFIX             "linux")
    if(CMAKE_SIZEOF_VOID_P MATCHES "8" AND NOT(FORCE32) )
        set(CMAKE_EXECUTABLE_SUFFIX ".bin.x86_64")
        set(BIN_RPATH               "\$ORIGIN/lib64")
    else()
        set(CMAKE_EXECUTABLE_SUFFIX ".bin.x86")
        set(BIN_RPATH               "\$ORIGIN/lib")
        set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)

        ### Ensure LargeFileSupport on 32bit linux
        set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -D_FILE_OFFSET_BITS=64")
        set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
    endif()
    
    set(CMAKE_SKIP_BUILD_RPATH              TRUE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH      TRUE)
    set(CMAKE_INSTALL_RPATH                 ${BIN_RPATH})
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH   FALSE)
elseif(APPLE)
    set(PLATFORM_PREFIX             "macosx")

    ## NOTE setting the rpath this way only works with CMAKE 2.8.12+
    # A workaround for 2.8.11 is do also set 
    # set(CMAKE_XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS ${BIN_RPATH})
    ##
    
    set(BIN_RPATH "@executable_path/../Frameworks")

    set(CMAKE_SKIP_BUILD_RPATH              TRUE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH      TRUE)
    set(CMAKE_INSTALL_RPATH                 ${BIN_RPATH})
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH   FALSE)
elseif(WIN32)
    set(PLATFORM_PREFIX             "win32")

    MESSAGE(FATAL_ERROR "TODO Windows specific stuff")
else()
    MESSAGE(FATAL_ERROR "Unhandled Platform")
endif()

### Export parent scope
if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(STATUS "Exporting variables to parent scope")

    set(LINUX                               ${LINUX} PARENT_SCOPE)
    set(FORCE32                             ${FORCE32} PARENT_SCOPE)
    set(PLATFORM_PREFIX                     ${PLATFORM_PREFIX} PARENT_SCOPE)

    set(CMAKE_INCLUDE_CURRENT_DIR           ${CMAKE_INCLUDE_CURRENT_DIR} PARENT_SCOPE)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES             ${CMAKE_OSX_ARCHITECTURES} PARENT_SCOPE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET         ${CMAKE_OSX_DEPLOYMENT_TARGET} PARENT_SCOPE)
endif()

    set(CMAKE_C_FLAGS                       ${CMAKE_C_FLAGS} PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS                     ${CMAKE_CXX_FLAGS} PARENT_SCOPE)

    set(CMAKE_SKIP_BUILD_RPATH              ${CMAKE_SKIP_BUILD_RPATH} PARENT_SCOPE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH      ${CMAKE_BUILD_WITH_INSTALL_RPATH} PARENT_SCOPE)
    set(CMAKE_INSTALL_RPATH                 ${CMAKE_INSTALL_RPATH} PARENT_SCOPE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH   ${CMAKE_INSTALL_RPATH_USE_LINK_PATH} PARENT_SCOPE)
    set(CMAKE_EXECUTABLE_SUFFIX             ${CMAKE_EXECUTABLE_SUFFIX} PARENT_SCOPE)
endif()
