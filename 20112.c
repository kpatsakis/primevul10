bool address_space_access_valid(AddressSpace *as, hwaddr addr,
                                hwaddr len, bool is_write,
                                MemTxAttrs attrs)
{
    FlatView *fv;
    bool result;

    fv = address_space_to_flatview(as);
    result = flatview_access_valid(as->uc, fv, addr, len, is_write, attrs);
    return result;
}