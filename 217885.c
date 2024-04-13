encode_SAMPLE(const struct ofpact_sample *sample,
              enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    if (sample->ofpact.raw == NXAST_RAW_SAMPLE3
        || sample->direction != NX_ACTION_SAMPLE_DEFAULT) {
        encode_SAMPLE2(sample, put_NXAST_SAMPLE3(out));
    } else if (sample->ofpact.raw == NXAST_RAW_SAMPLE2
               || sample->sampling_port != OFPP_NONE) {
        encode_SAMPLE2(sample, put_NXAST_SAMPLE2(out));
    } else {
        struct nx_action_sample *nas = put_NXAST_SAMPLE(out);
        nas->probability = htons(sample->probability);
        nas->collector_set_id = htonl(sample->collector_set_id);
        nas->obs_domain_id = htonl(sample->obs_domain_id);
        nas->obs_point_id = htonl(sample->obs_point_id);
    }
}