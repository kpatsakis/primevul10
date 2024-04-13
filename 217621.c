decode_NXAST_RAW_SAMPLE(const struct nx_action_sample *nas,
                        enum ofp_version ofp_version OVS_UNUSED,
                        struct ofpbuf *out)
{
    struct ofpact_sample *sample;

    sample = ofpact_put_SAMPLE(out);
    sample->ofpact.raw = NXAST_RAW_SAMPLE;
    sample->probability = ntohs(nas->probability);
    sample->collector_set_id = ntohl(nas->collector_set_id);
    sample->obs_domain_id = ntohl(nas->obs_domain_id);
    sample->obs_point_id = ntohl(nas->obs_point_id);
    sample->sampling_port = OFPP_NONE;
    sample->direction = NX_ACTION_SAMPLE_DEFAULT;

    if (sample->probability == 0) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    return 0;
}