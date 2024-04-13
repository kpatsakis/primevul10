decode_NXAST_RAW_BUNDLE(const struct nx_action_bundle *nab,
                        enum ofp_version ofp_version OVS_UNUSED,
                        struct ofpbuf *out)
{
    return decode_bundle(false, nab, NULL, NULL, out);
}