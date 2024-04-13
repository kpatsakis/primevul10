tc_cls_offload_cnt_reset(struct tcf_block *block, struct tcf_proto *tp,
			 u32 *cnt, u32 *flags)
{
	lockdep_assert_held(&block->cb_lock);

	spin_lock(&tp->lock);
	tcf_block_offload_dec(block, flags);
	*cnt = 0;
	spin_unlock(&tp->lock);
}