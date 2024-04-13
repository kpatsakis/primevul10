gen8_emit_fini_breadcrumb_footer(struct i915_request *request,
				 u32 *cs)
{
	*cs++ = MI_USER_INTERRUPT;

	*cs++ = MI_ARB_ON_OFF | MI_ARB_ENABLE;
	if (intel_engine_has_semaphores(request->engine))
		cs = emit_preempt_busywait(request, cs);

	request->tail = intel_ring_offset(request, cs);
	assert_ring_tail_valid(request->ring, request->tail);

	return gen8_emit_wa_tail(request, cs);
}