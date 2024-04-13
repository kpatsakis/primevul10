int __neigh_event_send(struct neighbour *neigh, struct sk_buff *skb)
{
	int rc;
	unsigned long now;

	write_lock_bh(&neigh->lock);

	rc = 0;
	if (neigh->nud_state & (NUD_CONNECTED | NUD_DELAY | NUD_PROBE))
		goto out_unlock_bh;

	now = jiffies;
	
	if (!(neigh->nud_state & (NUD_STALE | NUD_INCOMPLETE))) {
		if (neigh->parms->mcast_probes + neigh->parms->app_probes) {
			atomic_set(&neigh->probes, neigh->parms->ucast_probes);
			neigh->nud_state     = NUD_INCOMPLETE;
			neigh_hold(neigh);
			neigh->timer.expires = now + 1;
			add_timer(&neigh->timer);
		} else {
			neigh->nud_state = NUD_FAILED;
			write_unlock_bh(&neigh->lock);

			if (skb)
				kfree_skb(skb);
			return 1;
		}
	} else if (neigh->nud_state & NUD_STALE) {
		NEIGH_PRINTK2("neigh %p is delayed.\n", neigh);
		neigh_hold(neigh);
		neigh->nud_state = NUD_DELAY;
		neigh->timer.expires = jiffies + neigh->parms->delay_probe_time;
		add_timer(&neigh->timer);
	}

	if (neigh->nud_state == NUD_INCOMPLETE) {
		if (skb) {
			if (skb_queue_len(&neigh->arp_queue) >=
			    neigh->parms->queue_len) {
				struct sk_buff *buff;
				buff = neigh->arp_queue.next;
				__skb_unlink(buff, &neigh->arp_queue);
				kfree_skb(buff);
			}
			__skb_queue_tail(&neigh->arp_queue, skb);
		}
		rc = 1;
	}
out_unlock_bh:
	write_unlock_bh(&neigh->lock);
	return rc;
}