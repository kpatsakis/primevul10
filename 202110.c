static void cbq_link_class(struct cbq_class *this)
{
	struct cbq_sched_data *q = qdisc_priv(this->qdisc);
	unsigned h = cbq_hash(this->classid);
	struct cbq_class *parent = this->tparent;

	this->sibling = this;
	this->next = q->classes[h];
	q->classes[h] = this;

	if (parent == NULL)
		return;

	if (parent->children == NULL) {
		parent->children = this;
	} else {
		this->sibling = parent->children->sibling;
		parent->children->sibling = this;
	}
}