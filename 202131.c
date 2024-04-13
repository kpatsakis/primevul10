static void cbq_unlink_class(struct cbq_class *this)
{
	struct cbq_class *cl, **clp;
	struct cbq_sched_data *q = qdisc_priv(this->qdisc);

	for (clp = &q->classes[cbq_hash(this->classid)]; (cl = *clp) != NULL; clp = &cl->next) {
		if (cl == this) {
			*clp = cl->next;
			cl->next = NULL;
			break;
		}
	}

	if (this->tparent) {
		clp=&this->sibling;
		cl = *clp;
		do {
			if (cl == this) {
				*clp = cl->sibling;
				break;
			}
			clp = &cl->sibling;
		} while ((cl = *clp) != this->sibling);

		if (this->tparent->children == this) {
			this->tparent->children = this->sibling;
			if (this->sibling == this)
				this->tparent->children = NULL;
		}
	} else {
		BUG_TRAP(this->sibling == this);
	}
}