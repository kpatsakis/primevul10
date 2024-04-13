decode_ct_zone(const struct nx_action_conntrack *nac,
               struct ofpact_conntrack *out,
               const struct vl_mff_map *vl_mff_map, uint64_t *tlv_bitmap)
{
    if (nac->zone_src) {
        enum ofperr error;

        out->zone_src.ofs = nxm_decode_ofs(nac->zone_ofs_nbits);
        out->zone_src.n_bits = nxm_decode_n_bits(nac->zone_ofs_nbits);
        error = mf_vl_mff_mf_from_nxm_header(ntohl(nac->zone_src),
                                             vl_mff_map, &out->zone_src.field,
                                             tlv_bitmap);
        if (error) {
            return error;
        }

        error = mf_check_src(&out->zone_src, NULL);
        if (error) {
            return error;
        }

        if (out->zone_src.n_bits != 16) {
            VLOG_WARN_RL(&rl, "zone n_bits %d not within valid range [16..16]",
                         out->zone_src.n_bits);
            return OFPERR_OFPBAC_BAD_SET_LEN;
        }
    } else {
        out->zone_src.field = NULL;
        out->zone_imm = ntohs(nac->zone_imm);
    }

    return 0;
}