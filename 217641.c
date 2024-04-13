parse_RESUBMIT(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_resubmit *resubmit;
    char *in_port_s, *table_s, *ct_s;

    resubmit = ofpact_put_RESUBMIT(pp->ofpacts);

    in_port_s = strsep(&arg, ",");
    if (in_port_s && in_port_s[0]) {
        if (!ofputil_port_from_string(in_port_s, pp->port_map,
                                      &resubmit->in_port)) {
            return xasprintf("%s: resubmit to unknown port", in_port_s);
        }
    } else {
        resubmit->in_port = OFPP_IN_PORT;
    }

    table_s = strsep(&arg, ",");
    if (table_s && table_s[0]) {
        if (!ofputil_table_from_string(table_s, pp->table_map,
                                       &resubmit->table_id)) {
            return xasprintf("%s: resubmit to unknown table", table_s);
        }
    } else {
        resubmit->table_id = 255;
    }

    ct_s = strsep(&arg, ",");
    if (ct_s && ct_s[0]) {
        if (strcmp(ct_s, "ct")) {
            return xasprintf("%s: unknown parameter", ct_s);
        }
        resubmit->with_ct_orig = true;
    } else {
        resubmit->with_ct_orig = false;
    }

    if (resubmit->in_port == OFPP_IN_PORT && resubmit->table_id == 255) {
        return xstrdup("at least one \"in_port\" or \"table\" must be "
                       "specified  on resubmit");
    }
    return NULL;
}