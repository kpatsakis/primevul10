void memory_region_init_ram(struct uc_struct *uc,
                            MemoryRegion *mr,
                            uint64_t size,
                            uint32_t perms)
{
    memory_region_init(uc, mr, size);
    mr->ram = true;
    if (!(perms & UC_PROT_WRITE)) {
        mr->readonly = true;
    }
    mr->perms = perms;
    mr->terminates = true;
    mr->destructor = memory_region_destructor_ram;
    mr->ram_block = qemu_ram_alloc(uc, size, mr);
}