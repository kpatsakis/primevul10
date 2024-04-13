int tcf_qevent_init(struct tcf_qevent *qe, struct Qdisc *sch,
		    enum flow_block_binder_type binder_type,
		    struct nlattr *block_index_attr,
		    struct netlink_ext_ack *extack)
{
	u32 block_index;
	int err;

	if (!block_index_attr)
		return 0;

	err = tcf_qevent_parse_block_index(block_index_attr, &block_index, extack);
	if (err)
		return err;

	if (!block_index)
		return 0;

	qe->info.binder_type = binder_type;
	qe->info.chain_head_change = tcf_chain_head_change_dflt;
	qe->info.chain_head_change_priv = &qe->filter_chain;
	qe->info.block_index = block_index;

	return tcf_block_get_ext(&qe->block, sch, &qe->info, extack);
}