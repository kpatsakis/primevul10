AddressSpaceDispatch *address_space_dispatch_new(struct uc_struct *uc, FlatView *fv)
{
    AddressSpaceDispatch *d = g_new0(AddressSpaceDispatch, 1);
#ifndef NDEBUG
    uint16_t n;

    n = dummy_section(uc, &d->map, fv, &(uc->io_mem_unassigned));
    assert(n == PHYS_SECTION_UNASSIGNED);
#else
    dummy_section(uc, &d->map, fv, &(uc->io_mem_unassigned));
#endif

    d->phys_map  = (PhysPageEntry) { .ptr = PHYS_MAP_NODE_NIL, .skip = 1 };
    d->uc = uc;

    return d;
}