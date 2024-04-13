parse_FIN_TIMEOUT(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_fin_timeout *oft = ofpact_put_FIN_TIMEOUT(pp->ofpacts);
    char *key, *value;

    while (ofputil_parse_key_value(&arg, &key, &value)) {
        char *error;

        if (!strcmp(key, "idle_timeout")) {
            error =  str_to_u16(value, key, &oft->fin_idle_timeout);
        } else if (!strcmp(key, "hard_timeout")) {
            error = str_to_u16(value, key, &oft->fin_hard_timeout);
        } else {
            error = xasprintf("invalid key '%s' in 'fin_timeout' argument",
                              key);
        }

        if (error) {
            return error;
        }
    }
    return NULL;
}