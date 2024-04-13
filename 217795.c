decode_NXAST_RAW_SAMPLE2(const struct nx_action_sample2 *nas,
                         enum ofp_version ofp_version OVS_UNUSED,
                         struct ofpbuf *out)
{
    return decode_SAMPLE2(nas, NXAST_RAW_SAMPLE2, NX_ACTION_SAMPLE_DEFAULT,
                          ofpact_put_SAMPLE(out));
}