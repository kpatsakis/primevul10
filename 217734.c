ofpacts_decode(const void *actions, size_t actions_len,
               enum ofp_version ofp_version,
               const struct vl_mff_map *vl_mff_map,
               uint64_t *ofpacts_tlv_bitmap, struct ofpbuf *ofpacts)
{
    struct ofpbuf openflow = ofpbuf_const_initializer(actions, actions_len);
    while (openflow.size) {
        const struct ofp_action_header *action = openflow.data;
        enum ofp_raw_action_type raw;
        enum ofperr error;
        uint64_t arg;

        error = ofpact_pull_raw(&openflow, ofp_version, &raw, &arg);
        if (!error) {
            error = ofpact_decode(action, raw, ofp_version, arg, vl_mff_map,
                                  ofpacts_tlv_bitmap, ofpacts);
        }

        if (error) {
            log_bad_action(actions, actions_len, action, error);
            return error;
        }
    }
    return 0;
}