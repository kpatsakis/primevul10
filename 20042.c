int memory_free(struct uc_struct *uc)
{
    MemoryRegion *mr;
    int i;

    for (i = 0; i < uc->mapped_block_count; i++) {
        mr = uc->mapped_blocks[i];
        mr->enabled = false;
        memory_region_del_subregion(uc->system_memory, mr);
        mr->destructor(mr);
        /* destroy subregion */
        g_free(mr);
    }

    return 0;
}