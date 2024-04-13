ovs_instruction_type_from_inst_type(enum ovs_instruction_type *instruction_type,
                                    const uint16_t inst_type)
{
    switch (inst_type) {

#define DEFINE_INST(ENUM, STRUCT, EXTENSIBLE, NAME) \
    case ENUM:                                      \
        *instruction_type = OVSINST_##ENUM;         \
        return 0;
OVS_INSTRUCTIONS
#undef DEFINE_INST

    default:
        return OFPERR_OFPBIC_UNKNOWN_INST;
    }
}