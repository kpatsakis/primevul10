decode_NXAST_RAW_CT_CLEAR(struct ofpbuf *out)
{
    ofpact_put_CT_CLEAR(out);
    return 0;
}