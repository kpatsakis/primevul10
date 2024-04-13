multi_select_virtual_addr(struct multi_context *m, struct multi_instance *mi)
{
    struct gc_arena gc = gc_new();

    /*
     * If ifconfig addresses were set by dynamic config file,
     * release pool addresses, otherwise keep them.
     */
    if (mi->context.options.push_ifconfig_defined)
    {
        /* ifconfig addresses were set statically,
         * release dynamic allocation */
        if (mi->vaddr_handle >= 0)
        {
            ifconfig_pool_release(m->ifconfig_pool, mi->vaddr_handle, true);
            mi->vaddr_handle = -1;
        }

        mi->context.c2.push_ifconfig_defined = true;
        mi->context.c2.push_ifconfig_local = mi->context.options.push_ifconfig_local;
        mi->context.c2.push_ifconfig_remote_netmask = mi->context.options.push_ifconfig_remote_netmask;
        mi->context.c2.push_ifconfig_local_alias = mi->context.options.push_ifconfig_local_alias;

        /* the current implementation does not allow "static IPv4, pool IPv6",
         * (see below) so issue a warning if that happens - don't break the
         * session, though, as we don't even know if this client WANTS IPv6
         */
        if (mi->context.options.ifconfig_ipv6_pool_defined
            && !mi->context.options.push_ifconfig_ipv6_defined)
        {
            msg( M_INFO, "MULTI_sva: WARNING: if --ifconfig-push is used for IPv4, automatic IPv6 assignment from --ifconfig-ipv6-pool does not work.  Use --ifconfig-ipv6-push for IPv6 then." );
        }
    }
    else if (m->ifconfig_pool && mi->vaddr_handle < 0) /* otherwise, choose a pool address */
    {
        in_addr_t local = 0, remote = 0;
        struct in6_addr remote_ipv6;
        const char *cn = NULL;

        if (!mi->context.options.duplicate_cn)
        {
            cn = tls_common_name(mi->context.c2.tls_multi, true);
        }

        CLEAR(remote_ipv6);
        mi->vaddr_handle = ifconfig_pool_acquire(m->ifconfig_pool, &local, &remote, &remote_ipv6, cn);
        if (mi->vaddr_handle >= 0)
        {
            const int tunnel_type = TUNNEL_TYPE(mi->context.c1.tuntap);
            const int tunnel_topology = TUNNEL_TOPOLOGY(mi->context.c1.tuntap);

            msg( M_INFO, "MULTI_sva: pool returned IPv4=%s, IPv6=%s",
                 print_in_addr_t( remote, 0, &gc ),
                 (mi->context.options.ifconfig_ipv6_pool_defined
                  ? print_in6_addr( remote_ipv6, 0, &gc )
                  : "(Not enabled)") );

            /* set push_ifconfig_remote_netmask from pool ifconfig address(es) */
            mi->context.c2.push_ifconfig_local = remote;
            if (tunnel_type == DEV_TYPE_TAP || (tunnel_type == DEV_TYPE_TUN && tunnel_topology == TOP_SUBNET))
            {
                mi->context.c2.push_ifconfig_remote_netmask = mi->context.options.ifconfig_pool_netmask;
                if (!mi->context.c2.push_ifconfig_remote_netmask)
                {
                    mi->context.c2.push_ifconfig_remote_netmask = mi->context.c1.tuntap->remote_netmask;
                }
            }
            else if (tunnel_type == DEV_TYPE_TUN)
            {
                if (tunnel_topology == TOP_P2P)
                {
                    mi->context.c2.push_ifconfig_remote_netmask = mi->context.c1.tuntap->local;
                }
                else if (tunnel_topology == TOP_NET30)
                {
                    mi->context.c2.push_ifconfig_remote_netmask = local;
                }
            }

            if (mi->context.c2.push_ifconfig_remote_netmask)
            {
                mi->context.c2.push_ifconfig_defined = true;
            }
            else
            {
                msg(D_MULTI_ERRORS, "MULTI: no --ifconfig-pool netmask parameter is available to push to %s",
                    multi_instance_string(mi, false, &gc));
            }

            if (mi->context.options.ifconfig_ipv6_pool_defined)
            {
                mi->context.c2.push_ifconfig_ipv6_local = remote_ipv6;
                mi->context.c2.push_ifconfig_ipv6_remote =
                    mi->context.c1.tuntap->local_ipv6;
                mi->context.c2.push_ifconfig_ipv6_netbits =
                    mi->context.options.ifconfig_ipv6_netbits;
                mi->context.c2.push_ifconfig_ipv6_defined = true;
            }
        }
        else
        {
            msg(D_MULTI_ERRORS, "MULTI: no free --ifconfig-pool addresses are available");
        }
    }

    /* IPv6 push_ifconfig is a bit problematic - since IPv6 shares the
     * pool handling with IPv4, the combination "static IPv4, dynamic IPv6"
     * will fail (because no pool will be allocated in this case).
     * OTOH, this doesn't make too much sense in reality - and the other
     * way round ("dynamic IPv4, static IPv6") or "both static" makes sense
     * -> and so it's implemented right now
     */
    if (mi->context.options.push_ifconfig_ipv6_defined)
    {
        mi->context.c2.push_ifconfig_ipv6_local =
            mi->context.options.push_ifconfig_ipv6_local;
        mi->context.c2.push_ifconfig_ipv6_remote =
            mi->context.options.push_ifconfig_ipv6_remote;
        mi->context.c2.push_ifconfig_ipv6_netbits =
            mi->context.options.push_ifconfig_ipv6_netbits;
        mi->context.c2.push_ifconfig_ipv6_defined = true;

        msg( M_INFO, "MULTI_sva: push_ifconfig_ipv6 %s/%d",
             print_in6_addr( mi->context.c2.push_ifconfig_ipv6_local, 0, &gc ),
             mi->context.c2.push_ifconfig_ipv6_netbits );
    }

    gc_free(&gc);
}