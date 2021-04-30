#include "format.h"
#include <unistd.h>

#ifndef DEBUG
#define exfat_debug(format, ...)
#endif

int logarithm2(int n);
int setup(struct exfat_dev* dev, int sector_bits, int spc_bits,
          const char* volume_label, uint32_t volume_serial,
          uint64_t first_sector);

int format_exfat(struct exfat_dev* dev, const char *volume_label, uint32_t volume_serial, uint64_t first_sector, int sectors_per_cluster)
{

    int spc_bits = -1;
    if(sectors_per_cluster > 0)
    {
        spc_bits = logarithm2(atoi(optarg));
        if (spc_bits < 0) {
            exfat_error("invalid option value: '%s'", optarg);
            return 1;
        }
    }

    if (setup(dev, 9, spc_bits, volume_label, volume_serial,
              first_sector) != 0)
        return 1;

    return 0;
}

int format_exfat_default(struct exfat_dev* dev)
{
    return format_exfat(dev, NULL, 0, 0, -1);
}