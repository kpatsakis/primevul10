tcf_chain0_head_change_cb_del(struct tcf_block *block,
			      struct tcf_block_ext_info *ei)
{
	struct tcf_filter_chain_list_item *item;

	mutex_lock(&block->lock);
	list_for_each_entry(item, &block->chain0.filter_chain_list, list) {
		if ((!ei->chain_head_change && !ei->chain_head_change_priv) ||
		    (item->chain_head_change == ei->chain_head_change &&
		     item->chain_head_change_priv == ei->chain_head_change_priv)) {
			if (block->chain0.chain)
				tcf_chain_head_change_item(item, NULL);
			list_del(&item->list);
			mutex_unlock(&block->lock);

			kfree(item);
			return;
		}
	}
	mutex_unlock(&block->lock);
	WARN_ON(1);
}