encode_OUTPUT(const struct ofpact_output *output,
              enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version == OFP10_VERSION) {
        struct ofp10_action_output *oao;

        oao = put_OFPAT10_OUTPUT(out);
        oao->port = htons(ofp_to_u16(output->port));
        oao->max_len = htons(output->max_len);
    } else {
        struct ofp11_action_output *oao;

        oao = put_OFPAT11_OUTPUT(out);
        oao->port = ofputil_port_to_ofp11(output->port);
        oao->max_len = htons(output->max_len);
    }
}