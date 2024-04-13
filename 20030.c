static FlatView *generate_memory_topology(struct uc_struct *uc, MemoryRegion *mr)
{
    int i;
    FlatView *view;

    view = flatview_new(mr);

    if (mr) {
        render_memory_region(view, mr, int128_zero(),
                             addrrange_make(int128_zero(), int128_2_64()),
                             false);
    }
    flatview_simplify(view);

    view->dispatch = address_space_dispatch_new(uc, view);
    for (i = 0; i < view->nr; i++) {
        MemoryRegionSection mrs =
            section_from_flat_range(&view->ranges[i], view);
        flatview_add_to_dispatch(uc, view, &mrs);
    }
    address_space_dispatch_compact(view->dispatch);
    g_hash_table_replace(uc->flat_views, mr, view);

    return view;
}