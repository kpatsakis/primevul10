parse_noargs_dec_ttl(const struct ofpact_parse_params *pp)
{
    struct ofpact_cnt_ids *ids;
    uint16_t id = 0;

    ofpact_put_DEC_TTL(pp->ofpacts);
    ofpbuf_put(pp->ofpacts, &id, sizeof id);
    ids = pp->ofpacts->header;
    ids->n_controllers++;
    ofpact_finish_DEC_TTL(pp->ofpacts, &ids);
}