decode_NXAST_RAW_EXIT(struct ofpbuf *out)
{
    ofpact_put_EXIT(out);
    return 0;
}