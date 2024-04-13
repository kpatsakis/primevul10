get_actions_from_instruction(const struct ofp11_instruction *inst,
                             const struct ofp_action_header **actions,
                             size_t *actions_len)
{
    *actions = ALIGNED_CAST(const struct ofp_action_header *, inst + 1);
    *actions_len = ntohs(inst->len) - sizeof *inst;
}