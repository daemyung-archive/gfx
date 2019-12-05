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
        src/std_lib_modules.h
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
            src/mtl/mtl_lib_modules.h
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
endif()

add_executable(gfx_demo
    demo/gfx_demo.cpp
    demo/stb_lib_modules.h
    demo/platform_lib_modules.h
    demo/sc_lib_modules.h
    demo/gfx_lib_modules.h
    demo/Gfx_triangle_demo.h
    demo/Gfx_triangle_demo.cpp
    demo/Gfx_texture_demo.h
    demo/Gfx_texture_demo.cpp)

target_link_libraries(gfx_demo
    PUBLIC
        gfx)

set_target_properties(gfx_demo
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON)
