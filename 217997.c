put_be32(struct ofpbuf *b, ovs_be32 x)
{
    ofpbuf_put(b, &x, sizeof x);
}