static int neightbl_fill_parms(struct sk_buff *skb, struct neigh_parms *parms)
{
	struct rtattr *nest = NULL;
	
	nest = RTA_NEST(skb, NDTA_PARMS);

	if (parms->dev)
		RTA_PUT_U32(skb, NDTPA_IFINDEX, parms->dev->ifindex);

	RTA_PUT_U32(skb, NDTPA_REFCNT, atomic_read(&parms->refcnt));
	RTA_PUT_U32(skb, NDTPA_QUEUE_LEN, parms->queue_len);
	RTA_PUT_U32(skb, NDTPA_PROXY_QLEN, parms->proxy_qlen);
	RTA_PUT_U32(skb, NDTPA_APP_PROBES, parms->app_probes);
	RTA_PUT_U32(skb, NDTPA_UCAST_PROBES, parms->ucast_probes);
	RTA_PUT_U32(skb, NDTPA_MCAST_PROBES, parms->mcast_probes);
	RTA_PUT_MSECS(skb, NDTPA_REACHABLE_TIME, parms->reachable_time);
	RTA_PUT_MSECS(skb, NDTPA_BASE_REACHABLE_TIME,
		      parms->base_reachable_time);
	RTA_PUT_MSECS(skb, NDTPA_GC_STALETIME, parms->gc_staletime);
	RTA_PUT_MSECS(skb, NDTPA_DELAY_PROBE_TIME, parms->delay_probe_time);
	RTA_PUT_MSECS(skb, NDTPA_RETRANS_TIME, parms->retrans_time);
	RTA_PUT_MSECS(skb, NDTPA_ANYCAST_DELAY, parms->anycast_delay);
	RTA_PUT_MSECS(skb, NDTPA_PROXY_DELAY, parms->proxy_delay);
	RTA_PUT_MSECS(skb, NDTPA_LOCKTIME, parms->locktime);

	return RTA_NEST_END(skb, nest);

rtattr_failure:
	return RTA_NEST_CANCEL(skb, nest);
}