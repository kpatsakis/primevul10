ovs_instruction_name_from_type(enum ovs_instruction_type type)
{
    return type < ARRAY_SIZE(inst_info) ? inst_info[type].name : NULL;
}