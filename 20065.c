static void memory_region_destructor_ram(MemoryRegion *mr)
{
    qemu_ram_free(mr->uc, mr->ram_block);
}