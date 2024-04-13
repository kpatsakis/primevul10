int tc_setup_cb_reoffload(struct tcf_block *block, struct tcf_proto *tp,
			  bool add, flow_setup_cb_t *cb,
			  enum tc_setup_type type, void *type_data,
			  void *cb_priv, u32 *flags, unsigned int *in_hw_count)
{
	int err = cb(type, type_data, cb_priv);

	if (err) {
		if (add && tc_skip_sw(*flags))
			return err;
	} else {
		tc_cls_offload_cnt_update(block, tp, in_hw_count, flags, 1,
					  add);
	}

	return 0;
}