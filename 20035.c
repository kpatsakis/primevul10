MemoryRegionSection memory_region_find(MemoryRegion *mr,
                                       hwaddr addr, uint64_t size)
{
    MemoryRegionSection ret;

    ret = memory_region_find_rcu(mr, addr, size);
    return ret;
}