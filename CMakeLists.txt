cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(gfx VERSION 0.1.0 LANGUAGES CXX)

add_library(gfx
    STATIC
        include/gfx/enums.h
        include/gfx/types.h
        include/gfx/Clear_value.h
        include/gfx/Device.h
        include/gfx/Buffer.h
        include/gfx/Image.h
        include/gfx/Sampler.h
        include/gfx/Shader.h
        include/gfx/Pipeline.h
        include/gfx/Swap_chain.h
        include/gfx/Cmd_list.h
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

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
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
            src/mtl/Mtl_cmd_list.h
            src/mtl/Mtl_cmd_list.mm
            src/mtl/Mtl_fence.h
            src/mtl/Mtl_fence.mm)

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
