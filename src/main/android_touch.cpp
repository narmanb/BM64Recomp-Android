#include "android_pad.h"
#include <jni.h>
#include <atomic>
#ifndef BM64_FOUNDATION
#include "recomp_ui.h"
#include "android_menu_event.h"
#include <SDL.h>
#endif
namespace {
    bm64::android::Pad touch;
    std::atomic<int> sensitivity{100};
    std::atomic<bool> foreground{false};
    std::mutex snapshot_mutex;
    bm64::android::State snapshot;
}
bool bm64::android::is_foreground() {return foreground.load(std::memory_order_relaxed);}
int bm64::android::physical_sensitivity() {return sensitivity.load(std::memory_order_relaxed);}
void bm64::android::poll_touch() { std::lock_guard lock(snapshot_mutex);snapshot=touch.poll(); }
bm64::android::State bm64::android::touch_state() { std::lock_guard lock(snapshot_mutex);return snapshot; }
void bm64::android::clear_touch() { std::lock_guard lock(snapshot_mutex);touch.clear();snapshot={}; }
extern "C" {
JNIEXPORT void JNICALL Java_com_narmanb_bm64_MainActivity_nativeSetForeground(JNIEnv*,jobject,jboolean active) {
    foreground.store(active,std::memory_order_relaxed);
    if(!active)bm64::android::clear_touch();
}
JNIEXPORT void JNICALL Java_com_narmanb_bm64_MainActivity_nativeControllerSensitivity(JNIEnv*,jobject,jint value) {sensitivity.store(std::clamp(static_cast<int>(value),0,100),std::memory_order_relaxed);}

JNIEXPORT void JNICALL Java_com_narmanb_bm64_touch_NativeTouch_nativeSetState(JNIEnv* env,jclass,jint mask,jfloatArray axes) {
    float v[2]={};
    if(axes)env->GetFloatArrayRegion(axes,0,std::min(env->GetArrayLength(axes),2),v);
    if(!env->ExceptionCheck())touch.set(static_cast<uint16_t>(mask),v[0],-v[1]);
}
JNIEXPORT void JNICALL Java_com_narmanb_bm64_touch_NativeTouch_nativeClearState(JNIEnv*,jclass) { bm64::android::clear_touch(); }
JNIEXPORT jboolean JNICALL Java_com_narmanb_bm64_touch_NativeTouch_nativeIsMenuOpen(JNIEnv*,jclass) {
#ifdef BM64_FOUNDATION
    return false;
#else
    return recompui::is_context_capturing_input_snapshot();
#endif
}
JNIEXPORT void JNICALL Java_com_narmanb_bm64_touch_NativeTouch_nativeRequestMenuToggle(JNIEnv*,jclass) {
#ifndef BM64_FOUNDATION
    recompui::queue_event(bm64::android::menu_event(true));
    recompui::queue_event(bm64::android::menu_event(false));
#endif
}
JNIEXPORT jfloatArray JNICALL Java_com_narmanb_bm64_touch_NativeTouch_nativePollPreview(JNIEnv* env,jclass) {
    bm64::android::poll_touch();auto s=bm64::android::touch_state();
    float v[]={static_cast<float>(s.buttons),s.x,s.y};
    auto ret=env->NewFloatArray(3);if(ret)env->SetFloatArrayRegion(ret,0,3,v);return ret;
}
}
