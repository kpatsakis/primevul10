format_SET_FIELD(const struct ofpact_set_field *a,
                 const struct ofpact_format_params *fp)
{
    if (a->ofpact.raw == NXAST_RAW_REG_LOAD) {
        struct mf_subfield dst;
        uint64_t value;

        dst.ofs = dst.n_bits = 0;
        while (next_load_segment(a, &dst, &value)) {
            ds_put_format(fp->s, "%sload:%s%#"PRIx64"%s->%s",
                          colors.special, colors.end, value,
                          colors.special, colors.end);
            mf_format_subfield(&dst, fp->s);
            ds_put_char(fp->s, ',');
        }
        ds_chomp(fp->s, ',');
    } else {
        ds_put_format(fp->s, "%sset_field:%s", colors.special, colors.end);
        mf_format(a->field, a->value, ofpact_set_field_mask(a),
                  fp->port_map, fp->s);
        ds_put_format(fp->s, "%s->%s%s",
                      colors.special, colors.end, a->field->name);
    }
}