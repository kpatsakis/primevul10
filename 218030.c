set_field_to_nxast(const struct ofpact_set_field *sf, struct ofpbuf *openflow)
{
    /* If 'sf' cannot be encoded as NXAST_REG_LOAD because it requires an
     * experimenter OXM or is variable length (or if it came in as
     * NXAST_REG_LOAD2), encode as NXAST_REG_LOAD2.  Otherwise use
     * NXAST_REG_LOAD, which is backward compatible. */
    if (sf->ofpact.raw == NXAST_RAW_REG_LOAD2
        || !mf_nxm_header(sf->field->id) || sf->field->variable_len) {
        struct ext_action_header *eah OVS_UNUSED;
        size_t start_ofs = openflow->size;

        eah = put_NXAST_REG_LOAD2(openflow);
        openflow->size = openflow->size - sizeof eah->pad;
        nx_put_entry(openflow, sf->field, 0, sf->value,
                     ofpact_set_field_mask(sf));
        pad_ofpat(openflow, start_ofs);
    } else {
        struct mf_subfield dst;
        uint64_t value;

        dst.ofs = dst.n_bits = 0;
        while (next_load_segment(sf, &dst, &value)) {
            put_reg_load(openflow, &dst, value);
        }
    }
}