static void __execlists_reset(struct intel_engine_cs *engine, bool stalled)
{
	struct intel_engine_execlists * const execlists = &engine->execlists;
	struct intel_context *ce;
	struct i915_request *rq;

	mb(); /* paranoia: read the CSB pointers from after the reset */
	clflush(execlists->csb_write);
	mb();

	process_csb(engine); /* drain preemption events */

	/* Following the reset, we need to reload the CSB read/write pointers */
	reset_csb_pointers(engine);

	/*
	 * Save the currently executing context, even if we completed
	 * its request, it was still running at the time of the
	 * reset and will have been clobbered.
	 */
	rq = execlists_active(execlists);
	if (!rq)
		goto unwind;

	/* We still have requests in-flight; the engine should be active */
	GEM_BUG_ON(!intel_engine_pm_is_awake(engine));

	ce = rq->hw_context;
	GEM_BUG_ON(!i915_vma_is_pinned(ce->state));

	if (i915_request_completed(rq)) {
		/* Idle context; tidy up the ring so we can restart afresh */
		ce->ring->head = intel_ring_wrap(ce->ring, rq->tail);
		goto out_replay;
	}

	/* Context has requests still in-flight; it should not be idle! */
	GEM_BUG_ON(i915_active_is_idle(&ce->active));
	rq = active_request(ce->timeline, rq);
	ce->ring->head = intel_ring_wrap(ce->ring, rq->head);
	GEM_BUG_ON(ce->ring->head == ce->ring->tail);

	/*
	 * If this request hasn't started yet, e.g. it is waiting on a
	 * semaphore, we need to avoid skipping the request or else we
	 * break the signaling chain. However, if the context is corrupt
	 * the request will not restart and we will be stuck with a wedged
	 * device. It is quite often the case that if we issue a reset
	 * while the GPU is loading the context image, that the context
	 * image becomes corrupt.
	 *
	 * Otherwise, if we have not started yet, the request should replay
	 * perfectly and we do not need to flag the result as being erroneous.
	 */
	if (!i915_request_started(rq))
		goto out_replay;

	/*
	 * If the request was innocent, we leave the request in the ELSP
	 * and will try to replay it on restarting. The context image may
	 * have been corrupted by the reset, in which case we may have
	 * to service a new GPU hang, but more likely we can continue on
	 * without impact.
	 *
	 * If the request was guilty, we presume the context is corrupt
	 * and have to at least restore the RING register in the context
	 * image back to the expected values to skip over the guilty request.
	 */
	__i915_request_reset(rq, stalled);
	if (!stalled)
		goto out_replay;

	/*
	 * We want a simple context + ring to execute the breadcrumb update.
	 * We cannot rely on the context being intact across the GPU hang,
	 * so clear it and rebuild just what we need for the breadcrumb.
	 * All pending requests for this context will be zapped, and any
	 * future request will be after userspace has had the opportunity
	 * to recreate its own state.
	 */
	GEM_BUG_ON(!intel_context_is_pinned(ce));
	restore_default_state(ce, engine);

out_replay:
	GEM_TRACE("%s replay {head:%04x, tail:%04x}\n",
		  engine->name, ce->ring->head, ce->ring->tail);
	intel_ring_update_space(ce->ring);
	__execlists_reset_reg_state(ce, engine);
	__execlists_update_reg_state(ce, engine);
	ce->lrc_desc |= CTX_DESC_FORCE_RESTORE; /* paranoid: GPU was reset! */

unwind:
	/* Push back any incomplete requests for replay after the reset. */
	cancel_port_requests(execlists);
	__unwind_incomplete_requests(engine);
}