MemTxResult flatview_read_continue(struct uc_struct *uc, FlatView *fv, hwaddr addr,
                                   MemTxAttrs attrs, void *ptr,
                                   hwaddr len, hwaddr addr1, hwaddr l,
                                   MemoryRegion *mr)
{
    uint8_t *ram_ptr;
    uint64_t val;
    MemTxResult result = MEMTX_OK;
    bool release_lock = false;
    uint8_t *buf = ptr;

    for (;;) {
        if (!memory_access_is_direct(mr, false)) {
            /* I/O case */
            release_lock |= prepare_mmio_access(mr);
            l = memory_access_size(mr, l, addr1);
            result |= memory_region_dispatch_read(uc, mr, addr1, &val,
                                                  size_memop(l), attrs);
            stn_he_p(buf, l, val);
        } else {
            /* RAM case */
            ram_ptr = qemu_ram_ptr_length(fv->root->uc, mr->ram_block, addr1, &l, false);
            memcpy(buf, ram_ptr, l);
        }

        if (release_lock) {
            release_lock = false;
        }

        len -= l;
        buf += l;
        addr += l;

        if (!len) {
            break;
        }

        l = len;
        mr = flatview_translate(uc, fv, addr, &addr1, &l, false, attrs);
    }

    return result;
}