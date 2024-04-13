static void listener_del_address_space(MemoryListener *listener,
                                       AddressSpace *as)
{
    FlatView *view;
    FlatRange *fr;

    if (listener->begin) {
        listener->begin(listener);
    }
    view = address_space_get_flatview(as);
    FOR_EACH_FLAT_RANGE(fr, view) {
        MemoryRegionSection section = section_from_flat_range(fr, view);

        if (listener->region_del) {
            listener->region_del(listener, &section);
        }
    }
    if (listener->commit) {
        listener->commit(listener);
    }
}