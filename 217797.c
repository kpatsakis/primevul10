ofpact_name(enum ofpact_type type)
{
    switch (type) {
#define OFPACT(ENUM, STRUCT, MEMBER, NAME) case OFPACT_##ENUM: return NAME;
        OFPACTS
#undef OFPACT
    }
    return "<unknown>";
}