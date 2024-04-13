__execlists_schedule_in(struct i915_request *rq)
{
	struct intel_engine_cs * const engine = rq->engine;
	struct intel_context * const ce = rq->hw_context;

	intel_context_get(ce);

	if (unlikely(i915_gem_context_is_banned(ce->gem_context)))
		reset_active(rq, engine);

	if (ce->tag) {
		/* Use a fixed tag for OA and friends */
		ce->lrc_desc |= (u64)ce->tag << 32;
	} else {
		/* We don't need a strict matching tag, just different values */
		ce->lrc_desc &= ~GENMASK_ULL(47, 37);
		ce->lrc_desc |=
			(u64)(engine->context_tag++ % NUM_CONTEXT_TAG) <<
			GEN11_SW_CTX_ID_SHIFT;
		BUILD_BUG_ON(NUM_CONTEXT_TAG > GEN12_MAX_CONTEXT_HW_ID);
	}

	intel_gt_pm_get(engine->gt);
	execlists_context_status_change(rq, INTEL_CONTEXT_SCHEDULE_IN);
	intel_engine_context_in(engine);

	return engine;
}