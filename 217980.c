decode_OFPAT_RAW11_SET_NW_ECN(uint8_t ecn,
                              enum ofp_version ofp_version OVS_UNUSED,
                              struct ofpbuf *out)
{
    if (ecn & ~IP_ECN_MASK) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    } else {
        ofpact_put_SET_IP_ECN(out)->ecn = ecn;
        return 0;
    }
}