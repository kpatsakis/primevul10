encode_STACK_PUSH(const struct ofpact_stack *stack,
                  enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    encode_STACK_op(stack, put_NXAST_STACK_PUSH(out));
}