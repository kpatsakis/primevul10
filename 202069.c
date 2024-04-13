static int rsvp_classify(struct sk_buff *skb, struct tcf_proto *tp,
			 struct tcf_result *res)
{
	struct rsvp_session **sht = ((struct rsvp_head*)tp->root)->ht;
	struct rsvp_session *s;
	struct rsvp_filter *f;
	unsigned h1, h2;
	u32 *dst, *src;
	u8 protocol;
	u8 tunnelid = 0;
	u8 *xprt;
#if RSVP_DST_LEN == 4
	struct ipv6hdr *nhptr = skb->nh.ipv6h;
#else
	struct iphdr *nhptr = skb->nh.iph;
#endif

restart:

#if RSVP_DST_LEN == 4
	src = &nhptr->saddr.s6_addr32[0];
	dst = &nhptr->daddr.s6_addr32[0];
	protocol = nhptr->nexthdr;
	xprt = ((u8*)nhptr) + sizeof(struct ipv6hdr);
#else
	src = &nhptr->saddr;
	dst = &nhptr->daddr;
	protocol = nhptr->protocol;
	xprt = ((u8*)nhptr) + (nhptr->ihl<<2);
	if (nhptr->frag_off&__constant_htons(IP_MF|IP_OFFSET))
		return -1;
#endif

	h1 = hash_dst(dst, protocol, tunnelid);
	h2 = hash_src(src);

	for (s = sht[h1]; s; s = s->next) {
		if (dst[RSVP_DST_LEN-1] == s->dst[RSVP_DST_LEN-1] &&
		    protocol == s->protocol &&
		    !(s->dpi.mask & (*(u32*)(xprt+s->dpi.offset)^s->dpi.key))
#if RSVP_DST_LEN == 4
		    && dst[0] == s->dst[0]
		    && dst[1] == s->dst[1]
		    && dst[2] == s->dst[2]
#endif
		    && tunnelid == s->tunnelid) {

			for (f = s->ht[h2]; f; f = f->next) {
				if (src[RSVP_DST_LEN-1] == f->src[RSVP_DST_LEN-1] &&
				    !(f->spi.mask & (*(u32*)(xprt+f->spi.offset)^f->spi.key))
#if RSVP_DST_LEN == 4
				    && src[0] == f->src[0]
				    && src[1] == f->src[1]
				    && src[2] == f->src[2]
#endif
				    ) {
					*res = f->res;
					RSVP_APPLY_RESULT();

matched:
					if (f->tunnelhdr == 0)
						return 0;

					tunnelid = f->res.classid;
					nhptr = (void*)(xprt + f->tunnelhdr - sizeof(*nhptr));
					goto restart;
				}
			}

			/* And wildcard bucket... */
			for (f = s->ht[16]; f; f = f->next) {
				*res = f->res;
				RSVP_APPLY_RESULT();
				goto matched;
			}
			return -1;
		}
	}
	return -1;
}