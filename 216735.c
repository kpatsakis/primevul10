multi_learn_addr(struct multi_context *m,
                 struct multi_instance *mi,
                 const struct mroute_addr *addr,
                 const unsigned int flags)
{
    struct hash_element *he;
    const uint32_t hv = hash_value(m->vhash, addr);
    struct hash_bucket *bucket = hash_bucket(m->vhash, hv);
    struct multi_route *oldroute = NULL;
    struct multi_instance *owner = NULL;
    struct gc_arena gc = gc_new();

    /* if route currently exists, get the instance which owns it */
    he = hash_lookup_fast(m->vhash, bucket, addr, hv);
    if (he)
    {
        oldroute = (struct multi_route *) he->value;
    }
    if (oldroute && multi_route_defined(m, oldroute))
    {
        owner = oldroute->instance;
    }

    /* do we need to add address to hash table? */
    if ((!owner || owner != mi) && mroute_learnable_address(addr, &gc)
        && !mroute_addr_equal(addr, &m->local))
    {
        struct multi_route *newroute;
        bool learn_succeeded = false;

        ALLOC_OBJ(newroute, struct multi_route);
        newroute->addr = *addr;
        newroute->instance = mi;
        newroute->flags = flags;
        newroute->last_reference = now;
        newroute->cache_generation = 0;

        /* The cache is invalidated when cache_generation is incremented */
        if (flags & MULTI_ROUTE_CACHE)
        {
            newroute->cache_generation = m->route_helper->cache_generation;
        }

        if (oldroute) /* route already exists? */
        {
            if (route_quota_test(mi) && learn_address_script(m, mi, "update", &newroute->addr))
            {
                learn_succeeded = true;
                owner = mi;
                multi_instance_inc_refcount(mi);
                route_quota_inc(mi);

                /* delete old route */
                multi_route_del(oldroute);

                /* modify hash table entry, replacing old route */
                he->key = &newroute->addr;
                he->value = newroute;
            }
        }
        else
        {
            if (route_quota_test(mi) && learn_address_script(m, mi, "add", &newroute->addr))
            {
                learn_succeeded = true;
                owner = mi;
                multi_instance_inc_refcount(mi);
                route_quota_inc(mi);

                /* add new route */
                hash_add_fast(m->vhash, bucket, &newroute->addr, hv, newroute);
            }
        }

        msg(D_MULTI_LOW, "MULTI: Learn%s: %s -> %s",
            learn_succeeded ? "" : " FAILED",
            mroute_addr_print(&newroute->addr, &gc),
            multi_instance_string(mi, false, &gc));

        if (!learn_succeeded)
        {
            free(newroute);
        }
    }
    gc_free(&gc);

    return owner;
}