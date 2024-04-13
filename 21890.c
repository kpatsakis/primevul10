int tcf_exts_validate(struct net *net, struct tcf_proto *tp, struct nlattr **tb,
		      struct nlattr *rate_tlv, struct tcf_exts *exts,
		      u32 flags, struct netlink_ext_ack *extack)
{
	return tcf_exts_validate_ex(net, tp, tb, rate_tlv, exts,
				    flags, 0, extack);
}