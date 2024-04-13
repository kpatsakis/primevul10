management_callback_kill_by_addr(void *arg, const in_addr_t addr, const int port)
{
    struct multi_context *m = (struct multi_context *) arg;
    struct hash_iterator hi;
    struct hash_element *he;
    struct openvpn_sockaddr saddr;
    struct mroute_addr maddr;
    int count = 0;

    CLEAR(saddr);
    saddr.addr.in4.sin_family = AF_INET;
    saddr.addr.in4.sin_addr.s_addr = htonl(addr);
    saddr.addr.in4.sin_port = htons(port);
    if (mroute_extract_openvpn_sockaddr(&maddr, &saddr, true))
    {
        hash_iterator_init(m->iter, &hi);
        while ((he = hash_iterator_next(&hi)))
        {
            struct multi_instance *mi = (struct multi_instance *) he->value;
            if (!mi->halt && mroute_addr_equal(&maddr, &mi->real))
            {
                multi_signal_instance(m, mi, SIGTERM);
                ++count;
            }
        }
        hash_iterator_free(&hi);
    }
    return count;
}