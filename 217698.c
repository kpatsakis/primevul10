parse_SET_IP_TTL(char *arg, const struct ofpact_parse_params *pp)

{
    uint8_t ttl;
    char *error;

    error = str_to_u8(arg, "TTL", &ttl);
    if (error) {
        return error;
    }

    ofpact_put_SET_IP_TTL(pp->ofpacts)->ttl = ttl;
    return NULL;
}