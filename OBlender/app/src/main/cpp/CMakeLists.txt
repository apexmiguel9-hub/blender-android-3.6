cmake_minimum_required(VERSION 3.10.2)

project("blender")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
add_definitions(-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-enum-constexpr-conversion")
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DWITH_PYTHON)
add_definitions(-DWITH_OPENEXR)
add_definitions(-DWITH_FLUID)
add_definitions(-DWITH_OPENVDB)
add_definitions(-DWITH_OPENVDB_BLOSC)
add_definitions(-DWITH_GREASE_PENCIL_V3)
add_definitions(-DWITH_ALEMBIC)
add_definitions(-DWITH_BULLET)
add_definitions(-DWITH_FREESTYLE)
add_definitions(-DWITH_FFMPEG)
add_definitions(-DWITH_OPENJPEG)
add_definitions(-DWITH_AUDASPACE)
add_definitions(-DWITH_XR_OPENXR)
add_definitions(-DWITH_CYCLES)
add_definitions(-DWITH_INTERNATIONAL)
add_definitions(-DWITH_GMP)
add_definitions(-DWITH_OPENIMAGEDENOISE)
add_definitions(-DWITH_BUILDINFO_HEADER)
add_definitions(-DWITH_USD)
add_definitions(-DWITH_LIBMV)

set(blenderLibDir D:/FreeProjects/Blender/Blender/Blender-android_arm64/)

add_library(blender SHARED
        creator/buildinfo.c
        creator/creator.cc
        creator/creator_args.cc
        creator/creator_signals.cc
        android_native_app_glue.cc
        native-lib.cpp
        java_godot_lib_jni.cpp
        )

target_include_directories(blender PUBLIC
        D:/FreeProjects/Blender/Blender/lib-android_arm64/boost/boost_1_71_0/
        D:/FreeProjects/Blender/Blender/lib-android_arm64/OpenVDB/include/
        D:/FreeProjects/Blender/Blender/lib-android_arm64/Tbb/include/tbb-master/include/
        ${blenderLibDir}/source/creator/
        ${blenderLibDir}/intern/
        ${blenderLibDir}/intern/clog/
        ${blenderLibDir}/intern/ghost/
        ${blenderLibDir}/intern/guardedalloc
        ${blenderLibDir}/intern/guardedalloc/intern
        ${blenderLibDir}/intern/cycles/blender
        ${blenderLibDir}/source/blender/bmesh
        ${blenderLibDir}/source/blender/blenlib
        ${blenderLibDir}/source/blender/blenkernel
        ${blenderLibDir}/source/blender/makesdna
        ${blenderLibDir}/source/blender/makesrna
        ${blenderLibDir}/source/blender/bmesh/intern
        ${blenderLibDir}/source/blender/editors/include
        ${blenderLibDir}/source/blender/editors/mesh/
        ${blenderLibDir}/source/blender/blenkernel/
        ${blenderLibDir}/source/blender/blenloader/
        ${blenderLibDir}/source/blender/imbuf/
        ${blenderLibDir}/source/blender/gpu/
        ${blenderLibDir}/source/blender/blentranslation/
        ${blenderLibDir}/source/blender/render/
        ${blenderLibDir}/source/blender/windowmanager/
        ${blenderLibDir}/source/blender/depsgraph/
        ${blenderLibDir}/extern/audaspace/bindings/C/
        ${blenderLibDir}/intern/guardedalloc/
        ${blenderLibDir}/source/blender/python/
        ${blenderLibDir}/source/blender/freestyle/
        ${blenderLibDir}/intern/libmv/
        ${blenderLibDir}/source/blender/io/usd/
        )

# Import Blender engine shared library (from BlenderMobile CI)
add_library(blender_engine SHARED IMPORTED)
set_target_properties(blender_engine PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libblender_engine.so)

target_link_libraries(blender
        blender_engine
        -Wl,--warn-unresolved-symbols
        GLESv3
        GLESv2
        EGL
        android
        jnigraphics
        log
        )
