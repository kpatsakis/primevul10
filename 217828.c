parse_SET_IPV4_DST(char *arg, const struct ofpact_parse_params *pp)
{
    return str_to_ip(arg, &ofpact_put_SET_IPV4_DST(pp->ofpacts)->ipv4);
}