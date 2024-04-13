multi_reap_range(const struct multi_context *m,
                 int start_bucket,
                 int end_bucket)
{
    struct gc_arena gc = gc_new();
    struct hash_iterator hi;
    struct hash_element *he;

    if (start_bucket < 0)
    {
        start_bucket = 0;
        end_bucket = hash_n_buckets(m->vhash);
    }

    dmsg(D_MULTI_DEBUG, "MULTI: REAP range %d -> %d", start_bucket, end_bucket);
    hash_iterator_init_range(m->vhash, &hi, start_bucket, end_bucket);
    while ((he = hash_iterator_next(&hi)) != NULL)
    {
        struct multi_route *r = (struct multi_route *) he->value;
        if (!multi_route_defined(m, r))
        {
            dmsg(D_MULTI_DEBUG, "MULTI: REAP DEL %s",
                 mroute_addr_print(&r->addr, &gc));
            learn_address_script(m, NULL, "delete", &r->addr);
            multi_route_del(r);
            hash_iterator_delete_element(&hi);
        }
    }
    hash_iterator_free(&hi);
    gc_free(&gc);
}