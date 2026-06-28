package com.blender.android;

import android.content.Intent;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    private SurfaceView surfaceView;
    private boolean nativeInitialized = false;
    private boolean surfaceReady = false;

    static {
        try {
            System.loadLibrary("blender_jni");
            android.util.Log.i("BlenderJava", "Library loaded successfully");
        } catch (UnsatisfiedLinkError e) {
            android.util.Log.e("BlenderJava", "Failed to load blender_jni: " + e.getMessage());
            throw e;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        surfaceView = new SurfaceView(this);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                surfaceReady = true;
                if (nativeInitialized) {
                    nativeOnSurfaceCreated(holder.getSurface());
                }
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {}

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                surfaceReady = false;
            }
        });
        setContentView(surfaceView);

        initBlender();
    }

    private void initBlender() {
        Intent intent = getIntent();
        String homePath = intent != null ? intent.getStringExtra("HomePath") : null;
        String configPath = intent != null ? intent.getStringExtra("ConfigPath") : null;
        if (homePath == null || configPath == null) {
            homePath = getExternalFilesDir("blender").getAbsolutePath() + "/";
            configPath = getFilesDir().getAbsolutePath() + "/";
        }
        nativeInit(homePath, configPath);
        nativeInitialized = true;
        if (surfaceReady) {
            nativeOnSurfaceCreated(surfaceView.getHolder().getSurface());
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        nativeOnPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        nativeOnResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        nativeOnDestroy();
    }

    private static native void nativeInit(String homePath, String configPath);
    private static native void nativeOnSurfaceCreated(Object surface);
    private static native void nativeOnPause();
    private static native void nativeOnResume();
    private static native void nativeOnDestroy();
}
