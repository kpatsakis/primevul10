static void init_wa_bb_reg_state(u32 * const regs,
				 const struct intel_engine_cs *engine,
				 u32 pos_bb_per_ctx)
{
	const struct i915_ctx_workarounds * const wa_ctx = &engine->wa_ctx;

	if (wa_ctx->per_ctx.size) {
		const u32 ggtt_offset = i915_ggtt_offset(wa_ctx->vma);

		regs[pos_bb_per_ctx] =
			(ggtt_offset + wa_ctx->per_ctx.offset) | 0x01;
	}

	if (wa_ctx->indirect_ctx.size) {
		const u32 ggtt_offset = i915_ggtt_offset(wa_ctx->vma);

		regs[pos_bb_per_ctx + 2] =
			(ggtt_offset + wa_ctx->indirect_ctx.offset) |
			(wa_ctx->indirect_ctx.size / CACHELINE_BYTES);

		regs[pos_bb_per_ctx + 4] =
			intel_lr_indirect_ctx_offset(engine) << 6;
	}
}