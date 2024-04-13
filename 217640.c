decode_OFPAT_RAW11_POP_VLAN(struct ofpbuf *out)
{
    ofpact_put_STRIP_VLAN(out)->ofpact.raw = OFPAT_RAW11_POP_VLAN;
    return 0;
}