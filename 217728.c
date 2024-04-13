ofpact_check_output_port(ofp_port_t port, ofp_port_t max_ports)
{
    switch (port) {
    case OFPP_IN_PORT:
    case OFPP_TABLE:
    case OFPP_NORMAL:
    case OFPP_FLOOD:
    case OFPP_ALL:
    case OFPP_CONTROLLER:
    case OFPP_LOCAL:
        return 0;

    case OFPP_NONE:
        return OFPERR_OFPBAC_BAD_OUT_PORT;

    default:
        if (ofp_to_u16(port) < ofp_to_u16(max_ports)) {
            return 0;
        }
        return OFPERR_OFPBAC_BAD_OUT_PORT;
    }
}