decode_NXAST_RAW_OUTPUT_TRUNC(const struct nx_action_output_trunc *natrc,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    struct ofpact_output_trunc *output_trunc;

    output_trunc = ofpact_put_OUTPUT_TRUNC(out);
    output_trunc->max_len = ntohl(natrc->max_len);
    output_trunc->port = u16_to_ofp(ntohs(natrc->port));

    if (output_trunc->max_len < ETH_HEADER_LEN) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }
    return 0;
}