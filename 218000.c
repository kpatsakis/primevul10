encode_EXIT(const struct ofpact_null *null OVS_UNUSED,
            enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    put_NXAST_EXIT(out);
}