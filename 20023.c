MemoryRegion *memory_region_from_host(struct uc_struct *uc,
                                      void *ptr, ram_addr_t *offset)
{
    RAMBlock *block;

    block = qemu_ram_block_from_host(uc, ptr, false, offset);
    if (!block) {
        return NULL;
    }

    return block->mr;
}