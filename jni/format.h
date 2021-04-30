#ifndef FORMAT_H
#define FORMAT_H

#include "../libexfat/exfat.h"

int format_exfat(struct exfat_dev* dev, const char *volume_label, uint32_t volume_serial, uint64_t first_sector, int sectors_per_cluster);
int format_exfat_default(struct exfat_dev* dev);


#endif //FORMAT_H
