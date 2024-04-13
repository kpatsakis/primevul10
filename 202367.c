int neightbl_set(struct sk_buff *skb, struct nlmsghdr *nlh, void *arg)
{
	struct neigh_table *tbl;
	struct ndtmsg *ndtmsg = NLMSG_DATA(nlh);
	struct rtattr **tb = arg;
	int err = -EINVAL;

	if (!tb[NDTA_NAME - 1] || !RTA_PAYLOAD(tb[NDTA_NAME - 1]))
		return -EINVAL;

	read_lock(&neigh_tbl_lock);
	for (tbl = neigh_tables; tbl; tbl = tbl->next) {
		if (ndtmsg->ndtm_family && tbl->family != ndtmsg->ndtm_family)
			continue;

		if (!rtattr_strcmp(tb[NDTA_NAME - 1], tbl->id))
			break;
	}

	if (tbl == NULL) {
		err = -ENOENT;
		goto errout;
	}

	/* 
	 * We acquire tbl->lock to be nice to the periodic timers and
	 * make sure they always see a consistent set of values.
	 */
	write_lock_bh(&tbl->lock);

	if (tb[NDTA_THRESH1 - 1])
		tbl->gc_thresh1 = RTA_GET_U32(tb[NDTA_THRESH1 - 1]);

	if (tb[NDTA_THRESH2 - 1])
		tbl->gc_thresh2 = RTA_GET_U32(tb[NDTA_THRESH2 - 1]);

	if (tb[NDTA_THRESH3 - 1])
		tbl->gc_thresh3 = RTA_GET_U32(tb[NDTA_THRESH3 - 1]);

	if (tb[NDTA_GC_INTERVAL - 1])
		tbl->gc_interval = RTA_GET_MSECS(tb[NDTA_GC_INTERVAL - 1]);

	if (tb[NDTA_PARMS - 1]) {
		struct rtattr *tbp[NDTPA_MAX];
		struct neigh_parms *p;
		u32 ifindex = 0;

		if (rtattr_parse_nested(tbp, NDTPA_MAX, tb[NDTA_PARMS - 1]) < 0)
			goto rtattr_failure;

		if (tbp[NDTPA_IFINDEX - 1])
			ifindex = RTA_GET_U32(tbp[NDTPA_IFINDEX - 1]);

		p = lookup_neigh_params(tbl, ifindex);
		if (p == NULL) {
			err = -ENOENT;
			goto rtattr_failure;
		}
	
		if (tbp[NDTPA_QUEUE_LEN - 1])
			p->queue_len = RTA_GET_U32(tbp[NDTPA_QUEUE_LEN - 1]);

		if (tbp[NDTPA_PROXY_QLEN - 1])
			p->proxy_qlen = RTA_GET_U32(tbp[NDTPA_PROXY_QLEN - 1]);

		if (tbp[NDTPA_APP_PROBES - 1])
			p->app_probes = RTA_GET_U32(tbp[NDTPA_APP_PROBES - 1]);

		if (tbp[NDTPA_UCAST_PROBES - 1])
			p->ucast_probes =
			   RTA_GET_U32(tbp[NDTPA_UCAST_PROBES - 1]);

		if (tbp[NDTPA_MCAST_PROBES - 1])
			p->mcast_probes =
			   RTA_GET_U32(tbp[NDTPA_MCAST_PROBES - 1]);

		if (tbp[NDTPA_BASE_REACHABLE_TIME - 1])
			p->base_reachable_time =
			   RTA_GET_MSECS(tbp[NDTPA_BASE_REACHABLE_TIME - 1]);

		if (tbp[NDTPA_GC_STALETIME - 1])
			p->gc_staletime =
			   RTA_GET_MSECS(tbp[NDTPA_GC_STALETIME - 1]);

		if (tbp[NDTPA_DELAY_PROBE_TIME - 1])
			p->delay_probe_time =
			   RTA_GET_MSECS(tbp[NDTPA_DELAY_PROBE_TIME - 1]);

		if (tbp[NDTPA_RETRANS_TIME - 1])
			p->retrans_time =
			   RTA_GET_MSECS(tbp[NDTPA_RETRANS_TIME - 1]);

		if (tbp[NDTPA_ANYCAST_DELAY - 1])
			p->anycast_delay =
			   RTA_GET_MSECS(tbp[NDTPA_ANYCAST_DELAY - 1]);

		if (tbp[NDTPA_PROXY_DELAY - 1])
			p->proxy_delay =
			   RTA_GET_MSECS(tbp[NDTPA_PROXY_DELAY - 1]);

		if (tbp[NDTPA_LOCKTIME - 1])
			p->locktime = RTA_GET_MSECS(tbp[NDTPA_LOCKTIME - 1]);
	}

	err = 0;

rtattr_failure:
	write_unlock_bh(&tbl->lock);
errout:
	read_unlock(&neigh_tbl_lock);
	return err;
}