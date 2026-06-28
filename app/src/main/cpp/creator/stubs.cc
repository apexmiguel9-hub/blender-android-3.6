#include <stdio.h>

extern "C" {

void IMB_ffmpeg_init(void) {
    fprintf(stderr, "IMB_ffmpeg_init: stub\n");
}

void USD_ensure_plugin_path_registered(void) {
    fprintf(stderr, "USD_ensure_plugin_path_registered: stub\n");
}

}

extern "C" {

unsigned long build_commit_timestamp = 0;
char build_commit_date[16] = "2024-01-01";
char build_commit_time[16] = "00:00:00";
char build_date[] = "2024-01-01";
char build_time[] = "00:00:00";
char build_hash[] = "stub";
char build_branch[] = "stub";
char build_platform[] = "Android";
char build_type[] = "Release";
char build_cflags[] = "";
char build_cxxflags[] = "";
char build_linkflags[] = "";
char build_system[] = "CMake";

}
