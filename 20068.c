static void address_space_update_topology(AddressSpace *as)
{
    MemoryRegion *physmr = memory_region_get_flatview_root(as->root);

    flatviews_init(as->uc);
    if (!g_hash_table_lookup(as->uc->flat_views, physmr)) {
        generate_memory_topology(as->uc, physmr);
    }
    address_space_set_flatview(as);
}