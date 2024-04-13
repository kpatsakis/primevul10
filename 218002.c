ofpacts_verify_nested(const struct ofpact *a, enum ofpact_type outer_action)
{
    const struct mf_field *field = ofpact_get_mf_dst(a);

    if (field && field_requires_ct(field->id) && outer_action != OFPACT_CT) {
        VLOG_WARN("cannot set CT fields outside of ct action");
        return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
    }
    if (a->type == OFPACT_NAT) {
        if (outer_action != OFPACT_CT) {
            VLOG_WARN("Cannot have NAT action outside of \"ct\" action");
            return OFPERR_OFPBAC_BAD_SET_ARGUMENT;
        }
        return 0;
    }

    if (outer_action) {
        ovs_assert(outer_action == OFPACT_WRITE_ACTIONS
                   || outer_action == OFPACT_CT);

        if (outer_action == OFPACT_CT) {
            if (!field) {
                return unsupported_nesting(a->type, outer_action);
            } else if (!field_requires_ct(field->id)) {
                VLOG_WARN("%s action doesn't support nested modification "
                          "of %s", ofpact_name(outer_action), field->name);
                return OFPERR_OFPBAC_BAD_ARGUMENT;
            }
        }
    }

    return 0;
}