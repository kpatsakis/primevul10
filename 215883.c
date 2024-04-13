static int pfkey_do_dump(struct pfkey_sock *pfk)
{
	struct sadb_msg *hdr;
	int rc;

	mutex_lock(&pfk->dump_lock);
	if (!pfk->dump.dump) {
		rc = 0;
		goto out;
	}

	rc = pfk->dump.dump(pfk);
	if (rc == -ENOBUFS) {
		rc = 0;
		goto out;
	}

	if (pfk->dump.skb) {
		if (!pfkey_can_dump(&pfk->sk)) {
			rc = 0;
			goto out;
		}

		hdr = (struct sadb_msg *) pfk->dump.skb->data;
		hdr->sadb_msg_seq = 0;
		hdr->sadb_msg_errno = rc;
		pfkey_broadcast(pfk->dump.skb, GFP_ATOMIC, BROADCAST_ONE,
				&pfk->sk, sock_net(&pfk->sk));
		pfk->dump.skb = NULL;
	}

	pfkey_terminate_dump(pfk);

out:
	mutex_unlock(&pfk->dump_lock);
	return rc;
}