__tcf_get_next_chain(struct tcf_block *block, struct tcf_chain *chain)
{
	mutex_lock(&block->lock);
	if (chain)
		chain = list_is_last(&chain->list, &block->chain_list) ?
			NULL : list_next_entry(chain, list);
	else
		chain = list_first_entry_or_null(&block->chain_list,
						 struct tcf_chain, list);

	/* skip all action-only chains */
	while (chain && tcf_chain_held_by_acts_only(chain))
		chain = list_is_last(&chain->list, &block->chain_list) ?
			NULL : list_next_entry(chain, list);

	if (chain)
		tcf_chain_hold(chain);
	mutex_unlock(&block->lock);

	return chain;
}