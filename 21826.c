static struct tcf_chain *tcf_chain_lookup_rcu(const struct tcf_block *block,
					      u32 chain_index)
{
	struct tcf_chain *chain;

	list_for_each_entry_rcu(chain, &block->chain_list, list) {
		if (chain->index == chain_index)
			return chain;
	}
	return NULL;
}