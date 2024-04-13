static void *qemu_ram_ptr_length(struct uc_struct *uc, RAMBlock *ram_block, ram_addr_t addr,
                                 hwaddr *size, bool lock)
{
    RAMBlock *block = ram_block;
    if (*size == 0) {
        return NULL;
    }

    if (block == NULL) {
        block = qemu_get_ram_block(uc, addr);
        addr -= block->offset;
    }
    *size = MIN(*size, block->max_length - addr);

    return ramblock_ptr(block, addr);
}