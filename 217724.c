decode_OFPAT_RAW_GROUP(uint32_t group_id,
                         enum ofp_version ofp_version OVS_UNUSED,
                         struct ofpbuf *out)
{
    ofpact_put_GROUP(out)->group_id = group_id;
    return 0;
}