static void set_timeslice(struct intel_engine_cs *engine)
{
	if (!intel_engine_has_timeslices(engine))
		return;

	set_timer_ms(&engine->execlists.timer, active_timeslice(engine));
}