static void __execlists_reset_reg_state(const struct intel_context *ce,
					const struct intel_engine_cs *engine)
{
	u32 *regs = ce->lrc_reg_state;
	int x;

	x = lrc_ring_mi_mode(engine);
	if (x != -1) {
		regs[x + 1] &= ~STOP_RING;
		regs[x + 1] |= STOP_RING << 16;
	}
}