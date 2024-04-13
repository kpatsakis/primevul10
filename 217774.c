decode_NXAST_RAW_DEC_TTL_CNT_IDS(const struct nx_action_cnt_ids *nac_ids,
                                 enum ofp_version ofp_version OVS_UNUSED,
                                 struct ofpbuf *out)
{
    struct ofpact_cnt_ids *ids;
    size_t ids_size;
    int i;

    ids = ofpact_put_DEC_TTL(out);
    ids->ofpact.raw = NXAST_RAW_DEC_TTL_CNT_IDS;
    ids->n_controllers = ntohs(nac_ids->n_controllers);
    ids_size = ntohs(nac_ids->len) - sizeof *nac_ids;

    if (!is_all_zeros(nac_ids->zeros, sizeof nac_ids->zeros)) {
        return OFPERR_NXBRC_MUST_BE_ZERO;
    }

    if (ids_size < ids->n_controllers * sizeof(ovs_be16)) {
        VLOG_WARN_RL(&rl, "Nicira action dec_ttl_cnt_ids only has %"PRIuSIZE" "
                     "bytes allocated for controller ids.  %"PRIuSIZE" bytes "
                     "are required for %u controllers.",
                     ids_size, ids->n_controllers * sizeof(ovs_be16),
                     ids->n_controllers);
        return OFPERR_OFPBAC_BAD_LEN;
    }

    for (i = 0; i < ids->n_controllers; i++) {
        uint16_t id = ntohs(((ovs_be16 *)(nac_ids + 1))[i]);
        ofpbuf_put(out, &id, sizeof id);
        ids = out->header;
    }

    ofpact_finish_DEC_TTL(out, &ids);

    return 0;
}