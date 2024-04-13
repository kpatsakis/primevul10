ofpacts_verify(const struct ofpact ofpacts[], size_t ofpacts_len,
               uint32_t allowed_ovsinsts, enum ofpact_type outer_action)
{
    const struct ofpact *a;
    enum ovs_instruction_type inst;

    inst = OVSINST_OFPIT13_METER;
    OFPACT_FOR_EACH (a, ofpacts, ofpacts_len) {
        enum ovs_instruction_type next;
        enum ofperr error;

        if (a->type == OFPACT_CONJUNCTION) {
            OFPACT_FOR_EACH (a, ofpacts, ofpacts_len) {
                if (a->type != OFPACT_CONJUNCTION && a->type != OFPACT_NOTE) {
                    VLOG_WARN("\"conjunction\" actions may be used along with "
                              "\"note\" but not any other kind of action "
                              "(such as the \"%s\" action used here)",
                              ofpact_name(a->type));
                    return OFPERR_NXBAC_BAD_CONJUNCTION;
                }
            }
            return 0;
        }

        error = ofpacts_verify_nested(a, outer_action);
        if (error) {
            return error;
        }

        next = ovs_instruction_type_from_ofpact_type(a->type);
        if (a > ofpacts
            && (inst == OVSINST_OFPIT11_APPLY_ACTIONS
                ? next < inst
                : next <= inst)) {
            const char *name = ovs_instruction_name_from_type(inst);
            const char *next_name = ovs_instruction_name_from_type(next);

            if (next == inst) {
                VLOG_WARN("duplicate %s instruction not allowed, for OpenFlow "
                          "1.1+ compatibility", name);
            } else {
                VLOG_WARN("invalid instruction ordering: %s must appear "
                          "before %s, for OpenFlow 1.1+ compatibility",
                          next_name, name);
            }
            return OFPERR_OFPBAC_UNSUPPORTED_ORDER;
        }
        if (!((1u << next) & allowed_ovsinsts)) {
            const char *name = ovs_instruction_name_from_type(next);

            VLOG_WARN("%s instruction not allowed here", name);
            return OFPERR_OFPBIC_UNSUP_INST;
        }

        inst = next;
    }

    return 0;
}