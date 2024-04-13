parse_SET_ETH_DST(char *arg, const struct ofpact_parse_params *pp)
{
    return str_to_mac(arg, &ofpact_put_SET_ETH_DST(pp->ofpacts)->mac);
}