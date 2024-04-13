check_GOTO_TABLE(const struct ofpact_goto_table *a,
                 const struct ofpact_check_params *cp)
{
    if ((cp->table_id != 255 && a->table_id <= cp->table_id)
        || (cp->n_tables != 255 && a->table_id >= cp->n_tables)) {
        return OFPERR_OFPBIC_BAD_TABLE_ID;
    }
    return 0;
}