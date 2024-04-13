__tc_setup_cb_call(struct tcf_block *block, enum tc_setup_type type,
		   void *type_data, bool err_stop)
{
	struct flow_block_cb *block_cb;
	int ok_count = 0;
	int err;

	list_for_each_entry(block_cb, &block->flow_block.cb_list, list) {
		err = block_cb->cb(type, type_data, block_cb->cb_priv);
		if (err) {
			if (err_stop)
				return err;
		} else {
			ok_count++;
		}
	}
	return ok_count;
}