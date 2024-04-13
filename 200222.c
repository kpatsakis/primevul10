static void moved_group_fair(struct task_struct *p)
{
	struct cfs_rq *cfs_rq = task_cfs_rq(p);

	update_curr(cfs_rq);
	place_entity(cfs_rq, &p->se, 1);
}