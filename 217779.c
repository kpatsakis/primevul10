check_UNROLL_XLATE(const struct ofpact_unroll_xlate *a OVS_UNUSED,
                   const struct ofpact_check_params *cp OVS_UNUSED)
{
    /* UNROLL is an internal action that should never be seen via OpenFlow. */
    return OFPERR_OFPBAC_BAD_TYPE;
}