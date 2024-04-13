encode_GROUP(const struct ofpact_group *group,
             enum ofp_version ofp_version, struct ofpbuf *out)
{
    put_OFPAT_GROUP(out, ofp_version, group->group_id);
}