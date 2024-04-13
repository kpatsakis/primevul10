void intel_execlists_set_default_submission(struct intel_engine_cs *engine)
{
	engine->submit_request = execlists_submit_request;
	engine->cancel_requests = execlists_cancel_requests;
	engine->schedule = i915_schedule;
	engine->execlists.tasklet.func = execlists_submission_tasklet;

	engine->reset.prepare = execlists_reset_prepare;
	engine->reset.reset = execlists_reset;
	engine->reset.finish = execlists_reset_finish;

	engine->park = execlists_park;
	engine->unpark = NULL;

	engine->flags |= I915_ENGINE_SUPPORTS_STATS;
	if (!intel_vgpu_active(engine->i915)) {
		engine->flags |= I915_ENGINE_HAS_SEMAPHORES;
		if (HAS_LOGICAL_RING_PREEMPTION(engine->i915))
			engine->flags |= I915_ENGINE_HAS_PREEMPTION;
	}

	if (INTEL_GEN(engine->i915) >= 12)
		engine->flags |= I915_ENGINE_HAS_RELATIVE_MMIO;
}