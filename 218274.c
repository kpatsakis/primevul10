static void reset_active(struct i915_request *rq,
			 struct intel_engine_cs *engine)
{
	struct intel_context * const ce = rq->hw_context;
	u32 head;

	/*
	 * The executing context has been cancelled. We want to prevent
	 * further execution along this context and propagate the error on
	 * to anything depending on its results.
	 *
	 * In __i915_request_submit(), we apply the -EIO and remove the
	 * requests' payloads for any banned requests. But first, we must
	 * rewind the context back to the start of the incomplete request so
	 * that we do not jump back into the middle of the batch.
	 *
	 * We preserve the breadcrumbs and semaphores of the incomplete
	 * requests so that inter-timeline dependencies (i.e other timelines)
	 * remain correctly ordered. And we defer to __i915_request_submit()
	 * so that all asynchronous waits are correctly handled.
	 */
	GEM_TRACE("%s(%s): { rq=%llx:%lld }\n",
		  __func__, engine->name, rq->fence.context, rq->fence.seqno);

	/* On resubmission of the active request, payload will be scrubbed */
	if (i915_request_completed(rq))
		head = rq->tail;
	else
		head = active_request(ce->timeline, rq)->head;
	ce->ring->head = intel_ring_wrap(ce->ring, head);
	intel_ring_update_space(ce->ring);

	/* Scrub the context image to prevent replaying the previous batch */
	restore_default_state(ce, engine);
	__execlists_update_reg_state(ce, engine);

	/* We've switched away, so this should be a no-op, but intent matters */
	ce->lrc_desc |= CTX_DESC_FORCE_RESTORE;
}