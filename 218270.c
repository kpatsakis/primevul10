static bool virtual_matches(const struct virtual_engine *ve,
			    const struct i915_request *rq,
			    const struct intel_engine_cs *engine)
{
	const struct intel_engine_cs *inflight;

	if (!(rq->execution_mask & engine->mask)) /* We peeked too soon! */
		return false;

	/*
	 * We track when the HW has completed saving the context image
	 * (i.e. when we have seen the final CS event switching out of
	 * the context) and must not overwrite the context image before
	 * then. This restricts us to only using the active engine
	 * while the previous virtualized request is inflight (so
	 * we reuse the register offsets). This is a very small
	 * hystersis on the greedy seelction algorithm.
	 */
	inflight = intel_context_inflight(&ve->context);
	if (inflight && inflight != engine)
		return false;

	return true;
}