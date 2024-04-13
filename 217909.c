ofpact_put_reg_load2(struct ofpbuf *ofpacts, const struct mf_field *field,
                     const void *value, const void *mask)
{
    struct ofpact_set_field *sf = ofpact_put_set_field(ofpacts, field, value,
                                                       mask);
    sf->ofpact.raw = NXAST_RAW_REG_LOAD2;

    return sf;
}