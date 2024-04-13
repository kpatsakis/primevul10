instruction_is_valid(const struct ofp11_instruction *inst,
                     size_t n_instructions)
{
    uint16_t len = ntohs(inst->len);
    return (!(len % OFP11_INSTRUCTION_ALIGN)
            && len >= sizeof *inst
            && len / sizeof *inst <= n_instructions);
}