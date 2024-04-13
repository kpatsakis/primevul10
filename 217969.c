encode_CLEAR_ACTIONS(const struct ofpact_null *null OVS_UNUSED,
                     enum ofp_version ofp_version OVS_UNUSED,
                     struct ofpbuf *out OVS_UNUSED)
{
    if (ofp_version > OFP10_VERSION) {
        instruction_put_OFPIT11_CLEAR_ACTIONS(out);
    }
}