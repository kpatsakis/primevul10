static void execlists_park(struct intel_engine_cs *engine)
{
	cancel_timer(&engine->execlists.timer);
	cancel_timer(&engine->execlists.preempt);
}