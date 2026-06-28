package com.blender.android;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

public class InstallAppFiles {
    private static final String TAG = "InstallAppFiles";

    public static class AppFilesPath {
        public String homePath;
        public String configPath;
    }

    public AppFilesPath installAppFiles(Context context) {
        AppFilesPath paths = new AppFilesPath();
        AssetManager assetManager = context.getAssets();

        paths.configPath = context.getFilesDir().getAbsolutePath() + File.separator;

        String[] configDirs = {"python", "4.0", "scripts", "usd"};
        for (String dir : configDirs) {
            copyAssetFolder(assetManager, dir, paths.configPath + dir);
        }

        paths.homePath = context.getExternalFilesDir("blender").getAbsolutePath() + File.separator;

        String[] homeDirs = {"examples"};
        for (String dir : homeDirs) {
            copyAssetFolder(assetManager, dir, paths.homePath + dir);
        }

        return paths;
    }

    private boolean copyAssetFolder(AssetManager assetManager, String assetPath, String destPath) {
        try {
            String[] files = assetManager.list(assetPath);
            if (files == null || files.length == 0) {
                return copyAssetFile(assetManager, assetPath, destPath);
            }

            File destDir = new File(destPath);
            if (destDir.exists()) {
                Log.i(TAG, "Already exists, skipping: " + destPath);
                return true;
            }
            if (!destDir.mkdirs()) {
                Log.e(TAG, "Failed to create dir: " + destPath);
                return false;
            }

            boolean ok = true;
            for (String file : files) {
                String subAssetPath = assetPath + "/" + file;
                String subDestPath = destPath + "/" + file;

                String[] subFiles = assetManager.list(subAssetPath);
                if (subFiles != null && subFiles.length > 0) {
                    ok &= copyAssetFolder(assetManager, subAssetPath, subDestPath);
                } else {
                    ok &= copyAssetFile(assetManager, subAssetPath, subDestPath);
                }
            }
            return ok;
        } catch (Exception e) {
            Log.e(TAG, "Error copying " + assetPath + ": " + e.getMessage());
            return false;
        }
    }

    private boolean copyAssetFile(AssetManager assetManager, String assetPath, String destPath) {
        try {
            File destFile = new File(destPath);
            if (destFile.exists()) {
                return true;
            }

            InputStream in = assetManager.open(assetPath);
            OutputStream out = new FileOutputStream(destPath);
            byte[] buf = new byte[65536];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
            in.close();
            out.close();
            return true;
        } catch (Exception e) {
            Log.e(TAG, "Error copying file " + assetPath + ": " + e.getMessage());
            return false;
        }
    }
}
