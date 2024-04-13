ofpact_copy(struct ofpbuf *out, const struct ofpact *a)
{
    ofpbuf_put(out, a, OFPACT_ALIGN(a->len));
}