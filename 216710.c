multi_get_instance_by_virtual_addr(struct multi_context *m,
                                   const struct mroute_addr *addr,
                                   bool cidr_routing)
{
    struct multi_route *route;
    struct multi_instance *ret = NULL;

    /* check for local address */
    if (mroute_addr_equal(addr, &m->local))
    {
        return NULL;
    }

    route = (struct multi_route *) hash_lookup(m->vhash, addr);

    /* does host route (possible cached) exist? */
    if (route && multi_route_defined(m, route))
    {
        struct multi_instance *mi = route->instance;
        route->last_reference = now;
        ret = mi;
    }
    else if (cidr_routing) /* do we need to regenerate a host route cache entry? */
    {
        struct mroute_helper *rh = m->route_helper;
        struct mroute_addr tryaddr;
        int i;

        /* cycle through each CIDR length */
        for (i = 0; i < rh->n_net_len; ++i)
        {
            tryaddr = *addr;
            tryaddr.type |= MR_WITH_NETBITS;
            tryaddr.netbits = rh->net_len[i];
            mroute_addr_mask_host_bits(&tryaddr);

            /* look up a possible route with netbits netmask */
            route = (struct multi_route *) hash_lookup(m->vhash, &tryaddr);

            if (route && multi_route_defined(m, route))
            {
                /* found an applicable route, cache host route */
                struct multi_instance *mi = route->instance;
                multi_learn_addr(m, mi, addr, MULTI_ROUTE_CACHE|MULTI_ROUTE_AGEABLE);
                ret = mi;
                break;
            }
        }
    }

#ifdef ENABLE_DEBUG
    if (check_debug_level(D_MULTI_DEBUG))
    {
        struct gc_arena gc = gc_new();
        const char *addr_text = mroute_addr_print(addr, &gc);
        if (ret)
        {
            dmsg(D_MULTI_DEBUG, "GET INST BY VIRT: %s -> %s via %s",
                 addr_text,
                 multi_instance_string(ret, false, &gc),
                 mroute_addr_print(&route->addr, &gc));
        }
        else
        {
            dmsg(D_MULTI_DEBUG, "GET INST BY VIRT: %s [failed]",
                 addr_text);
        }
        gc_free(&gc);
    }
#endif

    ASSERT(!(ret && ret->halt));
    return ret;
}