static void execlists_init_reg_state(u32 *regs,
				     const struct intel_context *ce,
				     const struct intel_engine_cs *engine,
				     const struct intel_ring *ring,
				     bool close)
{
	/*
	 * A context is actually a big batch buffer with several
	 * MI_LOAD_REGISTER_IMM commands followed by (reg, value) pairs. The
	 * values we are setting here are only for the first context restore:
	 * on a subsequent save, the GPU will recreate this batchbuffer with new
	 * values (including all the missing MI_LOAD_REGISTER_IMM commands that
	 * we are not initializing here).
	 *
	 * Must keep consistent with virtual_update_register_offsets().
	 */
	u32 *bbe = set_offsets(regs, reg_offsets(engine), engine);

	if (close) { /* Close the batch; used mainly by live_lrc_layout() */
		*bbe = MI_BATCH_BUFFER_END;
		if (INTEL_GEN(engine->i915) >= 10)
			*bbe |= BIT(0);
	}

	init_common_reg_state(regs, engine, ring);
	init_ppgtt_reg_state(regs, vm_alias(ce->vm));

	init_wa_bb_reg_state(regs, engine,
			     INTEL_GEN(engine->i915) >= 12 ?
			     GEN12_CTX_BB_PER_CTX_PTR :
			     CTX_BB_PER_CTX_PTR);
}