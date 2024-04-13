__load_balance_iterator(struct cfs_rq *cfs_rq, struct rb_node *curr)
{
	struct task_struct *p;

	if (!curr)
		return NULL;

	p = rb_entry(curr, struct task_struct, se.run_node);
	cfs_rq->rb_load_balance_curr = rb_next(curr);

	return p;
}