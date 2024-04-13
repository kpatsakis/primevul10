decode_LEARN_common(const struct nx_action_learn *nal,
                    enum ofp_raw_action_type raw,
                    struct ofpact_learn *learn)
{
    if (nal->pad) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    learn->ofpact.raw = raw;
    learn->idle_timeout = ntohs(nal->idle_timeout);
    learn->hard_timeout = ntohs(nal->hard_timeout);
    learn->priority = ntohs(nal->priority);
    learn->cookie = nal->cookie;
    learn->table_id = nal->table_id;
    learn->fin_idle_timeout = ntohs(nal->fin_idle_timeout);
    learn->fin_hard_timeout = ntohs(nal->fin_hard_timeout);
    learn->flags = ntohs(nal->flags);

    if (learn->table_id == 0xff) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    return 0;
}