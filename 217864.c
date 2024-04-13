put_be16(struct ofpbuf *b, ovs_be16 x)
{
    ofpbuf_put(b, &x, sizeof x);
}