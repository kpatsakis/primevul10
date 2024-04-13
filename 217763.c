encode_DEC_TTL(const struct ofpact_cnt_ids *dec_ttl,
               enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (dec_ttl->ofpact.raw == NXAST_RAW_DEC_TTL_CNT_IDS
        || dec_ttl->n_controllers != 1
        || dec_ttl->cnt_ids[0] != 0) {
        struct nx_action_cnt_ids *nac_ids = put_NXAST_DEC_TTL_CNT_IDS(out);
        int ids_len = ROUND_UP(2 * dec_ttl->n_controllers, OFP_ACTION_ALIGN);
        ovs_be16 *ids;
        size_t i;

        nac_ids->len = htons(ntohs(nac_ids->len) + ids_len);
        nac_ids->n_controllers = htons(dec_ttl->n_controllers);

        ids = ofpbuf_put_zeros(out, ids_len);
        for (i = 0; i < dec_ttl->n_controllers; i++) {
            ids[i] = htons(dec_ttl->cnt_ids[i]);
        }
    } else {
        put_OFPAT_DEC_NW_TTL(out, ofp_version);
    }
}