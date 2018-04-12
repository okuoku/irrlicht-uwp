project(irr)
cmake_minimum_required(VERSION 2.8.11)

# Paths
set(SDL2_ROOT "${CMAKE_CURRENT_LIST_DIR}/externals/sdl")
set(ANGLE_ROOT "${CMAKE_CURRENT_LIST_DIR}/externals/angle")
set(IRRLICHT_ROOT "${CMAKE_CURRENT_LIST_DIR}/externals/irrlicht-sdl")

# temp
set(SDL2_CMAKE "sdl2-static-cmake")
set(ANGLE_CMAKE "angle-static-cmake")
set(IRRLICHT_CMAKE "irrlicht-cmake")

add_subdirectory(${ANGLE_CMAKE} angle)

# Headers required for User
include_directories(${ANGLE_ROOT}/include)
# Definitions required for User
add_definitions(
    # Use static-link
    -DANGLE_EXPORT=
    -DEGLAPI=
    -DGL_API=
    -DGL_APIENTRY=
    -DGL_APICALL=)

# Configure SDL2
add_definitions(
    -DSDL_VIDEO_RENDER_D3D=0
    )

add_subdirectory(${SDL2_CMAKE} sdl2)

# Configure the engine
set(IRRLICHT_USE_SDL2_PATCH ON) # Local patch
include_directories(${SDL2_ROOT}/include)
add_definitions(
    -D_IRR_STATIC_LIB_ # Mandatory (uncool)
    -D_IRR_COMPILE_WITH_SDL2_DEVICE_
    -D_IRR_GENERIC_SDL2_PLATFORM_
    -D_IRR_COMPILE_WITH_EGL_MANAGER_
    #-D_IRR_COMPILE_WITH_JOYSTICK_EVENTS_ # Guessed
    -DNO_IRR_COMPILE_WITH_CONSOLE_DEVICE_
    -DNO_IRR_COMPILE_WITH_X11_
    -DNO_IRR_COMPILE_WITH_DIRECT3D_9_
    -DNO_IRR_COMPILE_WITH_WEBGL1_ # FIXME: Wrong eval. order..?
    -DNO_IRR_COMPILE_WITH_OPENGL_
    #-DNO_IRR_COMPILE_WITH_OGLES2_
    -DNO_IRR_COMPILE_WITH_OGLES1_
    #-DNO_IRR_COMPILE_WITH_X11_DEVICE_
    )

add_subdirectory(${IRRLICHT_CMAKE} irrlicht)

include_directories(${IRRLICHT_ROOT}/include)

add_executable(hello WIN32 main.cpp
    $<TARGET_OBJECTS:sdl2_static>
    $<TARGET_OBJECTS:sdl2_main_static>
    $<TARGET_OBJECTS:irrlicht_static>
    $<TARGET_OBJECTS:angle_static>
    )

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
    set(extra_libs kernel32 dxguid)
elseif(WIN32)
    set(extra_libs
        version
        advapi32
        imm32
        winmm
        dxguid)
endif()

target_link_libraries(hello 
    ${extra_libs})

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
    # Hey, it's 2018!
    # https://cmake.org/pipermail/cmake-developers/2014-November/023524.html
    # Release C++ runtime
    foreach(e RELEASE RELWITHDEBINFO)
        set_target_properties(hello
            PROPERTIES
            LINK_FLAGS_${e}
            "/nodefaultlib:vccorlib /nodefaultlib:msvcrt vccorlib.lib msvcrt.lib"
            )
    endforeach()
    # Debug C++ runtime
    foreach(e DEBUG)
        set_target_properties(hello
            PROPERTIES
            LINK_FLAGS_${e}
            "/nodefaultlib:vccorlibd /nodefaultlib:msvcrtd vccorlibd.lib msvcrtd.lib"
            )
    endforeach()
endif()
