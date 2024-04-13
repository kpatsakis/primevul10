static uint64_t loongarch_qemu_read(void *opaque, hwaddr addr, unsigned size)
{
    switch (addr) {
    case FEATURE_REG:
        return 1ULL << IOCSRF_MSI | 1ULL << IOCSRF_EXTIOI |
               1ULL << IOCSRF_CSRIPI;
    case VENDOR_REG:
        return 0x6e6f73676e6f6f4cULL; /* "Loongson" */
    case CPUNAME_REG:
        return 0x303030354133ULL;     /* "3A5000" */
    case MISC_FUNC_REG:
        return 1ULL << IOCSRM_EXTIOI_EN;
    }
    return 0ULL;
}