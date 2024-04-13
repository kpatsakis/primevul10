encode_SET_FIELD(const struct ofpact_set_field *sf,
                 enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version >= OFP15_VERSION) {
        /* OF1.5+ only has Set-Field (reg_load is redundant so we drop it
         * entirely). */
        set_field_to_set_field(sf, ofp_version, out);
    } else if (sf->ofpact.raw == NXAST_RAW_REG_LOAD ||
               sf->ofpact.raw == NXAST_RAW_REG_LOAD2) {
        /* It came in as reg_load, send it out the same way. */
        set_field_to_nxast(sf, out);
    } else if (ofp_version < OFP12_VERSION) {
        /* OpenFlow 1.0 and 1.1 don't have Set-Field. */
        set_field_to_legacy_openflow(sf, ofp_version, out);
    } else if (is_all_ones(ofpact_set_field_mask(sf), sf->field->n_bytes)) {
        /* We're encoding to OpenFlow 1.2, 1.3, or 1.4.  The action sets an
         * entire field, so encode it as OFPAT_SET_FIELD. */
        set_field_to_set_field(sf, ofp_version, out);
    } else {
        /* We're encoding to OpenFlow 1.2, 1.3, or 1.4.  The action cannot be
         * encoded as OFPAT_SET_FIELD because it does not set an entire field,
         * so encode it as reg_load. */
        set_field_to_nxast(sf, out);
    }
}