ofpact_format(const struct ofpact *a,
              const struct ofpact_format_params *fp)
{
    switch (a->type) {
#define OFPACT(ENUM, STRUCT, MEMBER, NAME)                              \
        case OFPACT_##ENUM:                                             \
            format_##ENUM(ALIGNED_CAST(const struct STRUCT *, a), fp);  \
            break;
        OFPACTS
#undef OFPACT
    default:
        OVS_NOT_REACHED();
    }
}