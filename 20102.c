void *qemu_map_ram_ptr(struct uc_struct *uc, RAMBlock *ram_block, ram_addr_t addr)
{
    RAMBlock *block = ram_block;

    if (block == NULL) {
        block = qemu_get_ram_block(uc, addr);
        addr -= block->offset;
    }

    return ramblock_ptr(block, addr);
}