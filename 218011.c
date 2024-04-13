ofpact_get_mf_dst(const struct ofpact *ofpact)
{
    if (ofpact->type == OFPACT_SET_FIELD) {
        const struct ofpact_set_field *orl;

        orl = CONTAINER_OF(ofpact, struct ofpact_set_field, ofpact);
        return orl->field;
    } else if (ofpact->type == OFPACT_REG_MOVE) {
        const struct ofpact_reg_move *orm;

        orm = CONTAINER_OF(ofpact, struct ofpact_reg_move, ofpact);
        return orm->dst.field;
    }

    return NULL;
}