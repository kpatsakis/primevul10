static void phys_map_node_reserve(AddressSpaceDispatch *d, PhysPageMap *map, unsigned nodes)
{
    if (map->nodes_nb + nodes > map->nodes_nb_alloc) {
        map->nodes_nb_alloc = MAX(d->uc->alloc_hint, map->nodes_nb + nodes);
        map->nodes = g_renew(Node, map->nodes, map->nodes_nb_alloc);
        d->uc->alloc_hint = map->nodes_nb_alloc;
    }
}