encode_MULTIPATH(const struct ofpact_multipath *mp,
                 enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    struct nx_action_multipath *nam = put_NXAST_MULTIPATH(out);

    nam->fields = htons(mp->fields);
    nam->basis = htons(mp->basis);
    nam->algorithm = htons(mp->algorithm);
    nam->max_link = htons(mp->max_link);
    nam->arg = htonl(mp->arg);
    nam->ofs_nbits = nxm_encode_ofs_nbits(mp->dst.ofs, mp->dst.n_bits);
    nam->dst = htonl(nxm_header_from_mff(mp->dst.field));
}