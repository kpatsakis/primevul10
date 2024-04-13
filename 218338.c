execlists_schedule_in(struct i915_request *rq, int idx)
{
	struct intel_context * const ce = rq->hw_context;
	struct intel_engine_cs *old;

	GEM_BUG_ON(!intel_engine_pm_is_awake(rq->engine));
	trace_i915_request_in(rq, idx);

	old = READ_ONCE(ce->inflight);
	do {
		if (!old) {
			WRITE_ONCE(ce->inflight, __execlists_schedule_in(rq));
			break;
		}
	} while (!try_cmpxchg(&ce->inflight, &old, ptr_inc(old)));

	GEM_BUG_ON(intel_context_inflight(ce) != rq->engine);
	return i915_request_get(rq);
}