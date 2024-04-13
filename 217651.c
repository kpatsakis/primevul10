decode_NXAST_RAW_STACK_PUSH(const struct nx_action_stack *nasp,
                            enum ofp_version ofp_version OVS_UNUSED,
                            const struct vl_mff_map *vl_mff_map,
                            uint64_t *tlv_bitmap, struct ofpbuf *ofpacts)
{
    struct ofpact_stack *push = ofpact_put_STACK_PUSH(ofpacts);
    enum ofperr error = decode_stack_action(nasp, vl_mff_map, tlv_bitmap,
                                            push);
    return error ? error : nxm_stack_push_check(push, NULL);
}