static int tc_dump_qdisc(struct sk_buff *skb, struct netlink_callback *cb)
{
	int idx, q_idx;
	int s_idx, s_q_idx;
	struct net_device *dev;
	struct Qdisc *q;

	s_idx = cb->args[0];
	s_q_idx = q_idx = cb->args[1];
	read_lock(&dev_base_lock);
	for (dev=dev_base, idx=0; dev; dev = dev->next, idx++) {
		if (idx < s_idx)
			continue;
		if (idx > s_idx)
			s_q_idx = 0;
		read_lock_bh(&qdisc_tree_lock);
		q_idx = 0;
		list_for_each_entry(q, &dev->qdisc_list, list) {
			if (q_idx < s_q_idx) {
				q_idx++;
				continue;
			}
			if (tc_fill_qdisc(skb, q, q->parent, NETLINK_CB(cb->skb).pid,
					  cb->nlh->nlmsg_seq, NLM_F_MULTI, RTM_NEWQDISC) <= 0) {
				read_unlock_bh(&qdisc_tree_lock);
				goto done;
			}
			q_idx++;
		}
		read_unlock_bh(&qdisc_tree_lock);
	}

done:
	read_unlock(&dev_base_lock);

	cb->args[0] = idx;
	cb->args[1] = q_idx;

	return skb->len;
}