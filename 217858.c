decode_NXAST_RAW_MULTIPATH(const struct nx_action_multipath *nam,
                           enum ofp_version ofp_version OVS_UNUSED,
                           const struct vl_mff_map *vl_mff_map,
                           uint64_t *tlv_bitmap, struct ofpbuf *out)
{
    uint32_t n_links = ntohs(nam->max_link) + 1;
    size_t min_n_bits = log_2_ceil(n_links);
    struct ofpact_multipath *mp;
    enum ofperr error;

    mp = ofpact_put_MULTIPATH(out);
    mp->fields = ntohs(nam->fields);
    mp->basis = ntohs(nam->basis);
    mp->algorithm = ntohs(nam->algorithm);
    mp->max_link = ntohs(nam->max_link);
    mp->arg = ntohl(nam->arg);
    mp->dst.ofs = nxm_decode_ofs(nam->ofs_nbits);
    mp->dst.n_bits = nxm_decode_n_bits(nam->ofs_nbits);
    error = mf_vl_mff_mf_from_nxm_header(ntohl(nam->dst), vl_mff_map,
                                         &mp->dst.field, tlv_bitmap);
    if (error) {
        return error;
    }

    if (!flow_hash_fields_valid(mp->fields)) {
        VLOG_WARN_RL(&rl, "unsupported fields %d", (int) mp->fields);
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    } else if (mp->algorithm != NX_MP_ALG_MODULO_N
               && mp->algorithm != NX_MP_ALG_HASH_THRESHOLD
               && mp->algorithm != NX_MP_ALG_HRW
               && mp->algorithm != NX_MP_ALG_ITER_HASH) {
        VLOG_WARN_RL(&rl, "unsupported algorithm %d", (int) mp->algorithm);
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    } else if (mp->dst.n_bits < min_n_bits) {
        VLOG_WARN_RL(&rl, "multipath action requires at least %"PRIuSIZE" bits for "
                     "%"PRIu32" links", min_n_bits, n_links);
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    return multipath_check(mp, NULL);
}