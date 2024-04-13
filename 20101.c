static void reclaim_ramblock(struct uc_struct *uc, RAMBlock *block)
{
    if (block->flags & RAM_PREALLOC) {
        ;
    } else if (false) {
    } else {
        qemu_anon_ram_free(uc, block->host, block->max_length);
    }
    g_free(block);
}