__execlists_update_reg_state(const struct intel_context *ce,
			     const struct intel_engine_cs *engine)
{
	struct intel_ring *ring = ce->ring;
	u32 *regs = ce->lrc_reg_state;

	GEM_BUG_ON(!intel_ring_offset_valid(ring, ring->head));
	GEM_BUG_ON(!intel_ring_offset_valid(ring, ring->tail));

	regs[CTX_RING_BUFFER_START] = i915_ggtt_offset(ring->vma);
	regs[CTX_RING_HEAD] = ring->head;
	regs[CTX_RING_TAIL] = ring->tail;

	/* RPCS */
	if (engine->class == RENDER_CLASS) {
		regs[CTX_R_PWR_CLK_STATE] =
			intel_sseu_make_rpcs(engine->i915, &ce->sseu);

		i915_oa_init_reg_state(ce, engine);
	}
}