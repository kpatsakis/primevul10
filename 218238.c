static void defer_request(struct i915_request *rq, struct list_head * const pl)
{
	LIST_HEAD(list);

	/*
	 * We want to move the interrupted request to the back of
	 * the round-robin list (i.e. its priority level), but
	 * in doing so, we must then move all requests that were in
	 * flight and were waiting for the interrupted request to
	 * be run after it again.
	 */
	do {
		struct i915_dependency *p;

		GEM_BUG_ON(i915_request_is_active(rq));
		list_move_tail(&rq->sched.link, pl);

		list_for_each_entry(p, &rq->sched.waiters_list, wait_link) {
			struct i915_request *w =
				container_of(p->waiter, typeof(*w), sched);

			/* Leave semaphores spinning on the other engines */
			if (w->engine != rq->engine)
				continue;

			/* No waiter should start before its signaler */
			GEM_BUG_ON(i915_request_started(w) &&
				   !i915_request_completed(rq));

			GEM_BUG_ON(i915_request_is_active(w));
			if (list_empty(&w->sched.link))
				continue; /* Not yet submitted; unready */

			if (rq_prio(w) < rq_prio(rq))
				continue;

			GEM_BUG_ON(rq_prio(w) > rq_prio(rq));
			list_move_tail(&w->sched.link, &list);
		}

		rq = list_first_entry_or_null(&list, typeof(*rq), sched.link);
	} while (rq);
}