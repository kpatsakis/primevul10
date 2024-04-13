parse_GROUP(char *arg, const struct ofpact_parse_params *pp)
{
    return str_to_u32(arg, &ofpact_put_GROUP(pp->ofpacts)->group_id);
}