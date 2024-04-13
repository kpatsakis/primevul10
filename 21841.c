static bool tcf_proto_cmp(const struct tcf_proto *tp1,
			  const struct tcf_proto *tp2)
{
	return tp1->chain->index == tp2->chain->index &&
	       tp1->prio == tp2->prio &&
	       tp1->protocol == tp2->protocol;
}