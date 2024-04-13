parse_truncate_subfield(const char *arg_,
                        const struct ofpact_parse_params *pp,
                        struct ofpact_output_trunc *output_trunc)
{
    char *key, *value;
    char *arg = CONST_CAST(char *, arg_);

    while (ofputil_parse_key_value(&arg, &key, &value)) {
        if (!strcmp(key, "port")) {
            if (!ofputil_port_from_string(value, pp->port_map,
                                          &output_trunc->port)) {
                return xasprintf("output to unknown truncate port: %s",
                                  value);
            }
            if (ofp_to_u16(output_trunc->port) > ofp_to_u16(OFPP_MAX)) {
                if (output_trunc->port != OFPP_LOCAL &&
                    output_trunc->port != OFPP_IN_PORT)
                return xasprintf("output to unsupported truncate port: %s",
                                 value);
            }
        } else if (!strcmp(key, "max_len")) {
            char *err;

            err = str_to_u32(value, &output_trunc->max_len);
            if (err) {
                return err;
            }
        } else {
            return xasprintf("invalid key '%s' in output_trunc argument",
                                key);
        }
    }
    return NULL;
}