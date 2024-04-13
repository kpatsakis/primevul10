static void submit_queue(struct intel_engine_cs *engine,
			 const struct i915_request *rq)
{
	struct intel_engine_execlists *execlists = &engine->execlists;

	if (rq_prio(rq) <= execlists->queue_priority_hint)
		return;

	execlists->queue_priority_hint = rq_prio(rq);
	__submit_queue_imm(engine);
}