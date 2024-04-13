decode_OFPAT_RAW10_STRIP_VLAN(struct ofpbuf *out)
{
    ofpact_put_STRIP_VLAN(out)->ofpact.raw = OFPAT_RAW10_STRIP_VLAN;
    return 0;
}