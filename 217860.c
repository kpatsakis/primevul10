parse_SET_QUEUE(char *arg, const struct ofpact_parse_params *pp)
{
    return str_to_u32(arg, &ofpact_put_SET_QUEUE(pp->ofpacts)->queue_id);
}