parse_ed_props(const uint16_t prop_class, char **arg, int *n_props, struct ofpbuf *out)
{
    char *key, *value, *err;
    uint8_t prop_type;

    while (ofputil_parse_key_value(arg, &key, &value)) {
        if (!parse_ed_prop_type(prop_class, key, &prop_type)) {
            return xasprintf("Invalid property: %s", key);
        }
        if (value == NULL) {
            return xasprintf("Missing the value for property: %s", key);
        }
        err = parse_ed_prop_value(prop_class, prop_type, value, out);
        if (err != NULL) {
            return err;
        }
        (*n_props)++;
    }
    return NULL;
}