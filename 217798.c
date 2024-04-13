ofpact_finish(struct ofpbuf *ofpacts, struct ofpact *ofpact)
{
    ptrdiff_t len;

    ovs_assert(ofpact == ofpacts->header);
    len = (char *) ofpbuf_tail(ofpacts) - (char *) ofpact;
    ovs_assert(len > 0 && len <= UINT16_MAX);
    ofpact->len = len;
    ofpbuf_padto(ofpacts, OFPACT_ALIGN(ofpacts->size));

    return ofpacts->header;
}