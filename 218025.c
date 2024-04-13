action_set_classify(const struct ofpact *a)
{
    switch (a->type) {
#define SLOT(OFPACT) case OFPACT: return ACTION_SLOT_##OFPACT;
        ACTION_SET_ORDER
#undef SLOT

#define FINAL(OFPACT) case OFPACT: return ACTION_SLOT_##OFPACT;
        ACTION_SET_FINAL_PRIORITY
#undef FINAL

    case OFPACT_SET_FIELD:
    case OFPACT_REG_MOVE:
    case OFPACT_SET_ETH_DST:
    case OFPACT_SET_ETH_SRC:
    case OFPACT_SET_IP_DSCP:
    case OFPACT_SET_IP_ECN:
    case OFPACT_SET_IP_TTL:
    case OFPACT_SET_IPV4_DST:
    case OFPACT_SET_IPV4_SRC:
    case OFPACT_SET_L4_DST_PORT:
    case OFPACT_SET_L4_SRC_PORT:
    case OFPACT_SET_MPLS_LABEL:
    case OFPACT_SET_MPLS_TC:
    case OFPACT_SET_MPLS_TTL:
    case OFPACT_SET_QUEUE:
    case OFPACT_SET_TUNNEL:
    case OFPACT_SET_VLAN_PCP:
    case OFPACT_SET_VLAN_VID:
        return ACTION_SLOT_SET_OR_MOVE;

    case OFPACT_BUNDLE:
    case OFPACT_CLEAR_ACTIONS:
    case OFPACT_CLONE:
    case OFPACT_NAT:
    case OFPACT_CONTROLLER:
    case OFPACT_ENQUEUE:
    case OFPACT_EXIT:
    case OFPACT_UNROLL_XLATE:
    case OFPACT_FIN_TIMEOUT:
    case OFPACT_GOTO_TABLE:
    case OFPACT_LEARN:
    case OFPACT_CONJUNCTION:
    case OFPACT_METER:
    case OFPACT_MULTIPATH:
    case OFPACT_NOTE:
    case OFPACT_OUTPUT_REG:
    case OFPACT_OUTPUT_TRUNC:
    case OFPACT_POP_QUEUE:
    case OFPACT_SAMPLE:
    case OFPACT_STACK_POP:
    case OFPACT_STACK_PUSH:
    case OFPACT_WRITE_ACTIONS:
    case OFPACT_WRITE_METADATA:
    case OFPACT_DEBUG_RECIRC:
    case OFPACT_DEBUG_SLOW:
        return ACTION_SLOT_INVALID;

    default:
        OVS_NOT_REACHED();
    }
}