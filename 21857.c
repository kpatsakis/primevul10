static int tcf_qevent_parse_block_index(struct nlattr *block_index_attr,
					u32 *p_block_index,
					struct netlink_ext_ack *extack)
{
	*p_block_index = nla_get_u32(block_index_attr);
	if (!*p_block_index) {
		NL_SET_ERR_MSG(extack, "Block number may not be zero");
		return -EINVAL;
	}

	return 0;
}