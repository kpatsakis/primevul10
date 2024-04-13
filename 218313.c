static void execlists_context_unpin(struct intel_context *ce)
{
	check_redzone((void *)ce->lrc_reg_state - LRC_STATE_PN * PAGE_SIZE,
		      ce->engine);

	i915_gem_object_unpin_map(ce->state->obj);
	intel_ring_reset(ce->ring, ce->ring->tail);
}