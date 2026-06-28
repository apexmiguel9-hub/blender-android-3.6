#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/choreographer.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <atomic>

#include "BLI_path_util.h"
#include "creator/creator.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "BlenderNative", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "BlenderNative", __VA_ARGS__))

static void *gContext = nullptr;
static std::atomic<bool> gRunning(false);
static ANativeWindow *gNativeWindow = nullptr;
static AChoreographer *gChoreographer = nullptr;

extern "C" {
extern void setAndroidApp(bool hasApp);
extern bool hasAndroidApp();
}

static void frameCallback(long frameTimeNanos, void *data) {
    if (!gRunning) {
        return;
    }
    
    if (gContext) {
        mainBlenderLoop(gContext);
    }
    
    // Schedule next frame
    if (gChoreographer && gRunning) {
        AChoreographer_postFrameCallback(gChoreographer, frameCallback, nullptr);
    }
}

extern "C" {

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeInit(JNIEnv *env, jclass clazz,
                                                  jstring homePath, jstring configPath) {
    const char *home = env->GetStringUTFChars(homePath, nullptr);
    const char *config = env->GetStringUTFChars(configPath, nullptr);

    strncpy(strHomePath, home, sizeof(strHomePath) - 1);
    strncpy(strConfigPath, config, sizeof(strConfigPath) - 1);
    strHomePath[sizeof(strHomePath) - 1] = '\0';
    strConfigPath[sizeof(strConfigPath) - 1] = '\0';

    LOGI("=== nativeInit - Home: %s, Config: %s", strHomePath, strConfigPath);

    env->ReleaseStringUTFChars(homePath, home);
    env->ReleaseStringUTFChars(configPath, config);
}

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeOnSurfaceCreated(JNIEnv *env, jclass clazz,
                                                              jobject surface) {
    LOGI("=== nativeOnSurfaceCreated - start");
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (!window) {
        LOGE("Failed to get native window from surface");
        return;
    }
    gNativeWindow = window;

    gChoreographer = AChoreographer_getInstance();
    
    setAndroidApp(false);
    initialLib((void*)window);
    LOGI("=== initialLib done");

    LOGI("=== Setting env vars...");
    BLI_setenv("XDG_CACHE_HOME", strHomePath);
    BLI_setenv("HOME", strHomePath);
    LOGI("=== HOME set");

    char datafilesPath[256] = {0};
    strcat(datafilesPath, strConfigPath);
    strcat(datafilesPath, "4.0/config/datafiles");
    BLI_setenv("BLENDER_SYSTEM_DATAFILES", datafilesPath);
    LOGI("=== DATAFILES set");

    char scriptsPath[256] = {0};
    strcat(scriptsPath, strConfigPath);
    strcat(scriptsPath, "scripts");
    BLI_setenv("BLENDER_SYSTEM_SCRIPTS", scriptsPath);
    LOGI("=== SCRIPTS set");

    char pythonPath[256] = {0};
    strcat(pythonPath, strConfigPath);
    strcat(pythonPath, "python");
    BLI_setenv("PYTHONHOME", pythonPath);
    BLI_setenv("PYTHONDONTWRITEBYTECODE", "1");
    LOGI("=== PYTHONHOME set");

    char blenderPath[256] = {0};
    strcat(blenderPath, strHomePath);
    strcat(blenderPath, "blender");
    const char *argv[] = {blenderPath, "--factory-startup", "-d"};
    LOGI("=== Calling mainBlenderInitial...");
    gContext = mainBlenderInitial(3, argv);
    LOGI("=== Blender initialized, context: %p", gContext);

    if (gContext) {
        gRunning = true;
        LOGI("=== Starting render loop via Choreographer");
        AChoreographer_postFrameCallback(gChoreographer, frameCallback, nullptr);
    }
}

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeOnPause(JNIEnv *env, jclass clazz) {
    LOGI("nativeOnPause");
    gRunning = false;
}

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeOnResume(JNIEnv *env, jclass clazz) {
    LOGI("nativeOnResume");
    if (gContext && !gRunning) {
        gRunning = true;
        if (gChoreographer) {
            AChoreographer_postFrameCallback(gChoreographer, frameCallback, nullptr);
        }
    }
}

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeOnDestroy(JNIEnv *env, jclass clazz) {
    LOGI("nativeOnDestroy");
    gRunning = false;
    if (gNativeWindow) {
        ANativeWindow_release(gNativeWindow);
        gNativeWindow = nullptr;
    }
}

} // extern "C"