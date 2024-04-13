int intel_execlists_submission_setup(struct intel_engine_cs *engine)
{
	tasklet_init(&engine->execlists.tasklet,
		     execlists_submission_tasklet, (unsigned long)engine);
	timer_setup(&engine->execlists.timer, execlists_timeslice, 0);
	timer_setup(&engine->execlists.preempt, execlists_preempt, 0);

	logical_ring_default_vfuncs(engine);
	logical_ring_default_irqs(engine);

	if (engine->class == RENDER_CLASS)
		rcs_submission_override(engine);

	return 0;
}