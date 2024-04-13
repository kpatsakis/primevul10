next_load_segment(const struct ofpact_set_field *sf,
                  struct mf_subfield *dst, uint64_t *value)
{
    int n_bits = sf->field->n_bits;
    int n_bytes = sf->field->n_bytes;
    int start = dst->ofs + dst->n_bits;

    if (start < n_bits) {
        dst->field = sf->field;
        dst->ofs = bitwise_scan(ofpact_set_field_mask(sf), n_bytes, 1, start,
                                n_bits);
        if (dst->ofs < n_bits) {
            dst->n_bits = bitwise_scan(ofpact_set_field_mask(sf), n_bytes, 0,
                                       dst->ofs + 1,
                                       MIN(dst->ofs + 64, n_bits)) - dst->ofs;
            *value = bitwise_get(sf->value, n_bytes, dst->ofs, dst->n_bits);
            return true;
        }
    }
    return false;
}