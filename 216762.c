multi_get_queue(struct mbuf_set *ms)
{
    struct mbuf_item item;

    if (mbuf_extract_item(ms, &item)) /* cleartext IP packet */
    {
        unsigned int pip_flags = PIPV4_PASSTOS | PIPV6_IMCP_NOHOST_SERVER;

        set_prefix(item.instance);
        item.instance->context.c2.buf = item.buffer->buf;
        if (item.buffer->flags & MF_UNICAST) /* --mssfix doesn't make sense for broadcast or multicast */
        {
            pip_flags |= PIP_MSSFIX;
        }
        process_ip_header(&item.instance->context, pip_flags, &item.instance->context.c2.buf);
        encrypt_sign(&item.instance->context, true);
        mbuf_free_buf(item.buffer);

        dmsg(D_MULTI_DEBUG, "MULTI: C2C/MCAST/BCAST");

        clear_prefix();
        return item.instance;
    }
    else
    {
        return NULL;
    }
}