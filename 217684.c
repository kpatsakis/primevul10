ofpact_init(struct ofpact *ofpact, enum ofpact_type type, size_t len)
{
    memset(ofpact, 0, len);
    ofpact->type = type;
    ofpact->raw = -1;
    ofpact->len = len;
}