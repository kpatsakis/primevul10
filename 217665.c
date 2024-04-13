parse_NAT(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_nat *on = ofpact_put_NAT(pp->ofpacts);
    char *key, *value;

    on->flags = 0;
    on->range_af = AF_UNSPEC;

    while (ofputil_parse_key_value(&arg, &key, &value)) {
        char *error = NULL;

        if (!strcmp(key, "src")) {
            on->flags |= NX_NAT_F_SRC;
            error = str_to_nat_range(value, on);
        } else if (!strcmp(key, "dst")) {
            on->flags |= NX_NAT_F_DST;
            error = str_to_nat_range(value, on);
        } else if (!strcmp(key, "persistent")) {
            on->flags |= NX_NAT_F_PERSISTENT;
        } else if (!strcmp(key, "hash")) {
            on->flags |= NX_NAT_F_PROTO_HASH;
        } else if (!strcmp(key, "random")) {
            on->flags |= NX_NAT_F_PROTO_RANDOM;
        } else {
            error = xasprintf("invalid key \"%s\" in \"nat\" argument",
                              key);
        }
        if (error) {
            return error;
        }
    }
    if (on->flags & NX_NAT_F_SRC && on->flags & NX_NAT_F_DST) {
        return xasprintf("May only specify one of \"src\" or \"dst\".");
    }
    if (!(on->flags & NX_NAT_F_SRC || on->flags & NX_NAT_F_DST)) {
        if (on->flags) {
            return xasprintf("Flags allowed only with \"src\" or \"dst\".");
        }
        if (on->range_af != AF_UNSPEC) {
            return xasprintf("Range allowed only with \"src\" or \"dst\".");
        }
    }
    if (on->flags & NX_NAT_F_PROTO_HASH && on->flags & NX_NAT_F_PROTO_RANDOM) {
        return xasprintf("Both \"hash\" and \"random\" are not allowed.");
    }

    return NULL;
}