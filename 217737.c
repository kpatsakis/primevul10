decode_OFPAT_RAW_DEC_NW_TTL(struct ofpbuf *out)
{
    uint16_t id = 0;
    struct ofpact_cnt_ids *ids;
    enum ofperr error = 0;

    ids = ofpact_put_DEC_TTL(out);
    ids->n_controllers = 1;
    ofpbuf_put(out, &id, sizeof id);
    ids = out->header;
    ofpact_finish_DEC_TTL(out, &ids);
    return error;
}