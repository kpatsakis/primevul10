decode_NXAST_RAW_BUNDLE_LOAD(const struct nx_action_bundle *nab,
                             enum ofp_version ofp_version OVS_UNUSED,
                             const struct vl_mff_map *vl_mff_map,
                             uint64_t *tlv_bitmap, struct ofpbuf *out)
{
    return decode_bundle(true, nab, vl_mff_map, tlv_bitmap, out);
}