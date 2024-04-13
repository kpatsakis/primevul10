need_timeslice(struct intel_engine_cs *engine, const struct i915_request *rq)
{
	int hint;

	if (!intel_engine_has_timeslices(engine))
		return false;

	if (list_is_last(&rq->sched.link, &engine->active.requests))
		return false;

	hint = max(rq_prio(list_next_entry(rq, sched.link)),
		   engine->execlists.queue_priority_hint);

	return hint >= effective_prio(rq);
}