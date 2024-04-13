void memory_region_init(struct uc_struct *uc,
                        MemoryRegion *mr,
                        uint64_t size)
{
    memset(mr, 0, sizeof(*mr));
    mr->uc = uc;
    /* memory_region_initfn */
    mr->ops = &unassigned_mem_ops;
    mr->enabled = true;
    mr->destructor = memory_region_destructor_none;
    QTAILQ_INIT(&mr->subregions);

    mr->size = int128_make64(size);
    if (size == UINT64_MAX) {
        mr->size = int128_2_64();
    }
}