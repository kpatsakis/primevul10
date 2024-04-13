static u32 *gen12_emit_fini_breadcrumb(struct i915_request *request, u32 *cs)
{
	cs = gen8_emit_ggtt_write(cs,
				  request->fence.seqno,
				  i915_request_active_timeline(request)->hwsp_offset,
				  0);

	return gen12_emit_fini_breadcrumb_footer(request, cs);
}