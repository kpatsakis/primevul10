multi_set_virtual_addr_env(struct multi_instance *mi)
{
    setenv_del(mi->context.c2.es, "ifconfig_pool_local_ip");
    setenv_del(mi->context.c2.es, "ifconfig_pool_remote_ip");
    setenv_del(mi->context.c2.es, "ifconfig_pool_netmask");

    if (mi->context.c2.push_ifconfig_defined)
    {
        const int tunnel_type = TUNNEL_TYPE(mi->context.c1.tuntap);
        const int tunnel_topology = TUNNEL_TOPOLOGY(mi->context.c1.tuntap);

        setenv_in_addr_t(mi->context.c2.es,
                         "ifconfig_pool_remote_ip",
                         mi->context.c2.push_ifconfig_local,
                         SA_SET_IF_NONZERO);

        if (tunnel_type == DEV_TYPE_TAP || (tunnel_type == DEV_TYPE_TUN && tunnel_topology == TOP_SUBNET))
        {
            setenv_in_addr_t(mi->context.c2.es,
                             "ifconfig_pool_netmask",
                             mi->context.c2.push_ifconfig_remote_netmask,
                             SA_SET_IF_NONZERO);
        }
        else if (tunnel_type == DEV_TYPE_TUN)
        {
            setenv_in_addr_t(mi->context.c2.es,
                             "ifconfig_pool_local_ip",
                             mi->context.c2.push_ifconfig_remote_netmask,
                             SA_SET_IF_NONZERO);
        }
    }

    setenv_del(mi->context.c2.es, "ifconfig_pool_local_ip6");
    setenv_del(mi->context.c2.es, "ifconfig_pool_remote_ip6");
    setenv_del(mi->context.c2.es, "ifconfig_pool_ip6_netbits");

    if (mi->context.c2.push_ifconfig_ipv6_defined)
    {
        setenv_in6_addr(mi->context.c2.es,
                        "ifconfig_pool_remote",
                        &mi->context.c2.push_ifconfig_ipv6_local,
                        SA_SET_IF_NONZERO);
        setenv_in6_addr(mi->context.c2.es,
                        "ifconfig_pool_local",
                        &mi->context.c2.push_ifconfig_ipv6_remote,
                        SA_SET_IF_NONZERO);
        setenv_int(mi->context.c2.es,
                   "ifconfig_pool_ip6_netbits",
                   mi->context.c2.push_ifconfig_ipv6_netbits);
    }
}