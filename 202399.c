tcf_action_dump_1(struct sk_buff *skb, struct tc_action *a, int bind, int ref)
{
	int err = -EINVAL;
	unsigned char *b = skb->tail;
	struct rtattr *r;

	if (a->ops == NULL || a->ops->dump == NULL)
		return err;

	RTA_PUT(skb, TCA_KIND, IFNAMSIZ, a->ops->kind);
	if (tcf_action_copy_stats(skb, a, 0))
		goto rtattr_failure;
	r = (struct rtattr*) skb->tail;
	RTA_PUT(skb, TCA_OPTIONS, 0, NULL);
	if ((err = tcf_action_dump_old(skb, a, bind, ref)) > 0) {
		r->rta_len = skb->tail - (u8*)r;
		return err;
	}

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}