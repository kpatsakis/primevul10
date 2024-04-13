static bool pfkey_is_alive(const struct km_event *c)
{
	struct netns_pfkey *net_pfkey = net_generic(c->net, pfkey_net_id);
	struct sock *sk;
	bool is_alive = false;

	rcu_read_lock();
	sk_for_each_rcu(sk, &net_pfkey->table) {
		if (pfkey_sk(sk)->registered) {
			is_alive = true;
			break;
		}
	}
	rcu_read_unlock();

	return is_alive;
}