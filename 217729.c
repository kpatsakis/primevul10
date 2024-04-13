encode_ofpact(const struct ofpact *a, enum ofp_version ofp_version,
              struct ofpbuf *out)
{
    switch (a->type) {
#define OFPACT(ENUM, STRUCT, MEMBER, NAME)                              \
        case OFPACT_##ENUM:                                             \
            encode_##ENUM(ofpact_get_##ENUM(a), ofp_version, out);      \
            return;
        OFPACTS
#undef OFPACT
    default:
        OVS_NOT_REACHED();
    }
}