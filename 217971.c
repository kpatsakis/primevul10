parse_set_tunnel(char *arg, enum ofp_raw_action_type raw,
                 const struct ofpact_parse_params *pp)
{
    struct ofpact_tunnel *tunnel;

    tunnel = ofpact_put_SET_TUNNEL(pp->ofpacts);
    tunnel->ofpact.raw = raw;
    return str_to_u64(arg, &tunnel->tun_id);
}