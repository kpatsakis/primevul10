parse_OUTPUT(const char *arg, const struct ofpact_parse_params *pp)
{
    if (strstr(arg, "port") && strstr(arg, "max_len")) {
        struct ofpact_output_trunc *output_trunc;

        output_trunc = ofpact_put_OUTPUT_TRUNC(pp->ofpacts);
        return parse_truncate_subfield(arg, pp, output_trunc);
    }

    ofp_port_t port;
    if (ofputil_port_from_string(arg, pp->port_map, &port)) {
        struct ofpact_output *output = ofpact_put_OUTPUT(pp->ofpacts);
        output->port = port;
        output->max_len = output->port == OFPP_CONTROLLER ? UINT16_MAX : 0;
        return NULL;
    }

    struct mf_subfield src;
    char *error = mf_parse_subfield(&src, arg);
    if (!error) {
        struct ofpact_output_reg *output_reg;

        output_reg = ofpact_put_OUTPUT_REG(pp->ofpacts);
        output_reg->max_len = UINT16_MAX;
        output_reg->src = src;
        return NULL;
    }
    free(error);

    return xasprintf("%s: output to unknown port", arg);
}