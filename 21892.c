static void tcf_block_unbind(struct tcf_block *block,
			     struct flow_block_offload *bo)
{
	struct flow_block_cb *block_cb, *next;

	lockdep_assert_held(&block->cb_lock);

	list_for_each_entry_safe(block_cb, next, &bo->cb_list, list) {
		tcf_block_playback_offloads(block, block_cb->cb,
					    block_cb->cb_priv, false,
					    tcf_block_offload_in_use(block),
					    NULL);
		list_del(&block_cb->list);
		flow_block_cb_free(block_cb);
		if (!bo->unlocked_driver_cb)
			block->lockeddevcnt--;
	}
}