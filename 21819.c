static void tc_cls_offload_cnt_update(struct tcf_block *block,
				      struct tcf_proto *tp, u32 *cnt,
				      u32 *flags, u32 diff, bool add)
{
	lockdep_assert_held(&block->cb_lock);

	spin_lock(&tp->lock);
	if (add) {
		if (!*cnt)
			tcf_block_offload_inc(block, flags);
		*cnt += diff;
	} else {
		*cnt -= diff;
		if (!*cnt)
			tcf_block_offload_dec(block, flags);
	}
	spin_unlock(&tp->lock);
}