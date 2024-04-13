decode_NXAST_RAW_CLONE(const struct ext_action_header *eah,
                       enum ofp_version ofp_version,
                       const struct vl_mff_map *vl_mff_map,
                       uint64_t *tlv_bitmap, struct ofpbuf *out)
{
    int error;
    struct ofpbuf openflow;
    const size_t clone_offset = ofpacts_pull(out);
    struct ofpact_nest *clone = ofpact_put_CLONE(out);

    /* decode action list */
    ofpbuf_pull(out, sizeof(*clone));
    openflow = ofpbuf_const_initializer(
                    eah + 1, ntohs(eah->len) - sizeof *eah);
    error = ofpacts_pull_openflow_actions__(&openflow, openflow.size,
                                            ofp_version,
                                            1u << OVSINST_OFPIT11_APPLY_ACTIONS,
                                            out, 0, vl_mff_map, tlv_bitmap);
    if (error) {
        return error;
    }
    clone = ofpbuf_push_uninit(out, sizeof *clone);
    out->header = &clone->ofpact;
    ofpact_finish_CLONE(out, &clone);
    ofpbuf_push_uninit(out, clone_offset);
    return error;
}