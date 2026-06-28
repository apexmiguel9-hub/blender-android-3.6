package com.blender.android;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class StartupActivity extends AppCompatActivity {
    private static final String TAG = "StartupActivity";
    private static final int REQUEST_MANAGE_STORAGE = 1000;
    private static final int REQUEST_WRITE_STORAGE = 1001;
    private static final int TIMER_DELAY = 1500;

    private enum MsgId {
        MSG_ID_PERMISSION,
        MSG_ID_COPY_FILES,
        MSG_ID_START_ACTIVITY
    }

    private final Handler mHandler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(@NonNull Message msg) {
            if (msg.what == MsgId.MSG_ID_PERMISSION.ordinal()) {
                if (!checkStoragePermission()) {
                    showStoragePermissionDialog();
                } else {
                    mHandler.sendEmptyMessage(MsgId.MSG_ID_COPY_FILES.ordinal());
                }
            } else if (msg.what == MsgId.MSG_ID_COPY_FILES.ordinal()) {
                new Thread(() -> {
                    copyAppFiles();
                    mHandler.sendEmptyMessage(MsgId.MSG_ID_START_ACTIVITY.ordinal());
                }).start();
            } else if (msg.what == MsgId.MSG_ID_START_ACTIVITY.ordinal()) {
                Intent intent = new Intent(StartupActivity.this, MainActivity.class);
                intent.putExtra("HomePath", mHomePath);
                intent.putExtra("ConfigPath", mConfigPath);
                startActivity(intent);
                finish();
            }
            return false;
        }
    });

    private volatile String mHomePath = "";
    private volatile String mConfigPath = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_startup);
        mHandler.sendEmptyMessageDelayed(MsgId.MSG_ID_PERMISSION.ordinal(), TIMER_DELAY);
    }

    private boolean checkStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return Environment.isExternalStorageManager();
        } else {
            return ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED;
        }
    }

    private void showStoragePermissionDialog() {
        AlertDialog.Builder dialog = new AlertDialog.Builder(this);
        dialog.setTitle("Storage Permission");
        dialog.setMessage("Storage access is needed to save and load files.");
        dialog.setPositiveButton("Grant", (d, which) -> {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivityForResult(intent, REQUEST_MANAGE_STORAGE);
            } else {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, REQUEST_WRITE_STORAGE);
            }
        });
        dialog.setNegativeButton("Cancel", (d, which) -> {
            Toast.makeText(this, "Storage permission is required", Toast.LENGTH_LONG).show();
            finish();
        });
        dialog.show();
    }

    private void copyAppFiles() {
        Log.i(TAG, "Copying app files...");
        InstallAppFiles installer = new InstallAppFiles();
        InstallAppFiles.AppFilesPath paths = installer.installAppFiles(this);
        mHomePath = paths.homePath;
        mConfigPath = paths.configPath;
        Log.i(TAG, "Files copied. Home: " + mHomePath + " Config: " + mConfigPath);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_WRITE_STORAGE) {
            mHandler.postDelayed(() ->
                    mHandler.sendEmptyMessage(MsgId.MSG_ID_PERMISSION.ordinal()), TIMER_DELAY);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_MANAGE_STORAGE) {
            mHandler.postDelayed(() ->
                    mHandler.sendEmptyMessage(MsgId.MSG_ID_PERMISSION.ordinal()), TIMER_DELAY);
        }
    }
}
