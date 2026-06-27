cmake_minimum_required(VERSION 3.10.2)
project("blender_jni")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
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
add_definitions(-DBOOST_NO_CXX98_FUNCTION_BASE)

set(BLENDER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../../../blender36)
set(LIB_ANDROID ${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib-android_arm64)

include_directories(
    ${BLENDER_SRC}/intern/
    ${BLENDER_SRC}/intern/clog/
    ${BLENDER_SRC}/intern/ghost/
    ${BLENDER_SRC}/intern/guardedalloc
    ${BLENDER_SRC}/intern/guardedalloc/intern
    ${BLENDER_SRC}/intern/cycles/blender
    ${BLENDER_SRC}/source/blender/bmesh
    ${BLENDER_SRC}/source/blender/blenlib
    ${BLENDER_SRC}/source/blender/blenkernel
    ${BLENDER_SRC}/source/blender/makesdna
    ${BLENDER_SRC}/source/blender/makesrna
    ${BLENDER_SRC}/source/blender/bmesh/intern
    ${BLENDER_SRC}/source/blender/editors/include
    ${BLENDER_SRC}/source/blender/editors/mesh/
    ${BLENDER_SRC}/source/blender/blenkernel/
    ${BLENDER_SRC}/source/blender/blenloader/
    ${BLENDER_SRC}/source/blender/imbuf/
    ${BLENDER_SRC}/source/blender/gpu/
    ${BLENDER_SRC}/source/blender/blentranslation/
    ${BLENDER_SRC}/source/blender/render/
    ${BLENDER_SRC}/source/blender/windowmanager/
    ${BLENDER_SRC}/source/blender/depsgraph/
    ${BLENDER_SRC}/extern/audaspace/bindings/C/
    ${BLENDER_SRC}/intern/guardedalloc/
    ${BLENDER_SRC}/source/blender/python/
    ${BLENDER_SRC}/source/blender/freestyle/
    ${BLENDER_SRC}/intern/libmv/
    ${BLENDER_SRC}/source/blender/io/usd/
    ${BLENDER_SRC}/source/creator/
    ${LIB_ANDROID}/boost/boost_1_71_0/
    ${LIB_ANDROID}/OpenVDB/include/
    ${LIB_ANDROID}/Tbb/include/tbb-master/include/
)

add_library(blender_jni SHARED
    creator/buildinfo.c
    creator/creator.cc
    creator/creator_args.cc
    creator/creator_signals.cc
    creator/stubs.cc
    native-lib.cpp
)

add_library(blender_engine SHARED IMPORTED)
set_target_properties(blender_engine PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libblender.so)

target_link_libraries(blender_jni
    blender_engine
    GLESv3
    EGL
    android
    log
    jnigraphics
    -Wl,-z,undefs
)
