multi_learn_in6_addr(struct multi_context *m,
                     struct multi_instance *mi,
                     struct in6_addr a6,
                     int netbits,   /* -1 if host route, otherwise # of network bits in address */
                     bool primary)
{
    struct mroute_addr addr;

    addr.len = 16;
    addr.type = MR_ADDR_IPV6;
    addr.netbits = 0;
    addr.v6.addr = a6;

    if (netbits >= 0)
    {
        addr.type |= MR_WITH_NETBITS;
        addr.netbits = (uint8_t) netbits;
        mroute_addr_mask_host_bits( &addr );
    }

    {
        struct multi_instance *owner = multi_learn_addr(m, mi, &addr, 0);
#ifdef MANAGEMENT_DEF_AUTH
        if (management && owner)
        {
            management_learn_addr(management, &mi->context.c2.mda_context, &addr, primary);
        }
#endif
        return owner;
    }
}