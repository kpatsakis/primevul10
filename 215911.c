static int pfkey_spddump(struct sock *sk, struct sk_buff *skb, const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	struct pfkey_sock *pfk = pfkey_sk(sk);

	mutex_lock(&pfk->dump_lock);
	if (pfk->dump.dump != NULL) {
		mutex_unlock(&pfk->dump_lock);
		return -EBUSY;
	}

	pfk->dump.msg_version = hdr->sadb_msg_version;
	pfk->dump.msg_portid = hdr->sadb_msg_pid;
	pfk->dump.dump = pfkey_dump_sp;
	pfk->dump.done = pfkey_dump_sp_done;
	xfrm_policy_walk_init(&pfk->dump.u.policy, XFRM_POLICY_TYPE_MAIN);
	mutex_unlock(&pfk->dump_lock);

	return pfkey_do_dump(pfk);
}