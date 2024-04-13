static void execlists_context_reset(struct intel_context *ce)
{
	/*
	 * Because we emit WA_TAIL_DWORDS there may be a disparity
	 * between our bookkeeping in ce->ring->head and ce->ring->tail and
	 * that stored in context. As we only write new commands from
	 * ce->ring->tail onwards, everything before that is junk. If the GPU
	 * starts reading from its RING_HEAD from the context, it may try to
	 * execute that junk and die.
	 *
	 * The contexts that are stilled pinned on resume belong to the
	 * kernel, and are local to each engine. All other contexts will
	 * have their head/tail sanitized upon pinning before use, so they
	 * will never see garbage,
	 *
	 * So to avoid that we reset the context images upon resume. For
	 * simplicity, we just zero everything out.
	 */
	intel_ring_reset(ce->ring, 0);
	__execlists_update_reg_state(ce, ce->engine);
}