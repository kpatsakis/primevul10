decode_OFPAT_RAW_SET_DL_SRC(const struct ofp_action_dl_addr *a,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    ofpact_put_SET_ETH_SRC(out)->mac = a->dl_addr;
    return 0;
}