multi_add_mbuf(struct multi_context *m,
               struct multi_instance *mi,
               struct mbuf_buffer *mb)
{
    if (multi_output_queue_ready(m, mi))
    {
        struct mbuf_item item;
        item.buffer = mb;
        item.instance = mi;
        mbuf_add_item(m->mbuf, &item);
    }
    else
    {
        msg(D_MULTI_DROPPED, "MULTI: packet dropped due to output saturation (multi_add_mbuf)");
    }
}