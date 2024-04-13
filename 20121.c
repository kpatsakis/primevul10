RAMBlock *qemu_ram_block_from_host(struct uc_struct *uc, void *ptr,
                                   bool round_offset, ram_addr_t *offset)
{
    RAMBlock *block;
    uint8_t *host = ptr;

    block = uc->ram_list.mru_block;
    if (block && block->host && host - block->host < block->max_length) {
        goto found;
    }

    RAMBLOCK_FOREACH(block) {
        /* This case append when the block is not mapped. */
        if (block->host == NULL) {
            continue;
        }
        if (host - block->host < block->max_length) {
            goto found;
        }
    }

    return NULL;

found:
    *offset = (host - block->host);
    if (round_offset) {
        *offset &= TARGET_PAGE_MASK;
    }
    return block;
}