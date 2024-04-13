decode_NXAST_RAW_LEARN(const struct nx_action_learn *nal,
                       enum ofp_version ofp_version OVS_UNUSED,
                       const struct vl_mff_map *vl_mff_map,
                       uint64_t *tlv_bitmap, struct ofpbuf *ofpacts)
{
    struct ofpact_learn *learn;
    enum ofperr error;

    learn = ofpact_put_LEARN(ofpacts);

    error = decode_LEARN_common(nal, NXAST_RAW_LEARN, learn);
    if (error) {
        return error;
    }

    if (learn->flags & ~(NX_LEARN_F_SEND_FLOW_REM |
                         NX_LEARN_F_DELETE_LEARNED)) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    return decode_LEARN_specs(nal + 1, (char *) nal + ntohs(nal->len),
                              vl_mff_map, tlv_bitmap, ofpacts);
}