get_ofpact_map(enum ofp_version version)
{
    /* OpenFlow 1.0 actions. */
    static const struct ofpact_map of10[] = {
        { OFPACT_OUTPUT, 0 },
        { OFPACT_SET_VLAN_VID, 1 },
        { OFPACT_SET_VLAN_PCP, 2 },
        { OFPACT_STRIP_VLAN, 3 },
        { OFPACT_SET_ETH_SRC, 4 },
        { OFPACT_SET_ETH_DST, 5 },
        { OFPACT_SET_IPV4_SRC, 6 },
        { OFPACT_SET_IPV4_DST, 7 },
        { OFPACT_SET_IP_DSCP, 8 },
        { OFPACT_SET_L4_SRC_PORT, 9 },
        { OFPACT_SET_L4_DST_PORT, 10 },
        { OFPACT_ENQUEUE, 11 },
        { 0, -1 },
    };

    /* OpenFlow 1.1 actions. */
    static const struct ofpact_map of11[] = {
        { OFPACT_OUTPUT, 0 },
        { OFPACT_SET_VLAN_VID, 1 },
        { OFPACT_SET_VLAN_PCP, 2 },
        { OFPACT_SET_ETH_SRC, 3 },
        { OFPACT_SET_ETH_DST, 4 },
        { OFPACT_SET_IPV4_SRC, 5 },
        { OFPACT_SET_IPV4_DST, 6 },
        { OFPACT_SET_IP_DSCP, 7 },
        { OFPACT_SET_IP_ECN, 8 },
        { OFPACT_SET_L4_SRC_PORT, 9 },
        { OFPACT_SET_L4_DST_PORT, 10 },
        /* OFPAT_COPY_TTL_OUT (11) not supported. */
        /* OFPAT_COPY_TTL_IN (12) not supported. */
        { OFPACT_SET_MPLS_LABEL, 13 },
        { OFPACT_SET_MPLS_TC, 14 },
        { OFPACT_SET_MPLS_TTL, 15 },
        { OFPACT_DEC_MPLS_TTL, 16 },
        { OFPACT_PUSH_VLAN, 17 },
        { OFPACT_STRIP_VLAN, 18 },
        { OFPACT_PUSH_MPLS, 19 },
        { OFPACT_POP_MPLS, 20 },
        { OFPACT_SET_QUEUE, 21 },
        { OFPACT_GROUP, 22 },
        { OFPACT_SET_IP_TTL, 23 },
        { OFPACT_DEC_TTL, 24 },
        { 0, -1 },
    };

    /* OpenFlow 1.2, 1.3, and 1.4 actions. */
    static const struct ofpact_map of12[] = {
        { OFPACT_OUTPUT, 0 },
        /* OFPAT_COPY_TTL_OUT (11) not supported. */
        /* OFPAT_COPY_TTL_IN (12) not supported. */
        { OFPACT_SET_MPLS_TTL, 15 },
        { OFPACT_DEC_MPLS_TTL, 16 },
        { OFPACT_PUSH_VLAN, 17 },
        { OFPACT_STRIP_VLAN, 18 },
        { OFPACT_PUSH_MPLS, 19 },
        { OFPACT_POP_MPLS, 20 },
        { OFPACT_SET_QUEUE, 21 },
        { OFPACT_GROUP, 22 },
        { OFPACT_SET_IP_TTL, 23 },
        { OFPACT_DEC_TTL, 24 },
        { OFPACT_SET_FIELD, 25 },
        /* OF1.3+ OFPAT_PUSH_PBB (26) not supported. */
        /* OF1.3+ OFPAT_POP_PBB (27) not supported. */
        { 0, -1 },
    };

    switch (version) {
    case OFP10_VERSION:
        return of10;

    case OFP11_VERSION:
        return of11;

    case OFP12_VERSION:
    case OFP13_VERSION:
    case OFP14_VERSION:
    case OFP15_VERSION:
    case OFP16_VERSION:
    default:
        return of12;
    }
}