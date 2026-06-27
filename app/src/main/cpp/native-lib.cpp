#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "BLI_path_util.h"
#include "creator/creator.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "BlenderNative", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "BlenderNative", __VA_ARGS__))

static void *gContext = nullptr;
static EGLDisplay gEglDisplay = EGL_NO_DISPLAY;
static EGLContext gEglContext = EGL_NO_CONTEXT;
static EGLSurface gEglSurface = EGL_NO_SURFACE;
static ANativeWindow *gNativeWindow = nullptr;

static bool createEglContext(ANativeWindow *window) {
    if (gEglDisplay != EGL_NO_DISPLAY) return true;

    gEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (gEglDisplay == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(gEglDisplay, &major, &minor)) {
        LOGE("eglInitialize failed");
        return false;
    }
    LOGI("EGL version %d.%d", major, minor);

    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(gEglDisplay, attribs, &config, 1, &numConfigs);

    gEglSurface = eglCreateWindowSurface(gEglDisplay, config, window, nullptr);
    if (gEglSurface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return false;
    }

    EGLint ctxAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    gEglContext = eglCreateContext(gEglDisplay, config, EGL_NO_CONTEXT, ctxAttribs);
    if (gEglContext == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return false;
    }

    if (!eglMakeCurrent(gEglDisplay, gEglSurface, gEglSurface, gEglContext)) {
        LOGE("eglMakeCurrent failed");
        return false;
    }

    LOGI("EGL context created successfully");
    return true;
}

static void destroyEglContext() {
    if (gEglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(gEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (gEglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(gEglDisplay, gEglContext);
            gEglContext = EGL_NO_CONTEXT;
        }
        if (gEglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(gEglDisplay, gEglSurface);
            gEglSurface = EGL_NO_SURFACE;
        }
        eglTerminate(gEglDisplay);
        gEglDisplay = EGL_NO_DISPLAY;
    }
    if (gNativeWindow) {
        ANativeWindow_release(gNativeWindow);
        gNativeWindow = nullptr;
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

    LOGI("Home path: %s", strHomePath);
    LOGI("Config path: %s", strConfigPath);

    env->ReleaseStringUTFChars(homePath, home);
    env->ReleaseStringUTFChars(configPath, config);
}

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeOnSurfaceCreated(JNIEnv *env, jclass clazz,
                                                              jobject surface) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (!window) {
        LOGE("Failed to get native window from surface");
        return;
    }

    if (!createEglContext(window)) {
        LOGE("Failed to create EGL context");
        ANativeWindow_release(window);
        return;
    }
    gNativeWindow = window;

    initialLib((void*)window);

    BLI_setenv("XDG_CACHE_HOME", strHomePath);
    BLI_setenv("HOME", strHomePath);

    char datafilesPath[256] = {0};
    strcat(datafilesPath, strConfigPath);
    strcat(datafilesPath, "4.0/config/datafiles");
    BLI_setenv("BLENDER_SYSTEM_DATAFILES", datafilesPath);

    char scriptsPath[256] = {0};
    strcat(scriptsPath, strConfigPath);
    strcat(scriptsPath, "scripts");
    BLI_setenv("BLENDER_SYSTEM_SCRIPTS", scriptsPath);

    char pythonPath[256] = {0};
    strcat(pythonPath, strConfigPath);
    strcat(pythonPath, "python");
    BLI_setenv("PYTHONPATH", pythonPath);
    BLI_setenv("PYTHONHOME", pythonPath);

    char blenderPath[256] = {0};
    strcat(blenderPath, strHomePath);
    strcat(blenderPath, "blender");
    const char *argv[] = {blenderPath, "--factory-startup", "-d"};
    gContext = mainBlenderInitial(3, argv);
    LOGI("Blender initialized, context: %p", gContext);
}

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeOnPause(JNIEnv *env, jclass clazz) {
    LOGI("nativeOnPause");
}

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeOnResume(JNIEnv *env, jclass clazz) {
    LOGI("nativeOnResume");
}

JNIEXPORT void JNICALL
Java_com_blender_android_MainActivity_nativeOnDestroy(JNIEnv *env, jclass clazz) {
    LOGI("nativeOnDestroy");
    destroyEglContext();
}

} // extern "C"
