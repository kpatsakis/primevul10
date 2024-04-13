static uint64_t unassigned_io_read(struct uc_struct *uc, void* opaque, hwaddr addr, unsigned size)
{
#ifdef _MSC_VER
    return (uint64_t)0xffffffffffffffffULL;
#else
    return (uint64_t)-1ULL;
#endif
}