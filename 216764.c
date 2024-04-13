multi_learn_in_addr_t(struct multi_context *m,
                      struct multi_instance *mi,
                      in_addr_t a,
                      int netbits,  /* -1 if host route, otherwise # of network bits in address */
                      bool primary)
{
    struct openvpn_sockaddr remote_si;
    struct mroute_addr addr;

    CLEAR(remote_si);
    remote_si.addr.in4.sin_family = AF_INET;
    remote_si.addr.in4.sin_addr.s_addr = htonl(a);
    ASSERT(mroute_extract_openvpn_sockaddr(&addr, &remote_si, false));

    if (netbits >= 0)
    {
        addr.type |= MR_WITH_NETBITS;
        addr.netbits = (uint8_t) netbits;
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