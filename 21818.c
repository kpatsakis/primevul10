static int tcf_block_bind(struct tcf_block *block,
			  struct flow_block_offload *bo)
{
	struct flow_block_cb *block_cb, *next;
	int err, i = 0;

	lockdep_assert_held(&block->cb_lock);

	list_for_each_entry(block_cb, &bo->cb_list, list) {
		err = tcf_block_playback_offloads(block, block_cb->cb,
						  block_cb->cb_priv, true,
						  tcf_block_offload_in_use(block),
						  bo->extack);
		if (err)
			goto err_unroll;
		if (!bo->unlocked_driver_cb)
			block->lockeddevcnt++;

		i++;
	}
	list_splice(&bo->cb_list, &block->flow_block.cb_list);

	return 0;

err_unroll:
	list_for_each_entry_safe(block_cb, next, &bo->cb_list, list) {
		if (i-- > 0) {
			list_del(&block_cb->list);
			tcf_block_playback_offloads(block, block_cb->cb,
						    block_cb->cb_priv, false,
						    tcf_block_offload_in_use(block),
						    NULL);
			if (!bo->unlocked_driver_cb)
				block->lockeddevcnt--;
		}
		flow_block_cb_free(block_cb);
	}

	return err;
}