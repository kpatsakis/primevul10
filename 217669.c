check_OUTPUT(const struct ofpact_output *a,
             const struct ofpact_check_params *cp)
{
    return ofpact_check_output_port(a->port, cp->max_ports);
}