check_OUTPUT_TRUNC(const struct ofpact_output_trunc *a,
                   const struct ofpact_check_params *cp)
{
    return ofpact_check_output_port(a->port, cp->max_ports);
}