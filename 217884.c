decode_SAMPLE2(const struct nx_action_sample2 *nas,
               enum ofp_raw_action_type raw,
               enum nx_action_sample_direction direction,
               struct ofpact_sample *sample)
{
    sample->ofpact.raw = raw;
    sample->probability = ntohs(nas->probability);
    sample->collector_set_id = ntohl(nas->collector_set_id);
    sample->obs_domain_id = ntohl(nas->obs_domain_id);
    sample->obs_point_id = ntohl(nas->obs_point_id);
    sample->sampling_port = u16_to_ofp(ntohs(nas->sampling_port));
    sample->direction = direction;

    if (sample->probability == 0) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    return 0;
}