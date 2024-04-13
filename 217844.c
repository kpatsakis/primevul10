encode_REG_MOVE(const struct ofpact_reg_move *move,
                enum ofp_version ofp_version, struct ofpbuf *out)
{
    /* For OpenFlow 1.3, the choice of ONFACT_RAW13_COPY_FIELD versus
     * NXAST_RAW_REG_MOVE is somewhat difficult.  Neither one is guaranteed to
     * be supported by every OpenFlow 1.3 implementation.  It would be ideal to
     * probe for support.  Until we have that ability, we currently prefer
     * NXAST_RAW_REG_MOVE for backward compatibility with older Open vSwitch
     * versions.  */
    size_t start_ofs = out->size;
    if (ofp_version >= OFP15_VERSION) {
        struct ofp15_action_copy_field *copy = put_OFPAT15_COPY_FIELD(out);
        copy->n_bits = htons(move->dst.n_bits);
        copy->src_offset = htons(move->src.ofs);
        copy->dst_offset = htons(move->dst.ofs);
        out->size = out->size - sizeof copy->pad2;
        nx_put_mff_header(out, move->src.field, ofp_version, false);
        nx_put_mff_header(out, move->dst.field, ofp_version, false);
    } else if (ofp_version == OFP13_VERSION
               && move->ofpact.raw == ONFACT_RAW13_COPY_FIELD) {
        struct onf_action_copy_field *copy = put_ONFACT13_COPY_FIELD(out);
        copy->n_bits = htons(move->dst.n_bits);
        copy->src_offset = htons(move->src.ofs);
        copy->dst_offset = htons(move->dst.ofs);
        out->size = out->size - sizeof copy->pad3;
        nx_put_mff_header(out, move->src.field, ofp_version, false);
        nx_put_mff_header(out, move->dst.field, ofp_version, false);
    } else {
        struct nx_action_reg_move *narm = put_NXAST_REG_MOVE(out);
        narm->n_bits = htons(move->dst.n_bits);
        narm->src_ofs = htons(move->src.ofs);
        narm->dst_ofs = htons(move->dst.ofs);
        nx_put_mff_header(out, move->src.field, 0, false);
        nx_put_mff_header(out, move->dst.field, 0, false);
    }
    pad_ofpat(out, start_ofs);
}