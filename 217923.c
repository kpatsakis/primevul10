decode_NXAST_RAW_DEC_NSH_TTL(struct ofpbuf *out)
{
    ofpact_put_DEC_NSH_TTL(out);
    return 0;
}