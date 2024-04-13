__execlists_schedule_out(struct i915_request *rq,
			 struct intel_engine_cs * const engine)
{
	struct intel_context * const ce = rq->hw_context;

	/*
	 * NB process_csb() is not under the engine->active.lock and hence
	 * schedule_out can race with schedule_in meaning that we should
	 * refrain from doing non-trivial work here.
	 */

	/*
	 * If we have just completed this context, the engine may now be
	 * idle and we want to re-enter powersaving.
	 */
	if (list_is_last(&rq->link, &ce->timeline->requests) &&
	    i915_request_completed(rq))
		intel_engine_add_retire(engine, ce->timeline);

	intel_engine_context_out(engine);
	execlists_context_status_change(rq, INTEL_CONTEXT_SCHEDULE_OUT);
	intel_gt_pm_put_async(engine->gt);

	/*
	 * If this is part of a virtual engine, its next request may
	 * have been blocked waiting for access to the active context.
	 * We have to kick all the siblings again in case we need to
	 * switch (e.g. the next request is not runnable on this
	 * engine). Hopefully, we will already have submitted the next
	 * request before the tasklet runs and do not need to rebuild
	 * each virtual tree and kick everyone again.
	 */
	if (ce->engine != engine)
		kick_siblings(rq, ce);

	intel_context_put(ce);
}