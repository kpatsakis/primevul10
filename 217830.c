decode_NXAST_RAW_REG_MOVE(const struct nx_action_reg_move *narm,
                          enum ofp_version ofp_version OVS_UNUSED,
                          const struct vl_mff_map *vl_mff_map,
                          uint64_t *tlv_bitmap, struct ofpbuf *ofpacts)
{
    struct ofpact_reg_move *move = ofpact_put_REG_MOVE(ofpacts);
    enum ofperr error;

    move->ofpact.raw = NXAST_RAW_REG_MOVE;
    move->src.ofs = ntohs(narm->src_ofs);
    move->src.n_bits = ntohs(narm->n_bits);
    move->dst.ofs = ntohs(narm->dst_ofs);
    move->dst.n_bits = ntohs(narm->n_bits);

    struct ofpbuf b = ofpbuf_const_initializer(narm, ntohs(narm->len));
    ofpbuf_pull(&b, sizeof *narm);

    error = mf_vl_mff_nx_pull_header(&b, vl_mff_map, &move->src.field, NULL,
                                     tlv_bitmap);
    if (error) {
        return error;
    }

    error = mf_vl_mff_nx_pull_header(&b, vl_mff_map, &move->dst.field, NULL,
                                     tlv_bitmap);
    if (error) {
        return error;
    }

    if (!is_all_zeros(b.data, b.size)) {
        return OFPERR_NXBRC_MUST_BE_ZERO;
    }

    return nxm_reg_move_check(move, NULL);
}