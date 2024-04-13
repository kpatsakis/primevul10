void memory_region_set_readonly(MemoryRegion *mr, bool readonly)
{
    if (mr->readonly != readonly) {
        memory_region_transaction_begin();
        mr->readonly = readonly;
        mr->uc->memory_region_update_pending |= mr->enabled;
        memory_region_transaction_commit(mr);
    }
}