multi_unicast(struct multi_context *m,
              const struct buffer *buf,
              struct multi_instance *mi)
{
    struct mbuf_buffer *mb;

    if (BLEN(buf) > 0)
    {
        mb = mbuf_alloc_buf(buf);
        mb->flags = MF_UNICAST;
        multi_add_mbuf(m, mi, mb);
        mbuf_free_buf(mb);
    }
}