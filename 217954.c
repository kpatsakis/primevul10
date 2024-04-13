parse_SAMPLE(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_sample *os = ofpact_put_SAMPLE(pp->ofpacts);
    os->sampling_port = OFPP_NONE;
    os->direction = NX_ACTION_SAMPLE_DEFAULT;

    char *key, *value;
    while (ofputil_parse_key_value(&arg, &key, &value)) {
        char *error = NULL;

        if (!strcmp(key, "probability")) {
            error = str_to_u16(value, "probability", &os->probability);
            if (!error && os->probability == 0) {
                error = xasprintf("invalid probability value \"%s\"", value);
            }
        } else if (!strcmp(key, "collector_set_id")) {
            error = str_to_u32(value, &os->collector_set_id);
        } else if (!strcmp(key, "obs_domain_id")) {
            error = str_to_u32(value, &os->obs_domain_id);
        } else if (!strcmp(key, "obs_point_id")) {
            error = str_to_u32(value, &os->obs_point_id);
        } else if (!strcmp(key, "sampling_port")) {
            if (!ofputil_port_from_string(value, pp->port_map,
                                          &os->sampling_port)) {
                error = xasprintf("%s: unknown port", value);
            }
        } else if (!strcmp(key, "ingress")) {
            os->direction = NX_ACTION_SAMPLE_INGRESS;
        } else if (!strcmp(key, "egress")) {
            os->direction = NX_ACTION_SAMPLE_EGRESS;
        } else {
            error = xasprintf("invalid key \"%s\" in \"sample\" argument",
                              key);
        }
        if (error) {
            return error;
        }
    }
    if (os->probability == 0) {
        return xstrdup("non-zero \"probability\" must be specified on sample");
    }

    return NULL;
}