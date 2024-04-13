decode_stack_action(const struct nx_action_stack *nasp,
                    const struct vl_mff_map *vl_mff_map, uint64_t *tlv_bitmap,
                    struct ofpact_stack *stack_action)
{
    enum ofperr error;
    stack_action->subfield.ofs = ntohs(nasp->offset);

    struct ofpbuf b = ofpbuf_const_initializer(nasp, sizeof *nasp);
    ofpbuf_pull(&b, OBJECT_OFFSETOF(nasp, pad));
    error  = mf_vl_mff_nx_pull_header(&b, vl_mff_map,
                                      &stack_action->subfield.field, NULL,
                                      tlv_bitmap);
    if (error) {
        return error;
    }

    stack_action->subfield.n_bits = ntohs(*(const ovs_be16 *) b.data);
    ofpbuf_pull(&b, 2);
    if (!is_all_zeros(b.data, b.size)) {
        return OFPERR_NXBRC_MUST_BE_ZERO;
    }

    return 0;
}