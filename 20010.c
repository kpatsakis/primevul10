MemTxResult address_space_write(AddressSpace *as, hwaddr addr,
                                MemTxAttrs attrs,
                                const void *buf, hwaddr len)
{
    MemTxResult result = MEMTX_OK;
    FlatView *fv;

    if (len > 0) {
        fv = address_space_to_flatview(as);
        result = flatview_write(as->uc, fv, addr, attrs, buf, len);
    }

    return result;
}