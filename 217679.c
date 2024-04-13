decode_NXAST_RAW_REG_LOAD(const struct nx_action_reg_load *narl,
                          enum ofp_version ofp_version OVS_UNUSED,
                          const struct vl_mff_map *vl_mff_map,
                          uint64_t *tlv_bitmap, struct ofpbuf *out)
{
    struct mf_subfield dst;
    enum ofperr error;

    dst.ofs = nxm_decode_ofs(narl->ofs_nbits);
    dst.n_bits = nxm_decode_n_bits(narl->ofs_nbits);
    error = mf_vl_mff_mf_from_nxm_header(ntohl(narl->dst), vl_mff_map,
                                         &dst.field, tlv_bitmap);
    if (error) {
        return error;
    }

    error = mf_check_dst(&dst, NULL);
    if (error) {
        return error;
    }

    /* Reject 'narl' if a bit numbered 'n_bits' or higher is set to 1 in
     * narl->value. */
    if (dst.n_bits < 64 && ntohll(narl->value) >> dst.n_bits) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    struct ofpact_set_field *sf = ofpact_put_reg_load(out, dst.field, NULL,
                                                      NULL);
    bitwise_put(ntohll(narl->value),
                sf->value, dst.field->n_bytes, dst.ofs,
                dst.n_bits);
    bitwise_put(UINT64_MAX,
                ofpact_set_field_mask(sf), dst.field->n_bytes, dst.ofs,
                dst.n_bits);
    return 0;
}