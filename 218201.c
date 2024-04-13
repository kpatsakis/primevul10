active_timeslice(const struct intel_engine_cs *engine)
{
	const struct i915_request *rq = *engine->execlists.active;

	if (i915_request_completed(rq))
		return 0;

	if (engine->execlists.switch_priority_hint < effective_prio(rq))
		return 0;

	return timeslice(engine);
}