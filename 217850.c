ofpacts_pull_openflow_actions(struct ofpbuf *openflow,
                              unsigned int actions_len,
                              enum ofp_version version,
                              const struct vl_mff_map *vl_mff_map,
                              uint64_t *ofpacts_tlv_bitmap,
                              struct ofpbuf *ofpacts)
{
    return ofpacts_pull_openflow_actions__(openflow, actions_len, version,
                                           1u << OVSINST_OFPIT11_APPLY_ACTIONS,
                                           ofpacts, 0, vl_mff_map,
                                           ofpacts_tlv_bitmap);
}