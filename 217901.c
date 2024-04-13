parse_reg_load(char *arg, const struct ofpact_parse_params *pp)
{
    struct mf_subfield dst;
    char *key, *value_str;
    union mf_value value;
    char *error;

    error = set_field_split_str(arg, &key, &value_str, NULL);
    if (error) {
        return error;
    }

    error = mf_parse_subfield(&dst, key);
    if (error) {
        return error;
    }

    if (parse_int_string(value_str, (uint8_t *)&value, dst.field->n_bytes,
                         &key)) {
        return xasprintf("%s: cannot parse integer value", arg);
    }

    if (!bitwise_is_all_zeros(&value, dst.field->n_bytes, dst.n_bits,
                              dst.field->n_bytes * 8 - dst.n_bits)) {
        struct ds ds;

        ds_init(&ds);
        mf_format(dst.field, &value, NULL, NULL, &ds);
        error = xasprintf("%s: value %s does not fit into %d bits",
                          arg, ds_cstr(&ds), dst.n_bits);
        ds_destroy(&ds);
        return error;
    }

    struct ofpact_set_field *sf = ofpact_put_reg_load(pp->ofpacts, dst.field,
                                                      NULL, NULL);

    bitwise_copy(&value, dst.field->n_bytes, 0, sf->value,
                 dst.field->n_bytes, dst.ofs, dst.n_bits);
    bitwise_one(ofpact_set_field_mask(sf), dst.field->n_bytes, dst.ofs,
                dst.n_bits);
    return NULL;
}