static void mrtsock_destruct(struct sock *sk)
{
	rtnl_lock();
	if (sk == mroute_socket) {
		ipv4_devconf.mc_forwarding--;

		write_lock_bh(&mrt_lock);
		mroute_socket=NULL;
		write_unlock_bh(&mrt_lock);

		mroute_clean_tables(sk);
	}
	rtnl_unlock();
}