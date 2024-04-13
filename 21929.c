tcf_get_next_proto(struct tcf_chain *chain, struct tcf_proto *tp)
{
	struct tcf_proto *tp_next = __tcf_get_next_proto(chain, tp);

	if (tp)
		tcf_proto_put(tp, true, NULL);

	return tp_next;
}