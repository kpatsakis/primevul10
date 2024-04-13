check_stale_routes(struct multi_context *m)
{

    struct gc_arena gc = gc_new();
    struct hash_iterator hi;
    struct hash_element *he;

    dmsg(D_MULTI_DEBUG, "MULTI: Checking stale routes");
    hash_iterator_init_range(m->vhash, &hi, 0, hash_n_buckets(m->vhash));
    while ((he = hash_iterator_next(&hi)) != NULL)
    {
        struct multi_route *r = (struct multi_route *) he->value;
        if (multi_route_defined(m, r) && difftime(now, r->last_reference) >= m->top.options.stale_routes_ageing_time)
        {
            dmsg(D_MULTI_DEBUG, "MULTI: Deleting stale route for address '%s'",
                 mroute_addr_print(&r->addr, &gc));
            learn_address_script(m, NULL, "delete", &r->addr);
            multi_route_del(r);
            hash_iterator_delete_element(&hi);
        }
    }
    hash_iterator_free(&hi);
    gc_free(&gc);
}