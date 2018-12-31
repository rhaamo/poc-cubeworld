#/*
#-----------------------------------------------------------------------------
#Filename:    CMakeLists.txt
#-----------------------------------------------------------------------------
# Based on the Ogre 3D CMakeLists.txt tutorial framework

cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

# Use relative paths
# This is mostly to reduce path size for command-line limits on windows
if (WIN32)
    # This seems to break Xcode projects so definitely don't enable on Apple builds
    set(CMAKE_USE_RELATIVE_PATHS true)
    set(CMAKE_SUPPRESS_REGENERATION true)
    set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
    set(OGRE_SAMPLES_INCLUDEPATH
            $ENV{OGRE_HOME}/Samples/include)
endif ()

# Assign compiler for Mac OS X-based systems
# NOTE: This logic is order-dependent and must occur here because the
# configuration phase of CMake projects will fall into an infinite loop
# if compiler assignments are modified after issuing a project(...) command.
#
if (OGRE_BUILD_PLATFORM_IPHONE)
    # Force gcc <= 4.2 on iPhone
    include(CMakeForceCompiler)
    CMAKE_FORCE_C_COMPILER(gcc-4.2 GNU)
    CMAKE_FORCE_CXX_COMPILER(gcc-4.2 GNU)
elseif (APPLE AND NOT OGRE_BUILD_PLATFORM_IPHONE)
    # Force gcc <= 4.0 on Mac OS X because 4.2 is not supported prior to Mac OS X 10.5
    #include(CMakeForceCompiler)
    #CMAKE_FORCE_C_COMPILER(gcc-4.0 GNU)
    #CMAKE_FORCE_CXX_COMPILER(gcc-4.0 GNU)
endif ()

set(OGRE_PROJECT_NAME "CubeWorld")

set(CMAKE_INSTALL_PREFIX "/home/dashie/dev/poc-cubeworld/build/bin")

project(${OGRE_PROJECT_NAME})

if (APPLE)
    # Include necessary submodules
    set(OGRE_SOURCE_DIR "/Users/rhaamo/src/OgreSDK")
    set(CMAKE_MODULE_PATH
            "${OGRE_SOURCE_DIR}/CMake"
            "${OGRE_SOURCE_DIR}/CMake/Utils"
            "${OGRE_SOURCE_DIR}/CMake/Packages"
            )
    set(OGRE_SAMPLES_INCLUDEPATH "${OGRE_SOURCE_DIR}/Samples/Common/include")
endif (APPLE)

if (${CMAKE_SYSTEM} MATCHES "Linux")
    # Include necessary submodules
    set(OGRE_SOURCE_DIR "/home/dashie/hack/ogre/ogre-1.11.5")
    set(CMAKE_MODULE_PATH
            "${OGRE_SOURCE_DIR}/cmake/"
            "${OGRE_SOURCE_DIR}/CMake/Utils/"
            )
    set(OGRE_SAMPLES_INCLUDEPATH "${OGRE_SOURCE_DIR}/Samples/Common/include")
endif (${CMAKE_SYSTEM} MATCHES "Linux")

include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(MacroLogFeature)
include(OgreConfigTargets)
#include(PreprocessorUtils)
set(OGRE_TEMPLATES_DIR "${OGRE_SOURCE_DIR}/CMake/Templates")


#####################################################################
# Set up the basic build environment
#####################################################################

if (CMAKE_BUILD_TYPE STREQUAL "")
    # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
    # differentiation between debug and release builds.
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

if (NOT APPLE)
    # Create debug libraries with _d postfix
    set(CMAKE_DEBUG_POSTFIX "_d")
endif ()

# Set compiler specific build flags
if (CMAKE_COMPILER_IS_GNUCXX)
    check_cxx_compiler_flag(-msse OGRE_GCC_HAS_SSE)
    if (OGRE_GCC_HAS_SSE)
        add_definitions(-msse)
    endif ()
endif ()
if (MSVC)
    if (CMAKE_BUILD_TOOL STREQUAL "nmake")
        # set variable to state that we are using nmake makefiles
        set(NMAKE TRUE)
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
    # Enable intrinsics on MSVC in debug mode
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Oi")
    if (CMAKE_CL_64)
        # Visual Studio bails out on debug builds in 64bit mode unless
        # this flag is set...
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
    endif ()
endif ()
if (MINGW)
    add_definitions(-D_WIN32_WINNT=0x0500)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
    # Test for GCC visibility
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-fvisibility=hidden OGRE_GCC_VISIBILITY)
    if (OGRE_GCC_VISIBILITY)
        # determine gcc version
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                OUTPUT_VARIABLE OGRE_GCC_VERSION)
        message(STATUS "Detected g++ ${OGRE_GCC_VERSION}")
        message(STATUS "Enabling GCC visibility flags")
        set(OGRE_GCC_VISIBILITY_FLAGS "-DOGRE_GCC_VISIBILITY -fvisibility=hidden")

        # check if we can safely add -fvisibility-inlines-hidden
        string(TOLOWER "${CMAKE_BUILD_TYPE}" OGRE_BUILD_TYPE)
        if (OGRE_BUILD_TYPE STREQUAL "debug" AND OGRE_GCC_VERSION VERSION_LESS "4.2")
            message(STATUS "Skipping -fvisibility-inlines-hidden due to possible bug in g++ < 4.2")
        else ()
            set(OGRE_GCC_VISIBILITY_FLAGS "${OGRE_GCC_VISIBILITY_FLAGS} -fvisibility-inlines-hidden")
        endif ()
    endif (OGRE_GCC_VISIBILITY)

    # Fix x64 issues on Linux
    if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" AND NOT APPLE)
        add_definitions(-fPIC)
    endif ()
