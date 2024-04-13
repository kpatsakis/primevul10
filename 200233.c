static void __dequeue_entity(struct cfs_rq *cfs_rq, struct sched_entity *se)
{
	if (cfs_rq->rb_leftmost == &se->run_node) {
		struct rb_node *next_node;
		struct sched_entity *next;

		next_node = rb_next(&se->run_node);
		cfs_rq->rb_leftmost = next_node;

		if (next_node) {
			next = rb_entry(next_node,
					struct sched_entity, run_node);
			cfs_rq->min_vruntime =
				max_vruntime(cfs_rq->min_vruntime,
					     next->vruntime);
		}
	}

	if (cfs_rq->next == se)
		cfs_rq->next = NULL;

	rb_erase(&se->run_node, &cfs_rq->tasks_timeline);
}