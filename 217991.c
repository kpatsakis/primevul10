ofpact_put_set_field(struct ofpbuf *ofpacts, const struct mf_field *field,
                     const void *value, const void *mask)
{
    struct ofpact_set_field *sf = ofpact_put_SET_FIELD(ofpacts);
    sf->field = field;

    /* Fill in the value and mask if given, otherwise put zeroes so that the
     * caller may fill in the value and mask itself. */
    if (value) {
        ofpbuf_put_uninit(ofpacts, 2 * field->n_bytes);
        sf = ofpacts->header;
        memcpy(sf->value, value, field->n_bytes);
        if (mask) {
            memcpy(ofpact_set_field_mask(sf), mask, field->n_bytes);
        } else {
            memset(ofpact_set_field_mask(sf), 0xff, field->n_bytes);
        }
    } else {
        ofpbuf_put_zeros(ofpacts, 2 * field->n_bytes);
        sf = ofpacts->header;
    }
    /* Update length. */
    ofpact_finish_SET_FIELD(ofpacts, &sf);

    return sf;
}