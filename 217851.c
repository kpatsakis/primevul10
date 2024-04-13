ofpacts_parse__(char *str, const struct ofpact_parse_params *pp,
                bool allow_instructions, enum ofpact_type outer_action)
{
    int prev_inst = -1;
    enum ofperr retval;
    char *key, *value;
    bool drop = false;
    char *pos;

    pos = str;
    while (ofputil_parse_key_value(&pos, &key, &value)) {
        enum ovs_instruction_type inst = OVSINST_OFPIT11_APPLY_ACTIONS;
        enum ofpact_type type;
        char *error = NULL;
        ofp_port_t port;

        if (ofpact_type_from_name(key, &type)) {
            error = ofpact_parse(type, value, pp);
            inst = ovs_instruction_type_from_ofpact_type(type);
        } else if (!strcasecmp(key, "mod_vlan_vid")) {
            error = parse_set_vlan_vid(value, true, pp);
        } else if (!strcasecmp(key, "mod_vlan_pcp")) {
            error = parse_set_vlan_pcp(value, true, pp);
        } else if (!strcasecmp(key, "set_nw_ttl")) {
            error = parse_SET_IP_TTL(value, pp);
        } else if (!strcasecmp(key, "pop_vlan")) {
            error = parse_pop_vlan(pp);
        } else if (!strcasecmp(key, "set_tunnel64")) {
            error = parse_set_tunnel(value, NXAST_RAW_SET_TUNNEL64, pp);
        } else if (!strcasecmp(key, "load")) {
            error = parse_reg_load(value, pp);
        } else if (!strcasecmp(key, "bundle_load")) {
            error = parse_bundle_load(value, pp);
        } else if (!strcasecmp(key, "drop")) {
            drop = true;
        } else if (!strcasecmp(key, "apply_actions")) {
            return xstrdup("apply_actions is the default instruction");
        } else if (ofputil_port_from_string(key, pp->port_map, &port)) {
            ofpact_put_OUTPUT(pp->ofpacts)->port = port;
        } else {
            return xasprintf("unknown action %s", key);
        }
        if (error) {
            return error;
        }

        if (inst != OVSINST_OFPIT11_APPLY_ACTIONS) {
            if (!allow_instructions) {
                return xasprintf("only actions are allowed here (not "
                                 "instruction %s)",
                                 ovs_instruction_name_from_type(inst));
            }
            if (inst == prev_inst) {
                return xasprintf("instruction %s may be specified only once",
                                 ovs_instruction_name_from_type(inst));
            }
        }
        if (prev_inst != -1 && inst < prev_inst) {
            return xasprintf("instruction %s must be specified before %s",
                             ovs_instruction_name_from_type(inst),
                             ovs_instruction_name_from_type(prev_inst));
        }
        prev_inst = inst;
    }

    if (drop && pp->ofpacts->size) {
        return xstrdup("\"drop\" must not be accompanied by any other action "
                       "or instruction");
    }

    retval = ofpacts_verify(pp->ofpacts->data, pp->ofpacts->size,
                            (allow_instructions
                             ? (1u << N_OVS_INSTRUCTIONS) - 1
                             : 1u << OVSINST_OFPIT11_APPLY_ACTIONS),
                            outer_action);
    if (retval) {
        return xstrdup("Incorrect instruction ordering");
    }

    return NULL;
}