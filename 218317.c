__unwind_incomplete_requests(struct intel_engine_cs *engine)
{
	struct i915_request *rq, *rn, *active = NULL;
	struct list_head *uninitialized_var(pl);
	int prio = I915_PRIORITY_INVALID;

	lockdep_assert_held(&engine->active.lock);

	list_for_each_entry_safe_reverse(rq, rn,
					 &engine->active.requests,
					 sched.link) {
		if (i915_request_completed(rq))
			continue; /* XXX */

		__i915_request_unsubmit(rq);

		/*
		 * Push the request back into the queue for later resubmission.
		 * If this request is not native to this physical engine (i.e.
		 * it came from a virtual source), push it back onto the virtual
		 * engine so that it can be moved across onto another physical
		 * engine as load dictates.
		 */
		if (likely(rq->execution_mask == engine->mask)) {
			GEM_BUG_ON(rq_prio(rq) == I915_PRIORITY_INVALID);
			if (rq_prio(rq) != prio) {
				prio = rq_prio(rq);
				pl = i915_sched_lookup_priolist(engine, prio);
			}
			GEM_BUG_ON(RB_EMPTY_ROOT(&engine->execlists.queue.rb_root));

			list_move(&rq->sched.link, pl);
			active = rq;
		} else {
			struct intel_engine_cs *owner = rq->hw_context->engine;

			/*
			 * Decouple the virtual breadcrumb before moving it
			 * back to the virtual engine -- we don't want the
			 * request to complete in the background and try
			 * and cancel the breadcrumb on the virtual engine
			 * (instead of the old engine where it is linked)!
			 */
			if (test_bit(DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT,
				     &rq->fence.flags)) {
				spin_lock_nested(&rq->lock,
						 SINGLE_DEPTH_NESTING);
				i915_request_cancel_breadcrumb(rq);
				spin_unlock(&rq->lock);
			}
			rq->engine = owner;
			owner->submit_request(rq);
			active = NULL;
		}
	}

	return active;
}