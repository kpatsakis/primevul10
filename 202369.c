struct Qdisc *qdisc_lookup(struct net_device *dev, u32 handle)
{
	struct Qdisc *q;

	read_lock_bh(&qdisc_tree_lock);
	list_for_each_entry(q, &dev->qdisc_list, list) {
		if (q->handle == handle) {
			read_unlock_bh(&qdisc_tree_lock);
			return q;
		}
	}
	read_unlock_bh(&qdisc_tree_lock);
	return NULL;
}