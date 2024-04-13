management_get_peer_info(void *arg, const unsigned long cid)
{
    struct multi_context *m = (struct multi_context *) arg;
    struct multi_instance *mi = lookup_by_cid(m, cid);
    char *ret = NULL;

    if (mi)
    {
        ret = tls_get_peer_info(mi->context.c2.tls_multi);
    }

    return ret;
}