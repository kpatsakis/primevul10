tcf_action_dump(struct sk_buff *skb, struct tc_action *act, int bind, int ref)
{
	struct tc_action *a;
	int err = -EINVAL;
	unsigned char *b = skb->tail;
	struct rtattr *r ;

	while ((a = act) != NULL) {
		r = (struct rtattr*) skb->tail;
		act = a->next;
		RTA_PUT(skb, a->order, 0, NULL);
		err = tcf_action_dump_1(skb, a, bind, ref);
		if (err < 0)
			goto rtattr_failure;
		r->rta_len = skb->tail - (u8*)r;
	}

	return 0;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -err;
}