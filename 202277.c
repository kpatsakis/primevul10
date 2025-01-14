int tc_classify(struct sk_buff *skb, struct tcf_proto *tp,
	struct tcf_result *res)
{
	int err = 0;
	u32 protocol = skb->protocol;
#ifdef CONFIG_NET_CLS_ACT
	struct tcf_proto *otp = tp;
reclassify:
#endif
	protocol = skb->protocol;

	for ( ; tp; tp = tp->next) {
		if ((tp->protocol == protocol ||
			tp->protocol == __constant_htons(ETH_P_ALL)) &&
			(err = tp->classify(skb, tp, res)) >= 0) {
#ifdef CONFIG_NET_CLS_ACT
			if ( TC_ACT_RECLASSIFY == err) {
				__u32 verd = (__u32) G_TC_VERD(skb->tc_verd);
				tp = otp;

				if (MAX_REC_LOOP < verd++) {
					printk("rule prio %d protocol %02x reclassify is buggy packet dropped\n",
						tp->prio&0xffff, ntohs(tp->protocol));
					return TC_ACT_SHOT;
				}
				skb->tc_verd = SET_TC_VERD(skb->tc_verd,verd);
				goto reclassify;
			} else {
				if (skb->tc_verd) 
					skb->tc_verd = SET_TC_VERD(skb->tc_verd,0);
				return err;
			}
#else

			return err;
#endif
		}

	}
	return -1;
}