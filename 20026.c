static bool flatview_access_valid(struct uc_struct *uc, FlatView *fv, hwaddr addr, hwaddr len,
                                  bool is_write, MemTxAttrs attrs)
{
    MemoryRegion *mr;
    hwaddr l, xlat;

    while (len > 0) {
        l = len;
        mr = flatview_translate(uc, fv, addr, &xlat, &l, is_write, attrs);
        if (!memory_access_is_direct(mr, is_write)) {
            l = memory_access_size(mr, l, addr);
            if (!memory_region_access_valid(uc, mr, xlat, l, is_write, attrs)) {
                return false;
            }
        }

        len -= l;
        addr += l;
    }
    return true;
}