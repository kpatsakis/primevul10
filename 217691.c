decode_ofpat_set_field(const struct ofp12_action_set_field *oasf,
                       bool may_mask, const struct vl_mff_map *vl_mff_map,
                       uint64_t *tlv_bitmap, struct ofpbuf *ofpacts)
{
    struct ofpbuf b = ofpbuf_const_initializer(oasf, ntohs(oasf->len));
    ofpbuf_pull(&b, OBJECT_OFFSETOF(oasf, pad));

    union mf_value value, mask;
    const struct mf_field *field;
    enum ofperr error;
    error  = mf_vl_mff_nx_pull_entry(&b, vl_mff_map, &field, &value,
                                     may_mask ? &mask : NULL, tlv_bitmap);
    if (error) {
        return (error == OFPERR_OFPBMC_BAD_MASK
                ? OFPERR_OFPBAC_BAD_SET_MASK
                : error);
    }

    if (!may_mask) {
        memset(&mask, 0xff, field->n_bytes);
    }

    if (!is_all_zeros(b.data, b.size)) {
        return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
    }

    /* OpenFlow says specifically that one may not set OXM_OF_IN_PORT via
     * Set-Field. */
    if (field->id == MFF_IN_PORT_OXM) {
        return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
    }

    /* oxm_length is now validated to be compatible with mf_value. */
    if (!field->writable) {
        VLOG_WARN_RL(&rl, "destination field %s is not writable",
                     field->name);
        return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
    }

    /* The value must be valid for match.  OpenFlow 1.5 also says,
     * "In an OXM_OF_VLAN_VID set-field action, the OFPVID_PRESENT bit must be
     * a 1-bit in oxm_value and in oxm_mask." */
    if (!mf_is_value_valid(field, &value)
        || (field->id == MFF_VLAN_VID
            && (!(mask.be16 & htons(OFPVID12_PRESENT))
                || !(value.be16 & htons(OFPVID12_PRESENT))))) {
        struct ds ds = DS_EMPTY_INITIALIZER;
        mf_format(field, &value, NULL, NULL, &ds);
        VLOG_WARN_RL(&rl, "Invalid value for set field %s: %s",
                     field->name, ds_cstr(&ds));
        ds_destroy(&ds);

        return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
    }

    ofpact_put_set_field(ofpacts, field, &value, &mask);
    return 0;
}