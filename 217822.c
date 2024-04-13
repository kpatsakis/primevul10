ofpact_check__(struct ofpact *a, struct ofpact_check_params *cp)
{
    switch (a->type) {
#define OFPACT(ENUM, STRUCT, MEMBER, NAME)                  \
        case OFPACT_##ENUM:                                 \
            return check_##ENUM(ofpact_get_##ENUM(a), cp);
        OFPACTS
#undef OFPACT
    default:
        OVS_NOT_REACHED();
    }
}