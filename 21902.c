static void tcf_block_destroy(struct tcf_block *block)
{
	mutex_destroy(&block->lock);
	mutex_destroy(&block->proto_destroy_lock);
	kfree_rcu(block, rcu);
}