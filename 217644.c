decode_NXAST_RAW_OUTPUT_REG(const struct nx_action_output_reg *naor,
                            enum ofp_version ofp_version OVS_UNUSED,
                            const struct vl_mff_map *vl_mff_map,
                            uint64_t *tlv_bitmap, struct ofpbuf *out)
{
    struct ofpact_output_reg *output_reg;
    enum ofperr error;

    if (!is_all_zeros(naor->zero, sizeof naor->zero)) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    output_reg = ofpact_put_OUTPUT_REG(out);
    output_reg->ofpact.raw = NXAST_RAW_OUTPUT_REG;
    output_reg->src.ofs = nxm_decode_ofs(naor->ofs_nbits);
    output_reg->src.n_bits = nxm_decode_n_bits(naor->ofs_nbits);
    output_reg->max_len = ntohs(naor->max_len);
    error = mf_vl_mff_mf_from_nxm_header(ntohl(naor->src), vl_mff_map,
                                         &output_reg->src.field, tlv_bitmap);
    if (error) {
        return error;
    }

    return mf_check_src(&output_reg->src, NULL);
}