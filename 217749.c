set_field_parse__(char *arg, const struct ofpact_parse_params *pp)
{
    char *value;
    char *delim;
    char *key;
    const struct mf_field *mf;
    union mf_value sf_value, sf_mask;
    char *error;

    error = set_field_split_str(arg, &key, &value, &delim);
    if (error) {
        return error;
    }

    mf = mf_from_name(key);
    if (!mf) {
        return xasprintf("%s is not a valid OXM field name", key);
    }
    if (!mf->writable) {
        return xasprintf("%s is read-only", key);
    }

    delim[0] = '\0';
    error = mf_parse(mf, value, pp->port_map, &sf_value, &sf_mask);
    if (error) {
        return error;
    }

    if (!mf_is_value_valid(mf, &sf_value)) {
        return xasprintf("%s is not a valid value for field %s", value, key);
    }

    *pp->usable_protocols &= mf->usable_protocols_exact;

    ofpact_put_set_field(pp->ofpacts, mf, &sf_value, &sf_mask);
    return NULL;
}