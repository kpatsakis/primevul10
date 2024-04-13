flatview_extend_translation(struct uc_struct *uc, FlatView *fv, hwaddr addr,
                            hwaddr target_len,
                            MemoryRegion *mr, hwaddr base, hwaddr len,
                            bool is_write, MemTxAttrs attrs)
{
    hwaddr done = 0;
    hwaddr xlat;
    MemoryRegion *this_mr;

    for (;;) {
        target_len -= len;
        addr += len;
        done += len;
        if (target_len == 0) {
            return done;
        }

        len = target_len;
        this_mr = flatview_translate(uc, fv, addr, &xlat,
                                     &len, is_write, attrs);
        if (this_mr != mr || xlat != base + done) {
            return done;
        }
    }
}