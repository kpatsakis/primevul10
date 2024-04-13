encode_SAMPLE2(const struct ofpact_sample *sample,
               struct nx_action_sample2 *nas)
{
    nas->probability = htons(sample->probability);
    nas->collector_set_id = htonl(sample->collector_set_id);
    nas->obs_domain_id = htonl(sample->obs_domain_id);
    nas->obs_point_id = htonl(sample->obs_point_id);
    nas->sampling_port = htons(ofp_to_u16(sample->sampling_port));
    nas->direction = sample->direction;
}