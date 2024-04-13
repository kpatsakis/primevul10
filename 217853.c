parse_DEC_TTL(char *arg, const struct ofpact_parse_params *pp)
{
    if (*arg == '\0') {
        parse_noargs_dec_ttl(pp);
    } else {
        struct ofpact_cnt_ids *ids;
        char *cntr;

        ids = ofpact_put_DEC_TTL(pp->ofpacts);
        ids->ofpact.raw = NXAST_RAW_DEC_TTL_CNT_IDS;
        for (cntr = strtok_r(arg, ", ", &arg); cntr != NULL;
             cntr = strtok_r(NULL, ", ", &arg)) {
            uint16_t id = atoi(cntr);

            ofpbuf_put(pp->ofpacts, &id, sizeof id);
            ids = pp->ofpacts->header;
            ids->n_controllers++;
        }
        if (!ids->n_controllers) {
            return xstrdup("dec_ttl_cnt_ids: expected at least one controller "
                           "id.");
        }
        ofpact_finish_DEC_TTL(pp->ofpacts, &ids);
    }
    return NULL;
}