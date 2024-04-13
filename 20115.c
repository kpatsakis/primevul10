ram_addr_t memory_region_get_ram_addr(MemoryRegion *mr)
{
    return mr->ram_block ? mr->ram_block->offset : RAM_ADDR_INVALID;
}