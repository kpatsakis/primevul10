decode_NXAST_RAW_OUTPUT_REG2(const struct nx_action_output_reg2 *naor,
                             enum ofp_version ofp_version OVS_UNUSED,
                             const struct vl_mff_map *vl_mff_map,
                             uint64_t *tlv_bitmap, struct ofpbuf *out)
{
    struct ofpact_output_reg *output_reg;
    enum ofperr error;

    output_reg = ofpact_put_OUTPUT_REG(out);
    output_reg->ofpact.raw = NXAST_RAW_OUTPUT_REG2;
    output_reg->src.ofs = nxm_decode_ofs(naor->ofs_nbits);
    output_reg->src.n_bits = nxm_decode_n_bits(naor->ofs_nbits);
    output_reg->max_len = ntohs(naor->max_len);

    struct ofpbuf b = ofpbuf_const_initializer(naor, ntohs(naor->len));
    ofpbuf_pull(&b, OBJECT_OFFSETOF(naor, pad));

    error = mf_vl_mff_nx_pull_header(&b, vl_mff_map, &output_reg->src.field,
                                     NULL, tlv_bitmap);
    if (error) {
        return error;
    }

    if (!is_all_zeros(b.data, b.size)) {
        return OFPERR_NXBRC_MUST_BE_ZERO;
    }

    return mf_check_src(&output_reg->src, NULL);
}