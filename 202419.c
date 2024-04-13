int tcf_action_exec(struct sk_buff *skb, struct tc_action *act,
                    struct tcf_result *res)
{
	struct tc_action *a;
	int ret = -1;

	if (skb->tc_verd & TC_NCLS) {
		skb->tc_verd = CLR_TC_NCLS(skb->tc_verd);
		D2PRINTK("(%p)tcf_action_exec: cleared TC_NCLS in %s out %s\n",
		         skb, skb->input_dev ? skb->input_dev->name : "xxx",
		         skb->dev->name);
		ret = TC_ACT_OK;
		goto exec_done;
	}
	while ((a = act) != NULL) {
repeat:
		if (a->ops && a->ops->act) {
			ret = a->ops->act(&skb, a);
			if (TC_MUNGED & skb->tc_verd) {
				/* copied already, allow trampling */
				skb->tc_verd = SET_TC_OK2MUNGE(skb->tc_verd);
				skb->tc_verd = CLR_TC_MUNGED(skb->tc_verd);
			}
			if (ret == TC_ACT_REPEAT)
				goto repeat;	/* we need a ttl - JHS */
			if (ret != TC_ACT_PIPE)
				goto exec_done;
		}
		act = a->next;
	}
exec_done:
	if (skb->tc_classid > 0) {
		res->classid = skb->tc_classid;
		res->class = 0;
		skb->tc_classid = 0;
	}
	return ret;
}