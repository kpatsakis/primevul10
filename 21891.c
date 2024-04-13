static void tc_block_indr_cleanup(struct flow_block_cb *block_cb)
{
	struct tcf_block *block = block_cb->indr.data;
	struct net_device *dev = block_cb->indr.dev;
	struct Qdisc *sch = block_cb->indr.sch;
	struct netlink_ext_ack extack = {};
	struct flow_block_offload bo = {};

	tcf_block_offload_init(&bo, dev, sch, FLOW_BLOCK_UNBIND,
			       block_cb->indr.binder_type,
			       &block->flow_block, tcf_block_shared(block),
			       &extack);
	rtnl_lock();
	down_write(&block->cb_lock);
	list_del(&block_cb->driver_list);
	list_move(&block_cb->list, &bo.cb_list);
	tcf_block_unbind(block, &bo);
	up_write(&block->cb_lock);
	rtnl_unlock();
}