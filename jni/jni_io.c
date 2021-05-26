#include "jni_io.h"
#include <string.h>
#include "cutil/raio.h"
#include <exfat.h>
#include <asm/errno.h>
#include "inttypes.h"

int exfat_mount_opened(struct exfat *ef, enum exfat_mode mode);

struct exfat_dev *exfat_open_raio(JNIEnv *env, jobject raio, jboolean isReadOnly) {
    enum exfat_mode mode = isReadOnly ? EXFAT_MODE_RO : EXFAT_MODE_RW;
    struct exfat_dev *dev = malloc(sizeof(struct exfat_dev));
    if (dev == NULL) {
        exfat_error("failed to allocate memory for device structure");
        return NULL;
    }
    dev->env = env;
    dev->mode = mode;
    dev->raio = (*env)->NewGlobalRef(env, raio);
    if (dev->raio == NULL) {
        free(dev);
        return NULL;
    }

    off64_t size = exfat_seek(dev, 0, SEEK_END); //new
    if (size == -1) {
        (*env)->DeleteGlobalRef(env, dev->raio);
        free(dev);
        exfat_error("failed to get size of image");
        return NULL;
    }
    if (exfat_seek(dev, 0, SEEK_SET) == -1) {
        (*env)->DeleteGlobalRef(env, dev->raio);
        free(dev);
        exfat_error("failed to seek to the beginning of image");
        return NULL;
    }
    dev->size = size;
    return dev;
}

struct exfat *openFileSystem(JNIEnv *env, jobject exfatImage, jboolean isReadOnly) {
    exfat_tzset();
    struct exfat *ef = malloc(sizeof(struct exfat));
    if (ef == NULL) {
        exfat_error("failed to allocate memory for the exfat structure");
        return NULL;
    }
    memset(ef, 0, sizeof(struct exfat));

    ef->dev = exfat_open_raio(env, exfatImage, isReadOnly);
    if (ef->dev == NULL)
        return NULL;
    enum exfat_mode mode = isReadOnly ? EXFAT_MODE_RO : EXFAT_MODE_RW;
    exfat_mount_opened(ef, mode);
    return ef;
}