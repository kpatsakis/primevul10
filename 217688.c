ofpact_parse(enum ofpact_type type, char *value,
             const struct ofpact_parse_params *pp)
{
    switch (type) {
#define OFPACT(ENUM, STRUCT, MEMBER, NAME)                              \
        case OFPACT_##ENUM:                                             \
            return parse_##ENUM(value, pp);
        OFPACTS
#undef OFPACT
    default:
        OVS_NOT_REACHED();
    }
}