tcf_block_playback_offloads(struct tcf_block *block, flow_setup_cb_t *cb,
			    void *cb_priv, bool add, bool offload_in_use,
			    struct netlink_ext_ack *extack)
{
	struct tcf_chain *chain, *chain_prev;
	struct tcf_proto *tp, *tp_prev;
	int err;

	lockdep_assert_held(&block->cb_lock);

	for (chain = __tcf_get_next_chain(block, NULL);
	     chain;
	     chain_prev = chain,
		     chain = __tcf_get_next_chain(block, chain),
		     tcf_chain_put(chain_prev)) {
		for (tp = __tcf_get_next_proto(chain, NULL); tp;
		     tp_prev = tp,
			     tp = __tcf_get_next_proto(chain, tp),
			     tcf_proto_put(tp_prev, true, NULL)) {
			if (tp->ops->reoffload) {
				err = tp->ops->reoffload(tp, add, cb, cb_priv,
							 extack);
				if (err && add)
					goto err_playback_remove;
			} else if (add && offload_in_use) {
				err = -EOPNOTSUPP;
				NL_SET_ERR_MSG(extack, "Filter HW offload failed - classifier without re-offloading support");
				goto err_playback_remove;
			}
		}
	}

	return 0;

err_playback_remove:
	tcf_proto_put(tp, true, NULL);
	tcf_chain_put(chain);
	tcf_block_playback_offloads(block, cb, cb_priv, false, offload_in_use,
				    extack);
	return err;
}