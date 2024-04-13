static bool can_merge_rq(const struct i915_request *prev,
			 const struct i915_request *next)
{
	GEM_BUG_ON(prev == next);
	GEM_BUG_ON(!assert_priority_queue(prev, next));

	/*
	 * We do not submit known completed requests. Therefore if the next
	 * request is already completed, we can pretend to merge it in
	 * with the previous context (and we will skip updating the ELSP
	 * and tracking). Thus hopefully keeping the ELSP full with active
	 * contexts, despite the best efforts of preempt-to-busy to confuse
	 * us.
	 */
	if (i915_request_completed(next))
		return true;

	if (unlikely((prev->flags ^ next->flags) &
		     (I915_REQUEST_NOPREEMPT | I915_REQUEST_SENTINEL)))
		return false;

	if (!can_merge_ctx(prev->hw_context, next->hw_context))
		return false;

	return true;
}