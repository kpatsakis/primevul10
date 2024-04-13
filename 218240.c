gen12_emit_fini_breadcrumb_rcs(struct i915_request *request, u32 *cs)
{
	cs = gen8_emit_ggtt_write_rcs(cs,
				      request->fence.seqno,
				      i915_request_active_timeline(request)->hwsp_offset,
				      PIPE_CONTROL_CS_STALL |
				      PIPE_CONTROL_TILE_CACHE_FLUSH |
				      PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH |
				      PIPE_CONTROL_DEPTH_CACHE_FLUSH |
				      /* Wa_1409600907:tgl */
				      PIPE_CONTROL_DEPTH_STALL |
				      PIPE_CONTROL_DC_FLUSH_ENABLE |
				      PIPE_CONTROL_FLUSH_ENABLE |
				      PIPE_CONTROL_HDC_PIPELINE_FLUSH);

	return gen12_emit_fini_breadcrumb_footer(request, cs);
}