void *memory_region_get_ram_ptr(MemoryRegion *mr)
{
    void *ptr;

    ptr = qemu_map_ram_ptr(mr->uc, mr->ram_block, 0);

    return ptr;
}