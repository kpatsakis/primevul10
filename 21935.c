int tcf_qevent_validate_change(struct tcf_qevent *qe, struct nlattr *block_index_attr,
			       struct netlink_ext_ack *extack)
{
	u32 block_index;
	int err;

	if (!block_index_attr)
		return 0;

	err = tcf_qevent_parse_block_index(block_index_attr, &block_index, extack);
	if (err)
		return err;

	/* Bounce newly-configured block or change in block. */
	if (block_index != qe->info.block_index) {
		NL_SET_ERR_MSG(extack, "Change of blocks is not supported");
		return -EINVAL;
	}

	return 0;
}