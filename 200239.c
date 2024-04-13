static struct sched_entity *pick_next_entity(struct cfs_rq *cfs_rq)
{
	struct sched_entity *se = NULL;

	if (first_fair(cfs_rq)) {
		se = __pick_next_entity(cfs_rq);
		se = pick_next(cfs_rq, se);
		set_next_entity(cfs_rq, se);
	}

	return se;
}