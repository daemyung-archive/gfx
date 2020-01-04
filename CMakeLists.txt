cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(gfx VERSION 0.1.0 LANGUAGES CXX)

add_library(gfx
    STATIC
        include/gfx/enums.h
        include/gfx/types.h
        include/gfx/Device.h
        include/gfx/Buffer.h
        include/gfx/Image.h
        include/gfx/Sampler.h
        include/gfx/Shader.h
        include/gfx/Pipeline.h
        include/gfx/Swap_chain.h
        include/gfx/Cmd_buffer.h
        include/gfx/Fence.h
        src/std_lib.h
        src/Lru_cache.h
        src/Device.cpp)

target_include_directories(gfx
    PUBLIC
        include
    PRIVATE
        include/gfx
        src)

target_link_libraries(gfx
    PUBLIC
        prebuilt
        platform
        sc)

set_target_properties(gfx
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON)

if(CMAKE_SYSTEM_NAME MATCHES Darwin OR CMAKE_SYSTEM_NAME MATCHES iOS)
    target_sources(gfx
        PRIVATE
            src/mtl/mtl_lib.h
            src/mtl/Mtl_device.h
            src/mtl/Mtl_device.mm
            src/mtl/Mtl_buffer.h
            src/mtl/Mtl_buffer.mm
            src/mtl/Mtl_image.h
            src/mtl/Mtl_image.mm
            src/mtl/Mtl_sampler.h
            src/mtl/Mtl_sampler.mm
            src/mtl/Mtl_shader.h
            src/mtl/Mtl_shader.mm
            src/mtl/Mtl_pipeline.h
            src/mtl/Mtl_pipeline.mm
            src/mtl/Mtl_swap_chain.h
            src/mtl/Mtl_swap_chain.mm
            src/mtl/Mtl_cmd_buffer.h
            src/mtl/Mtl_cmd_buffer.mm
            src/mtl/Mtl_fence.h
            src/mtl/Mtl_fence.mm)

    target_include_directories(gfx
        PRIVATE
            src/mtl)

    target_compile_options(gfx
        PUBLIC
            -x objective-c++)

    target_link_libraries(gfx
        PUBLIC
            "-framework Metal"
            "-framework QuartzCore")

    set_target_properties(gfx
        PROPERTIES
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
elseif(CMAKE_SYSTEM_NAME MATCHES Android OR CMAKE_SYSTEM_NAME MATCHES Windows)
    target_sources(gfx
        PRIVATE
            src/vlk/vlk_lib.h
            src/vlk/Vlk_device.h
            src/vlk/Vlk_device.cpp
            src/vlk/Vlk_buffer.h
            src/vlk/Vlk_buffer.cpp
            src/vlk/Vlk_image.h
            src/vlk/Vlk_image.cpp
            src/vlk/Vlk_sampler.h
            src/vlk/Vlk_sampler.cpp
            src/vlk/Vlk_shader.h
            src/vlk/Vlk_shader.cpp
            src/vlk/Vlk_pipeline.h
            src/vlk/Vlk_pipeline.cpp
            src/vlk/Vlk_swap_chain.h
            src/vlk/Vlk_swap_chain.cpp
            src/vlk/Vlk_cmd_buffer.h
            src/vlk/Vlk_cmd_buffer.cpp
            src/vlk/Vlk_fence.h
            src/vlk/Vlk_fence.cpp
            src/vlk/Vlk_render_pass.h
            src/vlk/Vlk_render_pass.cpp
            src/vlk/Vlk_framebuffer.h
            src/vlk/Vlk_framebuffer.cpp
            src/vlk/Vlk_set_layout.h
            src/vlk/Vlk_set_layout.cpp)

    target_include_directories(gfx
        PRIVATE
            src/vlk)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Darwin OR CMAKE_SYSTEM_NAME MATCHES iOS)
    add_executable(gfx_demo MACOSX_BUNDLE)
elseif(CMAKE_SYSTEM_NAME MATCHES Android)
    add_library(gfx_demo SHARED)

    # Export ANativeActivity_onCreate(),
    # Refer to: https://github.com/android-ndk/ndk/issues/381.
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
elseif(CMAKE_SYSTEM_NAME MATCHES Windows)
    add_executable(gfx_demo WIN32)
endif()

target_sources(gfx_demo
    PRIVATE
        demo/main.cpp
        demo/stb_lib.h
        demo/Demo.h
        demo/Demo.cpp
        demo/Triangle_demo.h
        demo/Triangle_demo.cpp
        demo/Texture_demo.h
        demo/Texture_demo.cpp)

target_link_libraries(gfx_demo
    PUBLIC
        gfx)

set_target_properties(gfx_demo
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON)
