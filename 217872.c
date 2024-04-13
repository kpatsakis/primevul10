decode_NXAST_RAW_SAMPLE3(const struct nx_action_sample2 *nas,
                         enum ofp_version ofp_version OVS_UNUSED,
                         struct ofpbuf *out)
{
    struct ofpact_sample *sample = ofpact_put_SAMPLE(out);
    if (!is_all_zeros(nas->zeros, sizeof nas->zeros)) {
        return OFPERR_NXBRC_MUST_BE_ZERO;
    }
    if (nas->direction != NX_ACTION_SAMPLE_DEFAULT &&
        nas->direction != NX_ACTION_SAMPLE_INGRESS &&
        nas->direction != NX_ACTION_SAMPLE_EGRESS) {
        VLOG_WARN_RL(&rl, "invalid sample direction %"PRIu8, nas->direction);
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }
    return decode_SAMPLE2(nas, NXAST_RAW_SAMPLE3, nas->direction, sample);
}