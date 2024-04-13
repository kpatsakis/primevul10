static MemTxResult memory_region_write_with_attrs_accessor(struct uc_struct *uc, MemoryRegion *mr,
                                                           hwaddr addr,
                                                           uint64_t *value,
                                                           unsigned size,
                                                           signed shift,
                                                           uint64_t mask,
                                                           MemTxAttrs attrs)
{
    uint64_t tmp = memory_region_shift_write_access(value, shift, mask);

    return mr->ops->write_with_attrs(uc, mr->opaque, addr, tmp, size, attrs);
}