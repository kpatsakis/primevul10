encode_DEBUG_SLOW(const struct ofpact_null *n OVS_UNUSED,
                  enum ofp_version ofp_version OVS_UNUSED,
                  struct ofpbuf *out)
{
    put_NXAST_DEBUG_SLOW(out);
}