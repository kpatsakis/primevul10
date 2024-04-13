decode_OFPAT_RAW_DEC_MPLS_TTL(struct ofpbuf *out)
{
    ofpact_put_DEC_MPLS_TTL(out);
    return 0;
}