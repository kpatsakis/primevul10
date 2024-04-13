decode_NXAST_RAW_REG_LOAD2(const struct ext_action_header *eah,
                           enum ofp_version ofp_version OVS_UNUSED,
                           const struct vl_mff_map *vl_mff_map,
                           uint64_t *tlv_bitmap, struct ofpbuf *out)
{
    struct ofpbuf b = ofpbuf_const_initializer(eah, ntohs(eah->len));
    ofpbuf_pull(&b, OBJECT_OFFSETOF(eah, pad));

    union mf_value value, mask;
    const struct mf_field *field;
    enum ofperr error;
    error = mf_vl_mff_nx_pull_entry(&b, vl_mff_map, &field, &value, &mask,
                                    tlv_bitmap);
    if (error) {
        return error;
    }

    if (!is_all_zeros(b.data, b.size)) {
        return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
    }

    if (!field->writable) {
        VLOG_WARN_RL(&rl, "destination field %s is not writable", field->name);
        return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
    }

    /* Put value and mask. */
    ofpact_put_reg_load2(out, field, &value, &mask);
    return 0;
}