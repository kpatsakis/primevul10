static void virtual_submit_request(struct i915_request *rq)
{
	struct virtual_engine *ve = to_virtual_engine(rq->engine);
	struct i915_request *old;
	unsigned long flags;

	GEM_TRACE("%s: rq=%llx:%lld\n",
		  ve->base.name,
		  rq->fence.context,
		  rq->fence.seqno);

	GEM_BUG_ON(ve->base.submit_request != virtual_submit_request);

	spin_lock_irqsave(&ve->base.active.lock, flags);

	old = ve->request;
	if (old) { /* background completion event from preempt-to-busy */
		GEM_BUG_ON(!i915_request_completed(old));
		__i915_request_submit(old);
		i915_request_put(old);
	}

	if (i915_request_completed(rq)) {
		__i915_request_submit(rq);

		ve->base.execlists.queue_priority_hint = INT_MIN;
		ve->request = NULL;
	} else {
		ve->base.execlists.queue_priority_hint = rq_prio(rq);
		ve->request = i915_request_get(rq);

		GEM_BUG_ON(!list_empty(virtual_queue(ve)));
		list_move_tail(&rq->sched.link, virtual_queue(ve));

		tasklet_schedule(&ve->base.execlists.tasklet);
	}

	spin_unlock_irqrestore(&ve->base.active.lock, flags);
}