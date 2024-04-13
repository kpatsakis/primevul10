static __inline__ struct sock *udp_v4_lookup(__be32 saddr, __be16 sport,
					     __be32 daddr, __be16 dport, int dif)
{
	struct sock *sk;

	read_lock(&udp_hash_lock);
	sk = udp_v4_lookup_longway(saddr, sport, daddr, dport, dif);
	if (sk)
		sock_hold(sk);
	read_unlock(&udp_hash_lock);
	return sk;
}