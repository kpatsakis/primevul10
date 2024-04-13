static MemTxResult memory_region_write_accessor(struct uc_struct *uc, MemoryRegion *mr,
                                                hwaddr addr,
                                                uint64_t *value,
                                                unsigned size,
                                                signed shift,
                                                uint64_t mask,
                                                MemTxAttrs attrs)
{
    uint64_t tmp = memory_region_shift_write_access(value, shift, mask);

    mr->ops->write(uc, mr->opaque, addr, tmp, size);
    return MEMTX_OK;
}