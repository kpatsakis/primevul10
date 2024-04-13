cbq_reclassify(struct sk_buff *skb, struct cbq_class *this)
{
	struct cbq_class *cl, *new;

	for (cl = this->tparent; cl; cl = cl->tparent)
		if ((new = cl->defaults[TC_PRIO_BESTEFFORT]) != NULL && new != this)
			return new;

	return NULL;
}