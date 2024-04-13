parse_DECAP(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_decap *decap;
    char *key, *value, *pos;
    char *error = NULL;
    uint16_t ns, type;

    decap = ofpact_put_DECAP(pp->ofpacts);
    /* Default next packet_type is PT_USE_NEXT_PROTO. */
    decap->new_pkt_type = htonl(PT_USE_NEXT_PROTO);

    /* Parse decap packet_type if given. */
    if (ofputil_parse_key_value(&arg, &key, &value)) {
        if (strcmp(key, "packet_type") == 0) {
            pos = value;
            if (!ofputil_parse_key_value(&pos, &key, &value)
                || strcmp(key, "ns") != 0) {
                return xstrdup("Missing packet_type attribute ns");
            }
            error = str_to_u16(value, "ns", &ns);
            if (error) {
                return error;
            }
            if (ns >= OFPHTN_N_TYPES) {
                return xasprintf("Unsupported ns value: %"PRIu16, ns);
            }
            if (!ofputil_parse_key_value(&pos, &key, &value)
                || strcmp(key, "type") != 0) {
                return xstrdup("Missing packet_type attribute type");
            }
            error = str_to_u16(value, "type", &type);
            if (error) {
                return error;
            }
            decap->new_pkt_type = htonl(PACKET_TYPE(ns, type));
        } else {
            return xasprintf("Invalid decap argument: %s", key);
        }
    }
    return NULL;
}