parse_OUTPUT_TRUNC(const char *arg,
                   const struct ofpact_parse_params *pp OVS_UNUSED)
{
    /* Disable output_trunc parsing.  Expose as output(port=N,max_len=M) and
     * reuse parse_OUTPUT to parse output_trunc action. */
    return xasprintf("unknown action %s", arg);
}