check_ENQUEUE(const struct ofpact_enqueue *a,
              const struct ofpact_check_params *cp)
{
    if (ofp_to_u16(a->port) >= ofp_to_u16(cp->max_ports)
        && a->port != OFPP_IN_PORT
        && a->port != OFPP_LOCAL) {
        return OFPERR_OFPBAC_BAD_OUT_PORT;
    }
    return 0;
}