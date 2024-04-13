static void restore_default_state(struct intel_context *ce,
				  struct intel_engine_cs *engine)
{
	u32 *regs = ce->lrc_reg_state;

	if (engine->pinned_default_state)
		memcpy(regs, /* skip restoring the vanilla PPHWSP */
		       engine->pinned_default_state + LRC_STATE_PN * PAGE_SIZE,
		       engine->context_size - PAGE_SIZE);

	execlists_init_reg_state(regs, ce, engine, ce->ring, false);
}