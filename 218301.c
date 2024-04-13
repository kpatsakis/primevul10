switch_prio(struct intel_engine_cs *engine, const struct i915_request *rq)
{
	if (list_is_last(&rq->sched.link, &engine->active.requests))
		return INT_MIN;

	return rq_prio(list_next_entry(rq, sched.link));
}