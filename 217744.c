encode_OUTPUT_REG(const struct ofpact_output_reg *output_reg,
                  enum ofp_version ofp_version OVS_UNUSED,
                  struct ofpbuf *out)
{
    /* If 'output_reg' came in as an NXAST_RAW_OUTPUT_REG2 action, or if it
     * cannot be encoded in the older form, encode it as
     * NXAST_RAW_OUTPUT_REG2. */
    if (output_reg->ofpact.raw == NXAST_RAW_OUTPUT_REG2
        || !mf_nxm_header(output_reg->src.field->id)) {
        struct nx_action_output_reg2 *naor = put_NXAST_OUTPUT_REG2(out);
        size_t size = out->size;

        naor->ofs_nbits = nxm_encode_ofs_nbits(output_reg->src.ofs,
                                               output_reg->src.n_bits);
        naor->max_len = htons(output_reg->max_len);

        out->size = size - sizeof naor->pad;
        nx_put_mff_header(out, output_reg->src.field, 0, false);
        out->size = size;
    } else {
        struct nx_action_output_reg *naor = put_NXAST_OUTPUT_REG(out);

        naor->ofs_nbits = nxm_encode_ofs_nbits(output_reg->src.ofs,
                                               output_reg->src.n_bits);
        naor->src = htonl(nxm_header_from_mff(output_reg->src.field));
        naor->max_len = htons(output_reg->max_len);
    }
}