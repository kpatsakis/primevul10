
static void io_ring_ctx_wait_and_kill(struct io_ring_ctx *ctx)
{
	unsigned long index;
	struct creds *creds;

	mutex_lock(&ctx->uring_lock);
	percpu_ref_kill(&ctx->refs);
	if (ctx->rings)
		__io_cqring_overflow_flush(ctx, true);
	xa_for_each(&ctx->personalities, index, creds)
		io_unregister_personality(ctx, index);
	mutex_unlock(&ctx->uring_lock);

	io_kill_timeouts(ctx, NULL, true);
	io_poll_remove_all(ctx, NULL, true);

	/* if we failed setting up the ctx, we might not have any rings */
	io_iopoll_try_reap_events(ctx);

	INIT_WORK(&ctx->exit_work, io_ring_exit_work);
	/*
	 * Use system_unbound_wq to avoid spawning tons of event kworkers
	 * if we're exiting a ton of rings at the same time. It just adds
	 * noise and overhead, there's no discernable change in runtime
	 * over using system_wq.
	 */
	queue_work(system_unbound_wq, &ctx->exit_work);