static noinline void preempt_reset(struct intel_engine_cs *engine)
{
	const unsigned int bit = I915_RESET_ENGINE + engine->id;
	unsigned long *lock = &engine->gt->reset.flags;

	if (i915_modparams.reset < 3)
		return;

	if (test_and_set_bit(bit, lock))
		return;

	/* Mark this tasklet as disabled to avoid waiting for it to complete */
	tasklet_disable_nosync(&engine->execlists.tasklet);

	GEM_TRACE("%s: preempt timeout %lu+%ums\n",
		  engine->name,
		  READ_ONCE(engine->props.preempt_timeout_ms),
		  jiffies_to_msecs(jiffies - engine->execlists.preempt.expires));
	intel_engine_reset(engine, "preemption time out");

	tasklet_enable(&engine->execlists.tasklet);
	clear_and_wake_up_bit(bit, lock);
}