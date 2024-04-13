static void set_preempt_timeout(struct intel_engine_cs *engine)
{
	if (!intel_engine_has_preempt_reset(engine))
		return;

	set_timer_ms(&engine->execlists.preempt,
		     active_preempt_timeout(engine));
}