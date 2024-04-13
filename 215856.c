static int set_ipsecrequest(struct sk_buff *skb,
			    uint8_t proto, uint8_t mode, int level,
			    uint32_t reqid, uint8_t family,
			    const xfrm_address_t *src, const xfrm_address_t *dst)
{
	struct sadb_x_ipsecrequest *rq;
	u8 *sa;
	int socklen = pfkey_sockaddr_len(family);
	int size_req;

	size_req = sizeof(struct sadb_x_ipsecrequest) +
		   pfkey_sockaddr_pair_size(family);

	rq = skb_put_zero(skb, size_req);
	rq->sadb_x_ipsecrequest_len = size_req;
	rq->sadb_x_ipsecrequest_proto = proto;
	rq->sadb_x_ipsecrequest_mode = mode;
	rq->sadb_x_ipsecrequest_level = level;
	rq->sadb_x_ipsecrequest_reqid = reqid;

	sa = (u8 *) (rq + 1);
	if (!pfkey_sockaddr_fill(src, 0, (struct sockaddr *)sa, family) ||
	    !pfkey_sockaddr_fill(dst, 0, (struct sockaddr *)(sa + socklen), family))
		return -EINVAL;

	return 0;
}