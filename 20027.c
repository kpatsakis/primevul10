static MemTxResult flatview_write(struct uc_struct *uc, FlatView *fv, hwaddr addr, MemTxAttrs attrs,
                                  const void *buf, hwaddr len)
{
    hwaddr l;
    hwaddr addr1;
    MemoryRegion *mr;
    MemTxResult result = MEMTX_OK;

    l = len;
    mr = flatview_translate(uc, fv, addr, &addr1, &l, true, attrs);
    result = flatview_write_continue(uc, fv, addr, attrs, buf, len,
                                     addr1, l, mr);

    return result;
}