static MemTxResult  memory_region_read_accessor(struct uc_struct *uc, MemoryRegion *mr,
                                                hwaddr addr,
                                                uint64_t *value,
                                                unsigned size,
                                                signed shift,
                                                uint64_t mask,
                                                MemTxAttrs attrs)
{
    uint64_t tmp;

    tmp = mr->ops->read(uc, mr->opaque, addr, size);
    memory_region_shift_read_access(value, shift, mask, tmp);
    return MEMTX_OK;
}