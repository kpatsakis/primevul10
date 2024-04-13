parse_SET_ETH_SRC(char *arg, const struct ofpact_parse_params *pp)
{
    return str_to_mac(arg, &ofpact_put_SET_ETH_SRC(pp->ofpacts)->mac);
}