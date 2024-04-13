parse_GOTO_TABLE(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_goto_table *ogt = ofpact_put_GOTO_TABLE(pp->ofpacts);
    if (!ofputil_table_from_string(arg, pp->table_map, &ogt->table_id)) {
        return xasprintf("unknown table \"%s\"", arg);
    }
    return NULL;
}