parse_SET_FIELD(const char *arg, const struct ofpact_parse_params *pp)
{
    char *copy = xstrdup(arg);
    char *error = set_field_parse__(copy, pp);
    free(copy);
    return error;
}