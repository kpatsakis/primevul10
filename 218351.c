execlists_schedule_out(struct i915_request *rq)
{
	struct intel_context * const ce = rq->hw_context;
	struct intel_engine_cs *cur, *old;

	trace_i915_request_out(rq);

	old = READ_ONCE(ce->inflight);
	do
		cur = ptr_unmask_bits(old, 2) ? ptr_dec(old) : NULL;
	while (!try_cmpxchg(&ce->inflight, &old, cur));
	if (!cur)
		__execlists_schedule_out(rq, old);

	i915_request_put(rq);
}