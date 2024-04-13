static void io_cqring_ev_posted_iopoll(struct io_ring_ctx *ctx)
{
	/* see waitqueue_active() comment */
	smp_mb();

	if (ctx->flags & IORING_SETUP_SQPOLL) {
		if (waitqueue_active(&ctx->cq_wait))
			wake_up(&ctx->cq_wait);
	}
	if (io_should_trigger_evfd(ctx))
		eventfd_signal(ctx->cq_ev_fd, 1);
	if (waitqueue_active(&ctx->poll_wait)) {
		wake_up_interruptible(&ctx->poll_wait);
		kill_fasync(&ctx->cq_fasync, SIGIO, POLL_IN);
	}
}