decode_OFPAT_RAW10_OUTPUT(const struct ofp10_action_output *oao,
                          enum ofp_version ofp_version OVS_UNUSED,
                          struct ofpbuf *out)
{
    struct ofpact_output *output;

    output = ofpact_put_OUTPUT(out);
    output->port = u16_to_ofp(ntohs(oao->port));
    output->max_len = ntohs(oao->max_len);

    return ofpact_check_output_port(output->port, OFPP_MAX);
}