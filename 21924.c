static void tcf_block_offload_unbind(struct tcf_block *block, struct Qdisc *q,
				     struct tcf_block_ext_info *ei)
{
	struct net_device *dev = q->dev_queue->dev;
	int err;

	down_write(&block->cb_lock);
	err = tcf_block_offload_cmd(block, dev, q, ei, FLOW_BLOCK_UNBIND, NULL);
	if (err == -EOPNOTSUPP)
		goto no_offload_dev_dec;
	up_write(&block->cb_lock);
	return;

no_offload_dev_dec:
	WARN_ON(block->nooffloaddevcnt-- == 0);
	up_write(&block->cb_lock);
}