endif (CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)

# determine system endianess
#include(TestBigEndian)
#test_big_endian(OGRE_TEST_BIG_ENDIAN)
set(OGRE_TEST_BIG_ENDIAN FALSE)

# Add OgreMain include path
include_directories("${OGRE_SOURCE_DIR}/OgreMain/include")
include_directories("${OGRE_BINARY_DIR}/include")
if (APPLE)
    if (OGRE_BUILD_PLATFORM_IPHONE)
        include_directories("${OGRE_SOURCE_DIR}/OgreMain/include/iPhone")

        # Set static early for proper dependency detection
        set(OGRE_STATIC TRUE)
    else ()
        include_directories("${OGRE_SOURCE_DIR}/OgreMain/include/OSX")
    endif ()
endif (APPLE)


find_package(OGRE REQUIRED)
find_package(PkgConfig)

pkg_check_modules(OIS REQUIRED OIS)
add_definitions(${OIS_CFLAGS})
link_directories(${OIS_LIBRARY_DIRS})

pkg_check_modules(ZZ REQUIRED zziplib)
add_definitions(${ZZ_CFLAGS})
link_directories(${ZZ_LIBRARY_DIRS})

# Find Boost
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
    if (WIN32 OR APPLE)
        set(Boost_USE_STATIC_LIBS TRUE)
    else ()
        # Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
        set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
    endif ()
    if (MINGW)
        # this is probably a bug in CMake: the boost find module tries to look for
        # boost libraries with name libboost_*, but CMake already prefixes library
        # search names with "lib". This is the workaround.
        set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
    endif ()
    set(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37")
    # Components that need linking (NB does not include header-only components like bind)
    set(OGRE_BOOST_COMPONENTS thread date_time)
    find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    if (NOT Boost_FOUND)
        # Try again with the other type of libs
        set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
        find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    endif ()
    find_package(Boost QUIET)

    # Set up referencing of Boost
    include_directories(${Boost_INCLUDE_DIR})
    add_definitions(-DBOOST_ALL_NO_LIB)
    set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif ()

IF (APPLE)
    FIND_LIBRARY(IOKIT_LIBRARY IOKit)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
    FIND_LIBRARY(OGRE_LIBRARY OGRE)
    LINK_DIRECTORIES(${LINK_DIRECTORIES} /opt/local/lib)
    # Ogre 3D is a 32bit only SDK build ATM :(
    set(CMAKE_C_FLAGS "-m32")
    set(CMAKE_CXX_FLAGS "-m32")
ENDIF (APPLE)

set(LIBNOISE_LIBRARIES noise)

set(HDRS
        ./src/BaseApp.h
        ./src/CubeWorld.h
        ./src/noiseutils/noiseutils.h
        ./src/il_defs.h
        ./src/il_landscape.h
        ./src/il_map.cpp
        ./src/il_noiseutils.cpp
        ./src/compat.h
        )

set(SRCS
        ./src/BaseApp.cpp
        ./src/CubeWorld.cpp
        ./src/noiseutils/noiseutils.cpp
        ./src/il_defs.cpp
        ./src/il_landscape.cpp
        ./src/il_map.cpp
        ./src/il_noiseutils.cpp
        )

include_directories(${OIS_INCLUDE_DIRS}
        ${OGRE_INCLUDE_DIRS}
        ${OGRE_SAMPLES_INCLUDEPATH}
        )


add_executable(${OGRE_PROJECT_NAME} WIN32 ${HDRS} ${SRCS})

set_target_properties(${OGRE_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(${OGRE_PROJECT_NAME} ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${LIBNOISE_LIBRARIES} ${ZZ_LIBRARIES})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)

# post-build copy for win32
if (WIN32 AND NOT MINGW)
    add_custom_command(TARGET ${OGRE_PROJECT_NAME} PRE_BUILD
            COMMAND if not exist .\\dist\\bin mkdir .\\dist\\bin)
    add_custom_command(TARGET ${OGRE_PROJECT_NAME} POST_BUILD
            COMMAND copy \"$(TargetPath)\" .\\dist\\bin)
endif (WIN32 AND NOT MINGW)
if (MINGW)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin)
endif (MINGW)

if (WIN32)

    install(TARGETS ${OGRE_PROJECT_NAME}
            RUNTIME DESTINATION bin
            CONFIGURATIONS All)

    install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/Media
            DESTINATION ./
            CONFIGURATIONS Release RelWithDebInfo Debug
            )

    install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
            ${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
            DESTINATION bin
            CONFIGURATIONS Release RelWithDebInfo
            )

    install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins_d.cfg
            ${CMAKE_SOURCE_DIR}/dist/bin/resources_d.cfg
            DESTINATION bin
            CONFIGURATIONS Debug
            )

    install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
            ${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
            ${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
            ${OGRE_PLUGIN_DIR_REL}/OIS.dll
            DESTINATION bin
            CONFIGURATIONS Release RelWithDebInfo
            )

    install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
            ${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
            ${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
            ${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
            DESTINATION bin
            CONFIGURATIONS Debug
            )
endif (WIN32)

set_property(TARGET ${OGRE_PROJECT_NAME} PROPERTY MACOSX_BUNDLE TRUE)
