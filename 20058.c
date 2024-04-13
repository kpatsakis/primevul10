void memory_region_add_subregion(MemoryRegion *mr,
                                 hwaddr offset,
                                 MemoryRegion *subregion)
{
    memory_region_add_subregion_common(mr, offset, subregion);
}