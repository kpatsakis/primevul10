encode_OUTPUT_TRUNC(const struct ofpact_output_trunc *output_trunc,
                  enum ofp_version ofp_version OVS_UNUSED,
                  struct ofpbuf *out)
{
    struct nx_action_output_trunc *natrc = put_NXAST_OUTPUT_TRUNC(out);

    natrc->max_len = htonl(output_trunc->max_len);
    natrc->port = htons(ofp_to_u16(output_trunc->port));
}