ofpact_put(struct ofpbuf *ofpacts, enum ofpact_type type, size_t len)
{
    struct ofpact *ofpact;

    ofpacts->header = ofpbuf_put_uninit(ofpacts, len);
    ofpact = ofpacts->header;
    ofpact_init(ofpact, type, len);
    return ofpact;
}