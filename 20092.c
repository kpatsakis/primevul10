void address_space_dispatch_compact(AddressSpaceDispatch *d)
{
    if (d->phys_map.skip) {
        phys_page_compact(d->uc, &d->phys_map, d->map.nodes);
    }
}