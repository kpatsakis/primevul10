decode_NXAST_RAW_POP_QUEUE(struct ofpbuf *out)
{
    ofpact_put_POP_QUEUE(out);
    return 0;
}