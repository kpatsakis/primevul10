decode_OFPAT_RAW11_OUTPUT(const struct ofp11_action_output *oao,
                          enum ofp_version ofp_version OVS_UNUSED,
                          struct ofpbuf *out)
{
    struct ofpact_output *output;
    enum ofperr error;

    output = ofpact_put_OUTPUT(out);
    output->max_len = ntohs(oao->max_len);

    error = ofputil_port_from_ofp11(oao->port, &output->port);
    if (error) {
        return error;
    }

    return ofpact_check_output_port(output->port, OFPP_MAX);
}