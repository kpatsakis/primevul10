logical_ring_default_vfuncs(struct intel_engine_cs *engine)
{
	/* Default vfuncs which can be overriden by each engine. */

	engine->destroy = execlists_destroy;
	engine->resume = execlists_resume;

	engine->reset.prepare = execlists_reset_prepare;
	engine->reset.reset = execlists_reset;
	engine->reset.finish = execlists_reset_finish;

	engine->cops = &execlists_context_ops;
	engine->request_alloc = execlists_request_alloc;

	engine->emit_flush = gen8_emit_flush;
	engine->emit_init_breadcrumb = gen8_emit_init_breadcrumb;
	engine->emit_fini_breadcrumb = gen8_emit_fini_breadcrumb;
	if (INTEL_GEN(engine->i915) >= 12)
		engine->emit_fini_breadcrumb = gen12_emit_fini_breadcrumb;

	engine->set_default_submission = intel_execlists_set_default_submission;

	if (INTEL_GEN(engine->i915) < 11) {
		engine->irq_enable = gen8_logical_ring_enable_irq;
		engine->irq_disable = gen8_logical_ring_disable_irq;
	} else {
		/*
		 * TODO: On Gen11 interrupt masks need to be clear
		 * to allow C6 entry. Keep interrupts enabled at
		 * and take the hit of generating extra interrupts
		 * until a more refined solution exists.
		 */
	}
	if (IS_GEN(engine->i915, 8))
		engine->emit_bb_start = gen8_emit_bb_start;
	else
		engine->emit_bb_start = gen9_emit_bb_start;
}