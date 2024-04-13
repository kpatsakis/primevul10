ofpacts_equal_stringwise(const struct ofpact *a, size_t a_len,
                         const struct ofpact *b, size_t b_len)
{
    struct ds a_s = DS_EMPTY_INITIALIZER;
    struct ofpact_format_params a_fp = { .s = &a_s };
    ofpacts_format(a, a_len, &a_fp);

    struct ds b_s = DS_EMPTY_INITIALIZER;
    struct ofpact_format_params b_fp = { .s = &b_s };
    ofpacts_format(b, b_len, &b_fp);

    bool equal = !strcmp(ds_cstr(&a_s), ds_cstr(&b_s));

    ds_destroy(&a_s);
    ds_destroy(&b_s);

    return equal;
}