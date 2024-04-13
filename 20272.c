static void ctx_flush_and_put(struct io_ring_ctx *ctx)
{
	if (!ctx)
		return;
	if (ctx->submit_state.comp.nr) {
		mutex_lock(&ctx->uring_lock);
		io_submit_flush_completions(ctx);
		mutex_unlock(&ctx->uring_lock);
	}
	percpu_ref_put(&ctx->refs);
}