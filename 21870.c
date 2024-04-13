static void tcf_block_offload_init(struct flow_block_offload *bo,
				   struct net_device *dev, struct Qdisc *sch,
				   enum flow_block_command command,
				   enum flow_block_binder_type binder_type,
				   struct flow_block *flow_block,
				   bool shared, struct netlink_ext_ack *extack)
{
	bo->net = dev_net(dev);
	bo->command = command;
	bo->binder_type = binder_type;
	bo->block = flow_block;
	bo->block_shared = shared;
	bo->extack = extack;
	bo->sch = sch;
	bo->cb_list_head = &flow_block->cb_list;
	INIT_LIST_HEAD(&bo->cb_list);
}