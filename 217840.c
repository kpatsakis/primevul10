format_OUTPUT(const struct ofpact_output *a,
              const struct ofpact_format_params *fp)
{
    if (ofp_to_u16(a->port) < ofp_to_u16(OFPP_MAX)) {
        ds_put_format(fp->s, "%soutput:%s", colors.special, colors.end);
    }
    ofputil_format_port(a->port, fp->port_map, fp->s);
    if (a->port == OFPP_CONTROLLER) {
        ds_put_format(fp->s, ":%"PRIu16, a->max_len);
    }
}