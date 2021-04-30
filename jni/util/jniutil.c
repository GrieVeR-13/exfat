#include <stdio.h>
#include "jniutil.h"

JavaVM *Jvm = NULL;

JNIEnv *get_env() {
    JNIEnv *env;
    if (Jvm == NULL)
        return NULL;
    int res = (*Jvm)->AttachCurrentThread(Jvm, &env, NULL);
    if (res < 0) {
        return NULL;
    }
    return env;
}

void detach_thread(JNIEnv *env) {
    if (env && (*env)->ExceptionOccurred(env)) {
        (*env)->ExceptionDescribe(env);
    }
    (*Jvm)->DetachCurrentThread(Jvm);
}

static int check_exc(JNIEnv *env) {
    jthrowable exc;
    int errcode = 0;
    exc = (*env)->ExceptionOccurred(env);
    if (exc != NULL) {
        (*env)->ExceptionClear(env);
        errcode = -1;
    }
    return errcode;
}

int call_jni_object_func(JNIEnv *env, jobject instance, jmethodID method, jobject *result, ...) {
    va_list args;
    jobject res;
    va_start(args, result);
    res = (*env)->CallObjectMethodV(env, instance, method, args);
    va_end(args);
    if (result != NULL)
        *result = res;
    return check_exc(env);
}

int call_jni_int_func(JNIEnv *env, jobject instance, jmethodID method, jint *result, ...) {
    va_list args;
    jint res;
    va_start(args, result);
    res = (*env)->CallIntMethodV(env, instance, method, args);
    va_end(args);
    if (result != NULL)
        *result = res;
    return check_exc(env);
}

int call_jni_long_func(JNIEnv *env, jobject instance, jmethodID method, jlong *result, ...) {
    va_list args;
    jlong res;
    va_start(args, result);
    res = (*env)->CallLongMethodV(env, instance, method, args);
    va_end(args);
    if (result != NULL)
        *result = res;
    return check_exc(env);
}

int call_jni_void_func(JNIEnv *env, jobject instance, jmethodID method, ...) {
    va_list args;
    va_start(args, method);
    (*env)->CallVoidMethodV(env, instance, method, args);
    va_end(args);
    return check_exc(env);
}


int call_jni_static_void_func(JNIEnv *env, jclass cls, jmethodID method, ...) {
    va_list args;
    va_start(args, method);
    (*env)->CallStaticVoidMethodV(env, cls, method, args);
    va_end(args);

    return check_exc(env);
}

int call_jni_static_int_func(JNIEnv *env, jclass cls, jmethodID method, jint *result, ...) {
    va_list args;
    jint res;
    va_start(args, result);
    res = (*env)->CallStaticIntMethodV(env, cls, method, args);
    va_end(args);
    if (result != NULL)
        *result = res;
    return check_exc(env);
}

JNIEnv *init_jni(JavaVM *jvm) {
    Jvm = jvm;
    JNIEnv *env = get_env();
    if (!env)
        return NULL;
    return env;
}

void clear_jni() {
    Jvm = NULL;
}