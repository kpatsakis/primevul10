MemoryRegion *flatview_translate(struct uc_struct *uc, FlatView *fv, hwaddr addr, hwaddr *xlat,
                                 hwaddr *plen, bool is_write,
                                 MemTxAttrs attrs)
{
    MemoryRegion *mr;
    MemoryRegionSection section;
    AddressSpace *as = NULL;

    /* This can be MMIO, so setup MMIO bit. */
    section = flatview_do_translate(uc, fv, addr, xlat, plen, NULL,
                                    is_write, true, &as, attrs);
    mr = section.mr;

    return mr;
}