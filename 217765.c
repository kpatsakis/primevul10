decode_NXAST_RAW_STACK_POP(const struct nx_action_stack *nasp,
                           enum ofp_version ofp_version OVS_UNUSED,
                           const struct vl_mff_map *vl_mff_map,
                           uint64_t *tlv_bitmap, struct ofpbuf *ofpacts)
{
    struct ofpact_stack *pop = ofpact_put_STACK_POP(ofpacts);
    enum ofperr error = decode_stack_action(nasp, vl_mff_map, tlv_bitmap,
                                            pop);
    return error ? error : nxm_stack_pop_check(pop, NULL);
}