parse_CT(char *arg, const struct ofpact_parse_params *pp)
{
    const size_t ct_offset = ofpacts_pull(pp->ofpacts);
    struct ofpact_conntrack *oc;
    char *error = NULL;
    char *key, *value;

    oc = ofpact_put_CT(pp->ofpacts);
    oc->flags = 0;
    oc->recirc_table = NX_CT_RECIRC_NONE;
    while (ofputil_parse_key_value(&arg, &key, &value)) {
        if (!strcmp(key, "commit")) {
            oc->flags |= NX_CT_F_COMMIT;
        } else if (!strcmp(key, "force")) {
            oc->flags |= NX_CT_F_FORCE;
        } else if (!strcmp(key, "table")) {
            if (!ofputil_table_from_string(value, pp->table_map,
                                           &oc->recirc_table)) {
                error = xasprintf("unknown table %s", value);
            } else if (oc->recirc_table == NX_CT_RECIRC_NONE) {
                error = xasprintf("invalid table %#"PRIx8, oc->recirc_table);
            }
        } else if (!strcmp(key, "zone")) {
            error = str_to_u16(value, "zone", &oc->zone_imm);

            if (error) {
                free(error);
                error = mf_parse_subfield(&oc->zone_src, value);
                if (error) {
                    return error;
                }
            }
        } else if (!strcmp(key, "alg")) {
            error = str_to_connhelper(value, &oc->alg);
        } else if (!strcmp(key, "nat")) {
            const size_t nat_offset = ofpacts_pull(pp->ofpacts);

            error = parse_NAT(value, pp);
            /* Update CT action pointer and length. */
            pp->ofpacts->header = ofpbuf_push_uninit(pp->ofpacts, nat_offset);
            oc = pp->ofpacts->header;
        } else if (!strcmp(key, "exec")) {
            /* Hide existing actions from ofpacts_parse_copy(), so the
             * nesting can be handled transparently. */
            enum ofputil_protocol usable_protocols2;
            const size_t exec_offset = ofpacts_pull(pp->ofpacts);

            /* Initializes 'usable_protocol2', fold it back to
             * '*usable_protocols' afterwards, so that we do not lose
             * restrictions already in there. */
            struct ofpact_parse_params pp2 = *pp;
            pp2.usable_protocols = &usable_protocols2;
            error = ofpacts_parse_copy(value, &pp2, false, OFPACT_CT);
            *pp->usable_protocols &= usable_protocols2;
            pp->ofpacts->header = ofpbuf_push_uninit(pp->ofpacts, exec_offset);
            oc = pp->ofpacts->header;
        } else {
            error = xasprintf("invalid argument to \"ct\" action: `%s'", key);
        }
        if (error) {
            break;
        }
    }
    if (!error && oc->flags & NX_CT_F_FORCE && !(oc->flags & NX_CT_F_COMMIT)) {
        error = xasprintf("\"force\" flag requires \"commit\" flag.");
    }
    ofpact_finish_CT(pp->ofpacts, &oc);
    ofpbuf_push_uninit(pp->ofpacts, ct_offset);
    return error;
}