static void pfkey_dump_sp_done(struct pfkey_sock *pfk)
{
	struct net *net = sock_net((struct sock *)pfk);

	xfrm_policy_walk_done(&pfk->dump.u.policy, net);
}