static void pfkey_dump_sa_done(struct pfkey_sock *pfk)
{
	struct net *net = sock_net(&pfk->sk);

	xfrm_state_walk_done(&pfk->dump.u.state, net);
}