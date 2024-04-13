static int tcf_block_offload_cmd(struct tcf_block *block,
				 struct net_device *dev, struct Qdisc *sch,
				 struct tcf_block_ext_info *ei,
				 enum flow_block_command command,
				 struct netlink_ext_ack *extack)
{
	struct flow_block_offload bo = {};

	tcf_block_offload_init(&bo, dev, sch, command, ei->binder_type,
			       &block->flow_block, tcf_block_shared(block),
			       extack);

	if (dev->netdev_ops->ndo_setup_tc) {
		int err;

		err = dev->netdev_ops->ndo_setup_tc(dev, TC_SETUP_BLOCK, &bo);
		if (err < 0) {
			if (err != -EOPNOTSUPP)
				NL_SET_ERR_MSG(extack, "Driver ndo_setup_tc failed");
			return err;
		}

		return tcf_block_setup(block, &bo);
	}

	flow_indr_dev_setup_offload(dev, sch, TC_SETUP_BLOCK, block, &bo,
				    tc_block_indr_cleanup);
	tcf_block_setup(block, &bo);

	return -EOPNOTSUPP;
}