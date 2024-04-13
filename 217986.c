encode_STACK_op(const struct ofpact_stack *stack_action,
                struct nx_action_stack *nasp)
{
    struct ofpbuf b;
    ovs_be16 n_bits;

    nasp->offset = htons(stack_action->subfield.ofs);

    ofpbuf_use_stack(&b, nasp, ntohs(nasp->len));
    ofpbuf_put_uninit(&b, OBJECT_OFFSETOF(nasp, pad));
    nx_put_mff_header(&b, stack_action->subfield.field, 0, false);
    n_bits = htons(stack_action->subfield.n_bits);
    ofpbuf_put(&b, &n_bits, sizeof n_bits);
}