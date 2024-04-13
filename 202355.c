tcf_action_dump_old(struct sk_buff *skb, struct tc_action *a, int bind, int ref)
{
	int err = -EINVAL;

	if (a->ops == NULL || a->ops->dump == NULL)
		return err;
	return a->ops->dump(skb, a, bind, ref);
}