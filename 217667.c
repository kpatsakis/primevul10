decode_OFPAT_RAW_SET_DL_DST(const struct ofp_action_dl_addr *a,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    ofpact_put_SET_ETH_DST(out)->mac = a->dl_addr;
    return 0;
}