ofpact_next_flattened(const struct ofpact *ofpact)
{
    switch (ofpact->type) {
    case OFPACT_OUTPUT:
    case OFPACT_GROUP:
    case OFPACT_CONTROLLER:
    case OFPACT_ENQUEUE:
    case OFPACT_OUTPUT_REG:
    case OFPACT_OUTPUT_TRUNC:
    case OFPACT_BUNDLE:
    case OFPACT_SET_FIELD:
    case OFPACT_SET_VLAN_VID:
    case OFPACT_SET_VLAN_PCP:
    case OFPACT_STRIP_VLAN:
    case OFPACT_PUSH_VLAN:
    case OFPACT_SET_ETH_SRC:
    case OFPACT_SET_ETH_DST:
    case OFPACT_SET_IPV4_SRC:
    case OFPACT_SET_IPV4_DST:
    case OFPACT_SET_IP_DSCP:
    case OFPACT_SET_IP_ECN:
    case OFPACT_SET_IP_TTL:
    case OFPACT_SET_L4_SRC_PORT:
    case OFPACT_SET_L4_DST_PORT:
    case OFPACT_REG_MOVE:
    case OFPACT_STACK_PUSH:
    case OFPACT_STACK_POP:
    case OFPACT_DEC_TTL:
    case OFPACT_SET_MPLS_LABEL:
    case OFPACT_SET_MPLS_TC:
    case OFPACT_SET_MPLS_TTL:
    case OFPACT_DEC_MPLS_TTL:
    case OFPACT_PUSH_MPLS:
    case OFPACT_POP_MPLS:
    case OFPACT_SET_TUNNEL:
    case OFPACT_SET_QUEUE:
    case OFPACT_POP_QUEUE:
    case OFPACT_FIN_TIMEOUT:
    case OFPACT_RESUBMIT:
    case OFPACT_LEARN:
    case OFPACT_CONJUNCTION:
    case OFPACT_MULTIPATH:
    case OFPACT_NOTE:
    case OFPACT_EXIT:
    case OFPACT_SAMPLE:
    case OFPACT_UNROLL_XLATE:
    case OFPACT_CT_CLEAR:
    case OFPACT_DEBUG_RECIRC:
    case OFPACT_DEBUG_SLOW:
    case OFPACT_METER:
    case OFPACT_CLEAR_ACTIONS:
    case OFPACT_WRITE_METADATA:
    case OFPACT_GOTO_TABLE:
    case OFPACT_NAT:
    case OFPACT_ENCAP:
    case OFPACT_DECAP:
    case OFPACT_DEC_NSH_TTL:
        return ofpact_next(ofpact);

    case OFPACT_CLONE:
        return ofpact_get_CLONE(ofpact)->actions;

    case OFPACT_CT:
        return ofpact_get_CT(ofpact)->actions;

    case OFPACT_WRITE_ACTIONS:
        return ofpact_get_WRITE_ACTIONS(ofpact)->actions;
    }

    OVS_NOT_REACHED();
}