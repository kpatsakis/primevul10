static inline bool need_preempt(const struct intel_engine_cs *engine,
				const struct i915_request *rq,
				struct rb_node *rb)
{
	int last_prio;

	if (!intel_engine_has_semaphores(engine))
		return false;

	/*
	 * Check if the current priority hint merits a preemption attempt.
	 *
	 * We record the highest value priority we saw during rescheduling
	 * prior to this dequeue, therefore we know that if it is strictly
	 * less than the current tail of ESLP[0], we do not need to force
	 * a preempt-to-idle cycle.
	 *
	 * However, the priority hint is a mere hint that we may need to
	 * preempt. If that hint is stale or we may be trying to preempt
	 * ourselves, ignore the request.
	 *
	 * More naturally we would write
	 *      prio >= max(0, last);
	 * except that we wish to prevent triggering preemption at the same
	 * priority level: the task that is running should remain running
	 * to preserve FIFO ordering of dependencies.
	 */
	last_prio = max(effective_prio(rq), I915_PRIORITY_NORMAL - 1);
	if (engine->execlists.queue_priority_hint <= last_prio)
		return false;

	/*
	 * Check against the first request in ELSP[1], it will, thanks to the
	 * power of PI, be the highest priority of that context.
	 */
	if (!list_is_last(&rq->sched.link, &engine->active.requests) &&
	    rq_prio(list_next_entry(rq, sched.link)) > last_prio)
		return true;

	if (rb) {
		struct virtual_engine *ve =
			rb_entry(rb, typeof(*ve), nodes[engine->id].rb);
		bool preempt = false;

		if (engine == ve->siblings[0]) { /* only preempt one sibling */
			struct i915_request *next;

			rcu_read_lock();
			next = READ_ONCE(ve->request);
			if (next)
				preempt = rq_prio(next) > last_prio;
			rcu_read_unlock();
		}

		if (preempt)
			return preempt;
	}

	/*
	 * If the inflight context did not trigger the preemption, then maybe
	 * it was the set of queued requests? Pick the highest priority in
	 * the queue (the first active priolist) and see if it deserves to be
	 * running instead of ELSP[0].
	 *
	 * The highest priority request in the queue can not be either
	 * ELSP[0] or ELSP[1] as, thanks again to PI, if it was the same
	 * context, it's priority would not exceed ELSP[0] aka last_prio.
	 */
	return queue_prio(&engine->execlists) > last_prio;
}