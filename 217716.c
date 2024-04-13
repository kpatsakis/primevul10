decode_OFPAT_RAW_SET_NW_TOS(uint8_t dscp,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    if (dscp & ~IP_DSCP_MASK) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    } else {
        ofpact_put_SET_IP_DSCP(out)->dscp = dscp;
        return 0;
    }
}