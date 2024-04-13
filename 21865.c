static int tcf_chain_tp_insert(struct tcf_chain *chain,
			       struct tcf_chain_info *chain_info,
			       struct tcf_proto *tp)
{
	if (chain->flushing)
		return -EAGAIN;

	if (*chain_info->pprev == chain->filter_chain)
		tcf_chain0_head_change(chain, tp);
	tcf_proto_get(tp);
	RCU_INIT_POINTER(tp->next, tcf_chain_tp_prev(chain, chain_info));
	rcu_assign_pointer(*chain_info->pprev, tp);

	return 0;
}