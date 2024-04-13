void memory_unmap(struct uc_struct *uc, MemoryRegion *mr)
{
    int i;
    hwaddr addr;

    // Make sure all pages associated with the MemoryRegion are flushed
    // Only need to do this if we are in a running state
    if (uc->cpu) {
        for (addr = mr->addr; addr < mr->end; addr += uc->target_page_size) {
           tlb_flush_page(uc->cpu, addr);
        }
    }
    memory_region_del_subregion(uc->system_memory, mr);

    for (i = 0; i < uc->mapped_block_count; i++) {
        if (uc->mapped_blocks[i] == mr) {
            uc->mapped_block_count--;
            //shift remainder of array down over deleted pointer
            memmove(&uc->mapped_blocks[i], &uc->mapped_blocks[i + 1], sizeof(MemoryRegion*) * (uc->mapped_block_count - i));
            mr->destructor(mr);
            g_free(mr);
            break;
        }
    }
}