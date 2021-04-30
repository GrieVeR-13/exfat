#include <stdint.h>
#include <sys/types.h>

#include "util/jniutil.h"
#include "raio.h"

extern JavaVM *Jvm;

jclass UtilClass;

jmethodID readMethodId;
jmethodID writeMethodId;
jmethodID flushMethodId;
jmethodID seekMethodId;
jmethodID getFilePointerMethodId;
jmethodID lengthMethodId;

jmethodID preadMethodId;
jmethodID pwriteMethodId;

static jint cache_methods(JNIEnv *env) {

    jclass cls;

    cls = CHECK((*env)->FindClass(env, "com/sovworks/projecteds3/data/filemanager/vfat/fs/RandomAccessIO"));
    readMethodId = CHECK((*env)->GetMethodID(env, cls, "read", "([BII)I"));
    writeMethodId = CHECK((*env)->GetMethodID(env, cls, "write", "([BII)V"));
    flushMethodId = CHECK((*env)->GetMethodID(env, cls, "flush", "()V"));
    seekMethodId = CHECK((*env)->GetMethodID(env, cls, "seek", "(J)V"));
    getFilePointerMethodId = CHECK((*env)->GetMethodID(env, cls, "getFilePosition", "()J"));
    lengthMethodId = CHECK((*env)->GetMethodID(env, cls, "length", "()J"));
    (*env)->DeleteLocalRef(env, cls);

    cls = CHECK((*env)->FindClass(env, "com/sovworks/projecteds3/data/filemanager/vfat/util/StreamUtil"));
    UtilClass = CHECK((*env)->NewGlobalRef(env, cls));
    preadMethodId = CHECK((*env)->GetStaticMethodID(env, cls, "pread",
                                                    "(Lcom/sovworks/projecteds3/data/filemanager/vfat/fs/RandomAccessIO;[BIIJ)I"));
    pwriteMethodId = CHECK((*env)->GetStaticMethodID(env, cls, "pwrite",
                                                     "(Lcom/sovworks/projecteds3/data/filemanager/vfat/fs/RandomAccessIO;[BIIJ)I"));
    (*env)->DeleteLocalRef(env, cls);

    return JNI_OK;
}

int init_raio(JNIEnv *env) {
    if (cache_methods(env) == JNI_ERR)
        return JNI_ERR;

    return JNI_OK;
}

static void clean_classes_cache(JNIEnv *env) {
    (*env)->DeleteGlobalRef(env, UtilClass);
}

void clear_raio(JNIEnv *env) {
    clean_classes_cache(env);
}

ssize_t raio_read(JNIEnv *env, jobject raio, uint8_t *buf, size_t count) {
    int rc;
    ssize_t res = 0;
    jbyteArray byteArray;
    jint bytesRead;

    byteArray = (*env)->NewByteArray(env, count);
    if (byteArray == NULL) {
        res = (ssize_t) -1;
        return res;
    }

    rc = call_jni_int_func(env, raio, readMethodId, &bytesRead,
                           byteArray, (jint) 0, (jint) count);
    if (rc) {
        res = (ssize_t) -1;
        (*env)->DeleteLocalRef(env, byteArray);
        return res;
    }
    (*env)->GetByteArrayRegion(env, byteArray, 0, bytesRead, (jbyte *) buf);
    res = bytesRead;
    (*env)->DeleteLocalRef(env, byteArray);
    return res;
}

ssize_t raio_pread(JNIEnv *env, jobject raio, uint8_t *buf, size_t count, off64_t pos) {
    int rc;
    ssize_t res;
    jbyteArray byteArray;
    jint bytesRead;

    byteArray = (*env)->NewByteArray(env, count);
    if (byteArray == NULL) {
        res = (ssize_t) -1;
        return res;
    }

    rc = call_jni_static_int_func(env, UtilClass, preadMethodId, &bytesRead,
                                  raio, byteArray, (jint) 0, (jint) count, (jlong) pos);
    if (rc) {
        res = (ssize_t) -1;
        (*env)->DeleteLocalRef(env, byteArray);
        return res;
    }
    (*env)->GetByteArrayRegion(env, byteArray, 0, bytesRead, (jbyte *) buf);
    res = bytesRead;
    (*env)->DeleteLocalRef(env, byteArray);
    return res;
}

ssize_t raio_write(JNIEnv *env, jobject raio, const uint8_t *buf, size_t count) {
    int rc;
    ssize_t res;
    jbyteArray byteArray;

    byteArray = (*env)->NewByteArray(env, count);
    if (byteArray == NULL) {
        res = (ssize_t) -1;
        return res;
    }
    (*env)->SetByteArrayRegion(env, byteArray, 0, count, (const jbyte *) buf);

    rc = call_jni_void_func(env, raio, writeMethodId,
                            byteArray, (jint) 0, (jint) count);
    if (rc)
        res = (ssize_t) -1;
    else
        res = count;
    (*env)->DeleteLocalRef(env, byteArray);

    return res;
}

ssize_t raio_pwrite(JNIEnv *env, jobject raio, const uint8_t *buf, size_t count, off64_t pos) {
    int rc;
    ssize_t res = 0;
    jbyteArray byteArray;

    byteArray = (*env)->NewByteArray(env, count);
    if (byteArray == NULL) {
        res = (ssize_t) -1;
        return res;
    }
    (*env)->SetByteArrayRegion(env, byteArray, 0, count, (const jbyte *) buf);

    rc = call_jni_static_int_func(env, UtilClass, pwriteMethodId, &res,
                                  raio, byteArray, (jint) 0, (jint) count, (jlong) pos);
    if (rc)
        res = (ssize_t) -1;
    else
        res = count;
    (*env)->DeleteLocalRef(env, byteArray);

    return res;
}

int raio_seek(JNIEnv *env, jobject raio, off64_t pos) {
    return call_jni_void_func(env, raio, seekMethodId,
                              (jlong) pos);
}

off64_t raio_get_pos(JNIEnv *env, jobject raio) {
    int res = 0;
    jlong pos;

    res = call_jni_long_func(env, raio, getFilePointerMethodId, &pos);
    if (res)
        pos = -1;
    return (off64_t) pos;
}

int raio_flush(JNIEnv *env, jobject raio) {
    return call_jni_void_func(env, raio, flushMethodId);
}

off64_t raio_get_size(JNIEnv *env, jobject raio) {
    int res = 0;
    jlong size;

    res = call_jni_long_func(env, raio, lengthMethodId, &size);
    if (res)
        size = -1;
    return (off64_t) size;
}