static MemTxResult memory_region_read_with_attrs_accessor(struct uc_struct *uc, MemoryRegion *mr,
                                                          hwaddr addr,
                                                          uint64_t *value,
                                                          unsigned size,
                                                          signed shift,
                                                          uint64_t mask,
                                                          MemTxAttrs attrs)
{
    uint64_t tmp = 0;
    MemTxResult r;

    r = mr->ops->read_with_attrs(uc, mr->opaque, addr, &tmp, size, attrs);
    memory_region_shift_read_access(value, shift, mask, tmp);
    return r;
}