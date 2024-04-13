ofpacts_pull_openflow_actions__(struct ofpbuf *openflow,
                                unsigned int actions_len,
                                enum ofp_version version,
                                uint32_t allowed_ovsinsts,
                                struct ofpbuf *ofpacts,
                                enum ofpact_type outer_action,
                                const struct vl_mff_map *vl_mff_map,
                                uint64_t *ofpacts_tlv_bitmap)
{
    const struct ofp_action_header *actions;
    enum ofperr error;

    if (actions_len % OFP_ACTION_ALIGN != 0) {
        VLOG_WARN_RL(&rl, "OpenFlow message actions length %u is not a "
                     "multiple of %d", actions_len, OFP_ACTION_ALIGN);
        return OFPERR_OFPBRC_BAD_LEN;
    }

    actions = ofpbuf_try_pull(openflow, actions_len);
    if (actions == NULL) {
        VLOG_WARN_RL(&rl, "OpenFlow message actions length %u exceeds "
                     "remaining message length (%"PRIu32")",
                     actions_len, openflow->size);
        return OFPERR_OFPBRC_BAD_LEN;
    }

    error = ofpacts_decode(actions, actions_len, version, vl_mff_map,
                           ofpacts_tlv_bitmap, ofpacts);
    if (!error) {
        error = ofpacts_verify(ofpacts->data, ofpacts->size, allowed_ovsinsts,
                               outer_action);
    }
    if (error) {
        ofpbuf_clear(ofpacts);
    }
    return error;
}