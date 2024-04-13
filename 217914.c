parse_ENCAP(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_encap *encap;
    char *key, *value, *str;
    char *error = NULL;
    uint16_t prop_class;
    int n_props = 0;

    encap = ofpact_put_ENCAP(pp->ofpacts);
    encap->hdr_size = 0;
    /* Parse encap header type. */
    str = arg;
    if (!ofputil_parse_key_value(&arg, &key, &value)) {
        return xasprintf("Missing encap hdr: %s", str);
    }
    if (!parse_encap_header(key, &encap->new_pkt_type)) {
        return xasprintf("Encap hdr not supported: %s", value);
    }
    if (!parse_ed_prop_class(key, &prop_class)) {
        return xasprintf("Invalid encap prop class: %s", key);
    }
    /* Parse encap properties. */
    error = parse_ed_props(prop_class, &value, &n_props, pp->ofpacts);
    if (error != NULL) {
        return error;
    }
    /* ofpbuf may have been re-allocated. */
    encap = pp->ofpacts->header;
    encap->n_props = n_props;
    ofpact_finish_ENCAP(pp->ofpacts, &encap);
    return NULL;
}