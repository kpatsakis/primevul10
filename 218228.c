static u32 *gen8_emit_fini_breadcrumb_rcs(struct i915_request *request, u32 *cs)
{
	cs = gen8_emit_pipe_control(cs,
				    PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH |
				    PIPE_CONTROL_DEPTH_CACHE_FLUSH |
				    PIPE_CONTROL_DC_FLUSH_ENABLE,
				    0);

	/* XXX flush+write+CS_STALL all in one upsets gem_concurrent_blt:kbl */
	cs = gen8_emit_ggtt_write_rcs(cs,
				      request->fence.seqno,
				      i915_request_active_timeline(request)->hwsp_offset,
				      PIPE_CONTROL_FLUSH_ENABLE |
				      PIPE_CONTROL_CS_STALL);

	return gen8_emit_fini_breadcrumb_footer(request, cs);
}