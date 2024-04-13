parse_UNROLL_XLATE(char *arg OVS_UNUSED,
                   const struct ofpact_parse_params *pp OVS_UNUSED)
{
    return xasprintf("UNROLL is an internal action "
                     "that shouldn't be used via OpenFlow");
}