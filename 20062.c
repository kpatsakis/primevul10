bool memory_region_access_valid(struct uc_struct *uc, MemoryRegion *mr,
                                hwaddr addr,
                                unsigned size,
                                bool is_write,
                                MemTxAttrs attrs)
{
    if (mr->ops->valid.accepts
        && !mr->ops->valid.accepts(uc, mr->opaque, addr, size, is_write, attrs)) {
        return false;
    }

    if (!mr->ops->valid.unaligned && (addr & (size - 1))) {
        return false;
    }

    /* Treat zero as compatibility all valid */
    if (!mr->ops->valid.max_access_size) {
        return true;
    }

    if (size > mr->ops->valid.max_access_size
        || size < mr->ops->valid.min_access_size) {
        return false;
    }
    return true;
}