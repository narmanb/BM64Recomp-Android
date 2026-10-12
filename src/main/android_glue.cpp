#include "android_support.h"
#include <jni.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <android/native_window.h>
#include <android/log.h>
#include <atomic>
#include <mutex>
#include <cstdlib>
#include <stdexcept>
#include <cstdio>
#include <string>

namespace {
    JavaVM* vm = nullptr;
    jobject activity = nullptr;
    jmethodID request_document = nullptr;
    jmethodID touch_settings = nullptr;
    std::mutex document_mutex;
    std::function<void(bool, const std::list<std::filesystem::path>&)> pending;
    bool ready = false, success = false;
    std::list<std::filesystem::path> results;
    std::atomic<ANativeWindow*> replacement{nullptr};
    int event_filter(void*, SDL_Event* event) {
        if (event->type == SDL_APP_DIDENTERFOREGROUND) {
            SDL_Window* window = SDL_GetWindowFromID(1);
            SDL_SysWMinfo info{}; SDL_VERSION(&info.version);
            if (window && SDL_GetWindowWMInfo(window, &info)) {
                bm64::android::publish_window(info.info.android.window);
            }
        }
        return 1;
    }
}
extern "C" JNIEXPORT void JNICALL Java_com_narmanb_bm64_MainActivity_nativeInit(JNIEnv* env, jobject self, jstring path) {
    env->GetJavaVM(&vm);
    activity = env->NewGlobalRef(self);
    jclass cls = env->GetObjectClass(self);
    request_document = env->GetMethodID(cls, "requestOpenDocument", "(Z)V");
    touch_settings = env->GetMethodID(cls, "showTouchscreenSettings", "()V");
    env->DeleteLocalRef(cls);
    const char* dir = env->GetStringUTFChars(path, nullptr);
    SDL_setenv("APP_FOLDER_PATH", dir, 1);
    SDL_setenv("BM64_ASSET_PATH", dir, 1);
    const std::string log_path = std::string(dir) + "/native-startup.log";
    if (freopen(log_path.c_str(), "a", stdout)) setvbuf(stdout, nullptr, _IONBF, 0);
    if (freopen(log_path.c_str(), "a", stderr)) setvbuf(stderr, nullptr, _IONBF, 0);
    env->ReleaseStringUTFChars(path, dir);
    bm64::android::startup_stage("nativeInit complete");
}
extern "C" JNIEXPORT void JNICALL Java_com_narmanb_bm64_MainActivity_nativeDocumentResult(JNIEnv* env, jobject, jboolean ok, jobjectArray paths) {
    std::lock_guard lock(document_mutex);
    results.clear();
    for (jsize i=0;i<env->GetArrayLength(paths);i++) {
        auto value = static_cast<jstring>(env->GetObjectArrayElement(paths,i));
        const char* path=env->GetStringUTFChars(value,nullptr);
        results.emplace_back(path);
        env->ReleaseStringUTFChars(value,path);env->DeleteLocalRef(value);
    }
    success=ok;ready=true;
}
void bm64::android::startup_stage(const char* stage) {
    __android_log_print(ANDROID_LOG_INFO, "BM64", "%s", stage);
    const char* dir = std::getenv("APP_FOLDER_PATH");
    if (!dir) return;
    const std::string path = std::string(dir) + "/startup-stage.txt";
    if (FILE* file = fopen(path.c_str(), "a")) {
        fprintf(file, "Native: %s\n", stage);
        fclose(file);
    }
}
void bm64::android::initialize() {
    startup_stage("SDL_main entered");
    const char* dir=std::getenv("APP_FOLDER_PATH");
    if (!dir || !*dir) throw std::runtime_error("Android app path missing");
    std::filesystem::current_path(dir);
    SDL_AddEventWatch(event_filter,nullptr);
}
void bm64::android::open_document(bool multiple, std::function<void(bool, const std::list<std::filesystem::path>&)> callback) {
    {
        std::lock_guard lock(document_mutex);
        if (pending) { callback(false, {}); return; }
        pending=std::move(callback);ready=false;
    }
    JNIEnv* env=nullptr;
    bool attached=vm && vm->GetEnv(reinterpret_cast<void**>(&env),JNI_VERSION_1_6)==JNI_EDETACHED;
    if (attached && vm->AttachCurrentThread(&env,nullptr)!=JNI_OK) env=nullptr;
    if (env && activity && request_document) {
        env->CallVoidMethod(activity,request_document,static_cast<jboolean>(multiple));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();env->ExceptionClear();
            std::lock_guard lock(document_mutex);ready=true;success=false;results.clear();
        }
    } else {
        std::lock_guard lock(document_mutex);ready=true;success=false;results.clear();
    }
    if(attached && env)vm->DetachCurrentThread();
}
void bm64::android::dispatch_document_result() {
    std::function<void(bool,const std::list<std::filesystem::path>&)> callback;
    std::list<std::filesystem::path> paths;bool ok;
    {
        std::lock_guard lock(document_mutex);
        if(!ready || !pending)return;
        callback=std::move(pending);paths=std::move(results);ok=success;ready=false;
    }
    callback(ok,paths);
}
void bm64::android::publish_window(void* window) {
    auto* native=static_cast<ANativeWindow*>(window);
    if(native)ANativeWindow_acquire(native);
    auto* previous=replacement.exchange(native);
    if(previous)ANativeWindow_release(previous);
}
void* bm64::android::take_window() {return replacement.exchange(nullptr);}

void bm64::android::show_touch_settings() {
    JNIEnv* env = nullptr;
    bool attached = vm && vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_EDETACHED;
    if (attached && vm->AttachCurrentThread(&env, nullptr) != JNI_OK) env = nullptr;
    if (env && activity && touch_settings) {
        env->CallVoidMethod(activity, touch_settings);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            startup_stage("Could not open touchscreen settings");
        }
    }
    if (attached && env) vm->DetachCurrentThread();
}
