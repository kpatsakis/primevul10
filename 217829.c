decode_OFPAT_RAW15_SET_FIELD(const struct ofp12_action_set_field *oasf,
                             enum ofp_version ofp_version OVS_UNUSED,
                             const struct vl_mff_map *vl_mff_map,
                             uint64_t *tlv_bitmap, struct ofpbuf *ofpacts)
{
    return decode_ofpat_set_field(oasf, true, vl_mff_map, tlv_bitmap, ofpacts);
}