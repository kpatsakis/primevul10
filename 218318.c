static void init_common_reg_state(u32 * const regs,
				  const struct intel_engine_cs *engine,
				  const struct intel_ring *ring)
{
	regs[CTX_CONTEXT_CONTROL] =
		_MASKED_BIT_DISABLE(CTX_CTRL_ENGINE_CTX_RESTORE_INHIBIT) |
		_MASKED_BIT_ENABLE(CTX_CTRL_INHIBIT_SYN_CTX_SWITCH);
	if (INTEL_GEN(engine->i915) < 11)
		regs[CTX_CONTEXT_CONTROL] |=
			_MASKED_BIT_DISABLE(CTX_CTRL_ENGINE_CTX_SAVE_INHIBIT |
					    CTX_CTRL_RS_CTX_ENABLE);

	regs[CTX_RING_BUFFER_CONTROL] = RING_CTL_SIZE(ring->size) | RING_VALID;
	regs[CTX_BB_STATE] = RING_BB_PPGTT;
}