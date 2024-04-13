ofpacts_decode_for_action_set(const struct ofp_action_header *in,
                              size_t n_in, enum ofp_version version,
                              const struct vl_mff_map *vl_mff_map,
                              uint64_t *ofpacts_tlv_bitmap,
                              struct ofpbuf *out)
{
    enum ofperr error;
    struct ofpact *a;
    size_t start = out->size;

    error = ofpacts_decode(in, n_in, version, vl_mff_map, ofpacts_tlv_bitmap,
                           out);

    if (error) {
        return error;
    }

    OFPACT_FOR_EACH (a, ofpact_end(out->data, start), out->size - start) {
        if (!ofpact_is_allowed_in_actions_set(a)) {
            VLOG_WARN_RL(&rl, "disallowed action in action set");
            return OFPERR_OFPBAC_BAD_TYPE;
        }
    }

    return 0;
}