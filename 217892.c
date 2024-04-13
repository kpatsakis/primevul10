ovs_instruction_type_from_name(const char *name)
{
    const struct instruction_type_info *p;
    for (p = inst_info; p < &inst_info[ARRAY_SIZE(inst_info)]; p++) {
        if (!strcasecmp(name, p->name)) {
            return p->type;
        }
    }
    return -1;
}