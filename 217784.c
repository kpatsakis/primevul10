ofpact_pull_raw(struct ofpbuf *buf, enum ofp_version ofp_version,
                enum ofp_raw_action_type *raw, uint64_t *arg)
{
    const struct ofp_action_header *oah = buf->data;
    const struct ofpact_raw_instance *action;
    unsigned int length;
    enum ofperr error;

    *raw = *arg = 0;
    error = ofpact_decode_raw(ofp_version, oah, buf->size, &action);
    if (error) {
        return error;
    }

    if (action->deprecation) {
        VLOG_INFO_RL(&rl, "%s is deprecated in %s (%s)",
                     action->name, ofputil_version_to_string(ofp_version),
                     action->deprecation);
    }

    length = ntohs(oah->len);
    if (length > buf->size) {
        VLOG_WARN_RL(&rl, "OpenFlow action %s length %u exceeds action buffer "
                     "length %"PRIu32, action->name, length, buf->size);
        return OFPERR_OFPBAC_BAD_LEN;
    }
    if (length < action->min_length || length > action->max_length) {
        VLOG_WARN_RL(&rl, "OpenFlow action %s length %u not in valid range "
                     "[%hu,%hu]", action->name, length,
                     action->min_length, action->max_length);
        return OFPERR_OFPBAC_BAD_LEN;
    }
    if (length % 8) {
        VLOG_WARN_RL(&rl, "OpenFlow action %s length %u is not a multiple "
                     "of 8", action->name, length);
        return OFPERR_OFPBAC_BAD_LEN;
    }

    *raw = action->raw;
    *arg = 0;
    if (action->arg_len) {
        const uint8_t *p;
        int i;

        p = ofpbuf_at_assert(buf, action->arg_ofs, action->arg_len);
        for (i = 0; i < action->arg_len; i++) {
            *arg = (*arg << 8) | p[i];
        }
    }

    ofpbuf_pull(buf, length);

    return 0;
}