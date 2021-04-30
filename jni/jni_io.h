#ifndef JNI_IO_H
#define JNI_IO_H

#include <jni.h>
#include <exfat.h>

struct exfat * openFileSystem(JNIEnv *env, jobject exfatImage, jboolean isReadOnly);
struct exfat_dev *exfat_open_raio(JNIEnv *env, jobject raio, jboolean isReadOnly);

#endif //JNI_IO_H
