decode_openflow11_instructions(const struct ofp11_instruction insts[],
                               size_t n_insts,
                               const struct ofp11_instruction *out[])
{
    const struct ofp11_instruction *inst;
    size_t left;

    memset(out, 0, N_OVS_INSTRUCTIONS * sizeof *out);
    INSTRUCTION_FOR_EACH (inst, left, insts, n_insts) {
        enum ovs_instruction_type type;
        enum ofperr error;

        error = decode_openflow11_instruction(inst, &type);
        if (error) {
            return error;
        }

        if (out[type]) {
            return OFPERR_OFPBIC_DUP_INST;
        }
        out[type] = inst;
    }

    if (left) {
        VLOG_WARN_RL(&rl, "bad instruction format at offset %"PRIuSIZE,
                     (n_insts - left) * sizeof *inst);
        return OFPERR_OFPBIC_BAD_LEN;
    }
    return 0;
}