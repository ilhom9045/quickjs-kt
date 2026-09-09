/*
 * Сброс PGO-профиля. Собирается только в инструментированной сборке
 * (-DQUICKJS_PGO=generate, см. CMakeLists.txt).
 *
 * Приложение на Android не завершается штатно — процесс убивают, — поэтому
 * atexit-писатель профиля из libclang_rt.profile не срабатывает никогда. Фоновый поток
 * раз в 20 с пишет накопленные счётчики в
 *   /data/data/<pkg>/files/quickjs-<pid>.profraw
 * (внешнее хранилище отпало: каталог Android/data/<pkg> из нативного кода не создать).
 * Забирать из debug-сборки: adb shell run-as <pkg> cat files/quickjs-<pid>.profraw > x.profraw.
 * Счётчики накопительные, каждая запись перезаписывает файл целиком.
 * Сливать: llvm-profdata merge -o pgo/quickjs-android.profdata *.profraw (llvm-profdata из NDK).
 */
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int __llvm_profile_write_file(void);
void __llvm_profile_set_filename(const char *name);

static void *qjs_pgo_thread(void *arg) {
    (void)arg;
    /* Имя процесса приложения — это его пакет (для основного процесса). */
    char pkg[256] = {0};
    FILE *f = fopen("/proc/self/cmdline", "r");
    if (f) {
        size_t n = fread(pkg, 1, sizeof pkg - 1, f);
        pkg[n] = '\0';
        fclose(f);
    }
    char *colon = strchr(pkg, ':');
    if (colon) *colon = '\0';

    char dir[512];
    snprintf(dir, sizeof dir, "/data/data/%s/files", pkg);
    mkdir(dir, 0700);

    char path[640];
    snprintf(path, sizeof path, "%s/quickjs-%%p.profraw", dir);
    __llvm_profile_set_filename(path);

    for (;;) {
        sleep(20);
        __llvm_profile_write_file();
    }
    return NULL;
}

__attribute__((constructor)) static void qjs_pgo_start(void) {
    pthread_t t;
    if (pthread_create(&t, NULL, qjs_pgo_thread, NULL) == 0) pthread_detach(t);
}
