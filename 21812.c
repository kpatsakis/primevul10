static int tcf_block_offload_bind(struct tcf_block *block, struct Qdisc *q,
				  struct tcf_block_ext_info *ei,
				  struct netlink_ext_ack *extack)
{
	struct net_device *dev = q->dev_queue->dev;
	int err;

	down_write(&block->cb_lock);

	/* If tc offload feature is disabled and the block we try to bind
	 * to already has some offloaded filters, forbid to bind.
	 */
	if (dev->netdev_ops->ndo_setup_tc &&
	    !tc_can_offload(dev) &&
	    tcf_block_offload_in_use(block)) {
		NL_SET_ERR_MSG(extack, "Bind to offloaded block failed as dev has offload disabled");
		err = -EOPNOTSUPP;
		goto err_unlock;
	}

	err = tcf_block_offload_cmd(block, dev, q, ei, FLOW_BLOCK_BIND, extack);
	if (err == -EOPNOTSUPP)
		goto no_offload_dev_inc;
	if (err)
		goto err_unlock;

	up_write(&block->cb_lock);
	return 0;

no_offload_dev_inc:
	if (tcf_block_offload_in_use(block))
		goto err_unlock;

	err = 0;
	block->nooffloaddevcnt++;
err_unlock:
	up_write(&block->cb_lock);
	return err;
}