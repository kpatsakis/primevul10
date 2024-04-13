decode_copy_field__(ovs_be16 src_offset, ovs_be16 dst_offset, ovs_be16 n_bits,
                    const void *action, ovs_be16 action_len, size_t oxm_offset,
                    const struct vl_mff_map *vl_mff_map,
                    uint64_t *tlv_bitmap, struct ofpbuf *ofpacts)
{
    struct ofpact_reg_move *move = ofpact_put_REG_MOVE(ofpacts);
    enum ofperr error;

    move->ofpact.raw = ONFACT_RAW13_COPY_FIELD;
    move->src.ofs = ntohs(src_offset);
    move->src.n_bits = ntohs(n_bits);
    move->dst.ofs = ntohs(dst_offset);
    move->dst.n_bits = ntohs(n_bits);

    struct ofpbuf b = ofpbuf_const_initializer(action, ntohs(action_len));
    ofpbuf_pull(&b, oxm_offset);

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