#ifndef JNIUTIL_H
#define JNIUTIL_H

#include <jni.h>

#ifdef __cplusplus
#define CHECK(OBJ) ({ auto _tmp = OBJ; if (_tmp == nullptr) return JNI_ERR; _tmp; })
#else
#define CHECK(OBJ) ({ void *_tmp = OBJ; if (_tmp == NULL) return JNI_ERR; _tmp; })
#endif


JNIEnv *get_env();
void detach_thread(JNIEnv *env);

JNIEnv *init_jni(JavaVM *jvm);
void clear_jni();
int call_jni_object_func(JNIEnv *env,jobject instance,jmethodID method,jobject *result,...);
int call_jni_int_func(JNIEnv *env,jobject instance,jmethodID method,jint *result,...);
int call_jni_long_func(JNIEnv *env,jobject instance,jmethodID method,jlong *result,...);
int call_jni_void_func(JNIEnv *env,jobject instance,jmethodID method,...);
int call_jni_static_void_func(JNIEnv *env, jclass cls, jmethodID method,...);
int call_jni_static_int_func(JNIEnv *env,jclass cls,jmethodID method,jint *result,...);
#endif //JNIUTIL_H
