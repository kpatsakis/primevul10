static u32 *emit_preempt_busywait(struct i915_request *request, u32 *cs)
{
	*cs++ = MI_SEMAPHORE_WAIT |
		MI_SEMAPHORE_GLOBAL_GTT |
		MI_SEMAPHORE_POLL |
		MI_SEMAPHORE_SAD_EQ_SDD;
	*cs++ = 0;
	*cs++ = intel_hws_preempt_address(request->engine);
	*cs++ = 0;

	return cs;
}